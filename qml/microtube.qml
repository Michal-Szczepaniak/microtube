/*
    Copyright (C) 2018 Michał Szczepaniak

    This file is part of Microtube.

    Microtube is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Microtube is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Microtube.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0
import com.verdanditeam.yt 1.0
import Nemo.Notifications 1.0
import "pages"

ApplicationWindow
{
    id: app
    initialPage: settings.version === version ? mainPage : (settings.version === "" ? installPage : updatePage)
    cover: !videoCover ? Qt.resolvedUrl("cover/CoverPage.qml") : null
    property alias searchModel: searchModel

    allowedOrientations: defaultAllowedOrientations

    property string playing: ""
    property bool videoCover: false
    property string version: "3.6.16"
    property bool playlistMode: false
    property alias playlistModel: playlistModel

    Component {
        id: updatePage
        UpdateDialog { }
    }

    Component {
        id: installPage
        InstallDialog { }
    }

    Component {
        id: mainPage
        Main { }
    }

    YtPlaylist {
        id: searchModel
    }

    YtPlaylist {
        id: playlistModel
    }

    Notification {
         id: downloadNotification

         summary: qsTr("Microtube download")
         replacesId: 1
    }

    Connections {
        target: videoDownloader
        onDownloadStarted: {
            downloadNotification.body = qsTr("Downloading %1").arg(filename)
            downloadNotification.publish();
        }

        onDownloadStatusChanged: {
            if (videoDownloader.downloadStatus === VideoDownloader.Failed) {
                downloadNotification.body = qsTr("Download failed")
                downloadNotification.progress = undefined
                downloadNotification.publish()
            } else if (videoDownloader.downloadStatus === VideoDownloader.Finished) {
                downloadNotification.body = qsTr("Download complete")
                downloadNotification.progress = undefined
                downloadNotification.publish()
            }
        }
        onDownloadProgressChanged: {
            downloadNotification.progress = videoDownloader.downloadProgress
            downloadNotification.publish()
        }
    }

    Connections {
        target: userFilesHelper
        onUpdateFinished: {
            if (searchModel.rowCount() === 0 && Qt.application.arguments.length !== 2)
                searchModel.loadCategory(settings.categoryName, settings.currentRegion)
        }
    }

    Component.onCompleted: {
        if (Qt.application.arguments.length !== 2) {
            searchModel.loadCategory(settings.categoryName, settings.currentRegion)
        }
    }

    Connections {
        target: googleOAuthHelper
        onOpenBrowser: Qt.openUrlExternally(url)
    }

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property string version: ""
        property string categoryId: "0"
        property string categoryName: "Now"
        property string currentRegion: ""
    }
}
