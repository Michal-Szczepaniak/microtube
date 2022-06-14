#include "subscriptionsimportworker.h"
#include "entities/author.h"

SubscriptionsImportWorker::SubscriptionsImportWorker(QVector<QString> authorsToImport, QObject *parent)
{
    _authorsToImport = authorsToImport;
}

void SubscriptionsImportWorker::import()
{
    int progress = 0;

    for (QString channelId : _authorsToImport) {
        Author author = _authorRepository.getOneByChannelId(channelId);
        if (author.id == -1) {
            author = _jsProcessHelper.fetchChannelInfo(channelId);
            _authorRepository.put(author);
        }

        author.subscribed = true;
        _authorRepository.update(author);
        progress++;
        emit progressChanged(progress);
    }

    emit finished();
}
