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
    _playlistModel = new PlaylistModel();
    _channelModel = new ChannelModel();
    _commentsModel = new CommentsModel();
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
    context->setContextProperty("YTPlaylist",this->_playlistModel);
    context->setContextProperty("YTChannels",this->_channelModel);
    context->setContextProperty("YTComments",this->_commentsModel);
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

    if (safeSearch())
        searchParams->setSafeSearch(SearchParams::Strict);
    else
        searchParams->setSafeSearch(SearchParams::None);

    // go!
    this->watch(searchParams);
    //    emit search(searchParams);
}

void YT::loadCategory(QString id, QString label)
{
    QString regionId = YTRegions::currentRegionId();
    YTStandardFeed *feed = new YTStandardFeed(this);
    feed->setLabel(label);
    feed->setCategory(id);
    feed->setRegionId(regionId);
    setVideoSource(feed, false, false);
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
                return;
            }
        }
    }
    YTSearch *ytSearch = new YTSearch(searchParams);
    ytSearch->setAsyncDetails(true);
    connect(ytSearch, SIGNAL(gotDetails()), _playlistModel, SLOT(emitDataChanged()));
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
    _stopped = false;

    qDebug() << "Adding VideoSource" << videoSource->getName() << videoSource;

    if (addToHistory) {
        int currentIndex = getHistoryIndex();
        if (currentIndex >= 0 && currentIndex < _history.size() - 1) {
            while (_history.size() > currentIndex + 1) {
                VideoSource *vs = _history.takeLast();
                if (!vs->parent()) {
                    qDebug() << "Deleting VideoSource" << vs->getName() << vs;
                    delete vs;
                }
            }
        }
        _history.append(videoSource);
    }

    _playlistModel->setVideoSource(videoSource);
    emit searchParamsChanged();
}

void YT::searchAgain() {
    VideoSource *currentVideoSource = _playlistModel->getVideoSource();
    setVideoSource(currentVideoSource, false);
}

const QString &YT::getCurrentVideoId() {
    return _currentVideoId;
}

int YT::getHistoryIndex() {
    return _history.lastIndexOf(_playlistModel->getVideoSource());
}

void YT::setDefinition(QString definition) {
    QSettings().setValue("definition", definition);
}

void YT::toggleSubscription() {
    Video *video = _playlistModel->activeVideo();
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

    sql += " order by added desc";

    _channelModel->setQuery(sql, Database::instance().getConnection());
}

void YT::itemActivated(int index) {
    ChannelModel::ItemTypes itemType = _channelModel->typeForIndex(index);
    if (itemType == ChannelModel::ItemChannel) {
        YTChannel *channel = _channelModel->channelForIndex(index);
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

void YT::download(QString url, QString location) {
    QString id = _playlistModel->activeVideo()->getId();
    qDebug() << url;
    _downloader = new QEasyDownloader();
    _downloader->setDebug(true);
    connect(_downloader, SIGNAL (DownloadFinished(const QUrl&, const QString&)), this, SLOT (downloaded(const QUrl&, const QString&)));
    connect(_downloader, SIGNAL (DownloadProgress(qint64,qint64,int,double,const QString&,const QUrl&,const QString&)), this, SLOT (downloadProgressUpdate(qint64,qint64,int,double,const QString&,const QUrl&,const QString&)));
    connect(_downloader, SIGNAL (Debugger(const QString&)), this, SLOT (debug(const QString&)));
    _downloader->Download(url, location + id + ".mp4");
    _downloadNotification.setSummary("Downloading: " + id);
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

bool YT::safeSearch() {
    QSettings settings;
    return settings.value("safeSearch", false).toBool();
}

void YT::setSafeSearch(bool value) {
    QSettings settings;
    settings.setValue("safeSearch", value);
    emit safeSearchChanged(value);
}

void YT::debug(const QString &debug)
{
    qDebug() << debug;
}

QVariant YT::searchParams() {
    VideoSource *videoSource = _playlistModel->getVideoSource();
    if (videoSource && videoSource->metaObject()->className() == QLatin1String("YTSearch")) {
        YTSearch *search = qobject_cast<YTSearch *>(videoSource);
        return QVariant::fromValue(search->getSearchParams());
    }
    return QVariant();
}
