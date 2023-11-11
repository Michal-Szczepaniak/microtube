#include "subscriptionsaggregatorworker.h"
#include <QDebug>
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

void SubscriptionsAggregatorWorker::execute()
{
    QVector<Author> subscriptions = _authorRepository.getSubscriptions();
    int progress = 0;

    for (Author subscription : subscriptions) {
        if (_stop) break;

        SearchResults videos = _jsProcessHelper.loadChannelVideos(subscription.authorId, _full);
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
        progress++;
        emit updateProgress(progress);
    }

    emit finished();
}
