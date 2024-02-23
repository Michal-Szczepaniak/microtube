#include "subscriptionsaggregator.h"
#include <QSettings>
#include <QThread>

void SubscriptionsAggregator::updateSubscriptions(bool full, bool force)
{
    if (force) {
        if (_worker != nullptr) {
            _worker->stop();
            _workerThread->wait(10000);
            _workerThread->terminate();
            _worker->deleteLater();
            _workerThread->deleteLater();
        }
    } else if (_worker != nullptr || (QDateTime::currentMSecsSinceEpoch() - QSettings().value("lastSubscriptionsUpdate", 0).toLongLong() < 1800000 && !full)) return;

    QSettings().setValue("lastSubscriptionsUpdate", QDateTime::currentMSecsSinceEpoch());

    _workerThread = new QThread;
    _worker = new SubscriptionsAggregatorWorker;

    _worker->moveToThread(_workerThread);

    connect(_workerThread, &QThread::started, _worker, &SubscriptionsAggregatorWorker::execute);
    connect(_worker, &SubscriptionsAggregatorWorker::finished, _workerThread, &QThread::quit);
    connect(_worker, &SubscriptionsAggregatorWorker::updateProgress, [&](int progress){ _subscriptionsUpdateProgress = progress; emit subscriptionsUpdateProgressChanged(); });

    connect(_worker, &SubscriptionsAggregatorWorker::finished, _worker, &SubscriptionsAggregatorWorker::deleteLater);
    connect(_worker, &SubscriptionsAggregatorWorker::destroyed, [&](){ _worker = nullptr; });
    connect(_workerThread, &QThread::finished, _workerThread, &QThread::deleteLater);

    _worker->setFull(full);
    _worker->setSubscriptionToFullySync(_subscriptionToFullySync);
    _workerThread->start();

    if (_subscriptionToFullySync != "") {
        _subscriptionsCount = 1;
    } else {
        _subscriptionsCount = _authorRepository.getSubscriptions().count();
    }
    _subscriptionsUpdateProgress = 0;
    emit subscriptionsCountChanged();
    emit subscriptionsUpdateProgressChanged();
}

void SubscriptionsAggregator::updateSubscription(QString id)
{
    _subscriptionToFullySync = id;

    updateSubscriptions(true, false);
}

int SubscriptionsAggregator::getSubscriptionsCount()
{
    return _subscriptionsCount;
}

int SubscriptionsAggregator::getSubscriptionsUpdateProgress()
{
    return _subscriptionsUpdateProgress;
}

bool SubscriptionsAggregator::getSynchronizeVideos() const
{
    return QSettings().value("synchronize_videos", true).toBool();
}

void SubscriptionsAggregator::setSynchronizeVideos(bool synchronizeVideos)
{
    QSettings().setValue("synchronize_videos", synchronizeVideos);

    emit synchronizeVideosChanged();
}

bool SubscriptionsAggregator::getSynchronizeLivestreams() const
{
    return QSettings().value("synchronize_livestreams", false).toBool();
}

void SubscriptionsAggregator::setSynchronizeLivestreams(bool synchronizeLivestreams)
{
    QSettings().setValue("synchronize_livestreams", synchronizeLivestreams);

    emit synchronizeLivestreamsChanged();
}
