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

CoverBackground {
    Item {
        anchors.centerIn: parent
        width: parent.width
        height: label.height + playingLabel.height + title.height + Theme.paddingLarge*2
        Label {
            id: label
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: Theme.paddingLarge
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("µtube")
            font.pixelSize: Theme.fontSizeExtraLarge
        }

        Label {
            id: playingLabel
            visible: app.playing !== ""
            anchors.top: label.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: Theme.paddingSmall
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Playing:")
        }

        TextArea{
            id: title
            visible: app.playing !== ""
            anchors.top: playingLabel.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            readOnly: true
            text: app.playing
        }
    }
}
