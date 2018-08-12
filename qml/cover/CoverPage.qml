import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    Item {
        anchors.centerIn: parent
        width: parent.width
        height: label.height + playingLabel.height + title.height + Theme.paddingLarge*2
        Label {
            id: label
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: Theme.paddingLarge
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("µtube")
            font.pixelSize: Theme.fontSizeExtraLarge
        }

        Label {
            id: playingLabel
            visible: app.playing !== ""
            anchors.top: label.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: Theme.paddingSmall
            horizontalAlignment: Text.AlignHCenter
            text: "Playing:"
        }

        TextArea{
            id: title
            visible: app.playing !== ""
            anchors.top: playingLabel.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            readOnly: true
            text: app.playing
        }
    }
}
