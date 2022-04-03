#ifndef AVATARFACTORY_H
#define AVATARFACTORY_H

#include <QJsonObject>
#include "../entities/avatar.h"

class AvatarFactory
{
public:
    static Avatar fromJson(QJsonObject json);
};

#endif // AVATARFACTORY_H
