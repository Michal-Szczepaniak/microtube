#include "jsprocesshelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QIODevice>
#include <QEvent>
#include <QFile>
#include "src/parsers/videosparser.h"
#include "src/factories/videofactory.h"
#include "src/factories/authorfactory.h"
#include "factories/commentfactory.h"
#include <repositories/authorrepository.h>

JSProcessHelper::JSProcessHelper() :
    QObject(),
    _searchProcess(nullptr),
    _trendingScrapeProcess(nullptr),
    _getUrlProcess(nullptr),
    _getChannelInfoProcess(nullptr),
    _getChannelVideosProcess(nullptr),
    _getCommentsProcess(nullptr),
    _getCommentRepliesProcess(nullptr)
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
    options["location"] = country;
    options["lang"] = "en";
    options["page"] = category;
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

void JSProcessHelper::asyncLoadChannelVideos(QString channelId)
{
    if (_getChannelVideosProcess != nullptr) return;
    _getChannelVideosProcess = execute("channelVideos", {channelId, "{}"});
    _channelVideosContinuation = {};
    _loadChannelVideosLastAuthorId = channelId;
    connect(_getChannelVideosProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotChannelVideosJson);
}

void JSProcessHelper::asyncContinueChannelVideos()
{
    if (_getChannelVideosProcess != nullptr || _channelVideosContinuation.empty()) return;

    QJsonDocument d(_channelVideosContinuation);
    _getChannelVideosProcess = execute("channelVideos", {"", d.toJson(QJsonDocument::Compact)});
    connect(_getChannelVideosProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotChannelVideosJson);
}

void JSProcessHelper::asyncGetComments(QString videoId)
{
    if (_getCommentsProcess != nullptr) return;
    _getCommentsProcess = execute("comments", {videoId, "{}"});
    _commentsContinuation = {};
    connect(_getCommentsProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotCommentsJson);
}

void JSProcessHelper::asyncGetCommentsContinuation()
{
    if (_getCommentsProcess != nullptr || _commentsContinuation.empty()) return;

    QJsonDocument d(_commentsContinuation);
    _getCommentsProcess = execute("comments", {"", d.toJson(QJsonDocument::Compact)});
    connect(_getCommentsProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotCommentsJson);
}

void JSProcessHelper::asyncGetCommentReplies(QJsonObject continuationData)
{
    if (_getCommentRepliesProcess != nullptr) return;

    QString continuationJSON;
    if (continuationData.contains("button")) {
        QJsonDocument d(continuationData["button"].toObject());
        continuationJSON = d.toJson(QJsonDocument::Compact);
    } else {
        QJsonDocument d(continuationData);
        continuationJSON = d.toJson(QJsonDocument::Compact);
    }

    _getCommentRepliesProcess = execute("commentReplies", {continuationJSON});
    connect(_getCommentRepliesProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessHelper::gotCommentRepliesJson);
}

std::vector<std::unique_ptr<Video>> JSProcessHelper::loadChannelVideos(QString channelId)
{
    QProcess* process = execute("channelVideos", {channelId, "{}"});
    process->waitForFinished();

    QJsonDocument response = QJsonDocument::fromJson(process->readAll());
    return VideosParser::parseChanelVideos(response.object()["items"].toArray());
}

std::unique_ptr<Video> JSProcessHelper::getBasicVideoInfo(QString url)
{
    QProcess* process = execute("basicVideoInfo", {url});
    process->waitForFinished();

    QJsonDocument response = QJsonDocument::fromJson(process->readAll());
    return VideoFactory::fromVideoInfoJson(response.object());
}

Author JSProcessHelper::fetchChannelInfo(QString channelId)
{
    QProcess* process = execute("channelInfo", {channelId});
    process->waitForFinished();

    QJsonDocument response = QJsonDocument::fromJson(process->readAll());
    if (response.object().isEmpty()) {
        qDebug() << "Response is empty!";
    }
    return AuthorFactory::fromChannelInfoJson(response.object());
}

std::vector<std::unique_ptr<Video> > JSProcessHelper::getTrendingVideos()
{
    return move(_trendingVideos);
}

std::vector<std::unique_ptr<Video> > JSProcessHelper::getRecommendedVideos()
{
    return move(_recommendedVideos);
}

std::vector<std::unique_ptr<Video> > JSProcessHelper::getChannelVideos()
{
    return move(_channelVideos);
}

std::vector<Comment> JSProcessHelper::getComments()
{
    return _comments;
}

std::vector<Comment> JSProcessHelper::getCommentReplies()
{
    return _commentReplies;
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
    Q_ASSERT_X(QFile::exists(appPath + script + ".js"), "Missing js file", script.toStdString().c_str());
    QStringList params = {appPath + script + ".js"};
    params << args;
    process->start("node18", params, QIODevice::OpenModeFlag::ReadWrite);
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
    _channelInfo = AuthorFactory::fromChannelInfoJson(response.object());

    emit gotChannelInfo();

    QProcess* p = _getChannelInfoProcess;
    _getChannelInfoProcess = nullptr;
    p->deleteLater();
}

void JSProcessHelper::gotChannelVideosJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getChannelVideosProcess->readAll());
    bool isContinuation = !_channelVideosContinuation.empty();
    _channelVideosContinuation = response.object()["continuation"].toObject();
    _channelVideos = VideosParser::parseChanelVideos(response.object()["items"].toArray());

    AuthorRepository authorRepository;
    Author author = authorRepository.getOneByChannelId(_loadChannelVideosLastAuthorId);

    for (std::unique_ptr<Video> &video : _channelVideos) {
        video->author = author;
    }

    emit gotChannelVideos(isContinuation);

    QProcess* process = _getChannelVideosProcess;
    _getChannelVideosProcess = nullptr;
    process->deleteLater();
}

void JSProcessHelper::gotCommentsJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getCommentsProcess->readAll());
    QJsonObject resp = response.object();
    bool isContinuation = _commentsContinuation.length() != 0;
    _commentsContinuation = response.object()["continuation"].toObject();
    QJsonArray comments = response.object()["items"].toArray();

    _comments.clear();
    for (const QJsonValue &item : comments) {
        if (item.isUndefined()) break;
        QJsonObject jsonComment = item.toObject();
        _comments.push_back(CommentFactory::fromJson(jsonComment));
    }

    emit gotComments(_commentsContinuation.length() != 0, isContinuation);

    QProcess* p = _getCommentsProcess;
    _getCommentsProcess = nullptr;
    p->deleteLater();
}

void JSProcessHelper::gotCommentRepliesJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getCommentRepliesProcess->readAll());
    QJsonObject resp = response.object();
    QJsonObject continuation = response.object()["continuation"].toObject();
    QJsonArray comments = response.object()["items"].toArray();

    _commentReplies.clear();
    for (const QJsonValue &item : comments) {
        if (item.isUndefined()) break;
        QJsonObject jsonComment = item.toObject();
        _commentReplies.push_back(CommentFactory::fromReplyJson(jsonComment));
    }

    emit gotCommentReplies(continuation);

    QProcess* p = _getCommentRepliesProcess;
    _getCommentRepliesProcess = nullptr;
    p->deleteLater();
}
