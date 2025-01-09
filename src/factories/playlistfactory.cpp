#include "playlistfactory.h"
#include "authorfactory.h"
#include "thumbnailfactory.h"
#include <QJsonArray>
#include <QRegularExpression>
#include <helpers/jsonhelper.h>

Playlist PlaylistFactory::fromJson(QJsonObject json)
{
    Playlist parsed{};
    parsed.author = AuthorFactory::fromPlaylistJson(json["author"].toObject());
    parsed.playlistId = json["id"].toString();
    parsed.title = json["title"].toObject()["text"].toString();
    parsed.url = json["url"].toString();

    const QRegularExpression re("[0-9]+");
    auto matched = re.match(json["video_count"].toObject()["text"].toString().replace(",", ""));
    parsed.length = matched.captured().toInt();

    for (const QJsonValue &thumbnail : json["thumbnails"].toArray()) {
        parsed.thumbnails.append(ThumbnailFactory::fromJson(thumbnail.toObject()));
    }

    parsed.bestThumbnail = parsed.thumbnails.first();

    return parsed;
}

Playlist PlaylistFactory::fromLockupViewJson(QJsonObject json)
{
    Playlist parsed{};
    parsed.playlistId = json["content_id"].toString();
    parsed.title = json["metadata"].toObject()["title"].toObject()["text"].toString();
    parsed.url = "https://www.youtube.com/playlist?list=" + parsed.playlistId;

    QJsonObject thumbnail = json["content_image"].toObject()["primary_thumbnail"].toObject();
    for (const QJsonValue &image : thumbnail["image"].toArray()) {
        parsed.thumbnails.append(ThumbnailFactory::fromJson(image.toObject()));
    }
    parsed.bestThumbnail = parsed.thumbnails.first();

    QJsonObject overlay = JsonHelper::find(thumbnail["overlays"].toArray(), [](QJsonObject obj){ return obj["type"].toString() == "ThumbnailOverlayBadgeView"; }).toObject();
    QJsonObject badge = JsonHelper::find(overlay["badges"].toArray(), [](QJsonObject obj){ return obj["type"].toString() == "ThumbnailBadgeView"; }).toObject();

    if (!badge.isEmpty()) {
        const QRegularExpression re("[0-9]+");
        auto matched = re.match(badge["text"].toString().replace(",", ""));
        parsed.length = matched.captured().toInt();
    }

    return parsed;
}

Playlist PlaylistFactory::fromGridJson(QJsonObject json)
{
    Playlist parsed{};

    parsed.playlistId = json["id"].toString();
    parsed.title = json["title"].toObject()["text"].toString();

    const QRegularExpression re("[0-9]+");
    auto matched = re.match(json["video_count"].toObject()["text"].toString().replace(",", ""));
    parsed.length = matched.captured().toInt();

    for (const QJsonValue &thumbnail : json["thumbnails"].toArray()) {
        parsed.thumbnails.append(ThumbnailFactory::fromJson(thumbnail.toObject()));
    }

    parsed.bestThumbnail = parsed.thumbnails.first();

    return parsed;
}
