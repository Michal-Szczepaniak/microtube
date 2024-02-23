#include "subscriptionsaggregatorworker.h"
#include <QDebug>
#include <QSettings>
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

    if (_subscriptionToFullySync != "") {
        subscriptions.append(_authorRepository.getOneByChannelId(_subscriptionToFullySync));
    } else {
        subscriptions = _authorRepository.getSubscriptions();
    }
    int progress = 0;

    Search search;
    search.country = QSettings().value("country", false).toString();
    search.safeSearch = QSettings().value("safeSearch", false).toBool();

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

        progress++;
        emit updateProgress(progress);
    }

    emit finished();
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
