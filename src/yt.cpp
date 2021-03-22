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

#include "yt.h"
#include "ytsearch.h"
#include "ytsinglevideosource.h"
#include "video.h"
#include "ytchannel.h"
#include "database.h"
#include "aggregatevideosource.h"
#include "ytstandardfeed.h"
#include "ytregions.h"
#include "invidious.h"
#include <QDebug>
#include <QQmlEngine>

YT::YT(QObject *parent) : QObject(parent)
{
    _channelModel = new ChannelModel();
    _commentsModel = new CommentsModel();

    _channelModel->updateQuery();

//    QSettings settings;
//    _apiKey = settings.value("googleApiKey").toString();

    Invidious::instance().initServers();
}
void YT::registerObjectsInQml(QQmlContext* context) {
    context->setContextProperty("YT",this);
    context->setContextProperty("YTChannels",this->_channelModel);
    context->setContextProperty("YTComments",this->_commentsModel);
}

const QString &YT::getCurrentVideoId() {
    return _currentVideoId;
}

void YT::setDefinition(QString definition) {
    QSettings().setValue("definition", definition);
}

QVariant YT::getChannel(const QString &channelId)
{
    auto channel = YTChannel::fromId(channelId);
    QQmlEngine::setObjectOwnership(channel, QQmlEngine::CppOwnership);
    return QVariant::fromValue(channel);
}

void YT::download(QString name, QString url, QString location) {
    qDebug() << url;
    _downloader = new QEasyDownloader();
    _downloader->setDebug(true);
    connect(_downloader, SIGNAL (DownloadFinished(const QUrl&, const QString&)), this, SLOT (downloaded(const QUrl&, const QString&)));
    connect(_downloader, SIGNAL (DownloadProgress(qint64,qint64,int,double,const QString&,const QUrl&,const QString&)), this, SLOT (downloadProgressUpdate(qint64,qint64,int,double,const QString&,const QUrl&,const QString&)));
    connect(_downloader, SIGNAL (Debugger(const QString&)), this, SLOT (debug(const QString&)));
    _downloader->Download(url, location + name + ".mp4");
    _downloadNotification.setSummary("Downloading: " + name);
    _downloadNotification.setHintValue("x-nemo-progress", 0);
    _downloadNotification.publish();
    _downloadNotificationId = _downloadNotification.replacesId();
}

void YT::downloaded(QUrl url, QString name) {
    qDebug()<<"downloaded to "<<name;
    _downloadNotification.close();
    _downloadNotificationId = 0;
    emit this->notifyDownloaded(url, name);
    delete _downloader;
}

void YT::downloadProgressUpdate(qint64 bytesReceived, qint64 bytesTotal, int nPercentage, double speed, const QString &unit, const QUrl &_URL, const QString &_qsFileName)
{
    Q_UNUSED(bytesReceived)
    Q_UNUSED(bytesTotal)
    Q_UNUSED(speed)
    Q_UNUSED(unit)
    Q_UNUSED(_URL)
    Q_UNUSED(_qsFileName)
    _downloadNotification.setHintValue("x-nemo-progress", (static_cast<double>(nPercentage))/static_cast<double>(100));
    _downloadNotification.setReplacesId(_downloadNotificationId);
    _downloadNotification.publish();
    emit downloadProgress(nPercentage);
}

QStringList YT::getRegions() {
    auto regions = YTRegions::list();
    QStringList regionsList;

    for(YTRegion region: regions) {
        regionsList << region.name;
    }

    return regionsList;
}

bool YT::safeSearch() {
    QSettings settings;
    return settings.value("safeSearch", false).toBool();
}

void YT::setSafeSearch(bool value) {
    QSettings settings;
    settings.setValue("safeSearch", value);
    emit safeSearchChanged(value);
}

bool YT::getSponsorBlockEnabled()
{
    QSettings settings;
    return settings.value("sponsorBlockEnabled", false).toBool();
}

void YT::setSponsorBlockEnabled(bool sponsorBlockEnabled)
{
    QSettings settings;
    settings.setValue("sponsorBlockEnabled", sponsorBlockEnabled);

    emit this->sponsorBlockEnabledChanged(sponsorBlockEnabled);
}

QStringList YT::getSponsorBlockCategories()
{
    QSettings settings;
    return settings.value("sponsorBlockCategories").toStringList();
}

void YT::setSponsorBlockCategories(QStringList sponsorBlockCategories)
{
    QSettings settings;
    settings.setValue("sponsorBlockCategories", sponsorBlockCategories);

    emit sponsorBlockCategoriesChanged(sponsorBlockCategories);
}

QString YT::apiKey()
{
    qDebug() << __PRETTY_FUNCTION__ << " " << _apiKey;
    return _apiKey;
}

void YT::setApiKey(QString apiKey)
{
    _apiKey = apiKey;

    QSettings settings;
    settings.setValue("googleApiKey", apiKey);

    emit apiKeyChanged(apiKey);
}

int YT::region()
{
    QString regionCode = YTRegions::currentRegionId();
    if (regionCode.isEmpty()) regionCode = "";
    auto regions = YTRegions::list();

    auto region = std::find_if(regions.begin(), regions.end(), [&](YTRegion &region) { return region.id.toLower().compare(regionCode.toLower()) == 0; } );
    return std::distance(regions.begin(), region);
}

void YT::setRegion(int id)
{
    auto regions = YTRegions::list();

    auto region = regions.at(id);

    YTRegions::setRegion(region.id.toLower());
}

void YT::debug(const QString &debug)
{
    qDebug() << debug;
}
