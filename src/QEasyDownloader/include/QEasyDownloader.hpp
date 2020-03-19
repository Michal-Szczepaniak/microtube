/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2018, Antony jr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -----------------------------------------------------------------------------
 *  @filename 	 	: QEasyDownloader.cc
 *  @description 	: Header for QEasyDownloader.
 * -----------------------------------------------------------------------------
*/
#if !defined(QEASY_DOWNLOADER_HPP_INCLUDED)
#define QEASY_DOWNLOADER_HPP_INCLUDED
#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#define NONEED(x) (void)x

// Just for readability.
#define ENTRY_URL 0
#define ENTRY_FILELOCATION 1

/*
 * Class QEasyDownloader <- Inherits QObject
 * --------------------
 *
 *  The main class that handles downloads like a pro.
 *
 *  Constructors:
 *  	QEasyDownloader(QObject *parent = NULL , QNetworkAccessManager *toUseManager = NULL);
 *
 * 	Assigns a parent if the user gives one or assigns a QNetworkAccessManager if the user
 * 	already uses a single NetworkManager for his/her application.
 *
 *  Methods:
 *  	void setDebug(bool) -  Enable or Disable Debuging
 *  	void setIterated(bool) - Enable iterated Downloading.
 *  			      ( i.e Download a file in Queue , Stop , Get Approved then Download again.)
 *  	void setResumeDownloads(bool) - Enable or Disable Resuming of Downloads!
 *
 *  	Warning: Disabling Resuming of Downloads will overwrite the file if found!
 *
 * 	    void setTimeoutTime(int) - sets the timeout time (in miliseconds) for a request! default is 5000 = 5 secs
 * 	    void setRetryTime(int)   - sets the retry time (in miliseconds) for a request! default is 6000 = 6 secs
 *
 *  Public Slots:
 *	    void Download(const QString& , const QString&) - Download a file and save it in the location provided.
 *	    void Download(const QString&) - Simply download a file.
 *	    void Pause() - Pause the current download.
 *	    void Resume() - Resume any paused download.
 *	    bool HasNext() - If iterated is true then this function will return true if download queue is not empty.
 *	    void Next() - Downloads the next entry in the download queue.
 *
 *
 *  Signals:
 *  	void Finished() - Emitted when all jobs are done.
 *  	void DownloadFinished(const QUrl &url, const QString& fileName) - Emitted when a single file is downloaded.
 *  	void DownloadProgress(qint64 bytesReceived,
 *                        qint64 bytesTotal,
 *                        int percent,
 *                        double speed,
 *                        const QString &unit,
 *                        const QUrl &url,
 *                        const QString &fileName) - Full Download Progress , Emitted on every download.
 *      void Error(QNetworkReply::NetworkError errorCode,
 *  	           const QUrl &url,
 *  	           const QString &fileName) - Emitted on error.
 *      void Timeout(const QUrl &url, const QString &fileName) - Emitted when there is a timeout.
 *
 *      void Paused(const QUrl &url , const QString &fileName) - Emitted when paused a download.
 *      void Resumed(const QUrl &url , const QString &fileName) - Emitted when resumed a download.
 *      void Debugger(const QString &msg) - Emitted when a debug message is passed.
 *
 *
*/
class QEasyDownloader : public QObject
{
    Q_OBJECT
public:
    explicit QEasyDownloader(QObject *parent = nullptr, QNetworkAccessManager *toUseManager = nullptr);
    void setDebug(bool);
    void setIterated(bool);
    void setResumeDownloads(bool);
    void setTimeoutTime(int);
    void setRetryTime(int);
    void setDownloadPath(const QString &qsFolderPath);
    ~QEasyDownloader();

private slots:
    void download();
    void checkHead(qint64,qint64);
    void finished();
    void downloadProgress(qint64,qint64);
    void startNextDownload();
    void retry(QNetworkAccessManager::NetworkAccessibility);
    void error(QNetworkReply::NetworkError);
    void printDebug(const QString&);
    void timeout();
    QString saveFileName(const QString&);
public slots:
    void Download(const QString&, const QString&);
    void Download(const QString&);
    void Pause();
    void Resume();
    bool HasNext();
    void Next();
signals:
    void Finished();
    void Debugger(const QString&);
    void Paused(const QUrl&,const QString&);
    void Resumed(const QUrl&,const QString&);
    void DownloadFinished(const QUrl&, const QString&);
    void DownloadProgress(qint64,qint64,int,double,const QString&,const QUrl&,const QString&);
    void Error(QNetworkReply::NetworkError errorCode, const QUrl &url, const QString &fileName);
    void Timeout(const QUrl &url, const QString &fileName);
private:
    QMutex mutex;
    QNetworkAccessManager    *_pManager = nullptr;
    QNetworkRequest           _CurrentRequest;
    QNetworkReply            *_pCurrentReply = nullptr,
                              *_pCurrentGetReply = nullptr;
    QFile		     *_pFile = nullptr;

    QTimer _Timer;
    QTime  _downloadSpeed;
    QUrl    _URL;
    QString _qsFileName, _qsFolderPath;
    QQueue<QStringList> _downloadQueue;

    int _nDownloadTotal = 0,
        _nDownloadSize = 0,
        _nDownloadSizeAtPause = 0,
        _nTimeoutTime = 5000,
        _nRetryTime = 6000;
    bool _bAcceptRanges = false,
         _bStopDownload = false,
         _bIsError = false,
         _bDoResumeDownloads = true,
         _bAutoStartDownload = true,
         _bDoIterate = false,
         _bCanIterate = false,
         _bDoDebug = false;
};  // Class QEasyDownloader END
#endif // QEASY_DOWNLOADER_HPP_INCLUDED
