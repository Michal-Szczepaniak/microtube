#ifndef JSPROCESSHELPER_H
#define JSPROCESSHELPER_H

#include <QString>
#include <QProcess>
#include "../entities/search.h"
#include "../converters/xmltosrtconverter.h"

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
    std::vector<std::unique_ptr<Video>> getTrendingVideos();
    std::vector<std::unique_ptr<Video>> getRecommendedVideos();
    std::unique_ptr<Video> getVideoInfo();
    Author getChannelInfo();

signals:
    void searchFinished(bool continuation);
    void gotVideoInfo(QHash<int, QString> formats);
    void gotTrendingVideos();
    void gotRecommendedVideos();
    void gotChannelInfo();

private:
    static QProcess* execute(QString script, QStringList args);
    static QJsonObject prepareSearchOptions(Search *query);
    void searchDone(int exitStatus);
    void searchContinuationDone(int exitStatus);
    void gotVideoInfoJson(int exitStatus);
    void gotRecommendedVideosInfo(int exitStatus);
    void trendingScrapeDone(int exitStatus);
    void gotChannelInfoJson(int exitStatus);

    QProcess* _searchProcess;
    QProcess* _trendingScrapeProcess;
    Search* _searchInProgress;
    QProcess* _getUrlProcess;
    QProcess* _getChannelInfoProcess;

    std::vector<std::unique_ptr<Video>> _trendingVideos{};
    std::vector<std::unique_ptr<Video>> _recommendedVideos{};
    std::unique_ptr<Video> _videoInfo{};
    Author _channelInfo{};
};

#endif // JSPROCESSHELPER_H
