#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QString>
#include <QHash>
#include "../entities/thumbnail.h"
#include "../entities/author.h"

class Video : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle NOTIFY infoChanged)
    Q_PROPERTY(QString description READ getDescription NOTIFY infoChanged)
    Q_PROPERTY(Author author READ getAuthor NOTIFY infoChanged)
    Q_PROPERTY(QString bigThumbnail READ getBigThumbnail NOTIFY infoChanged)
    Q_PROPERTY(quint32 viewCount READ getViewCount NOTIFY infoChanged)
    Q_PROPERTY(QString id READ getId NOTIFY infoChanged)
    Q_PROPERTY(QString published READ getPublished NOTIFY infoChanged)
    Q_PROPERTY(QString url READ getUrl NOTIFY infoChanged)
public:
    Video(): QObject() { };

    QString getTitle() const { return title; };
    QString getDescription() const { return description; };
    Author getAuthor() const { return author; };
    QString getBigThumbnail() const { return thumbnails.contains(Thumbnail::HD) ? thumbnails[Thumbnail::HD].url : thumbnails[Thumbnail::SD].url; };
    quint32 getViewCount() const { return views; };
    QString getId() const { return id; };
    QString getPublished() const { return uploadedAt; };
    QString getUrl() const { return url; };

    Author author;
    QString description;
    QString duration;
    QString id;
    bool isLive;
    bool isUpcoming;
    QHash<Thumbnail::Size, Thumbnail> thumbnails;
    QString title;
    quint32 upcoming;
    QString uploadedAt;
    QString url;
    quint32 views;

signals:
    void infoChanged();
};

#endif // VIDEO_H
