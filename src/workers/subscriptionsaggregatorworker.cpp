#include "subscriptionsaggregatorworker.h"
#include <QDebug>
#include "entities/author.h"
#include "entities/video.h"
#include "repositories/authorrepository.h"
#include "repositories/videorepository.h"

void SubscriptionsAggregatorWorker::execute()
{
    QVector<Author> subscriptions = _authorRepository.getSubscriptions();
    int end = subscriptions.count();
    int progress = 0;

    for (Author subscription : subscriptions) {
        std::vector<std::unique_ptr<Video>> videos = _jsProcessHelper.loadChannelVideos(subscription.authorId);
        for (std::unique_ptr<Video> &video : videos) {
            Video* v = _videoRepository.getOneByVideoId(video->videoId);
            if (v != nullptr) {
                *v = *video;
                _videoRepository.update(v->id);
            } else {
                _videoRepository.put(video.get());
            }
        }
        progress++;
        emit updateProgress(progress);
    }

    emit finished();
}
