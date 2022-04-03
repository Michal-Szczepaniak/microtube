#include "videofactory.h"
#include "thumbnailfactory.h"
#include "authorfactory.h"
#include <QJsonArray>
#include <QDebug>

std::unique_ptr<Video> VideoFactory::fromJson(QJsonObject video)
{
    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromJson(video["author"].toObject());
    parsed->description = video["description"].toString();
    parsed->duration = video["lengthSeconds"].toString();
    parsed->id = video["id"].toString();
    parsed->isLive = video["isLive"].toBool();
    parsed->isUpcoming = video["isUpcoming"].toBool();
    parsed->title = video["title"].toString();
    parsed->upcoming = video["upcoming"].toInt();
    parsed->uploadedAt = video["uploadDate"].toString();
    parsed->url = video["video_url"].toString();
    parsed->views = video["viewCount"].toString().toInt();

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromJson(jsonThumbnail.toObject());
        parsed->thumbnails[thumbnail.size] = thumbnail;
    }

    return parsed;
}

std::unique_ptr<Video> VideoFactory::fromTrendingJson(QJsonObject video)
{
    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromTrendingJson(video);
    parsed->description = video["description"].toString();
    parsed->duration = video["timeText"].toString();
    parsed->id = video["videoId"].toString();
    parsed->isLive = video["liveNow"].toBool();
    parsed->isUpcoming = video["isUpcoming"].toBool();
    parsed->title = video["title"].toString();
    parsed->upcoming = video["upcoming"].toInt();
    parsed->uploadedAt = video["publishedText"].toString();
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->id;
    parsed->views = video["viewCount"].toInt();

    QJsonArray thumbnails = video["videoThumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromTrendingJson(jsonThumbnail.toObject());
        parsed->thumbnails[thumbnail.size] = thumbnail;
    }

    return parsed;
}

std::unique_ptr<Video> VideoFactory::fromRecommendedJson(QJsonObject video)
{
    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromJson(video["author"].toObject());
    parsed->duration = video["length_seconds"].toString();
    parsed->id = video["id"].toString();
    parsed->isLive = video["isLive"].toBool();
    parsed->title = video["title"].toString();
    parsed->uploadedAt = video["published"].toString();
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->id;
    parsed->views = video["view_count"].toString().toInt();

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromRecommendedJson(jsonThumbnail.toObject());
        parsed->thumbnails[thumbnail.size] = thumbnail;
    }

    return parsed;
}
