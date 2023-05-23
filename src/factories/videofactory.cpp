#include "videofactory.h"
#include "thumbnailfactory.h"
#include "authorfactory.h"
#include "captionfactory.h"
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>
#include <src/repositories/authorrepository.h>

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
    parsed->timestamp = parseTimestamp(video["uploadedAt"].toString());
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
    parsed->author = AuthorFactory::fromTrendingJson(video["author"].toObject());
    parsed->description = video["description_snippet"].toObject()["text"].toString();
    parsed->duration = video["duration"].toObject()["text"].toString();
    parsed->videoId = video["id"].toString();
    parsed->isLive = false;
    parsed->isUpcoming = false;//video["isUpcoming"].toBool();
    parsed->title = video["title"].toObject()["text"].toString();
    //parsed->upcoming = video["upcoming"].toInt();
    parsed->uploadedAt = video["published"].toObject()["text"].toString();
    parsed->timestamp = parseTimestamp(parsed->uploadedAt);
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;

    const QRegularExpression re("[0-9]+");
    auto matched = re.match(video["view_count"].toObject()["text"].toString().replace(",", ""));
    parsed->views = matched.captured().toInt();

    QJsonArray thumbnails = video["thumbnails"].toArray();
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
    parsed->duration = formatDuration(QTime::fromMSecsSinceStartOfDay(video["length_seconds"].toInt()*1000));
    parsed->videoId = video["id"].toString();
    parsed->isLive = video["isLive"].toBool();
    parsed->title = video["title"].toString();
    parsed->uploadedAt = video["published"].toString();
    parsed->upcoming = false;
    parsed->timestamp = parseTimestamp(video["published"].toString());
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
    parsed->likes = videoDetails["likes"].toInt();

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
    parsed->uploadedAt = record.value("uploadedAt").toString();
    parsed->timestamp = record.value("timestamp").toUInt();
    parsed->isLive = record.value("isLive").toBool();
    parsed->isUpcoming = record.value("isUpcoming").toBool();
    parsed->upcoming = record.value("upcoming").toInt();

    AuthorRepository authorRepository;
    parsed->author = authorRepository.get(record.value("author").toInt());

    Thumbnail sd{};
    sd.url = record.value("thumbnail").toString();
    if (sd.url.length() > 0)
        parsed->thumbnails[Thumbnail::SD] = sd;
    Thumbnail hd{};
    hd.url = record.value("bigThumbnail").toString();
    if (hd.url.length() > 0)
        parsed->thumbnails[Thumbnail::HD] = hd;

    return parsed;
}

std::unique_ptr<Video> VideoFactory::fromChannelVideosJson(QJsonObject video)
{
    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromChannelVideosJson(video["author"].toObject());
    parsed->duration = video["duration"].toObject()["text"].toString();
    parsed->description = video["description_snippet"].toObject()["text"].toString();
    parsed->videoId = video["id"].toString();
    parsed->isLive = false;
    parsed->isUpcoming = false;
    parsed->title = video["title"].toObject()["text"].toString();
    parsed->uploadedAt = video["published"].toObject()["text"].toString();
    parsed->timestamp = parseTimestamp(parsed->uploadedAt);
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;
    parsed->views = parseAmount(video["view_count"].toObject()["text"].toString());

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromJson(jsonThumbnail.toObject());
        parsed->thumbnails[thumbnail.size] = thumbnail;
    }

    return parsed;
}

uint VideoFactory::parseTimestamp(QString timestamp)
{
    int num = 0;
    const auto parts = timestamp.split(' ');
    for (const auto &part : parts) {
        num = part.toInt();
        if (num > 0) break;
    }
    if (num == 0) return QDateTime::currentDateTimeUtc().toTime_t();

    auto now = QDateTime::currentDateTimeUtc();
    if (timestamp.contains("hour")) {
        return now.addSecs(-num * 3600).toTime_t();
    } else if (timestamp.contains("day")) {
        return now.addDays(-num).toTime_t();
    } else if (timestamp.contains("week")) {
        return now.addDays(-num * 7).toTime_t();
    } else if (timestamp.contains("month")) {
        return now.addMonths(-num).toTime_t();
    } else if (timestamp.contains("year")) {
        return now.addDays(-num * 365).toTime_t();
    }
    return now.toTime_t();
}

QString VideoFactory::formatDuration(QTime duration)
{
    QString result = "";

    if (duration.hour() > 0)
        result += QString::number(duration.hour()) + ":";

    result += duration.toString("m:ss");

    return result;
}

int VideoFactory::parseAmount(QString amount)
{
    QString numberWithSuffix = amount.split(" ").first().replace(",", "");
    QChar suffix = numberWithSuffix.right(1).toLower()[0];
    double number;

    if (suffix != 'm' && suffix != 'k') {
        number = numberWithSuffix.left(numberWithSuffix.length()).toDouble();
        suffix = ' ';
    } else {
        number = numberWithSuffix.left(numberWithSuffix.length()-1).toDouble();
    }

    switch (suffix.toLatin1()) {
    case 'm':
        number *= 1000000;
        break;
    case 'k':
        number *= 1000;
        break;
    }

    return number;
}

