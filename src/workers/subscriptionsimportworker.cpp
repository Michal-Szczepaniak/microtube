#include "subscriptionsimportworker.h"
#include "entities/author.h"

#include <QSettings>

SubscriptionsImportWorker::SubscriptionsImportWorker(QVector<QString> authorsToImport, QObject *parent)
{
    _authorsToImport = authorsToImport;
}

void SubscriptionsImportWorker::import()
{
    int progress = 0;

    Search search;
    search.country = QSettings().value("country", false).toString();
    search.safeSearch = QSettings().value("safeSearch", false).toBool();

    for (QString channelId : _authorsToImport) {
        Author author = _authorRepository.getOneByChannelId(channelId);
        if (author.id == -1) {
            search.query = channelId;
            author = _jsProcessHelper.fetchChannelInfo(&search);
            _authorRepository.put(author);
        }

        author.subscribed = true;
        _authorRepository.update(author);
        progress++;
        emit progressChanged(progress);
    }

    emit finished();
}
