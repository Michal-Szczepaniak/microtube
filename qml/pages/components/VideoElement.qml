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
import QtGraphicalEffects 1.0
import "./helpers.js" as Helpers
import "./humanized_time_span.js" as HumanizedTimeSpan

ListItem {
    id: listItem
    contentHeight: Theme.itemSizeHuge
    menu: contextMenuComponent

    property bool subPage: false
    property bool popPage: false

    Row {
        width: parent.width
        height: Theme.itemSizeHuge

        Item {
            id: left
            height: parent.height
            width: height*1.8
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: thumbnailImage
                anchors.right: parent.right
                width: elementType === YtPlaylist.ChannelType ? height : (parent.width - Theme.paddingLarge)
                height: parent.height - Theme.paddingLarge
                source: thumbnail
                anchors.centerIn: parent
                asynchronous: true
                cache: true
                antialiasing: false
                fillMode: Image.PreserveAspectCrop
                clip: true

                layer.enabled: elementType === YtPlaylist.ChannelType
                layer.effect: OpacityMask {
                    maskSource: mask
                }

                onStatusChanged: if (status === Image.Error) source = altThumbnail

                Rectangle {
                    id: mask
                    width: parent.height - Theme.paddingLarge
                    height: parent.height - Theme.paddingLarge
                    radius: (parent.height - Theme.paddingLarge)/2
                    visible: false
                }

                Rectangle {
                    color: Theme.rgba(Theme.highlightBackgroundColor, 0.7)
                    width: durationLabel.width + Theme.paddingMedium
                    height: durationLabel.height
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: Theme.paddingMedium
                    radius: 10

                    Label {
                        id: durationLabel
                        text: duration
                        highlighted: false
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            width: listItem.width - left.width
            height: this.contentHeight
            padding: Theme.paddingSmall

            Label {
                text: title
                width: parent.width - Theme.paddingLarge*2
                truncationMode: TruncationMode.Fade
                horizontalAlignment: Text.AlignLeft
                maximumLineCount: 2
                wrapMode: Text.WordWrap
            }

            Label {
                text: author.name
                width: parent.width - Theme.paddingLarge*2
                font.pixelSize: Theme.fontSizeExtraSmall
                truncationMode: TruncationMode.Fade
            }
            Label {
                text: {
                    if (isUpcoming) {
                        return "scheduled";
                    } else if (isLive) {
                        return "live";
                    } else if (elementType === YtPlaylist.ChannelType) {
                        return description;
                    } else {
                        return qsTr("%1 views - %2").arg(Helpers.parseViews(views)).arg(HumanizedTimeSpan.humanized_time_span(published*1000))
                    }
                }
                font.pixelSize: Theme.fontSizeExtraSmall
            }
        }
    }

    Component {
        id: contextMenuComponent

        ContextMenu {
            id: contextMenu

            ChannelHelper {
                id: channelHelper
            }

            MenuItem {
                property bool subscribed: channelHelper.isSubscribed(author.id)
                text: subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                onClicked: {
                    channelHelper.isSubscribed(author.id) ? channelHelper.unsubscribe(author.id) : channelHelper.subscribe(author.id)
                    subscribed = channelHelper.isSubscribed(author.id)
                }
            }

            MenuItem {
                text: qsTr("Copy url")
                onClicked: Clipboard.text = url
            }
        }
    }
}
