#ifndef AUTHOR_H
#define AUTHOR_H

#include <QObject>
#include <QVector>
#include "thumbnail.h"

struct Author {
    Q_GADGET
    Q_PROPERTY(QString id MEMBER authorId)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(QString url MEMBER url)
    Q_PROPERTY(bool verified MEMBER verified)
public:
    QVector<Thumbnail> avatars;
    Thumbnail bestAvatar{};
    QVector<Thumbnail> banners;
    Thumbnail bestBanner{};
    int id = -1;
    QString authorId;
    QString name;
    QString description;
    //QString<Badge> badges;
    QString url;
    bool verified;
    bool subscribed = false;
};

#endif // AUTHOR_H
