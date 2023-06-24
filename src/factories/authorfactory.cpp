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
    if (json.contains("bestAvatar")) {
        author.bestAvatar = ThumbnailFactory::fromJson(json["bestAvatar"].toObject());
    } else if (!author.avatars.empty()) {
        author.bestAvatar = author.avatars.last();
    }
    author.authorId = json.contains("id") ? json["id"].toString() : json["channelID"].toString();
    author.name = json["name"].toString();
    author.url = json["channel_url"].toString();
    author.verified = json["verified"].toBool();

    return author;
}

Author AuthorFactory::fromTrendingJson(QJsonObject json)
{
    Author author{};

    author.authorId = json["id"].toString();
    author.name = json["name"].toString();
    author.url = json["url"].toString();
    author.verified = json["is_verified"].toBool();
    author.bestAvatar = ThumbnailFactory::fromJson(json["thumbnails"].toArray().first().toObject());

    return author;
}

Author AuthorFactory::fromSqlRecord(QSqlRecord record)
{
    Author author{};

    author.id = record.value("id").toInt();
    author.authorId = record.value("authorId").toString();
    author.name = record.value("name").toString();
    author.bestAvatar.url = record.value("avatar").toString();
    author.url = record.value("url").toString();
    author.subscribed = record.value("subscribed").toBool();

    if (record.contains("unwatchedCount"))
        author.unwatchedVideosCount = record.value("unwatchedCount").toInt();

    return author;
}

Author AuthorFactory::fromChannelVideosJson(QJsonObject json)
{
    Author author{};

    author.authorId = json["authorId"].toString();
    author.name = json["author"].toString();
    author.url = "https://www.youtube.com" + json["authorUrl"].toString();

    return author;
}

Author AuthorFactory::fromChannelInfoJson(QJsonObject json)
{
    auto metadata = json["metadata"].toObject();
    auto header = json["header"].toObject();
    auto authorJson = header["author"].toObject();

    Author author{};
    author.description = metadata["description"].toString();
    author.authorId = authorJson["id"].toString();
    author.name = authorJson["name"].toString();
    author.url = authorJson["url"].toString();
    author.subscriberCount = parseAmount(header["subscribers"].toObject()["text"].toString());

    if (authorJson.contains("thumbnails") && !authorJson["thumbnails"].toArray().empty()) {
        for (const QJsonValue &avatar : authorJson["thumbnails"].toArray())
            author.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
        author.bestAvatar = author.avatars.first();
    }

    if (header.contains("banner") && !header["banner"].toArray().empty()) {
        for (const QJsonValue &banner : header["banner"].toArray())
            author.banners.append(ThumbnailFactory::fromJson(banner.toObject()));
        author.bestBanner = author.banners.first();
    }

    return author;
}

Author AuthorFactory::fromSubscriptionsJson(QJsonObject json)
{
    Author author{};
    author.description = json["description"].toString();
    author.authorId = json["channelId"].toString();
    author.name = json["title"].toString();
    author.url = json["authorUrl"].toString();
    author.verified = json["isVerified"].toBool();
    author.subscriberCount = json["subscriberCount"].toInt();

    for (const QJsonValue &avatar : json["authorThumbnails"].toArray())
        author.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
    author.bestAvatar = author.avatars.last();

    for (const QJsonValue &banner : json["authorBanners"].toArray())
        author.banners.append(ThumbnailFactory::fromJson(banner.toObject()));
    author.bestBanner = author.banners.last();

    return author;
}

Author AuthorFactory::fromCommentsJson(QJsonObject json)
{
    Author author{};

    author.authorId = json["id"].toString();
    author.name = json["name"].toString();
    author.url = json["url"].toString();

    for (const QJsonValue &avatar : json["thumbnails"].toArray())
        author.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
    author.bestAvatar = author.avatars.last();

    return author;
}

Author AuthorFactory::fromSearchJson(QJsonObject json)
{
    Author author{};
    author.authorId = json["channelID"].toString();
    author.description = json["descriptionShort"].toString();
    author.name = json["name"].toString();
    author.url = json["url"].toString();
    author.verified = json["verified"].toBool();

    for (const QJsonValue &avatar : json["avatars"].toArray())
        author.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
    author.bestAvatar = author.avatars.first();

    return author;
}

int AuthorFactory::parseAmount(QString amount)
{
    QString numberWithSuffix = amount.split(" ").first().replace(",", "");
    QChar suffix = numberWithSuffix.right(1).toLower()[0];
    QString n = numberWithSuffix.left(numberWithSuffix.length()-1);
    double number = numberWithSuffix.left(numberWithSuffix.length()-1).toDouble();

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
