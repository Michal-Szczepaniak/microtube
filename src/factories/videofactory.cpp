#include "videofactory.h"
#include "thumbnailfactory.h"
#include "authorfactory.h"
#include "captionfactory.h"
#include <QJsonArray>
#include <QDebug>

std::unique_ptr<Video> VideoFactory::fromJson(QJsonObject video)
{
    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromJson(video["author"].toObject());
    parsed->description = video["description"].toString();
    parsed->duration = video["duration"].toString();
    parsed->videoId = video["id"].toString();
    parsed->isLive = video["isLive"].toBool();
    parsed->isUpcoming = video["isUpcoming"].toBool();
    parsed->title = video["title"].toString();
    parsed->upcoming = video["upcoming"].toInt();
    parsed->uploadedAt = video["uploadedAt"].toString();
    parsed->url = video["url"].toString();
    parsed->views = video["views"].toInt();

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
    parsed->videoId = video["videoId"].toString();
    parsed->isLive = video["liveNow"].toBool();
    parsed->isUpcoming = video["isUpcoming"].toBool();
    parsed->title = video["title"].toString();
    parsed->upcoming = video["upcoming"].toInt();
    parsed->uploadedAt = video["publishedText"].toString();
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;
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
    parsed->videoId = video["id"].toString();
    parsed->isLive = video["isLive"].toBool();
    parsed->title = video["title"].toString();
    parsed->uploadedAt = video["published"].toString();
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;
    parsed->views = video["view_count"].toString().toInt();

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromRecommendedJson(jsonThumbnail.toObject());
        parsed->thumbnails[thumbnail.size] = thumbnail;
    }

    return parsed;
}

std::unique_ptr<Video> VideoFactory::fromVideoInfoJson(QJsonObject video)
{
    QJsonObject videoDetails = video["videoDetails"].toObject();

    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromJson(videoDetails["author"].toObject());
    parsed->description = videoDetails["description"].toString();
    parsed->duration = videoDetails["lengthSeconds"].toString();
    parsed->videoId = videoDetails["videoId"].toString();
    parsed->isLive = videoDetails["isLive"].toBool();
    parsed->isUpcoming = videoDetails["isUpcoming"].toBool();
    parsed->title = videoDetails["title"].toString();
    parsed->upcoming = videoDetails["upcoming"].toInt();
    parsed->uploadedAt = videoDetails["uploadDate"].toString();
    parsed->url = videoDetails["video_url"].toString();
    parsed->views = videoDetails["viewCount"].toString().toInt();

    QJsonArray thumbnails = videoDetails["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromJson(jsonThumbnail.toObject());
        parsed->thumbnails[thumbnail.size] = thumbnail;
    }

    QJsonArray captions = video["player_response"].toObject()["captions"].toObject()["playerCaptionsTracklistRenderer"].toObject()["captionTracks"].toArray();
    for (const QJsonValue &jsonCaption : captions) {
        Caption caption = CaptionFactory::fromJson(jsonCaption.toObject());
        parsed->subtitles.append(caption);
    }

    return parsed;
}

Video* VideoFactory::fromSqlRecord(QSqlRecord record)
{
    Video* parsed = new Video();

    parsed->id = record.value("id").toInt();
    parsed->duration = record.value("duration").toString();
    parsed->videoId = record.value("videoId").toString();
    parsed->title = record.value("title").toString();
    parsed->url = record.value("url").toString();
    parsed->views = record.value("views").toInt();
    parsed->watched = record.value("watched").toBool();

    Thumbnail sd;
    sd.url = record.value("thumbnail").toString();
    parsed->thumbnails[Thumbnail::SD] = sd;
    Thumbnail hd;
    hd.url = record.value("bigThumbnail").toString();
    parsed->thumbnails[Thumbnail::HD] = hd;

    return parsed;
}
