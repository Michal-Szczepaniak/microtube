#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QString>

struct Thumbnail {
    quint32 width;
    quint32 height;
    QString url;
    enum Size {
        SD,
        HD
    } size;
};

#endif // THUMBNAIL_H
