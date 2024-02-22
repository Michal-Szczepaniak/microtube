#include "thumbnailfactory.h"

Thumbnail ThumbnailFactory::fromJson(QJsonObject json)
{
    Thumbnail thumbnail{};
    thumbnail.url = json["url"].toString();
    thumbnail.width = json["width"].toInt();
    thumbnail.height = json["height"].toInt();

    if (thumbnail.width <= 360) {
        thumbnail.size = Thumbnail::Size::SD;
    } else if (thumbnail.width <= 720) {
        thumbnail.size = Thumbnail::Size::HD;
    }

    return thumbnail;
}

Thumbnail ThumbnailFactory::fromAuthorJson(QJsonObject json)
{
    Thumbnail thumbnail = fromJson(json);
    thumbnail.url = "https:" + thumbnail.url;
    return thumbnail;
}

Thumbnail ThumbnailFactory::fromTrendingJson(QJsonObject json)
{
    Thumbnail thumbnail{};
    thumbnail.url = json["url"].toString();
    thumbnail.width = json["width"].toInt();
    thumbnail.height = json["height"].toInt();

    if (thumbnail.height <= 480) {
        thumbnail.size = Thumbnail::Size::SD;
    } else if (thumbnail.height <= 720) {
        thumbnail.size = Thumbnail::Size::HD;
    }

    return thumbnail;
}

Thumbnail ThumbnailFactory::fromRecommendedJson(QJsonObject json)
{
    Thumbnail thumbnail{};
    thumbnail.url = json["url"].toString();
    thumbnail.width = json["width"].toInt();
    thumbnail.height = json["height"].toInt();

    if (thumbnail.width <= 360) {
        thumbnail.size = Thumbnail::Size::SD;
    } else if (thumbnail.width <= 720) {
        thumbnail.size = Thumbnail::Size::HD;
    }

    return thumbnail;
}
