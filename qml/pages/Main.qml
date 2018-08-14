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
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            MenuItem {
                text: qsTr("Subscriptions")
                onClicked: {
                    YT.updateQuery()
                    pageStack.push(Qt.resolvedUrl("Subscriptions.qml"))
                }
            }
        }

        PageHeader {
            id: header
            title: qsTr("Search")
        }

        SilicaListView {
            id: listView
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            clip: true
            header: SearchField {
                id: searchField
                width: parent.width
                placeholderText: "Search"
                Keys.onReturnPressed: {
                    if(searchField.text.length != 0) {
                        YT.search(searchField.text)
                    }
                }
            }
            spacing: Theme.paddingMedium
            model: YTPlaylist
            delegate: VideoElement {}
        }
    }
}
