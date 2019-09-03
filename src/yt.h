/*
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

#ifndef YT_H
#define YT_H

#include <QObject>
#include <QUrl>
#include <QQmlContext>
#include "playlistmodel.h"
#include "channelmodel.h"
#include "searchparams.h"
#include "ytvideo.h"
#include <QEasyDownloader.hpp>
#include <notification.h>
#include <QQmlPropertyMap>

class YT : public QObject
{
    Q_OBJECT

public:
    explicit YT(QObject *parent = nullptr);
    void registerObjectsInQml(QQmlContext* context);
    void setVideoSource(VideoSource *videoSource, bool addToHistory = true, bool back = false);
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void watchChannel(const QString &channelId);
    void watch(SearchParams *searchParams);
    const QVector<VideoSource*> & getHistory() { return history; }
    int getHistoryIndex();
    PlaylistModel* getPlaylistModel() { return playlistModel; }
    const QString &getCurrentVideoId();
    void updateSubscriptionAction(Video *video, bool subscribed);
    Q_INVOKABLE void setDefinition(QString definition);
    Q_INVOKABLE void toggleSubscription();
    Q_INVOKABLE void updateQuery();
    Q_INVOKABLE void itemActivated(int index);
    Q_INVOKABLE void setSafeSearch(bool value);
    Q_INVOKABLE bool getSafeSearch();
    Q_INVOKABLE void download(QString url);
    void loadDefaultVideos();
    Q_INVOKABLE QStringList getRegions();
    Q_INVOKABLE int getCurrentRegion();
    Q_INVOKABLE void setRegion(int id);

public slots:
    void downloaded(QUrl url, QString name);
    void downloadProgressUpdate(qint64 bytesReceived, qint64 bytesTotal, int nPercentage, double speed, const QString& unit, const QUrl& _URL, const QString& _qsFileName);

signals:
    void notifyDownloaded(QUrl url, QString name);
    void downloadProgress(int nPercentage);

private:
    SearchParams* getSearchParams();
    void searchAgain();

    bool stopped;
    QTimer *errorTimer;
    Video *skippedVideo;
    QString currentVideoId;

    PlaylistModel* playlistModel;
    ChannelModel* channelModel;
    QVector<VideoSource*> history;
    QEasyDownloader downloader;

    Notification downloadNotification;
    int downloadNotificationId = 0;
};

#endif // YT_H
