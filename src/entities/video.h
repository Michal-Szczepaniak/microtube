#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include "../entities/thumbnail.h"
#include "../entities/author.h"
#include "src/entities/caption.h"

class Video : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title MEMBER title NOTIFY infoChanged)
    Q_PROPERTY(QString description MEMBER description NOTIFY infoChanged)
    Q_PROPERTY(Author author READ getAuthor NOTIFY infoChanged)
    Q_PROPERTY(QString bigThumbnail READ getBigThumbnail NOTIFY infoChanged)
    Q_PROPERTY(quint32 viewCount MEMBER views NOTIFY infoChanged)
    Q_PROPERTY(QString videoId MEMBER videoId NOTIFY infoChanged)
    Q_PROPERTY(QString published MEMBER uploadedAt NOTIFY infoChanged)
    Q_PROPERTY(QString url MEMBER url NOTIFY infoChanged)
public:
    Video(): QObject() { };

    Author getAuthor() const { return author; };
    QString getBigThumbnail() const { return thumbnails.contains(Thumbnail::HD) ? thumbnails[Thumbnail::HD].url : thumbnails[Thumbnail::SD].url; };
    QList<Caption> getSubtitles() const { return subtitles; };

    int id;
    Author author;
    QString description;
    QString duration;
    QString videoId;
    bool isLive;
    bool isUpcoming;
    QHash<Thumbnail::Size, Thumbnail> thumbnails;
    QString title;
    quint32 upcoming;
    QString uploadedAt;
    QString url;
    QList<Caption> subtitles;
    quint32 views;
    bool watched = false;

signals:
    void infoChanged();
};

#endif // VIDEO_H
