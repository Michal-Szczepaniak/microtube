#ifndef SUBSCRIPTIONSAGGREGATORWORKER_H
#define SUBSCRIPTIONSAGGREGATORWORKER_H

#include <QObject>
#include "src/repositories/videorepository.h"
#include "src/repositories/authorrepository.h"
#include "src/managers/jsprocessmanager.h"

class SubscriptionsAggregatorWorker : public QObject
{
    Q_OBJECT
public:
    void stop();
    void setFull(bool full);
    void setSubscriptionToFullySync(QString subscriptionToFullySync);

signals:
    void finished();
    void updateProgress(int progress);

public slots:
    void execute();

private:
    void executeFor(Search::SearchType type, Author subscription);

private:
    VideoRepository _videoRepository{};
    AuthorRepository _authorRepository{};
    JSProcessManager _jsProcessHelper{};
    bool _stop = false;
    bool _full = false;
    QString _subscriptionToFullySync{};
};

#endif // SUBSCRIPTIONSAGGREGATORWORKER_H
