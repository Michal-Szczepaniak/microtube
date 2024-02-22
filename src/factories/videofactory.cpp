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
    parsed->duration = video["duration"].toObject()["text"].toString();
    parsed->videoId = video["id"].toString();
    parsed->title = video["title"].toObject()["text"].toString();
    parsed->uploadedAt = video["published"].toObject()["text"].toString();
    parsed->timestamp = parseTimestamp(parsed->uploadedAt);
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;
    parsed->views = parseAmount(video["view_count"].toObject()["text"].toString());
    parsed->isLive = isLive(video);
    parsed->isUpcoming = isUpcoming(video);

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromJson(jsonThumbnail.toObject());
        if (!parsed->thumbnails.contains(thumbnail.size) || parsed->thumbnails[thumbnail.size].width < thumbnail.width) {
            parsed->thumbnails[thumbnail.size] = thumbnail;
        }
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
    parsed->isLive = isLive(video);
    parsed->isUpcoming = isUpcoming(video);
    parsed->title = video["title"].toObject()["text"].toString();
    parsed->uploadedAt = video["published"].toObject()["text"].toString();
    parsed->timestamp = parseTimestamp(parsed->uploadedAt);
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;

    const QRegularExpression re("[0-9]+");
    auto matched = re.match(video["view_count"].toObject()["text"].toString().replace(",", ""));
    parsed->views = matched.captured().toInt();

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromTrendingJson(jsonThumbnail.toObject());
        if (!parsed->thumbnails.contains(thumbnail.size) || parsed->thumbnails[thumbnail.size].width < thumbnail.width) {
            parsed->thumbnails[thumbnail.size] = thumbnail;
        }
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
        if (!parsed->thumbnails.contains(thumbnail.size) || parsed->thumbnails[thumbnail.size].width < thumbnail.width) {
            parsed->thumbnails[thumbnail.size] = thumbnail;
        }
    }

    return parsed;
}

std::unique_ptr<Video> VideoFactory::fromVideoInfoJson(QJsonObject video)
{
    QJsonObject videoDetails = video["videoDetails"].toObject();
    QJsonObject playerConfig = video["player_response"].toObject()["playerConfig"].toObject();

    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromJson(videoDetails["author"].toObject());
    parsed->description = videoDetails["description"].toString();
    parsed->duration = formatDuration(QTime::fromMSecsSinceStartOfDay(videoDetails["lengthSeconds"].toString().toInt()*1000));
    parsed->videoId = videoDetails["videoId"].toString();
    parsed->isLive = videoDetails["isLive"].toBool();
    parsed->isUpcoming = videoDetails["isUpcoming"].toBool();
    parsed->title = videoDetails["title"].toString();
    parsed->upcoming = videoDetails["upcoming"].toInt();
    parsed->uploadedAt = videoDetails["publishDate"].toString();
    parsed->url = videoDetails["video_url"].toString();
    parsed->views = videoDetails["viewCount"].toString().toInt();
    parsed->likes = getLikeCount(video);

    bool isVR = false;
    for (const QJsonValue &jsonFormat : video["formats"].toArray()) {
        const QJsonObject format = jsonFormat.toObject();
        if (format["projectionType"].toString().compare("MESH") == 0) {
            isVR = true;
            break;
        }
    }

    if (isVR) {
        parsed->projection = Projection::s360;

        if (playerConfig.contains("vrConfig") && playerConfig["vrConfig"].toObject()["partialSpherical"].toBool()) {
            parsed->projection = Projection::s180;
        }
    }

    QJsonArray thumbnails = videoDetails["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromJson(jsonThumbnail.toObject());
        if (!parsed->thumbnails.contains(thumbnail.size) || parsed->thumbnails[thumbnail.size].width < thumbnail.width) {
            parsed->thumbnails[thumbnail.size] = thumbnail;
        }
    }

    QJsonArray captions = video["player_response"].toObject()["captions"].toObject()["playerCaptionsTracklistRenderer"].toObject()["captionTracks"].toArray();
    for (const QJsonValue &jsonCaption : captions) {
        Caption caption = CaptionFactory::fromJson(jsonCaption.toObject());
        parsed->subtitles.append(caption);
    }

    QString publishDate = videoDetails["publishDate"].toString();
    QDateTime dateTime = QDateTime::fromString(publishDate, Qt::ISODate);
    parsed->timestamp = dateTime.toTime_t();

    return parsed;
}

