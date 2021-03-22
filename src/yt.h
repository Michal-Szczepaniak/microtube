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
#include "commentsmodel.h"
#include "ytvideo.h"
#include <QEasyDownloader.hpp>
#include <notification.h>
#include <QQmlPropertyMap>

typedef QPointer<SearchParams> SearchParamsPointer;
Q_DECLARE_METATYPE(SearchParamsPointer)

class YT : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(int region READ region WRITE setRegion NOTIFY regionChanged)
    Q_PROPERTY(bool safeSearch READ safeSearch WRITE setSafeSearch NOTIFY safeSearchChanged)
    Q_PROPERTY(bool sponsorBlockEnabled READ getSponsorBlockEnabled WRITE setSponsorBlockEnabled NOTIFY sponsorBlockEnabledChanged)
    Q_PROPERTY(QStringList sponsorBlockCategories READ getSponsorBlockCategories WRITE setSponsorBlockCategories NOTIFY sponsorBlockCategoriesChanged)

public:
    explicit YT(QObject *parent = nullptr);
    void registerObjectsInQml(QQmlContext* context);
    const QString &getCurrentVideoId();
    Q_INVOKABLE void setDefinition(QString definition);
    Q_INVOKABLE QVariant getChannel(const QString &channelId);
    Q_INVOKABLE void download(QString name, QString url, QString location);
    Q_INVOKABLE QStringList getRegions();

    QString apiKey();
    void setApiKey(QString apiKey);
    int region();
    void setRegion(int id);
    bool safeSearch();
    void setSafeSearch(bool value);
    bool getSponsorBlockEnabled();
    void setSponsorBlockEnabled(bool sponsorBlockEnabled);
    QStringList getSponsorBlockCategories();
    void setSponsorBlockCategories(QStringList sponsorBlockCategories);

public slots:
    void debug(const QString&);
    void downloaded(QUrl url, QString name);
    void downloadProgressUpdate(qint64 bytesReceived, qint64 bytesTotal, int nPercentage, double speed, const QString& unit, const QUrl& _URL, const QString& _qsFileName);

signals:
    void notifyDownloaded(QUrl url, QString name);
    void downloadProgress(int nPercentage);
    void apiKeyChanged(QString apiKey);
    void regionChanged(int region);
    void safeSearchChanged(bool safeSearch);
    void sponsorBlockEnabledChanged(bool sponsorBlockEnabled);
    void sponsorBlockCategoriesChanged(QStringList sponsorBlockCategories);

private:
    bool _stopped;
    QTimer *_errorTimer;
    Video *_skippedVideo;
    QString _currentVideoId;
    QString _apiKey;

    ChannelModel* _channelModel;
    CommentsModel* _commentsModel;
    QVector<VideoSource*> _history;
    QEasyDownloader* _downloader;

    Notification _downloadNotification;
    quint32 _downloadNotificationId = 0;
};

#endif // YT_H
