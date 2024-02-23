#ifndef SUBSCRIPTIONSAGGREGATOR_H
#define SUBSCRIPTIONSAGGREGATOR_H

#include <QObject>
#include "workers/subscriptionsaggregatorworker.h"

class SubscriptionsAggregator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int subscriptionsCount READ getSubscriptionsCount NOTIFY subscriptionsCountChanged)
    Q_PROPERTY(int subscriptionsUpdateProgress READ getSubscriptionsUpdateProgress NOTIFY subscriptionsUpdateProgressChanged)
    Q_PROPERTY(bool synchronizeVideos READ getSynchronizeVideos WRITE setSynchronizeVideos NOTIFY synchronizeVideosChanged)
    Q_PROPERTY(bool synchronizeLivestreams READ getSynchronizeLivestreams WRITE setSynchronizeLivestreams NOTIFY synchronizeLivestreamsChanged)
public:
    Q_INVOKABLE void updateSubscriptions(bool full = false, bool force = false);
    Q_INVOKABLE void updateSubscription(QString id);
    int getSubscriptionsCount();
    int getSubscriptionsUpdateProgress();

    bool getSynchronizeVideos() const;
    void setSynchronizeVideos(bool synchronizeVideos);
    bool getSynchronizeLivestreams() const;
    void setSynchronizeLivestreams(bool synchronizeLivestreams);

signals:
    void subscriptionsCountChanged();
    void subscriptionsUpdateProgressChanged();
    void synchronizeVideosChanged();
    void synchronizeLivestreamsChanged();

private:
    QThread *_workerThread = nullptr;
    SubscriptionsAggregatorWorker *_worker = nullptr;
    AuthorRepository _authorRepository;
    int _subscriptionsCount = 0;
    int _subscriptionsUpdateProgress = 0;
    QString _subscriptionToFullySync{};
};

#endif // SUBSCRIPTIONSAGGREGATOR_H
