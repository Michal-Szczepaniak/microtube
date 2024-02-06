#include <QtQuick>
#include <sailfishapp.h>
#include "models/playlistmodel.h"
#include "models/categoriesmodel.h"
#include "helpers/userfileshelper.h"
#include "sponsorblock.h"
#include "entities/video.h"
#include "volume/pulseaudiocontrol.h"
#include "player/player.h"
#include "helpers/quickviewhelper.h"
#include "helpers/videohelper.h"
#include "entities/author.h"
#include "helpers/channelhelper.h"
#include "entities/caption.h"
#include <QtSql/QSqlDatabase>
#include "repositories/authorrepository.h"
#include "repositories/videorepository.h"
#include "services/subscriptionsaggregator.h"
#include "models/subscriptionsmodel.h"
#include "helpers/googleoauthhelper.h"
#include "models/commentsmodel.h"
#include "services/videodownloader.h"
#include <execinfo.h>
#include <unistd.h>
#include <services/jsdiagnostics.h>

void handler(int sig) {
    void *array[10];
    size_t size;

    size = backtrace(array, 10);

    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);
    gst_init (&argc, &argv);

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QSharedPointer<QQuickView> view(SailfishApp::createView());

    QuickViewHelper::setView(view.data());

    UserFilesHelper userFilesHelper;
    userFilesHelper.copyJsFiles();

    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(appDataLocation + "db.sqlite");
    bool opened = db.open();
    Q_ASSERT(opened);

    VideoRepository::initTable();
    AuthorRepository::initTable();

    SubscriptionsAggregator subscriptionsAggregator;
    subscriptionsAggregator.updateSubscriptions();

    PulseAudioControl pacontrol;

    GoogleOAuthHelper oauthHelper;

    VideoDownloader downloader;

    view->rootContext()->setContextProperty("pacontrol", &pacontrol);
    view->rootContext()->setContextProperty("subscriptionsAggregator", &subscriptionsAggregator);
    view->rootContext()->setContextProperty("googleOAuthHelper", &oauthHelper);
    view->rootContext()->setContextProperty("videoDownloader", &downloader);
    view->rootContext()->setContextProperty("userFilesHelper", &userFilesHelper);
    qRegisterMetaType<Author>();
    qRegisterMetaType<Caption>();
    qRegisterMetaType<Thumbnail>();
    qRegisterMetaType<RendererNemo::Projection>();
    qmlRegisterType<Video>("com.verdanditeam.yt", 1, 0, "Video");
    qmlRegisterType<SponsorBlock>("com.verdanditeam.sponsorblock", 1, 0, "SponsorBlockPlugin");
    qmlRegisterType<VideoPlayer>("com.verdanditeam.yt", 1, 0, "VideoPlayer");
    qmlRegisterType<PlaylistModel>("com.verdanditeam.yt", 1, 0, "YtPlaylist");
    qmlRegisterType<CategoriesModel>("com.verdanditeam.yt", 1, 0, "YtCategories");
    qmlRegisterType<VideoHelper>("com.verdanditeam.yt", 1, 0, "VideoHelper");
    qmlRegisterType<ChannelHelper>("com.verdanditeam.yt", 1, 0, "ChannelHelper");
    qmlRegisterType<SubscriptionsModel>("com.verdanditeam.yt", 1, 0, "SubscriptionsModel");
    qmlRegisterType<CommentsModel>("com.verdanditeam.yt", 1, 0, "CommentsModel");
    qmlRegisterType<VideoDownloader>("com.verdanditeam.yt", 1, 0, "VideoDownloader");
    qmlRegisterType<JsDiagnostics>("com.verdanditeam.yt", 1, 0, "JsDiagnostics");

    view->setSource(SailfishApp::pathTo("qml/microtube.qml"));
    view->show();

    return app->exec();
}
