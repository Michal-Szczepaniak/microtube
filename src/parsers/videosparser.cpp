#include "videosparser.h"
#include <QDebug>
#include <QJsonObject>
#include "factories/videofactory.h"
#include "factories/authorfactory.h"

SearchResults VideosParser::parse(const QJsonArray videos)
{
    SearchResults result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) break;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString();
        if (type == "video") {
            result.push_back(VideoFactory::fromJson(jsonVideo));
        } else if (type == "channel") {
            result.push_back(AuthorFactory::fromSearchJson(jsonVideo));
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
        if (item.isUndefined()) break;
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
        if (item.isUndefined()) break;
        QJsonObject jsonVideo = item.toObject();
        result.push_back(VideoFactory::fromRecommendedJson(jsonVideo));
    }

    return result;
}

SearchResults VideosParser::parseChanelVideos(const QJsonArray videos)
{
    SearchResults result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) break;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString().toLower();
        if (type == "video") {
            result.push_back(VideoFactory::fromChannelVideosJson(jsonVideo));
        } else {
            qDebug() << type;
        }
    }

    return result;
}
