#include "videosparser.h"
#include <QDebug>
#include <QJsonObject>
#include "factories/videofactory.h"
#include "factories/authorfactory.h"
#include <factories/playlistfactory.h>

SearchResults VideosParser::parse(const QJsonArray videos)
{
    SearchResults result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) continue;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString().toLower();
        if (type == "video" || type == "reelitem") {
            result.push_back(VideoFactory::fromJson(jsonVideo));
        } else if (type == "channel") {
            result.push_back(AuthorFactory::fromSearchJson(jsonVideo));
        } else if (type == "playlist" || type == "gridplaylist") {
            result.push_back(PlaylistFactory::fromJson(jsonVideo));
        } else {
            qDebug() << type;
        }
    }

    return result;
}

SearchResults VideosParser::parseTrending(const QJsonArray videos)
{
    SearchResults result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) continue;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString().toLower();
        if (type == "video") {
            result.push_back(VideoFactory::fromTrendingJson(jsonVideo));
        } else {
            qDebug() << type;
        }
    }

    return result;
}

SearchResults VideosParser::parseRecommended(const QJsonArray videos)
{
    SearchResults result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) continue;
        QJsonObject jsonVideo = item.toObject();
        result.push_back(VideoFactory::fromRecommendedJson(jsonVideo));
    }

    return result;
}

SearchResults VideosParser::parseChanelVideos(const QJsonArray videos)
{
    SearchResults result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) continue;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString().toLower();
        if (type == "video") {
            result.push_back(VideoFactory::fromChannelVideosJson(jsonVideo));
        } else if (type == "shortslockupview") {
            result.push_back(VideoFactory::fromChannelShortsJson(jsonVideo));
        } else if (type == "playlist") {
            result.push_back(PlaylistFactory::fromJson(jsonVideo));
        } else if (type == "lockupview") {
            result.push_back(PlaylistFactory::fromLockupViewJson(jsonVideo));
        } else if (type == "gridplaylist") {
            result.push_back(PlaylistFactory::fromGridJson(jsonVideo));
        } else {
            qDebug() << type;
        }
    }

    return result;
}

SearchResults VideosParser::parsePlaylist(const QJsonArray videos)
{
    SearchResults result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) continue;

        QJsonObject jsonVideo = item.toObject();
        result.push_back(VideoFactory::fromPlaylistJson(jsonVideo));
    }

    return result;
}
