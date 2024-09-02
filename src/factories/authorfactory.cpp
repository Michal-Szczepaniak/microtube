#include "authorfactory.h"
#include "thumbnailfactory.h"
#include <QJsonValue>
#include <QJsonArray>
#include <QVariant>

Author AuthorFactory::fromJson(QJsonObject json)
{
    Author author{};
    for (const QJsonValue &avatar : json["thumbnails"].toArray()) {
        author.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
    }
    if (!author.avatars.empty()) {
        author.bestAvatar = author.avatars.last();
    }
    author.authorId = json.contains("id") ? json["id"].toString() : json["channelID"].toString();
    author.name = json["name"].toString();
    author.url = json["url"].toString();
    author.verified = json["is_verified"].toBool();

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
    author.ignored = record.value("ignored").toBool();

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
    auto headerContent = json["header"].toObject()["content"].toObject();

    Author author{};
    author.description = metadata["description"].toString();
    author.authorId = metadata["external_id"].toString();
    author.name = metadata["title"].toString();
    author.url = metadata["vanity_channel_url"].toString();
    author.subscriberCount = getChannelSubscriberCount(headerContent);

    QJsonArray avatars;
    if (headerContent.contains("image")) {
        auto image = headerContent["image"].toObject();
        if (image.contains("avatar")) {
            auto avatar = image["avatar"].toObject();
            if (avatar.contains("image")) {
                avatars = avatar["image"].toArray();
            }
        }
    }

    if (!avatars.empty()) {
        for (const QJsonValue &avatar : avatars)
            author.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
        author.bestAvatar = author.avatars.first();
    }

    if (headerContent.contains("banner") && headerContent["banner"].toObject().contains("image") &&
        !headerContent["banner"].toObject()["image"].toArray().empty()) {

        auto banners = headerContent["banner"].toObject()["image"].toArray();
        for (const QJsonValue &banner : banners)
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

    if (!author.avatars.empty()) {
        author.bestAvatar = author.avatars.first();
    }

    for (const QJsonValue &banner : json["authorBanners"].toArray())
        author.banners.append(ThumbnailFactory::fromJson(banner.toObject()));

    if (!author.banners.empty()) {
        author.bestBanner = author.banners.first();
    }

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

    if (!author.avatars.empty()) {
        author.bestAvatar = author.avatars.first();
    }

    return author;
}

Author AuthorFactory::fromSearchJson(QJsonObject json)
{
    Author author{};
    author.authorId = json["id"].toString();
    author.description = json["description_snippet"].toObject()["text"].toString();
    author.name = json["author"].toObject()["name"].toString();
    author.url = json["author"].toObject()["url"].toString();
    author.verified = json["author"].toObject()["is_verified"].toBool();
    author.subscriberCount = parseAmount(json["video_count"].toString());

    for (const QJsonValue &avatar : json["author"].toObject()["thumbnails"].toArray())
        author.avatars.append(ThumbnailFactory::fromAuthorJson(avatar.toObject()));

    if (!author.avatars.empty()) {
        author.bestAvatar = author.avatars.first();
    }

    return author;
}

Author AuthorFactory::fromPlaylistJson(QJsonObject json)
{
    Author author{};

    author.authorId = json["id"].toString();
    author.name = json["name"].toString();
    author.url = json["url"].toString();
    author.verified = json["is_verified"].toBool();

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

int AuthorFactory::getChannelSubscriberCount(QJsonObject json)
{
    auto metadata = json["metadata"].toObject();

    if (!metadata.contains("metadata_rows")) return 0;

    auto metadataRows = metadata["metadata_rows"].toArray();

    if (metadataRows.size() < 2) return 0;

    auto metadataRow = metadataRows[1].toObject();

    if (!metadataRow.contains("metadata_parts")) return 0;

    auto metadataParts = metadataRow["metadata_parts"].toArray();

    if (metadataParts.size() < 1) return 0;

    auto metadataPart = metadataParts[0].toObject();

    if (!metadataPart.contains("text")) return 0;

    auto text = metadataPart["text"].toObject();

    if (!text.contains("text")) return 0;

    return parseAmount(text["text"].toString());
}
