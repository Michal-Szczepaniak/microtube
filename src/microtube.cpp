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
#include "helpers/subscriptionshelper.h"
#include "entities/author.h"
#include "helpers/channelhelper.h"
#include "entities/caption.h"
#include <QtSql/QSqlDatabase>
#include <src/repositories/authorrepository.h>
#include <src/repositories/videorepository.h>

int main(int argc, char *argv[])
{
    gst_init (&argc, &argv);

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QSharedPointer<QQuickView> view(SailfishApp::createView());

    QuickViewHelper::setView(view.data());

    UserFilesHelper userFilesHelper;
    userFilesHelper.copyJsFiles();

    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(appDataLocation + "db.sqlite");
    Q_ASSERT(db.open());

    VideoRepository::initTable();
    AuthorRepository::initTable();

    PulseAudioControl pacontrol;

    view->rootContext()->setContextProperty("pacontrol", &pacontrol);
    qRegisterMetaType<Author>();
    qRegisterMetaType<Caption>();
    qmlRegisterType<Video>("com.verdanditeam.yt", 1, 0, "Video");
    qmlRegisterType<SponsorBlock>("com.verdanditeam.sponsorblock", 1, 0, "SponsorBlockPlugin");
    qmlRegisterType<VideoPlayer>("com.verdanditeam.yt", 1, 0, "VideoPlayer");
    qmlRegisterType<PlaylistModel>("com.verdanditeam.yt", 1, 0, "YtPlaylist");
    qmlRegisterType<CategoriesModel>("com.verdanditeam.yt", 1, 0, "YtCategories");
    qmlRegisterType<VideoHelper>("com.verdanditeam.yt", 1, 0, "VideoHelper");
    qmlRegisterType<SubscriptionsHelper>("com.verdanditeam.yt", 1, 0, "SubscriptionsHelper");
    qmlRegisterType<ChannelHelper>("com.verdanditeam.yt", 1, 0, "ChannelHelper");

    view->setSource(SailfishApp::pathTo("qml/microtube.qml"));
    view->show();

    return app->exec();
}
