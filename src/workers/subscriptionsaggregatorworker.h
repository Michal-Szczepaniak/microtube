#ifndef SUBSCRIPTIONSAGGREGATORWORKER_H
#define SUBSCRIPTIONSAGGREGATORWORKER_H

#include <QObject>
#include "src/repositories/videorepository.h"
#include "src/repositories/authorrepository.h"
#include "src/helpers/jsprocesshelper.h"

class SubscriptionsAggregatorWorker : public QObject
{
    Q_OBJECT
signals:
    void finished();
    void updateProgress(int progress);

public slots:
    void execute();

private:
    VideoRepository _videoRepository{};
    AuthorRepository _authorRepository{};
    JSProcessHelper _jsProcessHelper{};
};

#endif // SUBSCRIPTIONSAGGREGATORWORKER_H
