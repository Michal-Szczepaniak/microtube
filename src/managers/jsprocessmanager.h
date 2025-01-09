#ifndef JSPROCESSMANAGER_H
#define JSPROCESSMANAGER_H

#include <QString>
#include <QProcess>
#include <QJsonObject>
#include "entities/comment.h"
#include "entities/search.h"
#include "converters/xmltosrtconverter.h"
#include "src/parsers/videosparser.h"

class JSProcessManager : public QObject
{
    Q_OBJECT
public:
    JSProcessManager();

    void asyncSearch(Search* query);
    void asyncContinueSearch(Search* query);
    bool asyncGetVideoInfo(QString url);
    void asyncLoadRecommendedVideos(QString url);
    void asyncGetTrending(Search* query);
    void asyncGetChannelInfo(Search* query);
    void asyncLoadChannelVideos(Search* query);
    void asyncContinueChannelVideos(Search* query);
    void asyncGetComments(QString videoId);
    void asyncGetCommentsContinuation();
    void asyncGetCommentReplies(QJsonObject continuationData);
    void asyncLoadPlaylist(Search* query);
    void asyncContinuePlaylist(Search* query);
    bool hasSearchContinuation() const;
    bool hasVideosContinuation() const;
    bool hasCommentsContinuation() const;
    bool hasCommentRepliesContinuation() const;
    bool hasPlaylistContinuation() const;
    SearchResults loadChannelVideos(Search* query, bool full = false);
    SearchResults aggregateSubscription(Search* query, bool includeVideos, bool includeLivestreams);
    std::unique_ptr<Video> getBasicVideoInfo(QString url);
    Author fetchChannelInfo(Search *query);
    SearchResults getSearchVideos();
    SearchResults getTrendingVideos();
    SearchResults getRecommendedVideos();
    SearchResults getChannelVideos();
    SearchResults getPlaylistVideos();
    std::vector<Comment> getComments();
    std::vector<Comment> getCommentReplies();
    std::unique_ptr<Video> getVideoInfo();
    Author getChannelInfo();

signals:
    void searchFinished(bool continuation);
    void gotVideoInfo(QHash<int, QString> formats);
    void gotTrendingVideos();
    void gotRecommendedVideos();
    void gotChannelInfo();
    void gotChannelVideos(bool isContinuation);
    void gotComments(bool canContinue, bool isContinuation);
    void gotCommentReplies(QJsonObject continuation);
    void gotPlaylist(bool isContinuation);

private slots:
    static QProcess* execute(QString script, QStringList args);
    QJsonObject prepareSearchOptions(Search *query, QJsonObject *continuation);
    void searchDone(int exitStatus);
    void gotVideoInfoJson(int exitStatus);
    void gotRecommendedVideosInfo(int exitStatus);
    void trendingScrapeDone(int exitStatus);
    void gotChannelInfoJson(int exitStatus);
    void gotChannelVideosJson(int exitStatus);
    void gotCommentsJson(int exitStatus);
    void gotCommentRepliesJson(int exitStatus);
    void gotPlaylistJson(int exitStatus);

private:
    QProcess* _searchProcess;
    QProcess* _trendingProcess;
    QProcess* _getUrlProcess;
    QProcess* _getChannelInfoProcess;
    QProcess* _getChannelVideosProcess;
    QProcess* _getCommentsProcess;
    QProcess* _getCommentRepliesProcess;
    QProcess* _playlistProcess;
    QJsonObject _searchContinuation{};
    QJsonObject _channelVideosContinuation{};
    QJsonObject _commentsContinuation{};
    QJsonObject _playlistContinuation{};
    QString _commentRepliesContinuation{};
    QString _loadChannelVideosLastAuthorId{};

    SearchResults _searchVideos{};
    SearchResults _trendingVideos{};
    SearchResults _recommendedVideos{};
    SearchResults _channelVideos{};
    SearchResults _playlistVideos{};
    std::vector<Comment> _comments{};
    std::vector<Comment> _commentReplies{};
    std::unique_ptr<Video> _videoInfo{};
    Author _channelInfo{};
};

#endif // JSPROCESSMANAGER_H
