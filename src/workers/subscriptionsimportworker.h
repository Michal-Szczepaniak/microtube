#ifndef SUBSCRIPTIONSIMPORTWORKER_H
#define SUBSCRIPTIONSIMPORTWORKER_H

#include <QObject>
#include <QString>
#include <QVector>
#include "helpers/jsprocesshelper.h"
#include "repositories/authorrepository.h"

class SubscriptionsImportWorker : public QObject
{
    Q_OBJECT
public:
    SubscriptionsImportWorker(QVector<QString> authorsToImport, QObject *parent = nullptr);

public slots:
    void import();

signals:
    void finished();
    void progressChanged(int progress);

private:
    QVector<QString> _authorsToImport;
    AuthorRepository _authorRepository;
    JSProcessHelper _jsProcessHelper;
};

#endif // SUBSCRIPTIONSIMPORTWORKER_H
