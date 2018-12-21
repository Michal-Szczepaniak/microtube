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
//    height: Theme.paddingLarge*8
    contentHeight: Theme.paddingLarge*8
    menu: contextMenuComponent

    onClicked: {
        if (video === undefined) return;
        YTPlaylist.setActiveRow(index)
        video.loadStreamUrl()
        if (!subPage)
            pageStack.push(Qt.resolvedUrl("../VideoPlayer.qml"),
                           { video: video, title: display, author: author, viewCount: viewCount, description: description })
    }

    property bool subPage: false

    Row {
        anchors.fill: parent
        Column {
            id: left
            width: listItem.width/2.3
            height: Theme.paddingLarge*8
            leftPadding: Theme.paddingLarge
            topPadding: Theme.paddingSmall
            bottomPadding: Theme.paddingSmall

            Image {
                width: parent.width - Theme.paddingLarge
                height: parent.height - Theme.paddingSmall*2
                source: thumbnail !== undefined ? thumbnail : ""
                asynchronous: true
                cache: true
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
            }

            Label {
                text: author !== undefined ? author : ""
                width: parent.width - Theme.paddingLarge*2
                font.pixelSize: Theme.fontSizeExtraSmall
                truncationMode: TruncationMode.Fade
            }

            Row {
                Label {
                    text: (published !== undefined ? published : "") + (viewCount !== undefined ? "  -  " + viewCount + " views" : "")
                    font.pixelSize: Theme.fontSizeExtraSmall
                }
            }
        }
    }

    Component {
        id: contextMenuComponent
        ContextMenu {
            id: contextMenu
            MenuItem {
                property bool subscribed: video.isSubscribed(video.getChannelId())
                text: subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                onClicked: {
                    YT.toggleSubscription()
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
