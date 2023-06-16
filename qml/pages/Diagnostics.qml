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

import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.Pickers 1.0
import com.verdanditeam.yt 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    JsDiagnostics {
        id: jsDiagnostics
    }

    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            id: header
            title: qsTr("Diagnostics")
        }

        Column {
            spacing: Theme.paddingMedium
            anchors {
                top: header.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Run diagnostics")
                onClicked: jsDiagnostics.runDiagnostics()
            }

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: jsDiagnostics.stepText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: jsDiagnostics.statusText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }
}
