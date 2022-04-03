#ifndef AUTHOR_H
#define AUTHOR_H

#include <QObject>
#include <QVector>
#include "avatar.h"

struct Author {
    Q_GADGET
    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString url MEMBER url)
    Q_PROPERTY(bool verified MEMBER verified)
public:
    QVector<Avatar> avatars;
    Avatar bestAvatar{};
    QString id;
    QString name;
    //QString<Badge> badges;
    QString url;
    bool verified;
};

#endif // AUTHOR_H
