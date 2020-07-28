---
id: ClassQEasyDownloader
title: Class QEasyDownloader
sidebar_label: Class QEasyDownloader
---

This is the only **class** that is provided by **QEasyDownloader** , this takes care of everything you   
want in a single class and thus its **easier** for you to **remember**.


## Public Functions

|	         	|				                                         						       |
|---------------|--------------------------------------------------------------------------------------|
| **explicit**  | QEasyDownloader(QObject *parent = nullptr , QNetworkAccessManager *toUseManager = nullptr)|
| **void**	    | setDebug(bool choose)								                                   |
| **void**	    | setResumeDownloads(bool choose)							                               |
| **void** 	    | setIterated(bool choose)										       |
| **void**	    | setTimeoutTime(int timeInMiliSecs)						                           |
| **void**	    | setRetryTime(int timeInMiliSecs)						                               |
| **void**      | setDownloadPath(const QString &path)                                                 |


### Slots
|		        |	    							                                                   |
|---------------|--------------------------------------------------------------------------------------|
| **void**	| Download(const QString& givenUrl , const QString& saveAs)			                       |
| **void** 	| Download(const QString& givenUrl)						                                   |
| **void**	| Pause(void)									                                           |
| **void**	| Resume(void)									                                           |
| **void** 	| Next(void) 														   |
| bool      | HasNext(void)                                                                            |

### Signals

|	         	|	     										                                                                                      |
|---------------|-------------------------------------------------------------------------------------------------------------------------------------|
| **void**	    | Finished(void) 									                                                                                  |
| **void** 	    | DownloadFinished(const QUrl &url, const QString& fileName)			                                                           	  |
| **void**	    | DownloadProgress(qint64 received,qint64 total,int percent,double speed,const QString &unit,const QUrl &url,const QString &fileName) |
| **void**      | Error(QNetworkReply::NetworkError **[errorCode](QEasyDownloaderErrorCodes.md)** , const QUrl &url, const QString &fileName)         |
| **void**      | Timeout(const QUrl &url, const QString &fileName)                                                                                   |
| **void**      | Paused(const QUrl &url , const QString &fileName)                                                                 |
| **void**      | Resumed(const QUrl &url , const QString &fileName)                                                                |
| **void**      | Debugger(const QString &msg)                                                                                      |


## Member Functions Documentation

#### explicit QEasyDownloader(QObject *parent = NULL, QNetworkAccessManager *toUseManager = NULL) 

The default Constructor for **QEasyDownloader** , takes no argument until the user gives it!

#### void setDebug(bool choose)

Sets debugging for **QEasyDownloader**.

> Default is set to **False**

#### void setResumeDownloads(bool choose)

Sets whether to resume download ( **Partial Download** ) or not.

#### void setIterated(bool choose)

Only starts the first download automatically , Then you have to iterate over the downloader.
In this sequence , Download , When Finished , Approve the next Download and so on until **Finished()** Signal is emitted.

#### void setTimeoutTime(int timeInMiliSecs)

Sets the **timeout** for a **request** , when timeout occurs **Timeout** signal is emitted.

> Default time is 5 seconds.

#### void setRetryTime(int timeInMiliSecs)

Sets the **retry** interval , when there is a **network interruption** the download is **paused** and when **network is accessible**
again , After the **retry** interval , The download is **resumed**

> Default time is 6 seconds.

#### void setDownloadPath(const QString &path)

Sets the **download path** for **all downloads** , This is useful if you need to set a defaul download path for your downloader.

> Default is the current working directory.


#### void	Download(const QString& givenUrl , const QString& saveAs)
<p align="right"> <b>[SLOT]</b> </p>

Starts the download right away if the **queue** is **empty** , if **queue** is not empty then adds the download to the **queue**.
Downloads from the **givenUrl** and saves it to **saveAs**(Path).

#### void	Download(const QString& givenUrl)
<p align="right"> <b>[SLOT]</b> </p>

This is an overload function. This derives the **filename** automatically from the **givenUrl**.

#### void Pause(void)
<p align="right"> <b>[SLOT]</b> </p>

Pauses any current download.

#### void Resume(void)
<p align="right"> <b>[SLOT]</b> </p>

Resumes if any download is paused.

#### bool HasNext(void)
<p align="right"> <b>[SLOT]</b> </p>

Returns **true** if download queue is not empty.

#### void Next(void)
<p align="right"> <b>[SLOT]</b> </p>

Download the next item in the download queue. Only enabled when **Iterated(bool)** is set to **True**.

#### void Finished(void)
<p align="right"> <b>[SIGNAL]</b> </p>

This is emitted when all downloads are finished in the **queue**.

#### void DownloadFinished(const QUrl &url, const QString& fileName)
<p align="right"> <b>[SIGNAL]</b> </p>

This is emitted when a single download is finished in the **queue**. Emitted with the **given url** and the **saved filename**.

#### void DownloadProgress(qint64 received,qint64 total,int percent,double speed,const QString &unit,const QUrl &url,const QString &fileName)
<p align="right"> <b>[SIGNAL]</b> </p>

This is emitted on each download progress with **the given url** and **filename** to identify it.

> **received** : The bytes recived.   
> **total**    : Total bytes to be retrived.   
> **percent**  : The percent of file downloaded from the url.   
> **speed**    : The speed of the download of the current progress.   
> **unit**     : The unit that needs to be used to represent **speed**.(ex: kbps/s)   
> **url**      : The current download url.
> **filename** : The filename which is beign writen to.

#### void Error(QNetworkReply::NetworkError **[errorCode](QEasyDownloaderErrorCodes.md)** , const QUrl &url, const QString &fileName)
<p align="right"> <b>[SIGNAL]</b> </p>

This is emitted when something goes wrong with the download , refer the  **[error codes](QEasyDownloaderErrorCodes.md)** for more information.

#### void Timeout(const QUrl &url, const QString &fileName)
<p align="right"> <b>[SIGNAL]</b> </p>

This is emitted when there is a **timeout** in the **request**.

#### void Paused(const QUrl &url , const QString &fileName)
<p align="right"> <b>[SIGNAL]</b> </p>

Emitted when a download is **paused** successfully.

#### void Resumed(const QUrl &url , const QString &fileName)
<p align="right"> <b>[SIGNAL]</b> </p>

Emitted when a download is **resumed** successfully.

#### void Debugger(const QString &msg)
<p align="right"> <b>[SIGNAL]</b> </p>

Emitted when a message is pushed to the debugger.
