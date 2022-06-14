#ifndef SUBSCRIPTIONSAGGREGATOR_H
#define SUBSCRIPTIONSAGGREGATOR_H

#include <QObject>
#include "workers/subscriptionsaggregatorworker.h"

class SubscriptionsAggregator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int subscriptionsCount READ getSubscriptionsCount NOTIFY subscriptionsCountChanged)
    Q_PROPERTY(int subscriptionsUpdateProgress READ getSubscriptionsUpdateProgress NOTIFY subscriptionsUpdateProgressChanged)
public:
    Q_INVOKABLE void updateSubscriptions();
    int getSubscriptionsCount();
    int getSubscriptionsUpdateProgress();

signals:
    void subscriptionsCountChanged();
    void subscriptionsUpdateProgressChanged();

private:
    SubscriptionsAggregatorWorker *_worker = nullptr;
    AuthorRepository _authorRepository;
    int _subscriptionsCount = 0;
    int _subscriptionsUpdateProgress = 0;
};

#endif // SUBSCRIPTIONSAGGREGATOR_H
