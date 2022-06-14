#include "videosparser.h"
#include <QDebug>
#include <QJsonObject>
#include "../factories/videofactory.h"

std::vector<std::unique_ptr<Video>> VideosParser::parse(const QJsonArray videos)
{
    std::vector<std::unique_ptr<Video>> result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) break;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString();
        if (type == "video") {
            result.push_back(VideoFactory::fromJson(jsonVideo));
        } else {
            qDebug() << type;
        }
    }

    return result;
}

std::vector<std::unique_ptr<Video>> VideosParser::parseTrending(const QJsonArray videos)
{
    std::vector<std::unique_ptr<Video>> result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) break;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString();
        if (type == "video") {
            result.push_back(VideoFactory::fromTrendingJson(jsonVideo));
        } else {
            qDebug() << type;
        }
    }

    return result;
}

std::vector<std::unique_ptr<Video> > VideosParser::parseRecommended(const QJsonArray videos)
{
    std::vector<std::unique_ptr<Video>> result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) break;
        QJsonObject jsonVideo = item.toObject();
        result.push_back(VideoFactory::fromRecommendedJson(jsonVideo));
    }

    return result;
}

std::vector<std::unique_ptr<Video> > VideosParser::parseChanelVideos(const QJsonArray videos)
{
    std::vector<std::unique_ptr<Video>> result;

    for (const QJsonValue &item : videos) {
        if (item.isUndefined()) break;
        QJsonObject jsonVideo = item.toObject();
        QString type = jsonVideo["type"].toString();
        if (type == "video") {
            result.push_back(VideoFactory::fromChannelVideosJson(jsonVideo));
        } else {
            qDebug() << type;
        }
    }

    return result;
}
