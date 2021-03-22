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

    YtCategories {
        id: categories
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: column.height + header.height

        PageHeader {
            id: header
            title: qsTr("Settings")
        }

        Column {
            id: column
            spacing: Theme.paddingLarge
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            TextSwitch {
               checked: YT.safeSearch
               width: parent.width
               text: qsTr("Restricted mode (safe for kids)")
               onClicked: YT.safeSearch = checked
            }

            TextSwitch {
               checked: settings.autoPlay
               width: parent.width
               text: qsTr("Automatic change to next video")
               onClicked: {
                   settings.autoPlay = checked
               }
            }

            Slider {
                id: bufferSlider
                width: parent.width
                minimumValue: 0
                maximumValue: 1
                stepSize: 0.1
                value: settings.buffer
                visible: settings.developerMode

                onValueChanged: settings.buffer = value
                label: qsTr("Buffer")
            }

            Button {
                id: testNewInstall
                anchors.horizontalCenter: parent.horizontalCenter
                visible: settings.developerMode
                onClicked: settings.version = ""
                text: qsTr("Test new install")
            }

            Button {
                id: testUpdate
                anchors.horizontalCenter: parent.horizontalCenter
                visible: settings.developerMode
                onClicked: settings.version = "1"
                text: qsTr("Test update")
            }

            TextField {
                width: parent.width
                text: settings.downloadLocation
                label: qsTr("Download location")
                onTextChanged: settings.downloadLocation = text
            }

            ComboBox {
                id: regions
                width: parent.width
                label: qsTr("Region")
                currentIndex: YT.region
                menu: ContextMenu {
                    Repeater {
                        model: YT.getRegions();
                        delegate: MenuItem { text: modelData }
                    }
                }
                onCurrentItemChanged: YT.region = currentIndex
            }

            ComboBox {
                id: defaultCategory
                width: parent.width
                label: qsTr("Default category")
                value: settings.categoryName
                currentIndex: -1
                menu: ContextMenu {
                    Repeater {
                        model: categories
                        delegate: MenuItem {
                            text: name
                            onClicked: {
                                settings.categoryId = id
                                settings.categoryName = name
                            }
                        }
                    }
                }
            }

            BackgroundItem {
                width: parent.width
                onClicked: pageStack.push(Qt.resolvedUrl("SponsorBlockSettings.qml"))

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.horizontalPageMargin
                    text: qsTr("SponsorBlock Plugin Configuration")
                }
            }
        }
        VerticalScrollDecorator {}
    }
}
