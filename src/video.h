/* $BEGIN_LICENSE

This file is part of Minitube.
Copyright 2009, Flavio Tordini <flavio.tordini@gmail.com>
Copyright 2018, Michał Szczepaniak <m.szczepaniak.000@gmail.com>

Minitube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Minitube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Minitube.  If not, see <http://www.gnu.org/licenses/>.

$END_LICENSE */

#ifndef VIDEO_H
#define VIDEO_H

#include <QtCore>
#include <QtGui>
#include "ytchannel.h"

class YTVideo;

class Video : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString streamUrl READ getStreamUrl NOTIFY gotStreamUrl)
    Q_PROPERTY(QString audioStreamUrl READ getAudioStreamUrl NOTIFY gotStreamUrl)
    Q_PROPERTY(QString viewCount READ getFormattedViewCount NOTIFY viewCountChanged)
    Q_PROPERTY(QString kind READ kind NOTIFY kindChanged)
    Q_PROPERTY(QString formattedDuration READ getFormattedDuration NOTIFY durationChanged)

public:
    Video();
    ~Video();
    Video *clone();

    enum License { LicenseYouTube = 1, LicenseCC };
    Q_ENUM(License)

    Q_INVOKABLE QString getTitle() const { return title; }
    void setTitle(const QString &value) { title = value; }

    Q_INVOKABLE QString getDescription() const { return description; }
    void setDescription(const QString &value) { description = value; }

    Q_INVOKABLE QString getChannelTitle() const { return channelTitle; }
    void setChannelTitle(const QString &value) { channelTitle = value; }

    Q_INVOKABLE QString getChannelId() const { return channelId; }
    void setChannelId(const QString &value) { channelId = value; }

    Q_INVOKABLE QString getWebpage();
    void setWebpage(const QString &value);

    void loadThumbnail();
    Q_INVOKABLE QPixmap getThumbnail() const { return thumbnail; }

    Q_INVOKABLE QString getThumbnailUrl() const { return thumbnailUrl; }
    void setThumbnailUrl(const QString &value) { thumbnailUrl = value; }

    Q_INVOKABLE QString getMediumThumbnailUrl() const { return mediumThumbnailUrl; }
    void setMediumThumbnailUrl(const QString &value) { mediumThumbnailUrl = value; }

    Q_INVOKABLE QString getLargeThumbnailUrl() const { return largeThumbnailUrl; }
    void setLargeThumbnailUrl(const QString &value) { largeThumbnailUrl = value; }

    int getDuration() const { return duration; }
    void setDuration(int value);
    Q_INVOKABLE QString getFormattedDuration() const { return formattedDuration; }

    int getViewCount() const { return viewCount; }
    void setViewCount(int value);
    Q_INVOKABLE QString getFormattedViewCount() const { return formattedViewCount; }

    Q_INVOKABLE QDateTime getPublished() const { return published; }
    void setPublished(const QDateTime &value);
    Q_INVOKABLE QString getFormattedPublished() const { return formattedPublished; }

    Q_INVOKABLE int getDefinitionCode() const { return definitionCode; }

    Q_INVOKABLE void loadStreamUrl();
    Q_INVOKABLE QString getStreamUrl() { return streamUrl; }
    Q_INVOKABLE QString getAudioStreamUrl() { return audioStreamUrl; }
    Q_INVOKABLE bool isLoadingStreamUrl() const { return ytVideo != nullptr; }
    Q_INVOKABLE void abortLoadStreamUrl();

    Q_INVOKABLE QString getId() const { return id; }
    void setId(const QString &value) { id = value; }
    QString kind() const { return _kind; }
    void setKind(const QString &value) { _kind = value; }

    License getLicense() const { return license; }
    void setLicense(License value) { license = value; }

signals:
    void gotThumbnail();
    void gotMediumThumbnail(const QByteArray &bytes);
    void gotLargeThumbnail(const QByteArray &bytes);
    void gotStreamUrl(const QString &videoUrl, const QString &audioUrl);
    void errorStreamUrl(const QString &message);
    void viewCountChanged(int viewCount);
    void kindChanged();
    void durationChanged();

private slots:
    void setThumbnail(const QByteArray &bytes);
    void streamUrlLoaded(const QString &streamUrl, const QString &audioUrl);

private:
    QString title;
    QString description;
    QString channelTitle;
    QString channelId;
    QString webpage;
    QString streamUrl;
    QString audioStreamUrl;
    QPixmap thumbnail;
    QString thumbnailUrl;
    QString mediumThumbnailUrl;
    QString largeThumbnailUrl;
    int duration;
    QString formattedDuration;

    QDateTime published;
    QString formattedPublished;
    int viewCount;
    QString formattedViewCount;
    License license;
    QString id;
    QString _kind;
    int definitionCode;

    bool loadingThumbnail;

    YTVideo *ytVideo;
};

// This is required in order to use QPointer<Video> as a QVariant
// as used by the Model/View playlist
typedef QPointer<Video> VideoPointer;
Q_DECLARE_METATYPE(VideoPointer)

#endif // VIDEO_H
