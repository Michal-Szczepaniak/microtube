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
import com.verdanditeam.sponsorblock 1.0

Page {
    id: settingsPage

    allowedOrientations: Orientation.All

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property bool autoPlay: true
        property bool audioOnlyMode: false
        property bool developerMode: false
        property double buffer: 1.0
        property string downloadLocation: "/home/nemo/Downloads/"
        property string version: ""
        property string categoryId: "0"
        property string categoryName: "Film & Animation"
    }

    SponsorBlockPlugin {
        id: sponsorBlockPlugin
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: column.height + header.height

        PageHeader {
            id: header
            title: qsTr("SponsorBlock Settings")
        }

        Column {
            id: column
            spacing: Theme.paddingLarge
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            function switchCategory(category) {
                var categories = sponsorBlockPlugin.categories
                if (categories.indexOf(category) !== -1) {
                    categories.splice(categories.indexOf(category), 1)
                } else {
                    categories.push(category)
                }
                sponsorBlockPlugin.categories = categories
                console.log(sponsorBlockPlugin.categories)
            }

            TextSwitch {
               checked: sponsorBlockPlugin.enabled
               width: parent.width
               text: qsTr("Enable SponsorBlock")
               onClicked: sponsorBlockPlugin.enabled = checked
               description: qsTr("Uses SponsorBlock plugin from https://sponsor.ajay.app")
            }

            SectionHeader {
                text: qsTr("Categories")
            }

            TextSwitch {
                checked: sponsorBlockPlugin.categories.indexOf("sponsor") !== -1
                width: parent.width
                text: qsTr("Sponsors")
                onClicked: column.switchCategory("sponsor")
            }

            TextSwitch {
                checked: sponsorBlockPlugin.categories.indexOf("intro") !== -1
                width: parent.width
                text: qsTr("Intermission/Intro Animation")
                onClicked: column.switchCategory("intro")
            }

            TextSwitch {
                checked: sponsorBlockPlugin.categories.indexOf("outro") !== -1
                width: parent.width
                text: qsTr("Endcards/Credits")
                onClicked: column.switchCategory("outro")
            }

            TextSwitch {
                checked: sponsorBlockPlugin.categories.indexOf("interaction") !== -1
                width: parent.width
                text: qsTr("Interaction Reminder (Subscribe)")
                onClicked: column.switchCategory("interaction")
            }

            TextSwitch {
                checked: sponsorBlockPlugin.categories.indexOf("selfpromo") !== -1
                width: parent.width
                text: qsTr("Unpaid/Self Promotion")
                onClicked: column.switchCategory("selfpromo")
            }

            TextSwitch {
                checked: sponsorBlockPlugin.categories.indexOf("music_offtopic") !== -1
                width: parent.width
                text: qsTr("Music: Non-Music Section")
                onClicked: column.switchCategory("music_offtopic")
            }
        }
    }
}
