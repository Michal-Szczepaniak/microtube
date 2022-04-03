#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QQuickPaintedItem>
#include <AudioResourceQt>
#include <gst/gst.h>

class QtCamViewfinderRenderer;

class VideoPlayer : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(QUrl videoSource READ getVideoSource WRITE setVideoSource NOTIFY videoSourceChanged);
    Q_PROPERTY(QUrl audioSource READ getAudioSource WRITE setAudioSource NOTIFY audioSourceChanged);
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged);
    Q_PROPERTY(qint64 position READ getPosition WRITE setPosition NOTIFY positionChanged);
    Q_PROPERTY(State state READ getState NOTIFY stateChanged);
    Q_ENUMS(State);

public:
    VideoPlayer(QQuickItem *parent = 0);
    ~VideoPlayer();

    virtual void componentComplete();
    virtual void classBegin();

    void paint(QPainter *painter);

    QUrl getVideoSource() const;
    void setVideoSource(const QUrl& videoSource);
    QUrl getAudioSource() const;
    void setAudioSource(const QUrl& audioSource);
    qint64 getDuration() const;
    qint64 getPosition();
    void setPosition(qint64 position);

    Q_INVOKABLE bool pause();
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool seek(qint64 offset);
    Q_INVOKABLE bool stop();
    Q_INVOKABLE void setAudioOnlyMode(bool audioOnlyMode);

    typedef enum {
        StateStopped,
        StatePaused,
        StatePlaying,
        StateBuffering,
    } State;

    State getState() const;

signals:
    void videoSourceChanged();
    void audioSourceChanged();
    void durationChanged();
    void positionChanged();
    void error(const QString& message, int code, const QString& debug);
    void stateChanged();

protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private slots:
    void updateRequested();

private:
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
    static void cbNewPad(GstElement *element, GstPad *pad, gpointer data);

    bool setState(const State& state);

    QtCamViewfinderRenderer *_renderer;
    AudioResourceQt::AudioResource _audio_resource;
    QUrl _videoUrl;
    QUrl _audioUrl;

    GstElement *_pipeline;
    GstElement *_videoSource;
    GstElement *_audioSource;
    GstElement *_textOverlay;
    State _state;
    QTimer *_timer;
    qint64 _pos;
    bool _created;
    bool _audioOnlyMode = false;
};

#endif /* VIDEO_PLAYER_H */
