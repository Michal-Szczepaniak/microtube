#include "player.h"
#include <QQmlInfo>
#include <QTimer>
#include <QPainter>
#include <QMatrix4x4>
#include <cmath>
#include <QTextDocument>
#include <QDateTime>

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
    _scaletempo(nullptr),
    _state(VideoPlayer::StateStopped),
    _previousState(VideoPlayer::StatePlaying),
    _timer(new QTimer((QObject*)this)),
    _pos(0),
    _playbackSpeed(1.0),
    _created(false),
    _subtitleEnd(0) {

    _timer->setSingleShot(false);
    _timer->setInterval(100);
    connect(_timer, &QTimer::timeout, this, &VideoPlayer::positionChanged);

    _bufferTimeoutTimer.setSingleShot(true);
    _bufferTimeoutTimer.setInterval(1000);
    connect(&_bufferTimeoutTimer, &QTimer::timeout, [&](){
        if (_state == StateBuffering) setState(_previousState);
    });

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

    _pulsesink = gst_element_factory_make("pulsesink", "PulseSink");
    Q_ASSERT(_pulsesink);

    _scaletempo = gst_element_factory_make("scaletempo", "Scaletempo");
    Q_ASSERT(_scaletempo);

    gst_bin_add_many(GST_BIN(_pipeline), _scaletempo, _pulsesink, NULL);

    gst_element_link(_scaletempo, _pulsesink);

    GstBus *bus = gst_element_get_bus(_pipeline);
    gst_bus_add_watch(bus, bus_call, this);
    gst_object_unref(bus);
}

QString VideoPlayer::getVideoSource() const {
    return _videoUrl;
}

void VideoPlayer::setVideoSource(const QString& videoSource) {
    if (_videoUrl != videoSource) {
        _videoUrl = videoSource;

        if (_videoSource != nullptr) {
            gst_object_ref(_videoSource);
            if (gst_bin_remove(GST_BIN(_pipeline), _videoSource) == false) {
                gst_element_set_state(_videoSource, GST_STATE_NULL);
                gst_object_unref(_videoSource);
                _videoSource = nullptr;
            }
        }

        if (!_audioOnlyMode) {
            _videoSource = gst_element_factory_make ("uridecodebin", "VideoSource");
            Q_ASSERT(_videoSource);
//            g_object_set(_videoSource, "buffer-duration", 1800000000000, NULL);
//            g_object_set(_videoSource, "download", TRUE, NULL);
            g_object_set(_videoSource, "uri", _videoUrl.toUtf8().constData(), NULL);

            gst_bin_add(GST_BIN(_pipeline), _videoSource);
            g_signal_connect (_videoSource, "pad-added", G_CALLBACK (cbNewVideoPad), this);
        }

        emit videoSourceChanged();
    }
}

QString VideoPlayer::getAudioSource() const {
    return _audioUrl;
}

