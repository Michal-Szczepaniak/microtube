#ifndef GOOGLEOAUTHHELPER_H
#define GOOGLEOAUTHHELPER_H

#include <QObject>
#include <o2google.h>
#include <o2requestor.h>
#include <QNetworkInfo>
#include <QThread>
#include "workers/subscriptionsimportworker.h"
#include "repositories/authorrepository.h"
#include "factories/authorfactory.h"
#include <managers/jsprocessmanager.h>

class GoogleOAuthHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool linked READ isLinked NOTIFY linkedChanged)
    Q_PROPERTY(int importProgress READ getImportProgress NOTIFY importProgressChanged)
    Q_PROPERTY(int importEnd READ getImportEnd NOTIFY importEndChanged)
    Q_PROPERTY(QString rating READ getRating NOTIFY ratingChanged)
public:
    explicit GoogleOAuthHelper(QObject *parent = nullptr);

    enum RequestType {
        None,
        Subscriptions,
        Rating,
        Rate
    };

    struct Request {
        int id;
        int retryCount = 0;
        QString nextPage;
    };

    bool isLinked();
    Q_INVOKABLE void importSubscriptions();
    int getImportProgress() const;
    int getImportEnd() const;
    Q_INVOKABLE void setClientId(QString clientId);
    Q_INVOKABLE void setClientSecret(QString clientSecret);
    Q_INVOKABLE void getRating(QString videoId);
    Q_INVOKABLE void rate(QString videoId, QString rating);
    QString getRating();

private:
    void processRequest(RequestType type, QString data);
    void processSubscriptionsRequest(QString data);
    void processRatingRequest(QString data);

public slots:
    Q_INVOKABLE void link();
    Q_INVOKABLE void unlink();
    void onLinkedChanged();
    void linkingFailed();
    void linkingSucceeded();
    void onOpenBrowser(QUrl url);
    void onCloseBrowser();
    void onRequestFinished(int requestId, QNetworkReply::NetworkError error, QByteArray replyData);

signals:
    void openBrowser(QUrl url);
    void linkedChanged();
    void importProgressChanged();
    void importEndChanged();
    void ratingChanged(QString rating);

private:
    QNetworkAccessManager _nm;
    O2Google _o2Google;
    O2Requestor _o2Requestor;
    QNetworkInfo _networkInfo;
    QHash<RequestType, Request> _pendingRequests;
    int _progress;
    int _progressEnd;
    QVector<QString> _authorsToImport;
    SubscriptionsImportWorker *_importWorker;
    QThread *_importThread;
    QString _rating;
    QString _rateInProcess;
};

#endif // GOOGLEOAUTHHELPER_H
