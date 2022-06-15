#ifndef VIDEODOWNLOADER_H
#define VIDEODOWNLOADER_H

#include <QObject>
#include <QFile>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include "helpers/jsprocesshelper.h"

class VideoDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DownloadStatus downloadStatus READ getDownloadStatus NOTIFY downloadStatusChanged)
    Q_PROPERTY(double downloadProgress READ getDownloadProgress NOTIFY downloadProgressChanged)
public:
    enum DownloadStatus {
        Null,
        Progress,
        Finished,
        Failed
    };
    Q_ENUM(DownloadStatus)

    explicit VideoDownloader(QObject *parent = nullptr);

    Q_INVOKABLE void download(QString url, QString path);
    DownloadStatus getDownloadStatus() const;
    void setDownloadStatus(DownloadStatus status);
    double getDownloadProgress() const;
    void setDownloadProgress(double progress);

private slots:
    void doDownload(QHash<int, QString> formats);

signals:
    void downloadStatusChanged();
    void downloadProgressChanged();
    void downloadStarted(QString filename);

private:
    JSProcessHelper _jsProcessHelper;
    QFile *_downloadFile;
    QNetworkAccessManager _nam;
    QNetworkReply *_reply;
    DownloadStatus _status;
    double _progress;
};

#endif // VIDEODOWNLOADER_H
