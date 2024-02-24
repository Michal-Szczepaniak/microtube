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
    backNavigation: false
    property bool initialized: false

    onStatusChanged: {
        if (!initialized) {
            swipeView.contentX = page.width;
            initialized = true;
        }
    }

    onOrientationChanged: {
        if (swipeView.contentX > 0) {
            swipeView.contentX = page.height;
        }
    }

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
                text: qsTr("Filters")
                enabled: searchModel.hasLastSearch
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Filters.qml"), {searchModel: app.searchModel})
                }
            }
        }

        PageHeader {
            id: header
            title: swipeView.contentX === 0 ? qsTr("Categories") : app.playlistMode ? qsTr("Queue") : qsTr("Search")
            description: swipeView.contentX === 0 ? qsTr("Select trending category") : app.playlistMode ? qsTr("Play queued videos") : qsTr("Find videos")

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
                    swipeView.contentX = page.width;
                }
            }
            Component.onCompleted: {
                if (Qt.application.arguments.length === 2) {
                    searchModel.search(Qt.application.arguments[1])
                    searchField.text = Qt.application.arguments[1]
                }
            }
        }

        SilicaListView {
            id: swipeView

            clip: true
            orientation: ListView.Horizontal
            layoutDirection: ListView.LeftToRight

            anchors.top: searchField.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            interactive: true
            contentHeight: height
            contentWidth: width*2
            snapMode: ListView.SnapOneItem

            Behavior on contentX { PropertyAnimation {} }

            model: ListModel {
                id: listModel

                ListElement {}
                ListElement {}
            }

            delegate: Item {
                width: swipeView.width
                height: swipeView.height

                SilicaFastListView {
                    id: browseList
                    width: swipeView.width
                    height: swipeView.height
                    clip: true
                    model: categories
                    visible: index === 0

                    delegate: CategoryElement {
                        onClicked: {
                            searchModel.loadCategory(code, settings.currentRegion)
                            swipeView.contentX = page.width
                        }
                    }
                }

                SilicaFastListView {
                    id: searchList
                    width: swipeView.width
                    height: swipeView.height
                    clip: true
                    spacing: Theme.paddingMedium
                    visible: index === 1
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
    }
}
