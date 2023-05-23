#ifndef JSPROCESSHELPER_H
#define JSPROCESSHELPER_H

#include <QString>
#include <QProcess>
#include <QJsonObject>
#include "entities/comment.h"
#include "entities/search.h"
#include "converters/xmltosrtconverter.h"

class JSProcessHelper : public QObject
{
    Q_OBJECT
public:
    JSProcessHelper();

    void asyncSearch(Search* query);
    void asyncContinueSearch(Search* query);
    void asyncGetVideoInfo(QString url);
    void asyncLoadRecommendedVideos(QString url);
    void asyncScrapeTrending(QString category, QString country);
    void asyncGetChannelInfo(QString channelId);
    void asyncLoadChannelVideos(QString channelId);
    void asyncContinueChannelVideos();
    void asyncGetComments(QString videoId);
    void asyncGetCommentsContinuation();
    void asyncGetCommentReplies(QJsonObject continuationData);
    std::vector<std::unique_ptr<Video>> loadChannelVideos(QString channelId);
    std::unique_ptr<Video> getBasicVideoInfo(QString url);
    Author fetchChannelInfo(QString channelId);
    std::vector<std::unique_ptr<Video>> getTrendingVideos();
    std::vector<std::unique_ptr<Video>> getRecommendedVideos();
    std::vector<std::unique_ptr<Video>> getChannelVideos();
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

private slots:
    static QProcess* execute(QString script, QStringList args);
    static QJsonObject prepareSearchOptions(Search *query);
    void searchDone(int exitStatus);
    void searchContinuationDone(int exitStatus);
    void gotVideoInfoJson(int exitStatus);
    void gotRecommendedVideosInfo(int exitStatus);
    void trendingScrapeDone(int exitStatus);
    void gotChannelInfoJson(int exitStatus);
    void gotChannelVideosJson(int exitStatus);
    void gotCommentsJson(int exitStatus);
    void gotCommentRepliesJson(int exitStatus);

private:
    QProcess* _searchProcess;
    QProcess* _trendingScrapeProcess;
    Search* _searchInProgress;
    QProcess* _getUrlProcess;
    QProcess* _getChannelInfoProcess;
    QProcess* _getChannelVideosProcess;
    QProcess* _getCommentsProcess;
    QProcess* _getCommentRepliesProcess;
    QJsonObject _channelVideosContinuation{};
    QJsonObject _commentsContinuation{};
    QString _commentRepliesContinuation{};
    QString _loadChannelVideosLastAuthorId{};

    std::vector<std::unique_ptr<Video>> _trendingVideos{};
    std::vector<std::unique_ptr<Video>> _recommendedVideos{};
    std::vector<std::unique_ptr<Video>> _channelVideos{};
    std::vector<Comment> _comments{};
    std::vector<Comment> _commentReplies{};
    std::unique_ptr<Video> _videoInfo{};
    Author _channelInfo{};
};

#endif // JSPROCESSHELPER_H
