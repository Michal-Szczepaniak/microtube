#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QString>
#include <QObject>

struct Thumbnail {
    Q_GADGET
    Q_PROPERTY(QString url MEMBER url)

public:
    quint32 width;
    quint32 height;
    QString url;
    enum Size {
        SD,
        HD
    } size;

    inline bool operator!=(const Thumbnail &t1) const { return !(t1.url == url); }
};

#endif // THUMBNAIL_H
