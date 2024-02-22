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

Page {
    id: settingsPage

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: column.height + header.height

        PageHeader {
            id: header
            title: qsTr("Subscriptions Settings")
        }

        Column {
            id: column
            spacing: Theme.paddingLarge
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            SectionHeader {
                text: qsTr("Elements to aggregate")
            }

            TextSwitch {
                checked: subscriptionsAggregator.synchronizeVideos
                width: parent.width
                text: qsTr("Videos")
                onClicked: subscriptionsAggregator.synchronizeVideos = !subscriptionsAggregator.synchronizeVideos
            }

            TextSwitch {
                checked: subscriptionsAggregator.synchronizeShorts
                width: parent.width
                text: qsTr("Shorts")
                onClicked: subscriptionsAggregator.synchronizeShorts = !subscriptionsAggregator.synchronizeShorts
            }

            TextSwitch {
                checked: subscriptionsAggregator.synchronizeLivestreams
                width: parent.width
                text: qsTr("Past livestreams")
                onClicked: subscriptionsAggregator.synchronizeLivestreams = !subscriptionsAggregator.synchronizeLivestreams
            }
        }
    }
}
