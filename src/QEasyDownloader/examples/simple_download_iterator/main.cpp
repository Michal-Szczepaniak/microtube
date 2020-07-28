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
     * Connect Callbacks!
    */

    QObject::connect(&Downloader, &QEasyDownloader::Finished,
    [&]() {
        qDebug() << "Downloaded Everything!";
        app.quit();
    });

    QObject::connect(&Downloader, &QEasyDownloader::DownloadFinished,
    [&](QUrl Url, QString file) {
        qDebug() << "Downloaded :: " << file << " :: FROM :: " << Url;
        if(Downloader.HasNext()) {
            qDebug() << "Downloading the Next Download... ";
        } else {
            qDebug() << "Finishing All Downloads... ";
        }
        Downloader.Next();
    });

    Downloader.setIterated(true); // Make it iterated

    /*
     * Just Download!
    */
    Downloader.Download("http://sample-videos.com/video/mp4/720/big_buck_bunny_720p_5mb.mp4");
    Downloader.Download("http://sample-videos.com/video/mp4/720/big_buck_bunny_720p_5mb.mp4", "Next.mp4");
    return app.exec();
}
