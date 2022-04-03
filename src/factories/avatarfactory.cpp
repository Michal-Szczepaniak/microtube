#include "avatarfactory.h"


Avatar AvatarFactory::fromJson(QJsonObject json)
{
    Avatar avatar{};

    avatar.url = json["url"].toString();
    avatar.width = json["width"].toInt();
    avatar.height = json["height"].toInt();

    return avatar;
}
