#include "downloader.h"
#include <QFile>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    connect(
        &m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
        this, SLOT (fileDownloaded(QNetworkReply*))
    );
}

Downloader::~Downloader() { }

void Downloader::download(QUrl url, QString name) {
    QNetworkRequest request(url);
    m_WebCtrl.get(request);
}

void Downloader::fileDownloaded(QNetworkReply* pReply) {
 m_DownloadedData = pReply->readAll();
 pReply->deleteLater();
 emit downloaded();
}

QByteArray Downloader::downloadedData() const {
 return m_DownloadedData;
}
