#ifndef YTJSNAMFACTORY_H
#define YTJSNAMFACTORY_H

#include <QtQml>

class JSNAMFactory : public QQmlNetworkAccessManagerFactory {
public:
    QNetworkAccessManager *create(QObject *parent);

    void setRequestHeaders(QMap<QByteArray, QByteArray> value) { requestHeaders = value; };
    const QMap<QByteArray, QByteArray> &getRequestHeaders() const { return requestHeaders; }

private:
    QMap<QByteArray, QByteArray> requestHeaders;
};

class JSNAM : public QNetworkAccessManager {
    Q_OBJECT

public:
    JSNAM(QObject *parent, const JSNAMFactory &factory);

protected:
    QNetworkReply *
    createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

private:
    const JSNAMFactory &factory;
};

class JSDiskCache : public QNetworkDiskCache {
public:
    JSDiskCache(QObject *parent);
    void updateMetaData(const QNetworkCacheMetaData &meta);
    QIODevice *prepare(const QNetworkCacheMetaData &meta);

private:
    QNetworkCacheMetaData fixMetadata(const QNetworkCacheMetaData &meta);
};

#endif // YTJSNAMFACTORY_H
