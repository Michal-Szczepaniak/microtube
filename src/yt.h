#ifndef YT_H
#define YT_H

#include <QObject>
#include <QUrl>
#include <QQmlContext>
#include "playlistmodel.h"
#include "channelmodel.h"
#include "searchparams.h"
#include "ytvideo.h"

class YT : public QObject
{
    Q_OBJECT

public:
    explicit YT(QObject *parent = nullptr);
    void registerObjectsInQml(QQmlContext* context);
    void setVideoSource(VideoSource *videoSource, bool addToHistory = true, bool back = false);
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void watchChannel(const QString &channelId);
    void watch(SearchParams *searchParams);
    const QVector<VideoSource*> & getHistory() { return history; }
    int getHistoryIndex();
    PlaylistModel* getPlaylistModel() { return playlistModel; }
    const QString &getCurrentVideoId();
    void updateSubscriptionAction(Video *video, bool subscribed);
    Q_INVOKABLE void setDefinition(QString definition);
    Q_INVOKABLE void toggleSubscription();
    Q_INVOKABLE void updateQuery();
    Q_INVOKABLE void itemActivated(int index);
private:
    SearchParams* getSearchParams();
    void searchAgain();

    bool stopped;
    QTimer *errorTimer;
    Video *skippedVideo;
    QString currentVideoId;

    PlaylistModel* playlistModel;
    ChannelModel* channelModel;
    QVector<VideoSource*> history;
};

#endif // YT_H
