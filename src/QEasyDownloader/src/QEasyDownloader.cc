/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2018, Antony jr
 * Copyright (c) 2018, Michał Szczepaniak <m.szczepaniak.000@gmail.com>
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
 *  @description 	: Source for QEasyDownloader.
 * -----------------------------------------------------------------------------
*/
#include <QEasyDownloader.hpp>

/*
 * Constructor and Destructor
 * ---------------------------
 *
*/
QEasyDownloader::QEasyDownloader(QObject *parent, QNetworkAccessManager *toUseManager)
    : QObject(parent)
{
    /*
     * Since a whole Qt Application only requires a single QNetworkAccessManager ,
     * We have to give users to utilise this feature and avoid overhead.
     */
    _pManager = (toUseManager == nullptr) ? new QNetworkAccessManager(this) : toUseManager;

    /*
     * Makes the requests follow urls
    */
//    _pManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

    /*
     * Automatically Handles Network Interruption!
    */
    connect(_pManager, &QNetworkAccessManager::networkAccessibleChanged, this, &QEasyDownloader::retry);
    return;
}

QEasyDownloader::~QEasyDownloader()
{
    /*
     * Since Qt uses parent to child deallocation , We don't need to deallocate
     * our children manually , When QEasyDownloader object is deallocated our
     * children also get deallocated.
     *
     * Also to avoid double-free corruption.
    */
    return;
}

/*
 * ------
*/

/*
 * Public Methods
 * --------------------------
*/
void QEasyDownloader::setDebug(bool ch)
{
    QMutexLocker locker(&mutex);
    _bDoDebug = ch;
    return;
}

void QEasyDownloader::setIterated(bool ch)
{
    QMutexLocker locker(&mutex);
    _bDoIterate = ch;
    return;
}

void QEasyDownloader::setResumeDownloads(bool ch)
{
    QMutexLocker locker(&mutex);
    _bDoResumeDownloads = ch;
    return;
}

void QEasyDownloader::setTimeoutTime(int time)
{
    QMutexLocker locker(&mutex);
    _nTimeoutTime = time;
    return;
}

void QEasyDownloader::setRetryTime(int time)
{
    QMutexLocker locker(&mutex);
    _nRetryTime = time;
    return;
}

void QEasyDownloader::setDownloadPath(const QString &qsFolderPath)
{
    QMutexLocker locker(&mutex);
    _qsFolderPath = qsFolderPath;
    return;
}

/*
 * ------
*/

