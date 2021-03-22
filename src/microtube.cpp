/* $BEGIN_LICENSE

This file is part of Minitube.
Copyright 2009, Flavio Tordini <flavio.tordini@gmail.com>
Copyright 2018, Michał Szczepaniak <m.szczepaniak.000@gmail.com>

Minitube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Minitube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Minitube.  If not, see <http://www.gnu.org/licenses/>.

$END_LICENSE */

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include "yt3.h"
#include "ytvideo.h"
#include "yt.h"
#include "video.h"
#include "ytchannel.h"
#include "categoriesmodel.h"
#include "volume/pulseaudiocontrol.h"
#include "channelaggregator.h"
#include "QEasyDownloader/include/QEasyDownloader.hpp"
#include "js.h"
#include "constants.h"
#include "sponsorblock.h"
#include "userfileshelper.h"
#include <QtQuick>

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QSharedPointer<QQuickView> view(SailfishApp::createView());

    UserFilesHelper userFilesHelper;
    userFilesHelper.copyJsFiles();
    userFilesHelper.copyDesktopFile();

    YT yt;
    yt.registerObjectsInQml(view->rootContext());

    if (argc == 2) {
        view->rootContext()->setContextProperty("startSearch", QString::fromUtf8(argv[1]).replace("invidio.us", "youtube.com"));
    }

    PulseAudioControl pacontrol;

    view->rootContext()->setContextProperty("pacontrol", &pacontrol);

    view->rootContext()->setContextProperty("ChannelAggregator", ChannelAggregator::instance());
    ChannelAggregator::instance()->run();
//    ChannelAggregator::instance()->updateUnwatchedCount();

    qmlRegisterType<Video>("com.verdanditeam.yt", 1, 0, "YtVideo");
    qmlRegisterType<CategoriesModel>("com.verdanditeam.yt", 1, 0, "YtCategories");
    qmlRegisterType<PlaylistModel>("com.verdanditeam.yt", 1, 0, "YtPlaylist");
    qmlRegisterType<SponsorBlock>("com.verdanditeam.sponsorblock", 1, 0, "SponsorBlockPlugin");

    view->setSource(SailfishApp::pathTo("qml/microtube.qml"));
    view->show();

    return app->exec();
}
