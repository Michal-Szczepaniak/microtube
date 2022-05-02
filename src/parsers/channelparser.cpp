#include "channelparser.h"
#include <QDebug>
#include <QJsonArray>
#include "src/factories/thumbnailfactory.h"

Author ChannelParser::parseAuthorInfo(const QJsonObject authorInfo)
{
    Author channelInfo;
    channelInfo.description = authorInfo["description"].toString();
    channelInfo.authorId = authorInfo["authorId"].toString();
    channelInfo.name = authorInfo["author"].toString();
    channelInfo.url = authorInfo["authorUrl"].toString();
    channelInfo.verified = authorInfo["isVerified"].toBool();

    for (const QJsonValue &avatar : authorInfo["authorThumbnails"].toArray())
        channelInfo.avatars.append(ThumbnailFactory::fromJson(avatar.toObject()));
    channelInfo.bestAvatar = channelInfo.avatars.last();

    for (const QJsonValue &banner : authorInfo["authorBanners"].toArray())
        channelInfo.banners.append(ThumbnailFactory::fromJson(banner.toObject()));
    channelInfo.bestBanner = channelInfo.banners.last();

    return channelInfo;
}
