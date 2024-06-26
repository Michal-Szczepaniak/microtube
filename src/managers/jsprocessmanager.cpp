﻿#include <managers/jsprocessmanager.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QIODevice>
#include <QEvent>
#include <QFile>
#include "src/factories/videofactory.h"
#include "src/factories/authorfactory.h"
#include "factories/commentfactory.h"
#include <repositories/authorrepository.h>

JSProcessManager::JSProcessManager() :
    QObject(),
    _searchProcess(nullptr),
    _trendingProcess(nullptr),
    _getUrlProcess(nullptr),
    _getChannelInfoProcess(nullptr),
    _getChannelVideosProcess(nullptr),
    _getCommentsProcess(nullptr),
    _getCommentRepliesProcess(nullptr),
    _playlistProcess(nullptr)
{
}

void JSProcessManager::asyncSearch(Search* query)
{
    if (_searchProcess != nullptr) return;

    _searchContinuation = {};
    QJsonDocument optionsDoc(prepareSearchOptions(query, nullptr));
    _searchProcess = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_searchProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::searchDone);
}

void JSProcessManager::asyncContinueSearch(Search* query)
{
    if (_searchProcess != nullptr || _searchContinuation.empty()) return;

    QJsonDocument optionsDoc(prepareSearchOptions(query, &_searchContinuation));
    _searchProcess = execute("unified", {"", optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_searchProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::searchDone);
}

bool JSProcessManager::asyncGetVideoInfo(QString url)
{
    if (_getUrlProcess != nullptr) return false;

    _getUrlProcess = execute("videoInfo", {url});
    connect(_getUrlProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotVideoInfoJson);
    return true;
}

void JSProcessManager::asyncLoadRecommendedVideos(QString url)
{
    if (_getUrlProcess != nullptr) return;

    _getUrlProcess = execute("videoInfo", {url});
    connect(_getUrlProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotRecommendedVideosInfo);
}

void JSProcessManager::asyncGetTrending(Search* query)
{
    if (_trendingProcess != nullptr) return;

    QJsonDocument optionsDoc(prepareSearchOptions(query, nullptr));
    qDebug() << query->query << " " << optionsDoc.toJson(QJsonDocument::Compact);
    _trendingProcess = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_trendingProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::trendingScrapeDone);
}

void JSProcessManager::asyncGetChannelInfo(Search* query)
{
    if (_getChannelInfoProcess != nullptr) return;

    QJsonDocument optionsDoc(prepareSearchOptions(query, nullptr));
    _getChannelInfoProcess = execute("channelInfo", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_getChannelInfoProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotChannelInfoJson);
}

void JSProcessManager::asyncLoadChannelVideos(Search* query)
{
    if (_getChannelVideosProcess != nullptr) return;

    _channelVideosContinuation = {};
    _loadChannelVideosLastAuthorId = query->query;
    QJsonDocument optionsDoc(prepareSearchOptions(query, nullptr));
    _getChannelVideosProcess = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_getChannelVideosProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotChannelVideosJson);
}

void JSProcessManager::asyncContinueChannelVideos(Search* query)
{
    if (_getChannelVideosProcess != nullptr || _channelVideosContinuation.empty()) return;

    QJsonDocument optionsDoc(prepareSearchOptions(query, &_channelVideosContinuation));
    qDebug() << "Continue: " << optionsDoc.toJson(QJsonDocument::Compact);
    _getChannelVideosProcess = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_getChannelVideosProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotChannelVideosJson);
}

void JSProcessManager::asyncGetComments(QString videoId)
{
    if (_getCommentsProcess != nullptr) return;
    _getCommentsProcess = execute("comments", {videoId, "{}"});
    _commentsContinuation = {};
    connect(_getCommentsProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotCommentsJson);
}

void JSProcessManager::asyncGetCommentsContinuation()
{
    if (_getCommentsProcess != nullptr || _commentsContinuation.empty()) return;

    QJsonDocument d(_commentsContinuation);
    _getCommentsProcess = execute("comments", {"", d.toJson(QJsonDocument::Compact)});
    connect(_getCommentsProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotCommentsJson);
}

void JSProcessManager::asyncGetCommentReplies(QJsonObject continuationData)
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
    connect(_getCommentRepliesProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotCommentRepliesJson);
}

