#ifndef YT_H
#define YT_H

#include <QObject>
#include <QUrl>
#include <QQmlContext>
#include "playlistmodel.h"
#include "searchparams.h"

class YT : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl streamUrl READ getStreamUrl WRITE setStreamUrl NOTIFY streamUrlChanged)

public:
    explicit YT(QObject *parent = nullptr);
    QUrl getStreamUrl() { return m_streamUrl; }
    void setStreamUrl(QUrl streamUrl) { m_streamUrl = streamUrl; }
    void registerObjectsInQml(QQmlContext* context);
    void setVideoSource(VideoSource *videoSource, bool addToHistory = true, bool back = false);
    Q_INVOKABLE void search(QString query);
    void watch(SearchParams *searchParams);
    const QVector<VideoSource*> & getHistory() { return history; }
    int getHistoryIndex();
    PlaylistModel* getPlaylistModel() { return playlistModel; }
    const QString &getCurrentVideoId();
    void updateSubscriptionAction(Video *video, bool subscribed);

signals:
    void streamUrlChanged(const QUrl &streamUrl);

public slots:
    void gotStreamUrl(const QUrl &streamUrl);

private:
    SearchParams* getSearchParams();
    void searchAgain();

    bool stopped;
    QTimer *errorTimer;
    Video *skippedVideo;
    QString currentVideoId;

    QUrl m_streamUrl;
    PlaylistModel* playlistModel;
    QVector<VideoSource*> history;
};

#endif // YT_H
