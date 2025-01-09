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
    void updateSubscriptionsCount(int count);

public slots:
    void execute();
    void onRequestFinished(QNetworkReply *reply);

private:
    void executeFor(Search::SearchType type, Author subscription);
    void fastSync();
    void checkSubscriptions(QVector<Author> subscriptions);

private:
    VideoRepository _videoRepository{};
    AuthorRepository _authorRepository{};
    JSProcessManager _jsProcessHelper{};
    bool _stop = false;
    bool _full = false;
    bool _pending = false;
    unsigned int _progress = 0;
    QString _subscriptionToFullySync{};
    QNetworkAccessManager _manager{this};
    QVector<Author> _subscriptionsToAggregate{};
};

#endif // SUBSCRIPTIONSAGGREGATORWORKER_H
