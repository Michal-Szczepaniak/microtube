---
id: UsingDownloader
title: Simple Download with QEasyDownloader
sidebar_label: A Simple Downloader in QEasyDownloader
---

This is a basic downloader which downloads a demo video and saves it in the current working directory.

## main.cpp

```
#include <QCoreApplication>
#include <QEasyDownloader>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    /*
     * Construct
    */
    QEasyDownloader Downloader;

    Object::connect(&Downloader, &QEasyDownloader::DownloadFinished,
    [&](QUrl Url, QString file) {
        qDebug() << "Downloaded :: " << file << " :: FROM :: " << Url;
        app.quit();
    });
    /*
     * Just Download!
    */
    Downloader.Download("http://sample-videos.com/video/mp4/720/big_buck_bunny_720p_5mb.mp4");
    return app.exec();
}
```

## simple_download.pro

```
TARGET = simple_download
TEMPLATE = app

QT += core network
INCLUDEPATH += QEasyDownloader QEasyDownloader/include 
SOURCES += main.cpp QEasyDownloader/src/QEasyDownloader.cc
HEADERS += QEasyDownloader/include/QEasyDownloader.hpp
```

## Compilation and Execution

```
 $ mkdir build
 $ cd build
 $ qmake ..
 $ make -j4
 $ ./simple_download
 $ # now you will have big_buck_bunny_720p_5mb.mp4 saved
 $ # Try aborting the download and starting it again
```

This Program is created in the examples tree on the official repo.

