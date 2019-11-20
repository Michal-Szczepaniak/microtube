/*
    Copyright (C) 2018 Michał Szczepaniak

    This file is part of Morsender.

    Morsender is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Morsender is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Morsender.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0

Page {
    id: settingsPage

    allowedOrientations: Orientation.All

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property bool autoPlay: true
        property bool relatedVideos: true
        property bool audioOnlyMode: false
        property bool developerMode: false
        property double buffer: 1.0
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
            anchors.topMargin: header.height
            anchors.fill: parent

            TextSwitch {
               checked: YT.getSafeSearch()
               width: parent.width
               text: qsTr("Restricted mode (safe for kids)")
               onClicked: YT.setSafeSearch(checked)
            }

            TextSwitch {
               checked: settings.autoPlay
               width: parent.width
               text: qsTr("Automatic change to next video")
               onClicked: {
                   settings.autoPlay = checked
               }
            }

            TextSwitch {
               checked: settings.relatedVideos
               width: parent.width
               text: qsTr("Search for related videos")
               onClicked: {
                   settings.relatedVideos = checked
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
            }

            ComboBox {
                id: regions
                width: parent.width
                label: qsTr("Region")
                currentIndex: YT.getCurrentRegion();
                menu: ContextMenu {
                    Repeater {
                        model: YT.getRegions();
                        delegate: MenuItem { text: modelData }
                    }
                }
                onCurrentItemChanged: YT.setRegion(currentIndex)
            }
        }
        VerticalScrollDecorator {}
    }
}
