#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include "yt3.h"
#include "ytvideo.h"
#include "yt.h"
#include "video.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QSharedPointer<QQuickView> view(SailfishApp::createView());

    YT yt;

    yt.registerObjectsInQml(view->rootContext());

    qmlRegisterType<Video>("com.verdanditeam.yt", 1, 0, "YtVideo");

    view->setSource(SailfishApp::pathTo("qml/microtube.qml"));
    view->show();

    return app->exec();
}