/*
 * Private Slots
 * ---------------------
*/
void QEasyDownloader::download()
{
    if (_bAcceptRanges) {
        /*
         * Debug
        */
        printDebug("It seems the server supports 'Range' requests.");
        // ---
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(_nDownloadSizeAtPause) + "-";
        if (_nDownloadTotal > 0) {
            rangeHeaderValue += QByteArray::number(_nDownloadTotal);
        }
        _CurrentRequest.setRawHeader("Range", rangeHeaderValue);
    } else {
        // Debug.
        printDebug("The server does not support 'Range' requests , Therefore downloading the entire file.");
        // ---
    }

    _pCurrentReply = _pManager->get(_CurrentRequest);

    _Timer.setInterval(_nTimeoutTime);
    _Timer.setSingleShot(true);

    connect(&_Timer, SIGNAL(timeout()), this, SLOT(timeout()));

    _Timer.start();
    _downloadSpeed.start();

    connect(_pCurrentReply, SIGNAL(finished()), this, SLOT(finished()));
    connect(_pCurrentReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
    connect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    return;
}

void QEasyDownloader::checkHead(qint64 bytesRecived, qint64 bytesTotal)
{

    NONEED(bytesRecived); // We do not need any progress.

    /*
     * Disconnect the reply as soon as possible since it may cause collison.
    */
    disconnect(_pCurrentReply, &QNetworkReply::downloadProgress, this, &QEasyDownloader::checkHead);

    _Timer.stop();
    _bAcceptRanges = false;

    _nDownloadTotal = bytesTotal; // less expensive than parsing the content length header.
    if (_pCurrentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() >= 400) {
        /*
         * Debug
        */
        printDebug("HTTP(S) Response code seems to be >= 400 , Therefore moving onto the next.");
        // ---

        if (!_bDoIterate) {
            QTimer::singleShot(0, this, SLOT(startNextDownload()));
        } else {
            _bCanIterate = true;
        }
        return;
    }

    if (_pCurrentReply->hasRawHeader("Accept-Ranges")) {
        QString qstrAcceptRanges = _pCurrentReply->rawHeader("Accept-Ranges");
        _bAcceptRanges = (qstrAcceptRanges.compare("bytes", Qt::CaseInsensitive) == 0);
    }

    /*
     * Delete it little later.
    */
    _pCurrentReply->abort(); // stop the request.
    _pCurrentReply->deleteLater();
    _pCurrentReply = nullptr;

    /*
     * Set the new request to download the file.
     */
    _CurrentRequest.setRawHeader("Connection", "Keep-Alive");
    _CurrentRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    _pFile = new QFile(_qsFileName);

    /*
     * Check if we want to delete the old file.
    */
    if (!_bAcceptRanges) {
        _pFile->remove();
    }
    if (!_bDoResumeDownloads) {
        _pFile->remove();
    }

    _pFile->open(QIODevice::ReadWrite | QIODevice::Append);
    _nDownloadSizeAtPause = _pFile->size();

    /*
     * If the total download size and download size at pause
     * is equal then the file is fully retrived so no need
     * to range request it again as it may give UnknownContentError
    */
    if (_nDownloadTotal == _nDownloadSizeAtPause) {
        emit(finished());
        return;
    }
    QTimer::singleShot(0, this, SLOT(download()));
    return;
}

void QEasyDownloader::finished()
{
    if (_bIsError) {
        _bIsError = false;
        return;
    }

    _Timer.stop();
    _pFile->close();
    _pFile = nullptr;
    _pCurrentReply = 0;

    if (!_bDoIterate) {
        startNextDownload();
    } else {
        _bCanIterate = true;
    }

    /*
     * Debug
    */
    printDebug("Finishing:: " + _URL.toString() + " -> " + _qsFileName);
    // ---

    emit(DownloadFinished(_URL, _qsFileName));
    return;
}

void QEasyDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    _Timer.stop();

    _nDownloadSize = _nDownloadSizeAtPause + bytesReceived;
    _pFile->write(_pCurrentReply->readAll());

    int nPercentage =
        static_cast<int>(
            (static_cast<float>
             (
                 _nDownloadSizeAtPause + bytesReceived
             ) * 100.0
            ) / static_cast<float>
            (
                _nDownloadSizeAtPause + bytesTotal
            )
        );

    double speed = bytesReceived * 1000.0 / _downloadSpeed.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024 * 1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024 * 1024;
        unit = "MB/s";
    }

    emit(DownloadProgress(bytesReceived,
                          bytesTotal,
                          nPercentage,
                          speed,
                          unit,
                          _URL,
                          _qsFileName));
    _Timer.start(_nTimeoutTime);
    return;
}

void QEasyDownloader::startNextDownload()
{
    if (_bStopDownload) {
        return;
    }

    if (_downloadQueue.isEmpty()) {
        _bAutoStartDownload = true;
        emit(Finished());
        return;
    }

    QStringList DownloadInformation = _downloadQueue.dequeue();

    _URL = QUrl(DownloadInformation.at(ENTRY_URL));
    _qsFileName = DownloadInformation.at(ENTRY_FILELOCATION);

    if (_URL.isEmpty() || _qsFileName.isEmpty()) {
        /*
         * Debug
        */
        printDebug("URL is empty , Therefore moving onto the next.");
        // ---
        QTimer::singleShot(0, this, SLOT(startNextDownload()));
        return;
    }

    /*
     * Debug
    */
    printDebug("Starting:: " + _URL.toString() + " -> " + _qsFileName);
    // ---

    /*
     * You may ask why we are not using HEAD ?
     * Because in some servers HEAD request is not supported or
     * not allowed but **range request** may be allowed.
     * Example:- Amazon AWS.
     *
     * So to solve this , We check the head by giving a get request
     * and abort it in a very short time. Getting all the information
     * like HEAD but having the advantages of GET.
    */
    _nDownloadSize = _nDownloadSizeAtPause = 0;
    _CurrentRequest = QNetworkRequest(_URL);
    _pCurrentReply = _pManager->get(_CurrentRequest);
    _Timer.setInterval(_nTimeoutTime);
    _Timer.setSingleShot(true);

    connect(&_Timer, &QTimer::timeout, this, &QEasyDownloader::timeout);
    _Timer.start();

    connect(_pCurrentReply, &QNetworkReply::downloadProgress, this, &QEasyDownloader::checkHead);
    connect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    return;
}

