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
    Q_PROPERTY(Thumbnail banner MEMBER bestBanner)
    Q_PROPERTY(Thumbnail avatar MEMBER bestAvatar)
    Q_PROPERTY(int subscriberCount MEMBER subscriberCount)
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
    int subscriberCount;
    bool subscribed = false;
    int unwatchedVideosCount = 0;
};

#endif // AUTHOR_H