void JSProcessManager::asyncLoadPlaylist(Search* query)
{
    if (_playlistProcess != nullptr) return;

    _playlistContinuation = {};
    QJsonDocument optionsDoc(prepareSearchOptions(query, nullptr));
    _playlistProcess = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_playlistProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotPlaylistJson);
}

void JSProcessManager::asyncContinuePlaylist(Search* query)
{
    if (_playlistProcess != nullptr || _playlistContinuation.empty()) return;

    QJsonDocument optionsDoc(prepareSearchOptions(query, &_playlistContinuation));
    _playlistProcess = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    connect(_playlistProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &JSProcessManager::gotPlaylistJson);
}

SearchResults JSProcessManager::loadChannelVideos(Search* query, bool full)
{
    QJsonDocument optionsDoc(prepareSearchOptions(query, nullptr));
    QProcess* process = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    process->waitForFinished();

    QJsonDocument response = QJsonDocument::fromJson(process->readAll());
    SearchResults results = VideosParser::parseChanelVideos(response.object()["items"].toArray());
    QJsonObject continuation = response.object()["continuation"].toObject();

    process->deleteLater();

    while (!continuation.empty() && full) {
        QJsonDocument optionsDoc(prepareSearchOptions(query, &continuation));
        QProcess* process = execute("unified", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
        process->waitForFinished();

        QJsonDocument response = QJsonDocument::fromJson(process->readAll());
        SearchResults tmpResults = VideosParser::parseChanelVideos(response.object()["items"].toArray());
        std::move(tmpResults.begin(), tmpResults.end(), std::back_inserter(results));

        continuation = response.object()["continuation"].toObject();
        process->deleteLater();
    }

    return results;
}

std::unique_ptr<Video> JSProcessManager::getBasicVideoInfo(QString url)
{
    QProcess* process = execute("basicVideoInfo", {url});
    process->waitForFinished();

    QJsonDocument response = QJsonDocument::fromJson(process->readAll());
    return VideoFactory::fromVideoInfoJson(response.object());
}

Author JSProcessManager::fetchChannelInfo(Search *query)
{
    QJsonDocument optionsDoc(prepareSearchOptions(query, nullptr));
    QProcess* process = execute("channelInfo", {query->query, optionsDoc.toJson(QJsonDocument::Compact)});
    process->waitForFinished();

    QJsonDocument response = QJsonDocument::fromJson(process->readAll());
    if (response.object().isEmpty()) {
        qDebug() << "Response is empty!";
    }

    return AuthorFactory::fromChannelInfoJson(response.object());
}

SearchResults JSProcessManager::getSearchVideos()
{
    return move(_searchVideos);
}

SearchResults JSProcessManager::getTrendingVideos()
{
    return move(_trendingVideos);
}

SearchResults JSProcessManager::getRecommendedVideos()
{
    return move(_recommendedVideos);
}

SearchResults JSProcessManager::getChannelVideos()
{
    return move(_channelVideos);
}

SearchResults JSProcessManager::getPlaylistVideos()
{
    return move(_playlistVideos);
}

std::vector<Comment> JSProcessManager::getComments()
{
    return _comments;
}

std::vector<Comment> JSProcessManager::getCommentReplies()
{
    return _commentReplies;
}

std::unique_ptr<Video> JSProcessManager::getVideoInfo()
{
    return move(_videoInfo);
}

Author JSProcessManager::getChannelInfo()
{
    return _channelInfo;
}

QProcess* JSProcessManager::execute(QString script, QStringList args)
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

QJsonObject JSProcessManager::prepareSearchOptions(Search *query, QJsonObject *continuation)
{
    QJsonObject options;

    options["type"] = query->type;
    options["language"] = query->language;
    options["country"] = query->country;
    options["safeSearch"] = query->safeSearch;

    switch (query->uploadDateFilter) {
    case Search::AllUploadDates:
        options["uploadDateFilter"] = "all";
        break;
    case Search::Hour:
        options["uploadDateFilter"] = "hour";
        break;
    case Search::Today:
        options["uploadDateFilter"] = "today";
        break;
    case Search::Week:
        options["uploadDateFilter"] = "week";
        break;
    case Search::Month:
        options["uploadDateFilter"] = "month";
        break;
    case Search::Year:
        options["uploadDateFilter"] = "year";
        break;
    }

    switch (query->durationFilter) {
    case Search::AllDurations:
        options["durationFilter"] = "all";
        break;
    case Search::Short:
        options["durationFilter"] = "short";
        break;
    case Search::Medium:
        options["durationFilter"] = "medium";
        break;
    case Search::Long:
        options["durationFilter"] = "long";
        break;
    }

    switch (query->sortBy) {
    case Search::Relevance:
        options["sortBy"] = "relevance";
        break;
    case Search::Rating:
        options["sortBy"] = "rating";
        break;
    case Search::UploadDate:
        options["sortBy"] = "upload_date";
        break;
    case Search::ViewCount:
        options["sortBy"] = "view_count";
        break;
    }

    if (continuation != nullptr) {
        options["continuation"] = *continuation;
    }

    return options;
}

void JSProcessManager::searchDone(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_searchProcess->readAll());
    bool isContinuation = !_searchContinuation.empty();
    _searchContinuation = response.object()["continuation"].toObject();
    _searchVideos = VideosParser::parse(response.object()["items"].toArray());

    emit searchFinished(isContinuation);

    QProcess* process = _searchProcess;
    _searchProcess = nullptr;
    process->deleteLater();
}

