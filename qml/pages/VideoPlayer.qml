import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.6
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        Video {
            id: video
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            width : page.width
            height: page.width/1.777777777777778
            source: YT.streamUrl

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    video.playbackState == MediaPlayer.PlayingState ? video.pause() : video.play()
                }
            }

            focus: true
            Keys.onSpacePressed: video.playbackState == MediaPlayer.PlayingState ? video.pause() : video.play()
            Keys.onLeftPressed: video.seek(video.position - 5000)
            Keys.onRightPressed: video.seek(video.position + 5000)
        }
    }
}