void VideoPlayer::setAudioSource(const QString& audioSource) {
    _audioUrl = audioSource;

    if (_audioSource != nullptr) {
        gst_object_ref(_audioSource);
        if (gst_bin_remove(GST_BIN(_pipeline), _audioSource) == false) {
            gst_element_set_state(_audioSource, GST_STATE_NULL);
            gst_object_unref(_audioSource);
            _audioSource = nullptr;
        }
    }

    if (audioSource != "") {
        _audioSource = gst_element_factory_make ("uridecodebin", "AudioSource");
        Q_ASSERT(_audioSource);
//            g_object_set(_videoSource, "download", TRUE, NULL);
//            g_object_set(_audioSource, "buffer-duration", 20000000000, NULL);

        gst_bin_add(GST_BIN(_pipeline), _audioSource);

        g_signal_connect(_audioSource, "pad-added", G_CALLBACK (cbNewPad), this);

        g_object_set(_audioSource, "uri", _audioUrl.toUtf8().constData(), NULL);
    }

    emit audioSourceChanged();
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
    if (_subSource) {
        gst_object_ref(_subSource);
        if (gst_bin_remove(GST_BIN(_pipeline), _subSource) == false) {
            gst_element_set_state(_subSource, GST_STATE_NULL);
            gst_object_unref(_subSource);
        }
    }

    if (_subParse) {
        gst_object_ref(_subParse);
        if (gst_bin_remove(GST_BIN(_pipeline), _subParse) == false) {
            gst_element_set_state(_subParse, GST_STATE_NULL);
            gst_object_unref(_subParse);
        }
    }

    if (_appSink) {
        gst_object_ref(_appSink);
        if (gst_bin_remove(GST_BIN(_pipeline), _appSink) == false) {
            gst_element_set_state(_appSink, GST_STATE_NULL);
            gst_object_unref(_appSink);
        }
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

    gint64 pos = 0;
    if (!gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos)) {
        return;
    }

    gst_bin_add_many(GST_BIN(_pipeline), _subSource, _subParse, _appSink, NULL);
    gst_element_link_many(_subSource, _subParse, _appSink, NULL);
    emit subtitleChanged();

    seek(pos);
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

int VideoPlayer::getProjection() const
{
    return _projection;
}

void VideoPlayer::setProjection(int projection)
{
    _projection = static_cast<Projection>(projection);

    if (_renderer) {
        _renderer->setProjection(_projection);
    }

    emit projectionChanged();
}

float VideoPlayer::getProjectionX() const
{
    if (!_renderer) {
        return 0.f;
    }

    return _renderer->getProjectionX();
}

void VideoPlayer::setProjectionX(float projectionX)
{
    if (_renderer) {
        _renderer->setProjectionX(projectionX);
    }
}

float VideoPlayer::getProjectionY() const
{
    if (!_renderer) {
        return 0.f;
    }

    return _renderer->getProjectionY();
}

void VideoPlayer::setProjectionY(float projectionY)
{
    if (_renderer) {
        _renderer->setProjectionY(projectionY);
    }
}

bool VideoPlayer::isAvc1() const
{
    return _avc1;
}

void VideoPlayer::setAvc1(bool avc1)
{
    _avc1 = avc1;
}

bool VideoPlayer::pause() {
    return setState(VideoPlayer::StatePaused);
}

bool VideoPlayer::play() {
    if (!_audioOnlyMode) {
        if (!_renderer) {
            _renderer = new RendererNemo(this);
            if (!_renderer) {
                qmlInfo(this) << "Failed to create viewfinder renderer";
                return false;
            }

            QObject::connect(_renderer, SIGNAL(updateRequested()), this, SLOT(updateRequested()));
            gst_bin_add(GST_BIN(_pipeline), _renderer->sinkElement());
            _renderer->setProjection(_projection);
            _renderer->setAvc1(_avc1);
        }

        _renderer->resize(QSizeF(width(), height()));
    }

    if (!_pipeline) {
        qmlInfo(this) << "no playbin";
        return false;
    }

    return setState(VideoPlayer::StatePlaying);
}

bool VideoPlayer::seek(qint64 offset) {
    if (!_pipeline) {
        qmlInfo(this) << "no playbin2";
        return false;
    }

    gint64 pos = offset;

    offset *= 1000000;

    gint64 dur = 0;
    if (!gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &dur)) {
        return false;
    }

    if (offset > dur) {
        offset = dur;
        stop();
    }

    bool ret = gst_element_seek(
        _pipeline, _playbackSpeed, GST_FORMAT_TIME, (GstSeekFlags) (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_TRICKMODE|GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET, offset, GST_SEEK_TYPE_NONE, -1
    );

    if (ret) {
        _pos = pos;
    }

    return TRUE;
}

bool VideoPlayer::stop() {
    return setState(VideoPlayer::StateStopped);
}

void VideoPlayer::setAudioOnlyMode(bool audioOnlyMode)
{
    _audioOnlyMode = audioOnlyMode;

    emit audioOnlyModeChanged();
}

bool VideoPlayer::getAudioOnlyMode() const
{
    return _audioOnlyMode;
}

bool VideoPlayer::setPlaybackSpeed(double speed)
{
    gint64 position;

    if (speed == _playbackSpeed) return false;

    if (!gst_element_query_position(_pipeline, GST_FORMAT_TIME, &position)) {
        g_printerr("Unable to retrieve current position.\n");
        return false;
    }

    bool ret = gst_element_seek(
        _pipeline, speed, GST_FORMAT_TIME, (GstSeekFlags) (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_TRICKMODE|GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET, position, GST_SEEK_TYPE_NONE, 0
    );

    if (!ret) {
        qDebug() << "Failed to change playback speed";
    }

    _playbackSpeed = speed;

    return true;
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

    gint64 pos;
    if (gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos) && pos > _subtitleEnd) {
        setDisplaySubtitle("");
    }

    if (needsNativePainting) {
        painter->beginNativePainting();
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
        if (state == VideoPlayer::StatePaused) {
            _previousState = VideoPlayer::StatePaused;
        }

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

        _state = state;
        emit stateChanged();

        return true;
    } else if (state == VideoPlayer::StatePlaying) {
        if (_state == VideoPlayer::StateBuffering) {
            _previousState = VideoPlayer::StatePlaying;
        }

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

        _playbackSpeed = 1.0;
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
        qCritical() << "Error" << err->message << " " << debug;

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
    case GST_MESSAGE_WARNING:
    {

        gchar *debug = NULL;
        GError *err = NULL;
        gst_message_parse_warning(msg, &err, &debug);
        qWarning() << "Warning" << err->message;

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
    case GST_MESSAGE_INFO:
    {

        gchar *debug = NULL;
        GError *err = NULL;
        gst_message_parse_info(msg, &err, &debug);
        qInfo() << "Info" << err->message;

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
            gst_element_get_name(other->_scaletempo));
    gst_element_link(element, other->_scaletempo);
}

void VideoPlayer::cbNewVideoPad(GstElement *element, GstPad *pad, gpointer data)
{
    gchar *name;
    VideoPlayer *other = (VideoPlayer*)data;

    name = gst_pad_get_name(pad);
    g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
    g_free (name);

    Q_ASSERT(other->_renderer);

    GstPad *compatiblePad = gst_element_get_compatible_pad(other->_renderer->sinkElement(), pad, NULL);
    if (compatiblePad != NULL) {
        g_print ("element %s will be linked to %s\n",
                gst_element_get_name(element),
                gst_element_get_name(other->_renderer->sinkElement()));
        gst_element_link(element, other->_renderer->sinkElement());
        gst_object_unref(GST_OBJECT(compatiblePad));

        return;
    }

    if (other->_audioUrl != "") return;

    compatiblePad = gst_element_get_compatible_pad(other->_scaletempo, pad, NULL);
    if (compatiblePad != NULL) {
        g_print ("element %s will be linked to %s\n",
                gst_element_get_name(element),
                gst_element_get_name(other->_scaletempo));
        gst_element_link(element, other->_scaletempo);
        gst_object_unref(GST_OBJECT(compatiblePad));
    }
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
    _bufferingProgress[name] = percent;

    for (int p : _bufferingProgress) {
        if (_audioOnlyMode ? (p < 2) : (p < 5)) {
            if (_state != VideoPlayer::StateBuffering) {
                _previousState = _state;
            }

            setState(VideoPlayer::StateBuffering);

            _bufferTimeoutTimer.start();

            _bufferTimestamp = QDateTime::currentMSecsSinceEpoch();
            return;
        }
    }

    if (_state != StateBuffering) {
        return;
    }

    if (_previousState == VideoPlayer::StatePlaying &&
        QDateTime::currentMSecsSinceEpoch() - _bufferTimestamp > 1000) {
            play();
    } else if (_previousState != VideoPlayer::StatePlaying) {
        setState(_previousState);
    }
}
