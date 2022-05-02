#include "authorfactory.h"
#include "thumbnailfactory.h"
#include <QJsonValue>
#include <QJsonArray>
#include <QVariant>

Author AuthorFactory::fromJson(QJsonObject json)
{
    Author author{};
    for (const QJsonValue &avatar : json["thumbnails"].toArray())
        author.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
    author.bestAvatar = ThumbnailFactory::fromJson(json["bestAvatar"].toObject());
    author.authorId = json["id"].toString();
    author.name = json["name"].toString();
    author.url = json["channel_url"].toString();
    author.verified = json["verified"].toBool();

    return author;
}

Author AuthorFactory::fromTrendingJson(QJsonObject json)
{
    Author author{};

    author.authorId = json["authorId"].toString();
    author.name = json["author"].toString();
    author.url = "https://www.youtube.com" + json["authorUrl"].toString();

    return author;
}

Author *AuthorFactory::fromSqlRecord(QSqlRecord record)
{
    Author *author = new Author;

    author->id = record.value("id").toInt();
    author->authorId = record.value("authorId").toString();
    author->name = record.value("name").toString();
    author->bestAvatar.url = record.value("avatar").toString();
    author->url = record.value("url").toString();
    author->subscribed = record.value("subscribed").toBool();

    return author;
}
