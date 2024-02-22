import QtQuick 2.0
import Sailfish.Silica 1.0
import com.verdanditeam.yt 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    property YtPlaylist searchModel: null

    onStatusChanged: if (status === PageStatus.Deactivating) searchModel.searchAgain()

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
                visible: false
                currentIndex: searchModel.sortBy
                property var values: [Search.Relevance, Search.Rating, Search.UploadDate, Search.ViewCount]

                menu: ContextMenu {
                    MenuItem { text: qsTr("Relevance") }
                    MenuItem { text: qsTr("Rating") }
                    MenuItem { text: qsTr("Upload date") }
                    MenuItem { text: qsTr("View Count") }
                }

                onCurrentIndexChanged: searchModel.sortBy = parseInt(currentIndex)
            }


            ComboBox {
                width: parent.width
                label: qsTr("Upload date")
                currentIndex: searchModel.uploadDateFilter

                menu: ContextMenu {
                    MenuItem { text: qsTr("Anytime") }
                    MenuItem { text: qsTr("Last hour") }
                    MenuItem { text: qsTr("Today") }
                    MenuItem { text: qsTr("This week") }
                    MenuItem { text: qsTr("This month") }
                    MenuItem { text: qsTr("This year") }
                }

                onCurrentIndexChanged: searchModel.uploadDateFilter = parseInt(currentIndex)
            }

            ComboBox {
                width: parent.width
                label: qsTr("Duration")
                currentIndex: searchModel.durationFilters

                menu: ContextMenu {
                    MenuItem { text: qsTr("All") }
                    MenuItem { text: qsTr("Short") }
                    MenuItem { text: qsTr("Medium") }
                    MenuItem { text: qsTr("Long") }
                }

                onCurrentIndexChanged: searchModel.durationFilter = parseInt(currentIndex)
            }
        }
    }
}
