#include "player.h"
#include <QQmlInfo>
#include <QTimer>
#include <QPainter>
#include <QMatrix4x4>
#include <cmath>
#include <QTextDocument>

VideoPlayer::VideoPlayer(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    _renderer(nullptr),
    _audioResource((QObject*)this, AudioResourceQt::AudioResource::MediaType),
    _pipeline(nullptr),
    _videoSource(nullptr),
    _audioSource(nullptr),
    _pulsesink(nullptr),
    _subParse(nullptr),
    _appSink(nullptr),
    _subSource(nullptr),
    _state(VideoPlayer::StateStopped),
    _timer(new QTimer((QObject*)this)),
    _pos(0),
    _created(false),
    _subtitleEnd(0) {

    _timer->setSingleShot(false);
    _timer->setInterval(500);
    connect(_timer, &QTimer::timeout, this, &VideoPlayer::positionChanged);

    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setSmooth(false);
    setAntialiasing(false);
    _audioResource.acquire();
}

VideoPlayer::~VideoPlayer() {
    stop();

    if (_pipeline) {
        gst_object_unref(_pipeline);
        _pipeline = nullptr;
    }
}

void VideoPlayer::componentComplete() {
    QQuickPaintedItem::componentComplete();
}

void VideoPlayer::classBegin() {
    QQuickPaintedItem::classBegin();

    _pipeline = gst_pipeline_new ("video-player");
    Q_ASSERT(_pipeline);

    _videoSource = gst_element_factory_make ("uridecodebin", "VideoSource");
    Q_ASSERT(_videoSource);
    g_object_set(_videoSource, "buffer-duration", 1800000000000, NULL);

    _audioSource = gst_element_factory_make ("uridecodebin", "AudioSource");
    Q_ASSERT(_audioSource);
    g_object_set(_audioSource, "buffer-duration", 20000000000, NULL);
//    g_object_set(_audioSource, "download", true, NULL);

    _pulsesink = gst_element_factory_make("pulsesink", "PulseSink");
    Q_ASSERT(_pulsesink);

    gst_bin_add_many(GST_BIN(_pipeline), _videoSource, _audioSource, _pulsesink, _subSource, _subParse, _appSink, NULL);
    g_signal_connect (_audioSource, "pad-added", G_CALLBACK (cbNewPad), this);
    g_signal_connect (_videoSource, "pad-added", G_CALLBACK (cbNewVideoPad), this);
    gst_element_link_many(_subSource, _subParse, _appSink, NULL);

    GstBus *bus = gst_element_get_bus(_pipeline);
    gst_bus_add_watch(bus, bus_call, this);
    gst_object_unref(bus);
}

QUrl VideoPlayer::getVideoSource() const {
    return _videoUrl;
}

void VideoPlayer::setVideoSource(const QUrl& videoSource) {
    if (_videoUrl != videoSource) {
        _videoUrl = videoSource;

        g_object_set(_videoSource, "uri", _videoUrl.toString().toUtf8().constData(), NULL);

        emit videoSourceChanged();
    }
}

QUrl VideoPlayer::getAudioSource() const {
    return _audioUrl;
}

void VideoPlayer::setAudioSource(const QUrl& audioSource) {
    if (_audioUrl != audioSource) {
        _audioUrl = audioSource;

        g_object_set(_audioSource, "uri", _audioUrl.toString().toUtf8().constData(), NULL);

        emit audioSourceChanged();
    }
}

qint64 VideoPlayer::getDuration() const {
    if (!_pipeline) {
        return 0;
    }

    gint64 dur = 0;
    if (!gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &dur)) {
        return 0;
    }

    dur /= 1000000;

    return dur;
}

qint64 VideoPlayer::getPosition() {
    if (!_pipeline) {
        return 0;
    }

    gint64 pos = 0;
    if (!gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos)) {
        return _pos;
    }

    pos /= 1000000;

    _pos = pos;

    return pos;
}

void VideoPlayer::setPosition(qint64 position) {
    seek(position);
}

QString VideoPlayer::getSubtitle() const
{
    return _currentSubtitle;
}

