#include "jsprocesshelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QIODevice>
#include <QEvent>
#include "../parsers/videosparser.h"
#include "../parsers/channelparser.h"
#include "../factories/videofactory.h"

JSProcessHelper::JSProcessHelper() : QObject(), _searchProcess(nullptr), _trendingScrapeProcess(nullptr), _getUrlProcess(nullptr), _getChannelInfoProcess(nullptr)
{
}

void JSProcessHelper::asyncSearch(Search* query)
{
    if (_searchProcess != nullptr) return;
    QJsonDocument optionsDoc(prepareSearchOptions(query));
    _searchInProgress = query;
    _searchProcess = execute("search", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_searchProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::searchDone);
}

void JSProcessHelper::asyncContinueSearch(Search *query)
{
    if (_searchProcess != nullptr) return;
    QJsonDocument optionsDoc(prepareSearchOptions(query));
    _searchInProgress = query;
    _searchProcess = execute("search", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_searchProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::searchContinuationDone);
}

void JSProcessHelper::asyncGetVideoInfo(QString url)
{
    if (_getUrlProcess != nullptr) return;
    _getUrlProcess = execute("videoInfo", {url});
    connect(_getUrlProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotVideoInfoJson);
}

void JSProcessHelper::asyncLoadRecommendedVideos(QString url)
{
    if (_getUrlProcess != nullptr) return;
    _getUrlProcess = execute("videoInfo", {url});
    connect(_getUrlProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotRecommendedVideosInfo);
}

void JSProcessHelper::asyncScrapeTrending(QString category, QString country)
{
    if (_trendingScrapeProcess != nullptr) return;

    QJsonObject options;
    options["geoLocation"] = country;
    options["page"] = category.toLower();
    QJsonDocument optionsDoc(options);
    _trendingScrapeProcess = execute("trending", {optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_trendingScrapeProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::trendingScrapeDone);
}

void JSProcessHelper::asyncGetChannelInfo(QString channelId)
{
    if (_getChannelInfoProcess != nullptr) return;
    _getChannelInfoProcess = execute("channelInfo", {channelId});
    connect(_getChannelInfoProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotChannelInfoJson);
}

std::vector<std::unique_ptr<Video> > JSProcessHelper::getTrendingVideos()
{
    return move(_trendingVideos);
}

std::vector<std::unique_ptr<Video> > JSProcessHelper::getRecommendedVideos()
{
    return move(_recommendedVideos);
}

std::unique_ptr<Video> JSProcessHelper::getVideoInfo()
{
    return move(_videoInfo);
}

Author JSProcessHelper::getChannelInfo()
{
    return _channelInfo;
}

QProcess* JSProcessHelper::execute(QString script, QStringList args)
{
    QString appPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/js/";
    QProcess* process = new QProcess();
    process->setWorkingDirectory(appPath);
    QStringList params = {appPath + script + ".js"};
    params << args;
    process->start("node", params, QIODevice::OpenModeFlag::ReadWrite);
    return process;
}

QJsonObject JSProcessHelper::prepareSearchOptions(Search *query)
{
    QJsonObject options;
    options["safeSearch"] = query->safeSearch;
    options["pages"] = query->pages;
    if (!query->continuation.empty()) {
        options["continuation"] = query->continuation;
    }
    return options;
}

void JSProcessHelper::searchDone(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_searchProcess->readAll());
    _searchInProgress->continuation = response.object()["continuation"].toArray();
    _searchInProgress->items = VideosParser::parse(response.object()["items"].toArray());

    emit searchFinished(false);

    QProcess* process = _searchProcess;
    _searchProcess = nullptr;
    process->deleteLater();
}

void JSProcessHelper::searchContinuationDone(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_searchProcess->readAll());
    _searchInProgress->continuation = response.object()["continuation"].toArray();
    _searchInProgress->items = VideosParser::parse(response.object()["items"].toArray());
    emit searchFinished(true);

    QProcess* process = _searchProcess;
    _searchProcess = nullptr;
    process->deleteLater();
}

void JSProcessHelper::gotVideoInfoJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getUrlProcess->readAll());
    QHash<int, QString> formats;
    for (const QJsonValue &jsonFormat : response.object()["formats"].toArray()) {
        const QJsonObject format = jsonFormat.toObject();
        formats[format["itag"].toInt()] = format["url"].toString();
    }

    auto obj = response.object();
    _videoInfo = VideoFactory::fromVideoInfoJson(response.object());
    emit gotVideoInfo(formats);

    QProcess* process = _getUrlProcess;
    _getUrlProcess = nullptr;
    process->deleteLater();
}

void JSProcessHelper::gotRecommendedVideosInfo(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getUrlProcess->readAll());
    QJsonObject obj = response.object();

    _recommendedVideos = VideosParser::parseRecommended(response.object()["related_videos"].toArray());

    emit gotRecommendedVideos();

    QProcess* process = _getUrlProcess;
    _getUrlProcess = nullptr;
    process->deleteLater();
}

void JSProcessHelper::trendingScrapeDone(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_trendingScrapeProcess->readAll());

    _trendingVideos = VideosParser::parseTrending(response.array());

    emit gotTrendingVideos();

    QProcess* process = _trendingScrapeProcess;
    _trendingScrapeProcess = nullptr;
    process->deleteLater();
}

void JSProcessHelper::gotChannelInfoJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getChannelInfoProcess->readAll());
    _channelInfo = ChannelParser::parseAuthorInfo(response.object());

    emit gotChannelInfo();

    QProcess* p = _getChannelInfoProcess;
    _getChannelInfoProcess = nullptr;
    p->deleteLater();
}
