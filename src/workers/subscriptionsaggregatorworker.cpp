#include "subscriptionsaggregatorworker.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QStack>
#include <QXmlStreamReader>
#include "entities/author.h"
#include "entities/video.h"
#include "repositories/authorrepository.h"
#include "repositories/videorepository.h"

void SubscriptionsAggregatorWorker::stop() {
    _stop = true;
}

void SubscriptionsAggregatorWorker::setFull(bool full)
{
    _full = full;
}

void SubscriptionsAggregatorWorker::setSubscriptionToFullySync(QString subscriptionToFullySync)
{
    _subscriptionToFullySync = subscriptionToFullySync;
}

void SubscriptionsAggregatorWorker::execute()
{
    QSettings settings;
    QVector<Author> subscriptions;

    connect(&_manager, &QNetworkAccessManager::finished, this, &SubscriptionsAggregatorWorker::onRequestFinished);

    if (_subscriptionToFullySync != "") {
        subscriptions.append(_authorRepository.getOneByChannelId(_subscriptionToFullySync));
    } else {
        subscriptions = _authorRepository.getSubscriptions();
    }

    Search search;
    search.country = QSettings().value("country", false).toString();
    search.safeSearch = QSettings().value("safeSearch", false).toBool();

    if (!_full) {
        checkSubscriptions(subscriptions);

        emit finished();
        return;
    }

    for (Author subscription : subscriptions) {
        if (_stop) break;

        if (subscription.authorId == "") {
            _authorRepository.remove(subscription);
            continue;
        }

        search.query = subscription.authorId;
        Author info = _jsProcessHelper.fetchChannelInfo(&search);

        if (info.name != "") {
            subscription.name = info.name;
        }
        if (info.bestAvatar.url != "") {
            subscription.bestAvatar = info.bestAvatar;
        }
        _authorRepository.update(subscription);

        if (settings.value("synchronize_videos", true).toBool()) {
            executeFor(Search::Channel, subscription);
        }

        if (settings.value("synchronize_livestreams", false).toBool()) {
            executeFor(Search::ChannelLiveStreams, subscription);
        }

        _progress++;
        emit updateProgress(_progress);
    }

    emit finished();
}

void SubscriptionsAggregatorWorker::onRequestFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qWarning() << "Error fetching rss feed: " << reply->errorString();
        _pending = false;
        return;
    }

    QXmlStreamReader reader(reply->readAll());
    Author author;

    while(!reader.atEnd() && !reader.hasError() && !_stop) {
        if(reader.readNext() == QXmlStreamReader::StartElement) {
            if (reader.qualifiedName() == "yt:channelId") {
                QString channelId = reader.readElementText();
                if (!channelId.startsWith("UC")) {
                    channelId = "UC" + channelId;
                }
                author = _authorRepository.getOneByChannelId(channelId);
            } else if (reader.qualifiedName() == "yt:videoId") {
                QString videoId = reader.readElementText();
                if (author.latestVideoId != videoId) {
                    author.latestVideoId = videoId;
                    _subscriptionsToAggregate.append(author);
                }

                break;
            }
        }
    }

    _pending = false;
}

void SubscriptionsAggregatorWorker::executeFor(Search::SearchType type, Author subscription)
{
    Search search;
    search.country = QSettings().value("country", false).toString();
    search.safeSearch = QSettings().value("safeSearch", false).toBool();
    search.type = type;
    search.query = subscription.authorId;
    SearchResults videos = _jsProcessHelper.loadChannelVideos(&search, _full);
    for (SearchResult &video : videos) {
        Video* v = _videoRepository.getOneByVideoId(std::get<std::unique_ptr<Video>>(video)->videoId);
        if (v != nullptr) {
            *v = *std::get<std::unique_ptr<Video>>(video);
            _videoRepository.update(v->id);
        } else {
            std::get<std::unique_ptr<Video>>(video)->author = subscription;
            _videoRepository.put(std::get<std::unique_ptr<Video>>(video).get());
        }
    }
}

void SubscriptionsAggregatorWorker::fastSync()
{
    QSettings settings;

    Search search;
    search.country = settings.value("country", false).toString();
    search.safeSearch = settings.value("safeSearch", false).toBool();
    bool includeVideos = settings.value("synchronize_videos", true).toBool();
    bool includeLivestreams = settings.value("synchronize_livestreams", false).toBool();

    for (Author subscription : _subscriptionsToAggregate) {
        search.query = subscription.authorId;
        SearchResults results = _jsProcessHelper.aggregateSubscription(&search, includeVideos, includeLivestreams);
        Author resultSubscription;

        for (SearchResult &entry : results) {
            if (std::holds_alternative<Author>(entry)) {
                resultSubscription = std::get<Author>(entry);
            } else if (std::holds_alternative<std::unique_ptr<Video>>(entry)) {
                Video* v = _videoRepository.getOneByVideoId(std::get<std::unique_ptr<Video>>(entry)->videoId);

                if (v != nullptr) {
                    *v = *std::get<std::unique_ptr<Video>>(entry);

                    if (v->author.id == -1) {
                        v->author = subscription;
                    }

                    _videoRepository.update(v->id);
                } else {
                    std::get<std::unique_ptr<Video>>(entry)->author = subscription;
                    _videoRepository.put(std::get<std::unique_ptr<Video>>(entry).get());
                }
            }
        }

        if (resultSubscription.name != "") {
            subscription.name = resultSubscription.name;
        }

        if (resultSubscription.bestAvatar.url != "") {
            subscription.bestAvatar = resultSubscription.bestAvatar;
        }

        _authorRepository.update(subscription);

        _progress++;
        emit updateProgress(_progress);
    }
}

void SubscriptionsAggregatorWorker::checkSubscriptions(QVector<Author> subscriptions)
{
    for (const Author &subscription : subscriptions) {
        if (_stop) break;

        _pending = true;
        _manager.get(QNetworkRequest("https://www.youtube.com/feeds/videos.xml?channel_id=" + subscription.authorId));

        while (_pending && !_stop) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }

    emit updateSubscriptionsCount(_subscriptionsToAggregate.count());

    fastSync();
}
