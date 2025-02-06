#include "videodownloader.h"

#include <QNetworkRequest>
#include <QSettings>


VideoDownloader::VideoDownloader(QObject *parent) : QObject(parent), _downloadFile(nullptr), _reply(nullptr), _status(Null), _progress(0)
{
    connect(&_jsProcessHelper, &JSProcessManager::gotVideoInfo, this, &VideoDownloader::doDownload);
}

void VideoDownloader::download(QString url, QString path)
{
    if (_downloadFile != nullptr) return;

    setDownloadStatus(DownloadStatus::Progress);
    emit downloadStarted(path.split('/').last());

    if (QFile::exists(path)) {
        QFile::remove(path);
    }

    _downloadFile = new QFile(path);

    Search search;
    search.country = QSettings().value("country", "US").toString();
    search.safeSearch = QSettings().value("safeSearch", false).toBool();
    search.type = Search::VideoInfo;
    search.query = url;
    _jsProcessHelper.asyncGetVideoInfo(search);
}

VideoDownloader::DownloadStatus VideoDownloader::getDownloadStatus() const
{
    return _status;
}

void VideoDownloader::setDownloadStatus(DownloadStatus status)
{
    _status = status;

    emit downloadStatusChanged();
}

double VideoDownloader::getDownloadProgress() const
{
    return _progress;
}

void VideoDownloader::setDownloadProgress(double progress)
{
    _progress = progress;

    emit downloadProgressChanged();
}

void VideoDownloader::doDownload(QHash<int, QString> formats)
{
    QString url;
    for (QHash<int, QString>::iterator it = formats.begin(); it != formats.end(); it++) {
        if (it.key() == 22 || it.key() == 18) {
            qDebug() << "Selecting download format: " << it.key();
            url = it.value();
            break;
        }
    }
    if (url == "") {
        setDownloadStatus(DownloadStatus::Failed);
        delete _downloadFile;
        _downloadFile = nullptr;
        return;
    }

    QUrl qurl(url);
    QNetworkRequest networkRequest(qurl);
    _reply = _nam.get(networkRequest);

    if (_downloadFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
        connect(_reply, &QNetworkReply::readyRead, [this]() {
            while (_reply->bytesAvailable()) {
                _downloadFile->write(_reply->read(1048576));
            }
        });
        connect (_reply, &QNetworkReply::downloadProgress, [this](quint64 bytesReceived, quint64 bytesTotal) {
            setDownloadProgress(((double)bytesReceived)/bytesTotal);
        });
        connect(_reply, &QNetworkReply::finished, [this]() {
            _downloadFile->close();
            delete _downloadFile;
            _downloadFile = nullptr;

            if (_reply->error() == QNetworkReply::NoError) {
                setDownloadStatus(DownloadStatus::Finished);
            } else {
                qWarning() << _reply->errorString();
                setDownloadStatus(DownloadStatus::Failed);
            }
        });
    } else {
        setDownloadStatus(DownloadStatus::Failed);
        delete _downloadFile;
        _downloadFile = nullptr;
        return;
    }
}
