#include "subscriptionsaggregator.h"
#include <QThread>

void SubscriptionsAggregator::updateSubscriptions()
{
    if (_worker != nullptr) return;

    QThread *thread = new QThread;
    _worker = new SubscriptionsAggregatorWorker;

    _worker->moveToThread(thread);

    connect(thread, &QThread::started, _worker, &SubscriptionsAggregatorWorker::execute);
    connect(_worker, &SubscriptionsAggregatorWorker::finished, thread, &QThread::quit);
    connect(_worker, &SubscriptionsAggregatorWorker::updateProgress, [&](int progress){ _subscriptionsUpdateProgress = progress; emit subscriptionsUpdateProgressChanged(); });

    connect(_worker, &SubscriptionsAggregatorWorker::finished, _worker, &SubscriptionsAggregatorWorker::deleteLater);
    connect(_worker, &SubscriptionsAggregatorWorker::destroyed, [&](){ _worker = nullptr; });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();

    _subscriptionsCount = _authorRepository.getSubscriptions().count();
    _subscriptionsUpdateProgress = 0;
    emit subscriptionsCountChanged();
    emit subscriptionsUpdateProgressChanged();
}

int SubscriptionsAggregator::getSubscriptionsCount()
{
    return _subscriptionsCount;
}

int SubscriptionsAggregator::getSubscriptionsUpdateProgress()
{
    return _subscriptionsUpdateProgress;
}
