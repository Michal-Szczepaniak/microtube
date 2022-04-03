#include "authorfactory.h"
#include "avatarfactory.h"
#include <QJsonValue>
#include <QJsonArray>

Author AuthorFactory::fromJson(QJsonObject json)
{
    Author author{};
    for (const QJsonValue &avatar : json["thumbnails"].toArray())
        author.avatars.append(AvatarFactory::fromJson(avatar.toObject()));
    author.bestAvatar = AvatarFactory::fromJson(json["bestAvatar"].toObject());
    author.id = json["id"].toString();
    author.name = json["name"].toString();
    author.url = json["channel_url"].toString();
    author.verified = json["verified"].toBool();

    return author;
}

Author AuthorFactory::fromTrendingJson(QJsonObject json)
{
    Author author{};

    author.id = json["authorId"].toString();
    author.name = json["author"].toString();
    author.url = "https://www.youtube.com" + json["authorUrl"].toString();

    return author;
}
