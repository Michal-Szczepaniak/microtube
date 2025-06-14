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
import Nemo.Configuration 1.0
import Nemo.DBus 2.0
import com.verdanditeam.yt 1.0
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All
    backNavigation: false

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property bool autoPlay: true
        property bool audioOnlyMode: false
        property bool developerMode: false
        property double buffer: 1.0
        property string categoryId: "0"
        property string categoryName: "Film & Animation"
        property int maxDefinition: 1080
        property int currentRegionId: 0
        property string currentRegion: ""
        property bool forceReverse: false
    }

    DBusAdaptor {
        id: shareDBusInterface

        service: "com.verdanditeam.microtube"
        path: "/"
        iface: "com.verdanditeam.microtube"
        xml: '<interface name="com.verdanditeam.microtube">
                  <method name="openUrl"> <arg type="s" name="url" direction="in"/> </method>
              </interface>'

        function openUrl(url) {
            app.activate()
            pageStack.pop(page, PageStackAction.Immediate)

            if (url != "") {
                searchModel.search(url)
                searchField.text = url
            }
        }
    }

    YtCategories {
        id: categories
    }

    ChannelHelper {
        id: channelHelper
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("About.qml"))
                }
            }

            MenuItem {
                text: qsTr("Settings")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Settings.qml"))
                }
            }

            MenuItem {
                text: qsTr("Subscriptions")
                onClicked: {
                    subscriptionsAggregator.updateSubscriptions()
                    pageStack.push(Qt.resolvedUrl("Subscriptions.qml"), {searchModel: app.searchModel})
                }
            }

            MenuItem {
                text: qsTr("Search results to queue")
                visible: !app.playlistMode
                onClicked: {
                    playlistModel.copyOtherModel(searchModel)
                    playlistMode = true;
                }
            }

            MenuItem {
                text: qsTr("Clear queue")
                visible: app.playlistMode
                onClicked: {
                    var remorse = Remorse.popupAction(page, qsTr("Clearing queue"), function() {
                        playlistModel.clear();
                    })
                }
            }

            MenuItem {
                text: qsTr("Reverse queue")
                visible: app.playlistMode && (playlistModel.canReverse || settings.forceReverse)
                onClicked: {
                    playlistModel.reverse()
                }
            }

            MenuItem {
                text: qsTr("Filters")
                enabled: searchModel.hasLastSearch
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Filters.qml"), {searchModel: app.searchModel})
                }
            }
        }

        PageHeader {
            id: header
            title: app.playlistMode ? qsTr("Queue") : qsTr("Search")
            description: app.playlistMode ? qsTr("Play queued videos") : qsTr("Find videos")

            TextSwitch {
                id: playlistModeSwitch
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: header._titleItem.width
                text: qsTr("Switch to queue")
                onCheckedChanged: {
                    app.playlistMode = checked
                }

                Connections {
                    target: app
                    onPlaylistModeChanged: playlistModeSwitch.checked = app.playlistMode
                }
            }
        }

        SearchField {
            id: searchField
            width: parent.width
            anchors.top: header.bottom
            placeholderText: qsTr("Search")
            Keys.onReturnPressed: {
                if(searchField.text.length != 0) {
                    if(searchField.text == "21379111488") settings.developerMode = !settings.developerMode
                    searchModel.search(searchField.text)
                    app.playlistMode = false
                }
            }
        }

        ComboBox {
            id: categoriesBox
            anchors.top: searchField.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            label: qsTr("Category")
            currentIndex: settings.categoryId
            menu: ContextMenu {
                Repeater {
                    model: categories
                    delegate: MenuItem {
                        text: name
                        property var value: code
                    }
                }
            }
            onCurrentItemChanged: {
                searchModel.loadCategory(currentItem.value)
            }
        }

        SilicaFastListView {
            id: searchList

            anchors.top: categoriesBox.bottom
            anchors.topMargin: Theme.paddingMedium
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            clip: true
            spacing: Theme.paddingMedium
            model: app.playlistMode ? playlistModel : searchModel

            ViewPlaceholder {
                enabled: !searchList.count
                text: playlistMode ? qsTr("No queued videos") : qsTr("No videos")
                hintText: playlistMode ? qsTr("Add videos to queue from pulldown or dropdown menus") : qsTr("Select category by swiping to the left or search for videos")
            }

            BusyIndicator {
                running: (app.playlistMode ? playlistModel : searchModel).busy
                size: BusyIndicatorSize.Large
                anchors.centerIn: parent
            }

            delegate: VideoElement {
                onClicked: {
                    if (elementType === YtPlaylist.ChannelType) {
                        pageStack.push(Qt.resolvedUrl("Channel.qml"), {channelId: id})
                    } else if (elementType === YtPlaylist.PlaylistType) {
                        app.playlistModel.loadPlaylist(id);
                        app.playlistMode = true;
                    } else {
                        if (pageStack.nextPage(page)) {
                            pageStack.popAttached(page, PageStackAction.Immediate);
                        }

                        if (app.playlistMode) {
                            app.playlistModel.currentVideoIndex = index;
                        }

                        pageStack.pushAttached(Qt.resolvedUrl("VideoPlayer.qml"), {videoIdToPlay: id})
                        pageStack.navigateForward()
                    }
                }
            }
        }
    }
}
