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
    contentHeight: isVideo ? Theme.paddingLarge*8 : Theme.itemSizeMedium
    menu: contextMenuComponent

    onClicked: {
        if (video === undefined) {
            YTPlaylist.searchMore()
            return;
        }
        ChannelAggregator.videoWatched(video)
        YTPlaylist.setActiveRow(index)
        if (!subPage)
            pageStack.push(Qt.resolvedUrl("../VideoPlayer.qml"),
                           { video: video, title: display, author: author, viewCount: viewCount, description: description })
    }

    property bool subPage: false
    property bool isVideo: itemType === 1 // video type

    Row {
        anchors.fill: parent
        Column {
            id: left
            width: isVideo ? listItem.width/2.3 : 0
            height: Theme.paddingLarge*8
            leftPadding: subPage ? 0 : Theme.paddingLarge
            topPadding: Theme.paddingSmall
            bottomPadding: Theme.paddingSmall
            visible: isVideo

            Image {
                width: parent.width - Theme.paddingLarge
                height: parent.height - Theme.paddingSmall*2
                source: thumbnail !== undefined ? thumbnail : ""
                asynchronous: true
                cache: true
                antialiasing: false
                fillMode: Image.PreserveAspectFit
            }
        }

        Column {
            width: listItem.width - left.width
            height: Theme.paddingLarge*8
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
                    text: (published !== undefined ? published : "") + (video !== undefined ? "  -  " + video.viewCount : "")
                    font.pixelSize: Theme.fontSizeExtraSmall
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
                property bool subscribed: isVideo ? video.isSubscribed(video.getChannelId()) : null
                text: subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                onClicked: {
                    YT.toggleSubscription(video.getChannelId())
                    subscribed = video.isSubscribed(video.getChannelId())
                }
            }

            MenuItem {
                text: qsTr("Copy url")
                onClicked: Clipboard.text = video.getWebpage()
            }
        }
    }
}
