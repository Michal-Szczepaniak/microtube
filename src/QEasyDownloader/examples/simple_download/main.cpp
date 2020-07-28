#include <QCoreApplication>
#include <QEasyDownloader>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    /*
     * Construct
    */
    QEasyDownloader Downloader;

    /*
     * By Default Debug is false , make it true to print the download progress and
     * other stuff!
    */
    Downloader.setDebug(true);

    /*
     * By Default auto Resuming of Downloads is true.
     *
     * You can also disable auto resuming of downloads.
     * But I strongly recommend you don't!
    */
    // Downloader.setResumeDownloads(false);

    /*
     * Connect Callbacks!
    */
    QObject::connect(&Downloader, &QEasyDownloader::Debugger,
    [&](QString msg) {
        qDebug() << msg;
        return;
    });
    QObject::connect(&Downloader, &QEasyDownloader::DownloadFinished,
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
