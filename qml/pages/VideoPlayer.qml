import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.6
import com.verdanditeam.yt 1.0
import "components"

Page {
    id: page
    property YtVideo video: null

    allowedOrientations: Orientation.All

    Connections {
        target: video
        onGotStreamUrl: mediaPlayer.play()
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        MediaPlayer {
            id: mediaPlayer
            source: video.streamUrl
        }

        VideoOutput {
            id: videoOutput
            anchors.fill: parent

            source: mediaPlayer
        }

        MouseArea {
            anchors.fill: videoOutput
            onClicked: {
                mediaPlayer.playbackState == MediaPlayer.PlayingState ? mediaPlayer.pause() : mediaPlayer.play()
            }
        }

//        Video {
//            id: videoPlayer
//            fillMode: VideoOutput.PreserveAspectCrop
//            anchors.top: parent.top
//            anchors.left: parent.left
//            anchors.right: parent.right
//            width : page.width
//            height: page.height
//            source: video.streamUrl

//            MouseArea {
//                anchors.fill: parent
//                onClicked: {
//                    console.log(videoPlayer.width, videoPlayer.height)
//                    videoPlayer.playbackState == MediaPlayer.PlayingState ? videoPlayer.pause() : videoPlayer.play()
//                }
//            }

//            focus: true
//            Keys.onSpacePressed: videoPlayer.playbackState == MediaPlayer.PlayingState ? videoPlayer.pause() : videoPlayer.play()
//            Keys.onLeftPressed: videoPlayer.seek(videoPlayer.position - 5000)
//            Keys.onRightPressed: videoPlayer.seek(videoPlayer.position + 5000)
//        }
    }
}
