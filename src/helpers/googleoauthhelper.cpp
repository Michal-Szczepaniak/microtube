#include "googleoauthhelper.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkConfigurationManager>
#include <QNetworkInterface>
#include <QNetworkSession>
#include <QUrlQuery>
#include <o0settingsstore.h>

GoogleOAuthHelper::GoogleOAuthHelper(QObject *parent) : QObject(parent), _nm(this), _o2Google(this),
    _o2Requestor(&_nm, &_o2Google, this), _progress(0), _progressEnd(0), _importWorker(nullptr), _importThread(nullptr), _rating("none"), _rateInProcess("")
{
    if (QSettings().contains("clientId") && QSettings().contains("clientSecret")) {
        _o2Google.setClientId(QSettings().value("clientId").toString());
        _o2Google.setClientSecret(QSettings().value("clientSecret").toString());
    }
    _o2Google.setScope("https://www.googleapis.com/auth/youtube");

    connect(&_o2Google, &O2Google::linkedChanged, this, &GoogleOAuthHelper::linkedChanged);
    connect(&_o2Google, &O2Google::linkingFailed, this, &GoogleOAuthHelper::linkingFailed);
    connect(&_o2Google, &O2Google::linkingSucceeded, this, &GoogleOAuthHelper::linkingSucceeded);
    connect(&_o2Google, &O2Google::openBrowser, this, &GoogleOAuthHelper::onOpenBrowser);
    connect(&_o2Google, &O2Google::closeBrowser, this, &GoogleOAuthHelper::onCloseBrowser);
    connect(&_o2Requestor, static_cast<void (O2Requestor::*)(int,QNetworkReply::NetworkError,QByteArray)>(&O2Requestor::finished), this, &GoogleOAuthHelper::onRequestFinished);
}

bool GoogleOAuthHelper::isLinked()
{
    return _o2Google.linked();
}

void GoogleOAuthHelper::importSubscriptions()
{
    QUrl url = QUrl("https://youtube.googleapis.com/youtube/v3/subscriptions");
    QUrlQuery query(url.query());
    query.addQueryItem("part", "snippet");
    query.addQueryItem("mine", "true");
    query.addQueryItem("maxResults", "50");
    if (_pendingRequests.contains(RequestType::Subscriptions)) {
        query.addQueryItem("pageToken", _pendingRequests[RequestType::Subscriptions].nextPage);
        if (_pendingRequests[RequestType::Subscriptions].retryCount >= 3) {
            _pendingRequests.remove(RequestType::Subscriptions);
            return;
        }
    } else {
        _pendingRequests[RequestType::Subscriptions] = {};
        _progress = 0;
        _progressEnd = 0;
        _authorsToImport.clear();
        emit importProgressChanged();
        emit importEndChanged();
    }
    url.setQuery(query);
    qDebug() << url;

    QNetworkRequest request(url);
    _pendingRequests[RequestType::Subscriptions].id = _o2Requestor.get(request);
}

int GoogleOAuthHelper::getImportProgress() const
{
    return _progress;
}

int GoogleOAuthHelper::getImportEnd() const
{
    return _progressEnd;
}

void GoogleOAuthHelper::setClientId(QString clientId)
{
    QSettings().setValue("clientId", clientId);
    _o2Google.setClientId(clientId);
}

void GoogleOAuthHelper::setClientSecret(QString clientSecret)
{
    QSettings().setValue("clientSecret", clientSecret);
    _o2Google.setClientSecret(clientSecret);
}

void GoogleOAuthHelper::getRating(QString videoId)
{
     QUrl url = QUrl("https://www.googleapis.com/youtube/v3/videos/getRating");
     QUrlQuery query(url.query());
     query.addQueryItem("id", videoId);
     url.setQuery(query);

     QNetworkRequest request(url);
     _pendingRequests[RequestType::Rating].id = _o2Requestor.get(request);
}

void GoogleOAuthHelper::rate(QString videoId, QString rating)
{
    QUrl url = QUrl("https://www.googleapis.com/youtube/v3/videos/rate");
    QUrlQuery query(url.query());
    query.addQueryItem("id", videoId);
    query.addQueryItem("rating", rating);
    url.setQuery(query);

    _rateInProcess = rating;
    QNetworkRequest request(url);
    QByteArray postData;
    _pendingRequests[RequestType::Rate].id = _o2Requestor.post(request, postData);
}

QString GoogleOAuthHelper::getRating()
{
    return _rating;
}

