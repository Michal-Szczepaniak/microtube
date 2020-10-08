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
    property bool importSuccessful: false

    function importSubscriptions(filePath) {

    }

    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            id: header
            title: qsTr("Import")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Import file")
                onClicked: pageStack.push(picker)
            }
        }

        Column {
            anchors.top: header.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingLarge


            SectionHeader {
                text: qsTr("Instructions")
            }

            LinkedLabel {
                width: parent.width
                text: qsTr("Go to YouTube website on desktop and while logged in, save the page by clicking ctrl+s. Upload saved file to your device and pick it from pull down menu.")
            }

            Label {
                width: parent.width
                text: qsTr("Import successful")
                visible: importSuccessful
            }
        }
    }

    Component {
        id: picker
        FilePickerPage {
            nameFilters: [ '*.mhtml', '*.html' ]
            onSelectedContentPropertiesChanged: {
                var xhr = new XMLHttpRequest;
                xhr.open("GET", selectedContentProperties.filePath);
                xhr.onreadystatechange = function() {
                    if (xhr.readyState === XMLHttpRequest.DONE) {
                        var response = xhr.responseText.replace(/(\r\n|\n|\r)/gm,"");
                        var scriptRegex = /<script[^>]*>[^<]*?ytInitialGuideData(.[^{]*?)<\/script>/g;
                        var script = response.match(scriptRegex)[1].replace(/.[^{]*/, "").replace(/;.*/, "");
                        var ytData = JSON.parse(script);
                        var subscriptions = (ytData.items[1].guideSubscriptionsSectionRenderer.items);
                        subscriptions.forEach(function(item){
                            if (typeof item.guideEntryRenderer !== 'undefined') {
                                var channel = YT.getChannel(item.guideEntryRenderer.navigationEndpoint.browseEndpoint.browseId)
                                if (!channel.isSubscribed &&
                                        item.guideEntryRenderer.navigationEndpoint.browseEndpoint.browseId !== "FEguide_builder") {
                                    channel.subscribe()
                                }
                            } else if (typeof item.guideCollapsibleEntryRenderer !== 'undefined') {
                                item.guideCollapsibleEntryRenderer.expandableItems.forEach(function(item){
                                    if (typeof item.guideEntryRenderer !== 'undefined') {
                                        var channel = YT.getChannel(item.guideEntryRenderer.navigationEndpoint.browseEndpoint.browseId)
                                        if (!channel.isSubscribed &&
                                                item.guideEntryRenderer.navigationEndpoint.browseEndpoint.browseId !== "FEguide_builder") {
                                            channel.subscribe()
                                        }
                                    }
                                });
                            }
                        });

                        page.importSuccessful = true
                        YTChannels.updateQuery()
                    }
                };
                xhr.send();
            }
        }
    }
}
