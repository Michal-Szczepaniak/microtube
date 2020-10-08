import QtQuick 2.0
import Sailfish.Silica 1.0
import com.verdanditeam.yt 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    property YtPlaylist playlistModel: null

    onStatusChanged: if (status === PageStatus.Deactivating) playlistModel.searchAgain()

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + header.height + Theme.paddingLarge

        PageHeader {
            id: header
            title: qsTr("Filters")
        }

        Column {
            id: column
            anchors.top: header.bottom
            anchors.topMargin: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            spacing: Theme.paddingSmall

            ComboBox {
                width: parent.width
                label: qsTr("Sort by")
                currentIndex: playlistModel.searchParams.sortBy

                menu: ContextMenu {
                    MenuItem { text: qsTr("Relevance") }
                    MenuItem { text: qsTr("Date") }
                    MenuItem { text: qsTr("View Count") }
                    MenuItem { text: qsTr("Rating") }
                }

                onCurrentIndexChanged: playlistModel.searchParams.sortBy = parseInt(currentIndex)
            }


            ComboBox {
                width: parent.width
                label: qsTr("Date")
                currentIndex: YT.searchParams.time

                menu: ContextMenu {
                    MenuItem { text: qsTr("Anytime") }
                    MenuItem { text: qsTr("Today") }
                    MenuItem { text: qsTr("7 Days") }
                    MenuItem { text: qsTr("30 Days") }
                }

                onCurrentIndexChanged: playlistModel.searchParams.time = parseInt(currentIndex)
            }

            ComboBox {
                width: parent.width
                label: qsTr("Duration")
                currentIndex: YT.searchParams.duration

                menu: ContextMenu {
                    MenuItem { text: qsTr("All") }
                    MenuItem { text: qsTr("Short") }
                    MenuItem { text: qsTr("Medium") }
                    MenuItem { text: qsTr("Long") }
                }

                onCurrentIndexChanged: playlistModel.searchParams.duration = parseInt(currentIndex)
            }
        }
    }
}
