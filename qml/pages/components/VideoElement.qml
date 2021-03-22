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
import com.verdanditeam.yt 1.0

ListItem {
    id: listItem
    contentHeight: isVideo ? Theme.itemSizeHuge : Theme.itemSizeMedium
    menu: contextMenuComponent

    onClicked: {
        if (video === undefined) {
            playlistModel.searchMore()
            return;
        }

        if (video.kind === "channel") {
            if (!subPage) {
                pageStack.push(Qt.resolvedUrl("../Channel.qml"), {channel: YT.getChannel(video.getChannelId())})
            } else {
                pageStack.navigateBack(PageStackAction.Immediate)
                pageStack.push(Qt.resolvedUrl("../Channel.qml"), {channel: YT.getChannel(video.getChannelId())})
            }
            return;
        }

        if (!subPage) {
            if (popPage) pageStack.navigateBack(PageStackAction.Immediate)
            pageStack.pushAttached(Qt.resolvedUrl("../VideoPlayer.qml"), {video: playlistModel.qmlVideoAt(index), model: playlistModel})
            pageStack.navigateForward()
        }

        playlistModel.setActiveRow(index)
    }

    property bool subPage: false
    property bool popPage: false
    property bool isVideo: itemType === 1 // video type

    Loader {
        asynchronous: true
        sourceComponent: rowComponent
    }

    Component {
        id: rowComponent

        Row {
            width: parent.width
            height: Theme.itemSizeHuge

            Item {
                id: left
                width: isVideo ? listItem.width/2.3 : 0
                height: parent.height
                visible: isVideo
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    anchors.right: parent.right
                    width: parent.width - Theme.paddingLarge
                    height: parent.height - Theme.paddingLarge
                    source: thumbnail !== undefined ? thumbnail : ""
                    anchors.centerIn: parent
                    asynchronous: true
                    cache: true
                    antialiasing: false
                    fillMode: Image.PreserveAspectFit

                    Rectangle {
                        color: Theme.rgba(Theme.highlightBackgroundColor, 0.7)
                        width: childrenRect.width
                        height: childrenRect.height
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: Theme.paddingMedium
                        radius: 10
                        visible: video.formattedDuration !== ""

                        Label {
                            id: duration
                            text: " " + video.formattedDuration + " "
                        }
                    }
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                width: listItem.width - left.width
                height: this.contentHeight
                padding: Theme.paddingLarge

                Label {
                    text: display
                    width: parent.width - Theme.paddingLarge*2
                    truncationMode: TruncationMode.Fade
                    horizontalAlignment: !isVideo ? Text.AlignHCenter : Text.AlignLeft
                }

                Label {
                    text: author !== undefined ? author : ""
                    width: parent.width - Theme.paddingLarge*2
                    font.pixelSize: Theme.fontSizeExtraSmall
                    truncationMode: TruncationMode.Fade
                }

                Row {
                    Label {
                        text: published + (published !== "" && video !== undefined ? "  -  " : "") + (video !== undefined ? video.viewCount : "") + " " + (video.kind === "channel" ? YT.getChannel(video.getChannelId()).subscriberCount : "")
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }
                }
            }
        }
    }

    Component {
        id: contextMenuComponent

        ContextMenu {
            id: contextMenu
            hasContent: isVideo

            MenuItem {
                property bool subscribed: isVideo ? YT.getChannel(video.getChannelId()).isSubscribed : null
                text: subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                onClicked: {
                    var channel = YT.getChannel(video.getChannelId())
                    channel.isSubscribed ? channel.unsubscribe() : channel.subscribe()
                }
            }

            MenuItem {
                text: qsTr("Copy url")
                onClicked: Clipboard.text = video.getWebpage()
            }
        }
    }
}
