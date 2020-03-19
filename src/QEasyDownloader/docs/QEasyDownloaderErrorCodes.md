---
id: QEasyDownloaderErrorCodes
title: Dealing with Errors on QEasyDownloader
sidebar_label: QEasyDownloader Error Codes
---

QEasyDownloader uses **[QNetworkAccessManager](http://doc.qt.io/qt-5/qnetworkaccessmanager.html)** for communication and downloading files and thus **error codes** are from **QNetworkReply::NetworkError**. The **Error** slot is emitted with **QNetworkReply::NetworkError** code and the **url** which it caused the error.

Please refer **[QNetworkReply::NetworkError](http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum)** to get the meaning of the errors **generated** by **QEasyDownloader**

> **Note:** Enabling **debuging** true will automatically print errors , most of the Network   
> Interruption is handled by **QEasyDownloader** itself!


