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
import SortFilterProxyModel 0.2
import com.verdanditeam.yt 1.0
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    property YtPlaylist playlistModel: null

    onStatusChanged: subscriptionsModel.loadSubscriptionsList()

    Connections {
        target: subscriptionsAggregator
        onSubscriptionsUpdateProgressChanged: subscriptionsModel.loadSubscriptionsList()
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            MenuItem {
                text: qsTr("Import subscriptions from YouTube")
                visible: googleOAuthHelper.linked
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("SubscriptionsImport.qml"))
                }
            }

            MenuItem {
                text: qsTr("Synchronize all videos")
                onClicked: {
                    subscriptionsAggregator.updateSubscriptions(true, false)
                }
            }

            MenuItem {
                text: qsTr("Mark all as watched")
                onClicked: {
                    subscriptionsModel.markAllAsWatched()
                }
            }

            MenuItem {
                text: qsTr("Load unwatched videos")
                onClicked: {
                    playlistModel.loadUnwatchedSubscriptions()
                    pageStack.pop()
                }
            }

            MenuItem {
                text: qsTr("Load all videos")
                onClicked: {
                    playlistModel.loadSubscriptions()
                    pageStack.pop()
                }
            }
        }

        PageHeader {
            id: header
            title: qsTr("Subscriptions")
        }

        ChannelHelper {
            id: channelHelper
        }

        SubscriptionsModel {
            id: subscriptionsModel

            Component.onCompleted: subscriptionsModel.loadSubscriptionsList()
        }

        SortFilterProxyModel {
            id: channelsProxyModel
            sourceModel: subscriptionsModel
            sorters: [
                RoleSorter { roleName: "unwatchedCount"; sortOrder: Qt.DescendingOrder}
            ]
        }

        Label {
            id: importLabel
            anchors.top: header.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Updating subscriptions: (%1/%2)").arg(subscriptionsAggregator.subscriptionsUpdateProgress).arg(subscriptionsAggregator.subscriptionsCount)
            visible: subscriptionsAggregator.subscriptionsCount != subscriptionsAggregator.subscriptionsUpdateProgress
        }

        SilicaGridView {
            id: listView

            readonly property int columnWidth: page.orientation === Orientation.Portrait ? listView.width/3 : listView.width/5

            anchors.top: importLabel.visible ? importLabel.bottom : header.bottom
            anchors.topMargin: importLabel.visible ? Theme.paddingLarge : 0
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: Theme.paddingLarge

            cellWidth: columnWidth
            cellHeight: columnWidth
            clip: true

            model: channelsProxyModel

            delegate: ListItem {
                width: listView.columnWidth
                height: listView.columnWidth + contextMenu.height
                contentWidth: listView.columnWidth
                contentHeight: listView.columnWidth + contextMenu.height

                onClicked: {
                    playlistModel.loadSubscriberVideos(authorId)
                    pageStack.pop()
                }

                Rectangle {
                    visible: unwatchedCount > 0
                    anchors.right: picture.right
                    anchors.top: picture.top
                    anchors.topMargin: -height/2
                    width: Theme.itemSizeExtraSmall/1.5
                    height: width
                    color: Theme.rgba(Theme.highlightBackgroundColor, 0.7)
                    border.color: "transparent"
                    border.width: 1
                    radius: width*0.5
                    z: 100
                    Text {
                        anchors.margins: Theme.paddingSmall/2
                        anchors.fill: parent
                        color: Theme.primaryColor
                        text: unwatchedCount
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                }

                Image {
                    id: picture
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.rightMargin: Theme.paddingLarge
                    anchors.topMargin: Theme.paddingLarge
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    width: Theme.itemSizeExtraLarge
                    height: Theme.itemSizeExtraLarge
                    source: avatar
                    fillMode: Image.PreserveAspectFit
                }
                Label {
                    text: name
                    anchors.top: picture.bottom
                    anchors.topMargin: Theme.paddingSmall
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Math.min(implicitWidth, parent.width - Theme.paddingLarge*2)
                    truncationMode: TruncationMode.Fade
                    font.pixelSize: Theme.fontSizeExtraSmall
                    horizontalAlignment: Text.AlignLeft
                }

                menu: ContextMenu {
                    id: contextMenu
                    width: page.width
                    MenuItem {
                        text: qsTr("Unsubscribe")
                        onClicked: {
                            channelHelper.unsubscribe(authorId)
                            subscriptionsModel.loadSubscriptionsList()
                        }
                    }
                }
            }
        }
    }
}
