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
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            MenuItem {
                text: qsTr("Mark all as watched")
                onClicked: {
                    ChannelAggregator.markAllAsWatched()
                }
            }
            MenuItem {
                text: qsTr("Import subscriptions")
                onClicked: pageStack.push(Qt.resolvedUrl("SubscriptionsImport.qml"))
            }
        }

        PageHeader {
            id: header
            title: qsTr("Subscriptions")
        }

        SortFilterProxyModel {
            id: channelsProxyModel
            sourceModel: YTChannels
            sorters: [
                RoleSorter { roleName: "notifyCount"; sortOrder: Qt.DescendingOrder}
            ]
        }

        SilicaGridView {
            id: listView

            readonly property int columnWidth: page.orientation === Orientation.Portrait ? listView.width/3 : listView.width/5

            anchors.top: header.bottom
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
                height: listView.columnWidth
                contentWidth: listView.columnWidth
                contentHeight: listView.columnWidth

                onClicked: {
                    YT.itemActivated(channelsProxyModel.mapToSource(index))
                    pageStack.navigateBack()
                }

                Rectangle {
                    visible: index > 0 && ( ( index === 1 && ChannelAggregator.unwatchedCount > 0 ) || ( index > 1 && channel.notifyCount > 0 ) )
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
                        text: index > 1 ? channel.notifyCount : ChannelAggregator.unwatchedCount
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
                    source: thumbnail
                    fillMode: Image.PreserveAspectFit
                }
                Label {
                    text: username
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: Theme.paddingSmall
                    anchors.rightMargin: Theme.paddingSmall
                    anchors.bottomMargin: Theme.paddingSmall
                    anchors.left: parent.left
                    anchors.right: parent.right
                    width: parent.width
                    truncationMode: TruncationMode.Fade
                    font.pixelSize: Theme.fontSizeExtraSmall
                    horizontalAlignment: Text.AlignHCenter
                }

                menu: ContextMenu {
                    hasContent: index >= 2
                    width: page.width
                    MenuItem {
                        text: qsTr("Unsubscribe")
                        onClicked: {
                            YTChannels.unsubscribe(channelsProxyModel.mapToSource(index));
                            YT.updateQuery()
                        }
                    }
                }
            }
        }
    }
}
