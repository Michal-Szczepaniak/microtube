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
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    onStatusChanged: {
        if (status === PageStatus.Active) pageStack.pushAttached(Qt.resolvedUrl("Main.qml"), {})
        if (status === PageStatus.Deactivating) settings.version = app.version
    }

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property string version: ""
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + header.height

        PageHeader {
            id: header
            title: qsTr("Update")
        }

        Column {
            id: column
            spacing: Theme.paddingLarge
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            width: parent.width

            CenteredLabel {
                text: qsTr("Microtube was updated to version %1!").arg(app.version)
                font.family: Theme.fontFamilyHeading
                font.pixelSize: Theme.fontSizeLarge
            }

            SectionHeader {
                text: qsTr("Changelog")
            }

            Column {
                width: parent.width - Theme.paddingLarge*2
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium

                Label {
                    width: parent.width
                    text: "• " + qsTr("Updated Translations")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    width: parent.width
                    text: "• " + qsTr("Full screen video in cover on landscape")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }

            SectionHeader {
                text: qsTr("Donation")
            }

            LinkedLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - Theme.paddingLarge*2
                plainText: "If you like my work, consider donating me on PayPal https://www.paypal.me/MisterMagister"
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                color: Theme.primaryColor
            }

            LinkedLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - Theme.paddingLarge*2
                plainText: "…or supporting me on Patreon https://www.patreon.com/Mister_Magister"
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                color: Theme.primaryColor
            }
        }
    }
}
