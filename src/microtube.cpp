#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include "yt3.h"
#include "ytvideo.h"
#include "yt.h"

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/microplayer.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //   - SailfishApp::pathToMainQml() to get a QUrl to the main QML file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    YT3::instance().testApiKey();
    YT yt;

    YTVideo video(QString("rlptvUi1xHs"),  NULL);
    video.loadStreamUrl();

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QSharedPointer<QQuickView> view(SailfishApp::createView());

    QObject::connect(&video, SIGNAL(gotStreamUrl(QUrl)), &yt, SLOT(gotStreamUrl(QUrl)));

    yt.registerObjectsInQml(view->rootContext());

    view->setSource(SailfishApp::pathTo("qml/microtube.qml"));
    view->show();

    return app->exec();
}