void VideoPlayer::setSubtitle(QString subtitle)
{
    gst_object_ref(_subSource);
    if (gst_bin_remove(GST_BIN(_pipeline), _subSource) == false) {
        gst_element_set_state(_subSource, GST_STATE_NULL);
        gst_object_unref(_subSource);
    }

    gst_object_ref(_subParse);
    if (gst_bin_remove(GST_BIN(_pipeline), _subParse) == false) {
        gst_element_set_state(_subParse, GST_STATE_NULL);
        gst_object_unref(_subParse);
    }

    gst_object_ref(_appSink);
    if (gst_bin_remove(GST_BIN(_pipeline), _appSink) == false) {
        gst_element_set_state(_appSink, GST_STATE_NULL);
        gst_object_unref(_appSink);
    }

    _subSource = nullptr;
    _subParse = nullptr;
    _appSink = nullptr;

    _currentSubtitle = subtitle;

    if (_currentSubtitle == "") {
        emit subtitleChanged();
        return;
    }

    _subSource = gst_element_factory_make ("dataurisrc", "SubtitleSource");
    Q_ASSERT(_subSource);

    _subParse = gst_element_factory_make("subparse", "subparse");
    Q_ASSERT(_subParse);

    _appSink = gst_element_factory_make("appsink", "appsink");
    Q_ASSERT(_appSink);

    g_object_set(_appSink, "emit-signals", true, NULL);
    g_signal_connect(_appSink, "new-sample", G_CALLBACK (cbNewSample), this);


    g_object_set(_subSource, "uri", ("data:text/plain;base64," + subtitle.toUtf8().toBase64()).constData(), NULL);
    gst_bin_add_many(GST_BIN(_pipeline), _subSource, _subParse, _appSink, NULL);
    gst_element_link_many(_subSource, _subParse, _appSink, NULL);
    emit subtitleChanged();
}

QString VideoPlayer::getDisplaySubtitle() const
{
    return _subtitle;
}

void VideoPlayer::setDisplaySubtitle(QString subtitle)
{
    QTextDocument d;
    d.setHtml(subtitle);

    _subtitle = d.toPlainText();

    emit displaySubtitleChanged();
}

bool VideoPlayer::pause() {
    return setState(VideoPlayer::StatePaused);
}

bool VideoPlayer::play() {
    if (!_renderer) {
        _renderer = new QtCamViewfinderRendererNemo(this);
        if (!_renderer) {
            qmlInfo(this) << "Failed to create viewfinder renderer";
            return false;
        }

        QObject::connect(_renderer, SIGNAL(updateRequested()), this, SLOT(updateRequested()));
        gst_bin_add(GST_BIN(_pipeline), _renderer->sinkElement());
    }

    _renderer->resize(QSizeF(width(), height()));

    if (!_pipeline) {
        qmlInfo(this) << "no playbin";
        return false;
    }

    qDebug() << "AudioResource: " << _audioResource.isAcquired();

    return setState(VideoPlayer::StatePlaying);
}

bool VideoPlayer::seek(qint64 offset) {
    if (!_pipeline) {
        qmlInfo(this) << "no playbin2";
        return false;
    }

    gint64 pos = offset;

    offset *= 1000000;

    GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);
    gboolean ret = gst_element_seek_simple (_pipeline, GST_FORMAT_TIME,
                                            flags, offset);

    if (ret) {
        _pos = pos;

        return TRUE;
    }

    return TRUE;
}

bool VideoPlayer::stop() {
    return setState(VideoPlayer::StateStopped);
}

void VideoPlayer::setAudioOnlyMode(bool audioOnlyMode)
{
    _audioOnlyMode = audioOnlyMode;
}

void VideoPlayer::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);

    if (_renderer) {
        _renderer->resize(newGeometry.size());
    }
}


void VideoPlayer::paint(QPainter *painter) {
    painter->fillRect(contentsBoundingRect(), Qt::black);

    if (!_renderer) {
        return;
    }

    bool needsNativePainting = _renderer->needsNativePainting();

    if (needsNativePainting) {
        painter->beginNativePainting();
    }

    gint64 pos;
    if (gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos) && pos > _subtitleEnd) {
        setDisplaySubtitle("");
    }

    _renderer->paint(QMatrix4x4(painter->combinedTransform()), painter->viewport());

    if (needsNativePainting) {
        painter->endNativePainting();
    }
}

VideoPlayer::State VideoPlayer::getState() const {
    return _state;
}

