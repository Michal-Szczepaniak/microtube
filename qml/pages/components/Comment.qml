import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

ListItem {
    height: name.height + commentLabel.height + (subCommentsLabel.visible ? subCommentsLabel.height : 0) + Theme.paddingLarge*2
    contentHeight: height

    onClicked: {
        if (repliesCount > 0){
            pageStack.push(Qt.resolvedUrl("../Replies.qml"), {videoId: videoId, replyToken: replyToken})
        }
    }

    Image {
        id: avatarPhoto
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingLarge
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge
        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
        fillMode: Image.PreserveAspectFit
        source: photo
        asynchronous: true
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall
        sourceSize.width: Theme.itemSizeSmall
        sourceSize.height: Theme.itemSizeSmall
        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: avatarPhotoMask
        }
    }

    Rectangle {
        id: avatarPhotoMask
        width: avatarPhoto.height
        height: avatarPhoto.height
        radius: 90
        visible: false
    }

    Label {
        id: name
        text: author
        anchors.left: avatarPhoto.right
        anchors.leftMargin: Theme.paddingLarge
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge
        anchors.right: parent.right
        font.pixelSize: Theme.fontSizeMedium
        horizontalAlignment: Text.AlignLeft
        color: Theme.highlightColor
    }

    Label {
        id: commentLabel
        anchors.left: avatarPhoto.right
        anchors.leftMargin: Theme.paddingLarge
        anchors.top: name.bottom
        anchors.topMargin: Theme.paddingSmall
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingLarge
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: commentText
    }

    Label {
        id: subCommentsLabel
        anchors.left: avatarPhoto.right
        anchors.leftMargin: Theme.paddingLarge
        anchors.top: commentLabel.bottom
        anchors.topMargin: Theme.paddingSmall
        anchors.right: parent.right
        color: Theme.secondaryColor
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        visible: repliesCount > 0
        text: qsTr("%1 replies").arg(repliesCount)
    }
}
