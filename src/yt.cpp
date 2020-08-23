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
#include <QDebug>

YT::YT(QObject *parent) : QObject(parent)
{
    playlistModel = new PlaylistModel();
    channelModel = new ChannelModel();
    connect(&downloader, SIGNAL (DownloadFinished(const QUrl&, const QString&)), this, SLOT (downloaded(const QUrl&, const QString&)));
    connect(&downloader, SIGNAL (DownloadProgress(qint64,qint64,int,double,const QString&,const QUrl&,const QString&)), this, SLOT (downloadProgressUpdate(qint64,qint64,int,double,const QString&,const QUrl&,const QString&)));
    updateQuery();

    QSettings settings;
    _apiKey = settings.value("googleApiKey").toString();
}

void YT::loadDefaultVideos() {
    QString regionId = YTRegions::currentRegionId();
    YTStandardFeed *feed = new YTStandardFeed(this);
    feed->setFeedId("most_popular");
    feed->setLabel("most_popular");
    feed->setRegionId(regionId);
    setVideoSource(feed, false, false);
}

void YT::registerObjectsInQml(QQmlContext* context) {
    context->setContextProperty("YT",this);
    context->setContextProperty("YTPlaylist",this->playlistModel);
    context->setContextProperty("YTChannels",this->channelModel);
}

void YT::search(QString query) {
    QString q = query.simplified();

    // check for empty query
    if (q.isEmpty()) {
//        queryEdit->toWidget()->setFocus(Qt::OtherFocusReason);
        return;
    }

    SearchParams *searchParams = new SearchParams();
    searchParams->setKeywords(q);

    if (getSafeSearch())
        searchParams->setSafeSearch(SearchParams::Strict);
    else
        searchParams->setSafeSearch(SearchParams::None);

    // go!
    this->watch(searchParams);
//    emit search(searchParams);
}

void YT::watch(SearchParams *searchParams) {
    if (!searchParams->keywords().isEmpty()) {
        if (searchParams->keywords().startsWith("http://") ||
            searchParams->keywords().startsWith("https://")) {
            QString videoId = YTSearch::videoIdFromUrl(searchParams->keywords());
            if (!videoId.isEmpty()) {
                YTSingleVideoSource *singleVideoSource = new YTSingleVideoSource(this);
                singleVideoSource->setVideoId(videoId);
                setVideoSource(singleVideoSource);
//                QTime tstamp = YTSearch::videoTimestampFromUrl(searchParams->keywords());
//                pauseTime = QTime(0, 0).msecsTo(tstamp);
                return;
            }
        }
    }
    YTSearch *ytSearch = new YTSearch(searchParams);
    ytSearch->setAsyncDetails(true);
    connect(ytSearch, SIGNAL(gotDetails()), playlistModel, SLOT(emitDataChanged()));
    setVideoSource(ytSearch);
}

void YT::watchChannel(const QString &channelId) {
    if (channelId.isEmpty()) {
        return;
    }

    QString id = channelId;

    // Fix old settings
    const QLatin1String uc("UC");
    if (!id.startsWith(uc)) id = uc + id;

    SearchParams *searchParams = new SearchParams();
    searchParams->setChannelId(id);
    searchParams->setSortBy(SearchParams::SortByNewest);

    // go!
    watch(searchParams);
}

void YT::setVideoSource(VideoSource *videoSource, bool addToHistory, bool back) {
    Q_UNUSED(back);
    stopped = false;
//    errorTimer->stop();

    // qDebug() << "Adding VideoSource" << videoSource->getName() << videoSource;

    if (addToHistory) {
        int currentIndex = getHistoryIndex();
        if (currentIndex >= 0 && currentIndex < history.size() - 1) {
            while (history.size() > currentIndex + 1) {
                VideoSource *vs = history.takeLast();
                if (!vs->parent()) {
                    qDebug() << "Deleting VideoSource" << vs->getName() << vs;
                    delete vs;
                }
            }
        }
        history.append(videoSource);
    }

#ifdef APP_EXTRA
    if (history.size() > 1)
        Extra::slideTransition(playlistView->viewport(), playlistView->viewport(), back);
#endif

    playlistModel->setVideoSource(videoSource);

//    QSettings settings;
//    if (settings.value("manualplay", false).toBool()) {
//        videoAreaWidget->showPickMessage();
//    }

//    sidebar->showPlaylist();
//    sidebar->getRefineSearchWidget()->setSearchParams(getSearchParams());
//    sidebar->hideSuggestions();
//    sidebar->getHeader()->updateInfo();

//    SearchParams *searchParams = getSearchParams();
//    bool isChannel = searchParams && !searchParams->channelId().isEmpty();
//    playlistView->setClickableAuthors(!isChannel);
}

void YT::searchAgain() {
    VideoSource *currentVideoSource = playlistModel->getVideoSource();
    setVideoSource(currentVideoSource, false);
}

SearchParams *YT::getSearchParams() {
    VideoSource *videoSource = playlistModel->getVideoSource();
    if (videoSource && videoSource->metaObject()->className() == QLatin1String("YTSearch")) {
        YTSearch *search = qobject_cast<YTSearch *>(videoSource);
        return search->getSearchParams();
    }
    return 0;
}

