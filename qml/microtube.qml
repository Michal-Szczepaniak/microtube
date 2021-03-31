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
import "pages"

ApplicationWindow
{
    id: app
    initialPage: settings.version === version ? mainPage : (settings.version === "" ? installPage : updatePage)
    cover: !videoCover ? Qt.resolvedUrl("cover/CoverPage.qml") : null

    allowedOrientations: defaultAllowedOrientations

    property string playing: ""
    property bool videoCover: false
    property string version: "2.1.2"
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
        id: playlistModel

        Component.onCompleted: {
            if (typeof startSearch !== "undefined") search(startSearch)
            else loadCategory(settings.categoryId, settings.categoryName)
        }
    }

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property string version: ""
        property string categoryId: "0"
        property string categoryName: "Film & Animation"
    }
}
