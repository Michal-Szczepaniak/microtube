import QtQuick 2.0
import Sailfish.Silica 1.0
import com.verdanditeam.yt 1.0

Dialog {
    property VideoHelper videoHelper
    canAccept: false
    canNavigateForward: false
    allowedOrientations: Orientation.All

    Column {
        anchors.fill: parent

        DialogHeader {
            id: header
        }

        SilicaFastListView {
            width: parent.width
            height: parent.height - header.height

            model: videoHelper.subtitlesLabels

            delegate: ListItem {
                width: parent.width
                height: Theme.itemSizeMedium
                Label {
                    x: Theme.horizontalPageMargin
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData
                }
                onClicked: {
                    videoHelper.loadSubtitle(index-1)
                    pageStack.pop()
                }
            }
        }
    }
}