bool VideoPlayer::setState(const VideoPlayer::State& state) {
    if (state == _state) {
        return true;
    }

    if (!_pipeline) {
        qmlInfo(this) << "no playbin2";
        return false;
    }

    if (state == VideoPlayer::StatePaused || state == VideoPlayer::StateBuffering) {
        _timer->stop();

        int ret = gst_element_set_state(_pipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qmlInfo(this) << "error setting pipeline to PAUSED";
            return false;
        }

        if (ret != GST_STATE_CHANGE_ASYNC) {
            GstState st;
            if (gst_element_get_state(_pipeline, &st, NULL, GST_CLOCK_TIME_NONE)
                == GST_STATE_CHANGE_FAILURE) {
                qmlInfo(this) << "setting pipeline to PAUSED failed";
                return false;
            }

            if (st != GST_STATE_PAUSED) {
                qmlInfo(this) << "pipeline failed to transition to to PAUSED state";
                return false;
            }
        }

//        _audio_resource.release();
        _state = state;
        emit stateChanged();

        return true;
    } else if (state == VideoPlayer::StatePlaying) {
        if (gst_element_set_state(_pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
                qmlInfo(this) << "error setting pipeline to PLAYING";
                return false;
        }

        _state = state;
        emit stateChanged();

        emit durationChanged();
        emit positionChanged();

        _timer->start();
        return true;
    } else {
        _timer->stop();
        _pos = 0;

        int ret = gst_element_set_state(_pipeline, GST_STATE_NULL);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qmlInfo(this) << "error setting pipeline to NULL";
            return false;
        }

        if (_renderer) {
            gst_bin_remove(GST_BIN(_pipeline), _renderer->sinkElement());
            delete _renderer;
            _renderer = nullptr;
        }

        if (ret != GST_STATE_CHANGE_ASYNC) {
            GstState st;
            if (gst_element_get_state(_pipeline, &st, NULL, GST_CLOCK_TIME_NONE)
            == GST_STATE_CHANGE_FAILURE) {
                qmlInfo(this) << "setting pipeline to NULL failed";
                return false;
            }

            if (st != GST_STATE_NULL) {
                qmlInfo(this) << "pipeline failed to transition to to NULL state";
                return false;
            }
        }

//        _audio_resource.release();
        _state = state;
        emit stateChanged();

        emit durationChanged();
        emit positionChanged();

        return true;
    }
}

gboolean VideoPlayer::bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    Q_UNUSED(bus);

    VideoPlayer *that = (VideoPlayer *) data;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_NEW_CLOCK:
    {
        emit that->durationChanged();
    }
        break;
    case GST_MESSAGE_BUFFERING:
    {
            gint percent = 0;
            gst_message_parse_buffering (msg, &percent);
//            qDebug() << "buffering " << GST_MESSAGE_SRC_NAME(msg) << " " << percent;
            that->updateBufferingState(percent, QString::fromUtf8(GST_MESSAGE_SRC_NAME(msg)));
    }
            break;
    case GST_MESSAGE_EOS:
        that->stop();
        break;

    case GST_MESSAGE_ERROR:
    {
        gchar *debug = NULL;
        GError *err = NULL;
        gst_message_parse_error (msg, &err, &debug);
        qWarning() << "Error" << err->message;

        emit that->error(err->message, err->code, debug);
        that->stop();

        if (err) {
            g_error_free (err);
        }

        if (debug) {
            g_free (debug);
        }

    }
        break;
    default:
        break;
    }

    return TRUE;
}

void VideoPlayer::cbNewPad(GstElement *element, GstPad *pad, gpointer data)
{
    gchar *name;
    VideoPlayer *other = (VideoPlayer*)data;

    name = gst_pad_get_name (pad);
    g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
    g_free (name);

    g_print ("element %s will be linked to %s\n",
            gst_element_get_name(element),
            gst_element_get_name(other->_pulsesink));
    gst_element_link(element, other->_pulsesink);
}

void VideoPlayer::cbNewVideoPad(GstElement *element, GstPad *pad, gpointer data)
{
    gchar *name;
    VideoPlayer *other = (VideoPlayer*)data;

    name = gst_pad_get_name (pad);
    g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
    g_free (name);

    Q_ASSERT(other->_renderer);
    g_print ("element %s will be linked to %s\n",
            gst_element_get_name(element),
            gst_element_get_name(other->_renderer->sinkElement()));
    gst_element_link(element, other->_renderer->sinkElement());
}

GstFlowReturn VideoPlayer::cbNewSample(GstElement *sink, gpointer *data)
{
    GstSample *sample;
    GstMapInfo map;
    guint8 *bufferData;
    gsize size;
    VideoPlayer *parent = (VideoPlayer*)data;

    g_signal_emit_by_name (sink, "pull-sample", &sample);
    if (sample) {
        GstBuffer *buffer = gst_sample_get_buffer(sample);
        gst_buffer_map (buffer, &map, GST_MAP_READ);
        bufferData = map.data;
        size = map.size;

        parent->_subtitleEnd = buffer->pts + buffer->duration;

        parent->setDisplaySubtitle(QString::fromUtf8((const char *)bufferData, size));

        gst_sample_unref (sample);
        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}

void VideoPlayer::updateRequested() {
    update();
}

void VideoPlayer::updateBufferingState(int percent, QString name)
{
    if (!name.startsWith("queue")) return;
    _bufferingProgress[name] = percent;

    for (int p : _bufferingProgress) {
        if (p < 10) {
            setState(VideoPlayer::StateBuffering);
            return;
        }
    }

    if (_state == StateBuffering) {
        play();
    }
}
