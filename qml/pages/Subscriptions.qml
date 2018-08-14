import QtQuick 2.6
import Sailfish.Silica 1.0
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PageHeader {
            id: header
            title: qsTr("Subscriptions")
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


            model: YTChannels

            delegate: ListItem {
                width: listView.columnWidth
                height: listView.columnWidth
                contentWidth: listView.columnWidth
                contentHeight: listView.columnWidth

                onClicked: {
                    YT.itemActivated(index)
                    pageStack.navigateBack()
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
                        text: "Unsubscribe"
                        onClicked: {
                            YTChannels.unsubscribe(index);
                            YT.updateQuery()
                        }
                    }
                }
            }
        }
    }
}