std::unique_ptr<Video> VideoFactory::fromPlaylistJson(QJsonObject video)
{
    std::unique_ptr<Video> parsed(new Video());
    parsed->author = AuthorFactory::fromPlaylistJson(video["author"].toObject());
    parsed->duration = video["duration"].toObject()["text"].toString();
    parsed->videoId = video["id"].toString();
    parsed->title = video["title"].toObject()["text"].toString();
    parsed->timestamp = QDateTime::currentDateTimeUtc().toTime_t();
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;
    parsed->isLive = isLive(video);
    parsed->isUpcoming = isUpcoming(video);
    parsed->upcoming = QDateTime::fromString(video["upcoming"].toString(), Qt::ISODate).toTime_t();

    QJsonArray runs = video["video_info"].toObject()["runs"].toArray();
    QJsonObject views = runs.first().toObject();
    QJsonObject timestamp = runs.last().toObject();

    parsed->views = parseAmount(views["text"].toString());
    parsed->timestamp = parseTimestamp(timestamp["text"].toString());

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromJson(jsonThumbnail.toObject());
        if (!parsed->thumbnails.contains(thumbnail.size) || parsed->thumbnails[thumbnail.size].width < thumbnail.width) {
            parsed->thumbnails[thumbnail.size] = thumbnail;
        }
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
    QString duration = video["duration"].toObject()["text"].toString();
    parsed->author = AuthorFactory::fromChannelVideosJson(video["author"].toObject());
    parsed->duration = duration;
    parsed->description = video["description_snippet"].toObject()["text"].toString();
    parsed->videoId = video["id"].toString();
    parsed->isLive = isLive(video);
    parsed->isUpcoming = isUpcoming(video);
    parsed->upcoming = QDateTime::fromString(video["upcoming"].toString(), Qt::ISODate).toTime_t();
    parsed->title = video["title"].toObject()["text"].toString();
    parsed->uploadedAt = video["published"].toObject()["text"].toString();
    parsed->timestamp = parseTimestamp(parsed->uploadedAt);
    parsed->url = "https://www.youtube.com/watch?v=" + parsed->videoId;
    parsed->views = parseAmount(video["view_count"].toObject()["text"].toString());

    QJsonArray thumbnails = video["thumbnails"].toArray();
    for (const QJsonValue &jsonThumbnail : thumbnails) {
        Thumbnail thumbnail = ThumbnailFactory::fromJson(jsonThumbnail.toObject());
        if (!parsed->thumbnails.contains(thumbnail.size) || parsed->thumbnails[thumbnail.size].width < thumbnail.width) {
            parsed->thumbnails[thumbnail.size] = thumbnail;
        }
    }

    return parsed;
}

int VideoFactory::parseAmount(QString amount)
{
    QString numberWithSuffix = amount.split(" ").first().replace(",", "").replace(".", "");
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
    if (timestamp.contains("minute")) {
        return now.addSecs(-num * 60).toTime_t();
    } else if (timestamp.contains("hour")) {
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

    if (duration.hour() > 0) {
        result += QString::number(duration.hour()) + ":";
    }

    result += duration.toString("mm:ss");

    return result;
}

uint VideoFactory::getLikeCount(QJsonObject video)
{
    QJsonObject currentObject;
    QJsonArray currentArray;

    currentObject  = video["response"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["contents"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["twoColumnWatchNextResults"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["results"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["results"].toObject();
    if (currentObject.empty()) return 0;

    currentArray = currentObject["contents"].toArray();
    if (currentArray.empty()) return 0;

    currentObject = currentArray.first().toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["videoPrimaryInfoRenderer"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["videoActions"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["menuRenderer"].toObject();
    if (currentObject.empty()) return 0;

    currentArray = currentObject["topLevelButtons"].toArray();
    if (currentArray.empty()) return 0;

    currentObject = currentArray.first().toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["segmentedLikeDislikeButtonViewModel"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["likeButtonViewModel"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["likeButtonViewModel"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["toggleButtonViewModel"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["toggleButtonViewModel"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["defaultButtonViewModel"].toObject();
    if (currentObject.empty()) return 0;

    currentObject = currentObject["buttonViewModel"].toObject();
    if (currentObject.empty()) return 0;

    if (!currentObject.contains("title")) return 0;
    return parseAmount(currentObject["title"].toString());
}

bool VideoFactory::isLive(QJsonObject video)
{
    bool result = false;

    for (const QJsonValue &badge : video["badges"].toArray()) {
        QJsonObject badgeObj = badge.toObject();

        if (badgeObj["style"].toString() == "BADGE_STYLE_TYPE_LIVE_NOW" ||
            badgeObj["label"].toString() == "LIVE") {
            result = true;
            break;
        }
    }

    for (const QJsonValue &overlay : video["thumbnail_overlays"].toArray()) {
        QJsonObject thumbObj = overlay.toObject();

        if (thumbObj["style"].toString() == "LIVE" ||
            thumbObj["text"].toString() == "LIVE") {
            result = true;
            break;
        }
    }

    return result;
}

bool VideoFactory::isUpcoming(QJsonObject video)
{
    if (video.contains("upcoming")) {
        qint64 timestamp = QDateTime::fromString(video["upcoming"].toString(), Qt::ISODate).toTime_t();

        return timestamp - QDateTime::currentDateTimeUtc().toTime_t() > 0;
    }


    for (const QJsonValue &overlay : video["thumbnail_overlays"].toArray()) {
        QJsonObject thumbObj = overlay.toObject();

        if (thumbObj["style"].toString() == "UPCOMING" ||
            thumbObj["text"].toString() == "UPCOMING") {
            return true;
        }
    }

    return false;
}