void JSProcessManager::gotVideoInfoJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getUrlProcess->readAll());
    QHash<int, QString> formats;
    for (const QJsonValue &jsonFormat : response.object()["formats"].toArray()) {
        const QJsonObject format = jsonFormat.toObject();
        if (!format.contains("audioTrack")) {
            formats[format["itag"].toInt()] = format["url"].toString();
        }
    }

    auto obj = response.object();
    _videoInfo = VideoFactory::fromVideoInfoJson(response.object());
    emit gotVideoInfo(formats);

    QProcess* process = _getUrlProcess;
    _getUrlProcess = nullptr;
    process->deleteLater();
}

void JSProcessManager::gotRecommendedVideosInfo(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getUrlProcess->readAll());
    QJsonObject obj = response.object();

    _recommendedVideos = VideosParser::parseRecommended(response.object()["related_videos"].toArray());

    emit gotRecommendedVideos();

    QProcess* process = _getUrlProcess;
    _getUrlProcess = nullptr;
    process->deleteLater();
}

void JSProcessManager::trendingScrapeDone(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_trendingProcess->readAll());

    _trendingVideos = VideosParser::parseTrending(response.object()["items"].toArray());

    emit gotTrendingVideos();

    QProcess* process = _trendingProcess;
    _trendingProcess = nullptr;
    process->deleteLater();
}

void JSProcessManager::gotChannelInfoJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getChannelInfoProcess->readAll());
    _channelInfo = AuthorFactory::fromChannelInfoJson(response.object());

    emit gotChannelInfo();

    QProcess* p = _getChannelInfoProcess;
    _getChannelInfoProcess = nullptr;
    p->deleteLater();
}

void JSProcessManager::gotChannelVideosJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_getChannelVideosProcess->readAll());
    bool isContinuation = !_channelVideosContinuation.empty();
    _channelVideosContinuation = response.object()["continuation"].toObject();
    _channelVideos = VideosParser::parseChanelVideos(response.object()["items"].toArray());

    AuthorRepository authorRepository;
    Author author = authorRepository.getOneByChannelId(_loadChannelVideosLastAuthorId);

    for (SearchResult &video : _channelVideos) {
        if (std::holds_alternative<std::unique_ptr<Video>>(video)) {
            std::get<std::unique_ptr<Video>>(video)->author = author;
        } else if (std::holds_alternative<Playlist>(video)) {
            std::get<Playlist>(video).author = author;
        }
    }

    emit gotChannelVideos(isContinuation);

    QProcess* process = _getChannelVideosProcess;
    _getChannelVideosProcess = nullptr;
    process->deleteLater();
}

void JSProcessManager::gotCommentsJson(int exitStatus)
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

void JSProcessManager::gotCommentRepliesJson(int exitStatus)
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

void JSProcessManager::gotPlaylistJson(int exitStatus)
{
    QJsonDocument response = QJsonDocument::fromJson(_playlistProcess->readAll());
    bool isContinuation = !_playlistContinuation.empty();
    _playlistVideos = VideosParser::parsePlaylist(response.object()["items"].toArray());
    _playlistContinuation = response.object()["continuation"].toObject();

    emit gotPlaylist(isContinuation);

    QProcess* process = _playlistProcess;
    _playlistProcess = nullptr;
    process->deleteLater();
}
