#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QVector>
#include "author.h"
#include "thumbnail.h"

struct Playlist {
    Q_GADGET
    Q_PROPERTY(QString id MEMBER playlistId)
    Q_PROPERTY(QString title MEMBER title)
    Q_PROPERTY(QString url MEMBER url)
    Q_PROPERTY(Author author MEMBER author)
    Q_PROPERTY(int length MEMBER length)
public:
    QString playlistId;
    QString title;
    QString url;
    Author author;
    int length = 0;
    QVector<Thumbnail> thumbnails;
    Thumbnail bestThumbnail{};
};
Q_DECLARE_METATYPE(Playlist*)

#endif // PLAYLIST_H