void QEasyDownloader::retry(QNetworkAccessManager::NetworkAccessibility access)
{
    if (access == QNetworkAccessManager::NotAccessible || access == QNetworkAccessManager::UnknownAccessibility) {
        /*
         * Debug
        */
        printDebug("NetworkInterruption:: Download will Automatically Restart When Internet is Available ");
        // ---

        _bIsError = false;
        QTimer::singleShot(0, this, SLOT(Pause()));
        return;
    }
    QTimer::singleShot(_nRetryTime, this, SLOT(Resume()));
    return;
}


void QEasyDownloader::error(QNetworkReply::NetworkError errorCode)
{
    /*
     * Avoid Operation cancel errors.
    */
    if (errorCode == QNetworkReply::OperationCanceledError) {
        return;
    }

    /*
     * Debug
    */
    printDebug("Error:: " + _URL.toString() + " (Error Code -> " + QString::number(errorCode) + " )");
    // ---

    _bIsError = true;
    emit(Error(errorCode, _URL, _qsFileName));
    return;
}

void QEasyDownloader::timeout()
{
    emit(Timeout(_URL, _qsFileName));

    /*
     * Debug
    */
    printDebug("Timeout:: " + _URL.toString());
    // ---

    return;
}

QString QEasyDownloader::saveFileName(const QString &url)
{
    QString path = QUrl(url).path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty()) {
        /*
         * Debug
        */
        printDebug("Cannot get the base name from " + url + " , using 'download' as basename.");
        // ---

        basename = "download";
    }
    return basename;
}

void QEasyDownloader::printDebug(const QString &msg)
{
    if (_bDoDebug) {
        emit(Debugger(msg));
    }
    return;
}

/*
 * ------
*/

/*
 * Public Slots
 * ---------------------
*/

void QEasyDownloader::Download(const QString &givenURL, const QString &fileName)
{
    QMutexLocker locker(&mutex);
    _downloadQueue.enqueue(QStringList() << givenURL
                           << (
                               (_qsFolderPath.isEmpty()) ?
                               fileName : _qsFolderPath + "/" + fileName
                           )
                          );

    /*
     * Debug
    */
    printDebug("Queued:: " + givenURL + " -> " + fileName);
    // ---

    if (_bAutoStartDownload) { // Do not use _downloadQueue.size() == 1.
        _bAutoStartDownload = false;
        QTimer::singleShot(0, this, SLOT(startNextDownload()));
    }
    return;
}

void QEasyDownloader::Download(const QString &givenURL)
{
    Download(givenURL, saveFileName(givenURL));
    return;
}

void QEasyDownloader::Pause()
{
    QMutexLocker locker(&mutex);
    if (_pCurrentReply == nullptr || _bStopDownload) {
        return;
    }

    _Timer.stop();
    disconnect(&_Timer, &QTimer::timeout, this, &QEasyDownloader::timeout);
    disconnect(_pCurrentReply, &QNetworkReply::finished, this, &QEasyDownloader::finished);
    disconnect(_pCurrentReply, &QNetworkReply::downloadProgress, this, &QEasyDownloader::downloadProgress);
    disconnect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    _pCurrentReply->abort();
    _pFile->flush();
    _pCurrentReply = 0;
    _nDownloadSizeAtPause = _nDownloadSize;
    _nDownloadSize = 0;
    _bStopDownload = true;

    /*
     * Debug and Reporting!
    */

    printDebug("Paused:: " + _URL.toString());
    emit(Paused(_URL, _qsFileName));

    // ----

    return;
}

void QEasyDownloader::Resume()
{
    QMutexLocker locker(&mutex);
    if (!_bStopDownload || _pCurrentReply != nullptr) {
        return;
    }
    _bStopDownload = false;
    QTimer::singleShot(0, this, SLOT(download()));

    /*
     * Debug and Reporting!
    */

    printDebug("Resumed:: " + _URL.toString());
    emit(Resumed(_URL, _qsFileName));

    // ----

    return;
}


bool QEasyDownloader::HasNext()
{
    QMutexLocker locker(&mutex);
    return !_downloadQueue.isEmpty();
}

void QEasyDownloader::Next()
{
    QMutexLocker locker(&mutex);
    if (!_bDoIterate) {
        return;
    }

    if (_bCanIterate) {
        QTimer::singleShot(0, this, SLOT(startNextDownload()));
        _bCanIterate = false;
    }
    return;
}

/*
 * ---------------
*/