const QString &YT::getCurrentVideoId() {
    return currentVideoId;
}

int YT::getHistoryIndex() {
    return history.lastIndexOf(playlistModel->getVideoSource());
}

void YT::setDefinition(QString definition) {
    QSettings().setValue("definition", definition);
}

void YT::toggleSubscription() {
    Video *video = playlistModel->activeVideo();
    if (!video) return;
    QString userId = video->getChannelId();
    if (userId.isEmpty()) return;
    bool subscribed = YTChannel::isSubscribed(userId);
    if (subscribed) {
        YTChannel::unsubscribe(userId);
    } else {
        YTChannel::subscribe(userId);
    }
}

void YT::toggleSubscription(const QString &channelId) {
    if (channelId.isEmpty()) return;
    bool subscribed = YTChannel::isSubscribed(channelId);
    if (subscribed) {
        YTChannel::unsubscribe(channelId);
    } else {
        YTChannel::subscribe(channelId);
    }
}

bool YT::isSubscribed(const QString &channelId)
{
    return YTChannel::isSubscribed(channelId);
}

void YT::updateQuery() {
    QString sql = "select user_id from subscriptions";
//    if (showUpdated)
//        sql += " where notify_count>0";

//    switch (sortBy) {
//    case SortByUpdated:
//        sql += " order by updated desc";
//        break;
//    case SortByAdded:
//        sql += " order by added desc";
//        break;
//    case SortByLastWatched:
//        sql += " order by watched desc";
//        break;
//    case SortByMostWatched:
//        sql += " order by views desc";
//        break;
//    default:
//        sql += " order by name collate nocase";
//        break;
//    }

    sql += " order by added desc";

    channelModel->setQuery(sql, Database::instance().getConnection());
}

void YT::itemActivated(int index) {
    ChannelModel::ItemTypes itemType = channelModel->typeForIndex(index);
    if (itemType == ChannelModel::ItemChannel) {
        YTChannel *channel = channelModel->channelForIndex(index);
        SearchParams *params = new SearchParams();
        params->setChannelId(channel->getChannelId());
        params->setSortBy(SearchParams::SortByNewest);
        params->setTransient(true);
        YTSearch *videoSource = new YTSearch(params);
        videoSource->setAsyncDetails(true);
        setVideoSource(videoSource);
        channel->updateWatched();
    } else if (itemType == ChannelModel::ItemAggregate) {
        AggregateVideoSource *videoSource = new AggregateVideoSource();
        videoSource->setName(tr("All Videos"));
        setVideoSource(videoSource);
    } else if (itemType == ChannelModel::ItemUnwatched) {
        AggregateVideoSource *videoSource = new AggregateVideoSource();
        videoSource->setName(tr("Unwatched Videos"));
        videoSource->setUnwatched(true);
        setVideoSource(videoSource);
    }
}

void YT::setSafeSearch(bool value) {
    QSettings settings;
    settings.setValue("safeSearch", value);
}

bool YT::getSafeSearch() {
    QSettings settings;
    return settings.value("safeSearch", false).toBool();
}

void YT::download(QString url, QString location) {
    QString name = playlistModel->activeVideo()->getTitle();
    name = name.replace("/", "");
    downloader.Download(url, location + name + ".mp4");
    downloadNotification.setSummary("Downloading: " + name);
    downloadNotification.setHintValue("x-nemo-progress", 0);
    downloadNotification.publish();
    downloadNotificationId = downloadNotification.replacesId();
}

void YT::downloaded(QUrl url, QString name) {
    qDebug()<<"downloaded to "<<name;
    downloadNotification.close();
    downloadNotificationId = 0;
    emit this->notifyDownloaded(url, name);
}

void YT::downloadProgressUpdate(qint64 bytesReceived, qint64 bytesTotal, int nPercentage, double speed, const QString &unit, const QUrl &_URL, const QString &_qsFileName)
{
    downloadNotification.setHintValue("x-nemo-progress", ((double)nPercentage)/(double)100);
    downloadNotification.setReplacesId(downloadNotificationId);
    downloadNotification.publish();
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

int YT::getCurrentRegion()
{
    QString regionCode = YTRegions::currentRegionId();
    if (regionCode.isEmpty()) regionCode = "";
    auto regions = YTRegions::list();

    int i = 0;
    for(YTRegion region: regions) {
        if (region.id.toLower().compare(regionCode.toLower()) == 0) return i;
        i++;
    }

    return i;
}

void YT::setRegion(int id)
{
    auto regions = YTRegions::list();

    int i = 0;
    for(YTRegion region: regions) {
        if (i == id) {
            YTRegions::setRegion(region.id.toLower());
            return;
        }
        i++;
    }
}

QString YT::apiKey()
{
    return _apiKey;
}

void YT::setApiKey(QString apiKey)
{
    _apiKey = apiKey;

    QSettings settings;
    settings.setValue("googleApiKey", apiKey);

    emit apiKeyChanged(apiKey);
}
