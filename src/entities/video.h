#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include "entities/thumbnail.h"
#include "entities/author.h"
#include "entities/caption.h"
#include "player/renderernemo.h"

using Projection = RendererNemo::Projection;

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
    Q_PROPERTY(quint32 likes MEMBER likes NOTIFY infoChanged)
public:
    Video(): QObject() { };

    Video &operator=(const Video& other) {
        description = other.description;
		duration = other.duration;	
		videoId = other.videoId;	
		isLive = other.isLive;	
		isUpcoming = other.isUpcoming;	
		thumbnails = other.thumbnails;	
		title = other.title;	
		upcoming = other.upcoming;	
		uploadedAt = other.uploadedAt;	
		timestamp = other.timestamp;	
		url = other.url;	
		subtitles = other.subtitles;	
        views = other.views;
        watched = other.watched ? other.watched : watched;
        likes = other.likes;

        return *this;
    }

    Author getAuthor() const { return author; };
    QString getBigThumbnail() const { return thumbnails.contains(Thumbnail::HD) ? thumbnails[Thumbnail::HD].url : thumbnails[Thumbnail::SD].url; };
    QList<Caption> getSubtitles() const { return subtitles; };

    int id = -1;
    Author author;
    QString description;
    QString duration;
    QString videoId;
    bool isLive = false;
    bool isUpcoming = false;;
    QHash<Thumbnail::Size, Thumbnail> thumbnails;
    QString title;
    quint32 upcoming = 0;
    QString uploadedAt;
    quint32 timestamp = -1;
    QString url;
    QList<Caption> subtitles;
    quint32 views = 0;
    bool watched = false;
    quint32 likes = 0;
    Projection projection = Projection::Flat;

signals:
    void infoChanged();
};

#endif // VIDEO_H
