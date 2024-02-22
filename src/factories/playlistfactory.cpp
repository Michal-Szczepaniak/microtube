#include "playlistfactory.h"
#include "authorfactory.h"
#include "thumbnailfactory.h"
#include <QJsonArray>
#include <QRegularExpression>

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
