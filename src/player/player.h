#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QQuickPaintedItem>
#include <AudioResourceQt>
#include <gst/gst.h>
#include <QTimer>
#include "renderernemo.h"

class QtCamViewfinderRenderer;
using Projection = RendererNemo::Projection;

class VideoPlayer : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(QUrl videoSource READ getVideoSource WRITE setVideoSource NOTIFY videoSourceChanged);
    Q_PROPERTY(QUrl audioSource READ getAudioSource WRITE setAudioSource NOTIFY audioSourceChanged);
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged);
    Q_PROPERTY(qint64 position READ getPosition WRITE setPosition NOTIFY positionChanged);
    Q_PROPERTY(State state READ getState NOTIFY stateChanged);
    Q_PROPERTY(QString subtitle READ getSubtitle WRITE setSubtitle NOTIFY subtitleChanged);
    Q_PROPERTY(QString displaySubtitle READ getDisplaySubtitle WRITE setDisplaySubtitle NOTIFY displaySubtitleChanged);
    Q_PROPERTY(Projection projection READ getProjection WRITE setProjection NOTIFY projectionChanged);
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
    QString getSubtitle() const;
    void setSubtitle(QString subtitle);
    QString getDisplaySubtitle() const;
    void setDisplaySubtitle(QString subtitle);
    Projection getProjection() const;
    void setProjection(Projection projection);

    Q_INVOKABLE bool pause();
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool seek(qint64 offset);
    Q_INVOKABLE bool stop();
    Q_INVOKABLE void setAudioOnlyMode(bool audioOnlyMode);
    Q_INVOKABLE bool setPlaybackSpeed(double speed);

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
    void subtitleChanged();
    void displaySubtitleChanged();
    void projectionChanged();

protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private slots:
    void updateRequested();
    void updateBufferingState(int percent, QString name);

private:
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
    static void cbNewPad(GstElement *element, GstPad *pad, gpointer data);
    static void cbNewVideoPad(GstElement *element, GstPad *pad, gpointer data);
    static GstFlowReturn cbNewSample(GstElement *sink, gpointer *data);

    bool setState(const State& state);

    RendererNemo *_renderer;
    AudioResourceQt::AudioResource _audioResource;
    QUrl _videoUrl;
    QUrl _audioUrl;

    GstElement *_pipeline;
    GstElement *_videoSource;
    GstElement *_audioSource;
    GstElement *_pulsesink;
    GstElement *_subParse;
    GstElement *_appSink;
    GstElement *_subSource;
    GstElement *_scaletempo;
    State _state;
    QTimer *_timer;
    qint64 _pos;
    double _playbackSpeed;
    bool _created;
    bool _audioOnlyMode = false;
    QString _currentSubtitle;
    QString _subtitle;
    quint64 _subtitleEnd;
    QHash<QString, int> _bufferingProgress;
    Projection _projection = Projection::Flat;
};

#endif /* VIDEO_PLAYER_H */
