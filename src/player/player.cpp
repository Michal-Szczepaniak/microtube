#include "player.h"
#include <QQmlInfo>
#include <QTimer>
#include "renderer.h"
#include <QPainter>
#include <QMatrix4x4>
#include <cmath>

VideoPlayer::VideoPlayer(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    _renderer(nullptr),
    _audio_resource((QObject*)this, AudioResourceQt::AudioResource::MediaType),
    _pipeline(nullptr),
    _videoSource(nullptr),
    _audioSource(nullptr),
    _state(VideoPlayer::StateStopped),
    _timer(new QTimer((QObject*)this)),
    _pos(0),
    _created(false) {

    _timer->setSingleShot(false);
    _timer->setInterval(500);
    connect(_timer, &QTimer::timeout, this, &VideoPlayer::positionChanged);

    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setSmooth(false);
    setAntialiasing(false);
    _audio_resource.acquire();
}

VideoPlayer::~VideoPlayer() {
    stop();
    _audio_resource.release();

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
    g_object_set(_videoSource, "use-buffering", false, NULL);
    _audioSource = gst_element_factory_make ("uridecodebin", "AudioSource");
    Q_ASSERT(_audioSource);
    g_object_set(_audioSource, "use-buffering", false, NULL);

    GstElement *elem = gst_element_factory_make("pulsesink", "VideoPlayerPulseSink");
    Q_ASSERT(elem);

    _textOverlay = gst_element_factory_make("textoverlay", "subs");
    Q_ASSERT(_textOverlay);

    GstElement *subFileSrc = gst_element_factory_make("filesrc", "subssrc");
    g_object_set(subFileSrc, "location", "/tmp/microtube-subtitle.srt", NULL);

    GstElement *subParse = gst_element_factory_make("subparse", "subsparse");
    Q_ASSERT(subParse);

    gst_bin_add_many(GST_BIN(_pipeline), _videoSource, _audioSource, elem, _textOverlay, subFileSrc, subParse, NULL);
    g_signal_connect (_audioSource, "pad-added", G_CALLBACK (cbNewPad), elem);
    g_signal_connect (_videoSource, "pad-added", G_CALLBACK (cbNewPad), _textOverlay);

    gst_element_link(subFileSrc, subParse);
    gst_element_link(subParse, _textOverlay);

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
        emit videoSourceChanged();
    }
}

QUrl VideoPlayer::getAudioSource() const {
    return _audioUrl;
}

void VideoPlayer::setAudioSource(const QUrl& audioSource) {
    if (_audioUrl != audioSource) {
        _audioUrl = audioSource;
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

bool VideoPlayer::pause() {
    return setState(VideoPlayer::StatePaused);
}

bool VideoPlayer::play() {
    if (!_renderer) {
        _renderer = QtCamViewfinderRenderer::create(this);
        if (!_renderer) {
            qmlInfo(this) << "Failed to create viewfinder renderer";
            return false;
        }

        QObject::connect(_renderer, SIGNAL(updateRequested()), this, SLOT(updateRequested()));
    }

    _renderer->resize(QSizeF(width(), height()));

    if (!_pipeline) {
        qmlInfo(this) << "no playbin";
        return false;
    }

    qDebug() << "AudioResource: " << _audio_resource.isAcquired();

//    if (_audioOnlyMode) gst_bin_remove(GST_BIN(_pipeline), _videoSource);
//    else gst_bin_add(GST_BIN(_pipeline), _videoSource);
    gst_bin_add(GST_BIN(_pipeline), _renderer->sinkElement());
    gst_element_link_pads(_textOverlay, "src", _renderer->sinkElement(), "sink");
//    g_object_set(_videoSource, "video-sink", _renderer->sinkElement(), NULL);

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

        _state = state;
        emit stateChanged();

        return true;
    } else if (state == VideoPlayer::StatePlaying) {
        if (_state == VideoPlayer::StateStopped) {
            QString string = _videoUrl.toString();
            QByteArray array = string.toUtf8();
            g_object_set(_videoSource, "uri", array.constData(), NULL);
            string = _audioUrl.toString();
            array = string.toUtf8();
            g_object_set(_audioSource, "uri", array.constData(), NULL);
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

    gchar *debug = NULL;
    GError *err = NULL;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_STREAM_START:
    {
        emit that->durationChanged();
    }
        break;
    case GST_MESSAGE_BUFFERING:
    {
            gint percent = 0;
            gst_message_parse_buffering (msg, &percent);
            qDebug() << "buffering " << percent;
            if (percent < 100) {
                if (that->_state != StateBuffering) {
                    that->setState(StateBuffering);
                }
            } else {
                that->play();
            }
    }
            break;
    case GST_MESSAGE_EOS:
        that->stop();
        break;

    case GST_MESSAGE_ERROR:
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

        break;

    default:
        break;
    }

    return TRUE;
}

void VideoPlayer::cbNewPad(GstElement *element, GstPad *pad, gpointer data)
{
    gchar *name;
    GstElement *other = (GstElement*)data;

    name = gst_pad_get_name (pad);
    g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
    g_free (name);

    g_print ("element %s will be linked to %s\n",
            gst_element_get_name(element),
            gst_element_get_name(other));
    gst_element_link(element, other);
}

void VideoPlayer::updateRequested() {
    update();
}
