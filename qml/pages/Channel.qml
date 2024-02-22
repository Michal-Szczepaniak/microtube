import QtQuick 2.5
import Sailfish.Silica 1.0
import com.verdanditeam.yt 1.0
import "components"
import "components/helpers.js" as Helpers

Page {
    id: page
    allowedOrientations: Orientation.All
    property var channelId: null

    onStatusChanged: {
        if (status === PageStatus.Active) {
            channelHelper.loadChannelInfo(channelId)
        }
    }

    ChannelHelper {
        id: channelHelper

        Component.onCompleted: channelHelper.loadChannelInfo(channelId)
    }

    Label {
        id: dummyDescription
        maximumLineCount: 1
        text: channelHelper.channelInfo.description
        visible: false
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: column.height + videosList.height + channelTab.height + Theme.paddingSmall*2

        PullDownMenu {
            MenuItem {
                text: channelHelper.isSubscribed(channelId) ? qsTr("Unsubscribe") : qsTr("Subscribe")
                onClicked: channelHelper.isSubscribed(channelId) ? channelHelper.unsubscribe(channelId) : channelHelper.subscribe(channelId)
            }

            MenuItem {
                text: qsTr("Copy url")
                onClicked: Clipboard.text = channelHelper.channelInfo.webpage
            }

            MenuItem {
                text: qsTr("Play all")
                onClicked: {
                    app.searchModel.loadChannelVideos(channelId, channelTab.values[channelTab.currentIndex])
                    pageStack.navigateBack()
                }
            }
        }

        Column {
            id: column
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            Image {
                width: parent.width
                height: width * (sourceSize.height / sourceSize.width)
                source: channelHelper.channelInfo.banner.url
                asynchronous: true
                fillMode: Image.PreserveAspectFit
            }

            Row {
                width: parent.width
                height: Theme.itemSizeHuge
                spacing: Theme.horizontalPageMargin

                Image {
                    id: profilePhoto
                    width: height
                    height: parent.height
                    source: channelHelper.channelInfo.avatar.url
                    asynchronous: true
                }

                Column {
                    height: parent.height
                    width: parent.width - profilePhoto.width - Theme.horizontalPageMargin*2

                    Label {
                        id: channelName
                        text: channelHelper.channelInfo.name
                        width: parent.width
                        height: Theme.itemSizeExtraSmall
                        font.pixelSize: Theme.fontSizeLarge
                        color: Theme.highlightColor
                        font.family: Theme.fontFamilyHeading
                        verticalAlignment: Text.AlignVCenter
                        truncationMode: TruncationMode.Fade
                    }

                    Label {
                        width: parent.width
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall

                        text: qsTr("%1 subscribers").arg(Helpers.parseViews(channelHelper.channelInfo.subscriberCount))
                    }
                }
            }

            SectionHeader {
                text: qsTr("Description")
            }

            Label {
                id: description
                x: Theme.horizontalPageMargin
                width: parent.width - Theme.horizontalPageMargin*2
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                maximumLineCount: expanded ? Number.MAX_VALUE : 1
                text: channelHelper.channelInfo.description

                property bool expanded: false
            }

            MouseArea {
                onClicked: description.expanded = true
                height: showMoreRow.height
                width: parent.width - Theme.horizontalPageMargin*2

                Row {
                    id: showMoreRow
                    anchors.right: parent.right
                    spacing: Theme.paddingSmall
                    visible: !description.expanded && (dummyDescription.width > (parent.width - Theme.horizontalPageMargin*2))
                    height: visible ? _showMoreLabel.height : 0

                    Label {
                        id: _showMoreLabel
                        textFormat: Text.StyledText;
                        font.pixelSize: Theme.fontSizeExtraSmall
                        text: "<i>%1</i>".arg(qsTr("Show more"))
                    }

                    Label {
                        anchors.verticalCenter: _showMoreLabel.verticalCenter
                        text: " \u2022 \u2022 \u2022" // three dots
                    }
                }
            }
        }

        YtPlaylist {
            id: channelVideos
        }

        ComboBox {
            id: channelTab
            anchors.top: column.bottom
            anchors.topMargin: Theme.paddingSmall
            anchors.left: parent.left
            anchors.right: parent.right
            label: qsTr("Category")
            currentIndex: 0
            property var values: [Search.Channel, Search.ChannelShorts, Search.ChannelLiveStreams, Search.ChannelPlaylists]
            menu: ContextMenu {
                Repeater {
                    model: [qsTr("Videos"),qsTr("Shorts"),qsTr("Livestreams"),qsTr("Playlists")]
                    delegate: MenuItem {
                        text: modelData
                    }
                }
            }
            onCurrentItemChanged: {
                channelVideos.loadChannelVideos(channelId, values[currentIndex])
            }
        }

        SilicaFastListView {
            id: videosList
            anchors.top: channelTab.bottom
            anchors.topMargin: Theme.paddingSmall
            anchors.left: parent.left
            anchors.right: parent.right
            height: page.height - channelTab.height
            maximumFlickVelocity: 9999
            spacing: Theme.paddingMedium
            model: channelVideos
            clip: true
            interactive: flickable.contentY >= Math.round(column.height + Theme.paddingSmall*2)

            ViewPlaceholder {
                enabled: !videosList.count
                text: qsTr("No videos")
            }

            delegate: VideoElement {
                id: delegate

                onClicked: {
                    if (elementType === YtPlaylist.PlaylistType) {
                        app.playlistModel.loadPlaylist(id);
                        app.playlistMode = true;
                        pageStack.navigateBack()
                    } else {
                        pageStack.navigateBack(PageStackAction.Immediate)
                        if (pageStack.nextPage(pageStack.currentPage)) {
                            pageStack.popAttached(pageStack.currentPage, PageStackAction.Immediate)
                        }

                        pageStack.pushAttached(Qt.resolvedUrl("VideoPlayer.qml"), {videoIdToPlay: id})
                        pageStack.navigateForward()
                    }
                }
            }
        }
    }
}
