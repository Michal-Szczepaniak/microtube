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

Page {
    id: page

    allowedOrientations: Orientation.All

    Component.onCompleted: googleOAuthHelper.importSubscriptions()

    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            id: header
            title: qsTr("Import")
        }

        Label {
            anchors.centerIn: parent
            text: qsTr("Importing (%1/%2)").arg(googleOAuthHelper.importProgress).arg(googleOAuthHelper.importEnd)
        }
    }
}