void GoogleOAuthHelper::processRequest(RequestType type, QString data)
{
    switch (type) {
    case RequestType::Subscriptions:
        processSubscriptionsRequest(data);
        break;
    case RequestType::Rating:
        processRatingRequest(data);
        break;
    case RequestType::Rate:
        _rating = _rateInProcess;
        emit ratingChanged(_rateInProcess);
        break;
    case RequestType::None:
        break;
    }
}

void GoogleOAuthHelper::processSubscriptionsRequest(QString data)
{
    QJsonDocument response = QJsonDocument::fromJson(data.toUtf8());

    QJsonObject object = response.object();

    _pendingRequests[RequestType::Subscriptions].nextPage = object["nextPageToken"].toString();

    for (const QJsonValue &item : object["items"].toArray()) {
        if (item.isUndefined()) break;
        QString channelId = item.toObject()["snippet"].toObject()["resourceId"].toObject()["channelId"].toString();

        _authorsToImport.append(channelId);
    }

    int count = object["items"].toArray().count();
    _progressEnd += count;
    emit importEndChanged();

    if (_pendingRequests[RequestType::Subscriptions].nextPage != "") {
        importSubscriptions();
    } else {
        _pendingRequests.remove(RequestType::Subscriptions);
        if (_importWorker == nullptr) {
            _importWorker = new SubscriptionsImportWorker(_authorsToImport, this);
            _importThread = new QThread();

            _importWorker->moveToThread(_importThread);

            connect(_importThread, &QThread::started, _importWorker, &SubscriptionsImportWorker::import);
            connect(_importWorker, &SubscriptionsImportWorker::finished, _importThread, &QThread::quit);
            connect(_importWorker, &SubscriptionsImportWorker::progressChanged, [&](int progress){ _progress = progress; emit importProgressChanged(); });

            connect(_importWorker, &SubscriptionsImportWorker::finished, _importWorker, &SubscriptionsImportWorker::deleteLater);
            connect(_importWorker, &SubscriptionsImportWorker::destroyed, [&](){ _importWorker = nullptr; });
            connect(_importThread, &QThread::destroyed, [&](){ _importThread = nullptr; });
            connect(_importThread, &QThread::finished, _importThread, &QThread::deleteLater);

            _importThread->start();
        }
    }
}

void GoogleOAuthHelper::processRatingRequest(QString data)
{
    QJsonDocument response = QJsonDocument::fromJson(data.toUtf8());

    QJsonObject object = response.object();

    _rating = object["items"].toArray().first().toObject()["rating"].toString();

    emit ratingChanged(_rating);
}

void GoogleOAuthHelper::link()
{
    _o2Google.link();
}

void GoogleOAuthHelper::unlink()
{
    _o2Google.unlink();
}

void GoogleOAuthHelper::onLinkedChanged()
{
    qDebug() << "GoogleOAuthHelper::linkedChanged()";
    emit linkedChanged();
}

void GoogleOAuthHelper::linkingFailed()
{
    qDebug() << "GoogleOAuthHelper::linkingFailed()";
}

void GoogleOAuthHelper::linkingSucceeded()
{
    qDebug() << "GoogleOAuthHelper::linkingSucceeded()";
}

void GoogleOAuthHelper::onOpenBrowser(QUrl url)
{
    qDebug() << "GoogleOAuthHelper::onOpenBrowser()";
    emit openBrowser(url);
}

void GoogleOAuthHelper::onCloseBrowser()
{
    qDebug() << "GoogleOAuthHelper::onCloseBrowser()";
}

void GoogleOAuthHelper::onRequestFinished(int requestId, QNetworkReply::NetworkError error, QByteArray replyData)
{
    RequestType type = RequestType::None;
    for (QHash<RequestType, Request>::iterator it = _pendingRequests.begin(); it != _pendingRequests.end(); it++) {
        if (it->id == requestId) {
            type = it.key();
            break;
        }
    }

    if (error != QNetworkReply::NoError) {
        qWarning() << "Reply error:" << error;
        switch (type) {
        case RequestType::None:
        case RequestType::Rating:
        case RequestType::Rate:
            qDebug() << replyData;
            break;
        case RequestType::Subscriptions:
            _pendingRequests[RequestType::Subscriptions].retryCount++;
            importSubscriptions();
            break;
        }
        return;
    }

    if (type != RequestType::None)
        processRequest(type, QString(replyData));
}
