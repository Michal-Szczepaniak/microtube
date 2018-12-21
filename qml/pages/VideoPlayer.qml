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
import QtMultimedia 5.6
import com.verdanditeam.yt 1.0
import com.verdanditeam.ytchannel 1.0
import Sailfish.Media 1.0
import org.nemomobile.mpris 1.0
import "components"

Page {
    id: page
    property YtVideo video: null
    property string title: ""
    property string description: ""
    property string viewCount: ""
    property string author: ""
    property bool subscribed: false
    property bool _controlsVisible: true
    property bool landscape: ( page.orientation === Orientation.Landscape || page.orientation === Orientation.LandscapeInverted )


    Timer {
        id: hideControlsAutomatically
        interval: 3000
        running: false
        repeat: false
        onTriggered: _controlsVisible = false
    }

    function showHideControls() {
        if (_controlsVisible) {
            showAnimation.start()
            hideControlsAutomatically.restart()
        } else {
            hideAnimation.start()
        }

        if ((_controlsVisible && page.landscape) || page.orientation === Orientation.Portrait) {
            showAnimation3.start()
        } else {
            hideAnimation3.start()
        }
    }

    onOrientationChanged: {

        if ((_controlsVisible && page.landscape) || page.orientation === Orientation.Portrait)
            showAnimation3.start()
        else
            hideAnimation3.start()
    }

    on_ControlsVisibleChanged: {
        showHideControls()
    }

    onTitleChanged: {
        app.playing = title
    }

    Component.onCompleted: {
        showHideControls()
        hideControlsAutomatically.restart()
    }

    showNavigationIndicator: page.orientation === Orientation.Portrait

    allowedOrientations: Orientation.All

    function changeVideo() {
        page.video = YTPlaylist.videoAt(YTPlaylist.activeRow())
        video.loadStreamUrl()
        title = video.getTitle()
        description = video.getDescription()
        viewCount = video.getViewCount()
        author = video.getChannelTitle()
        listView.positionViewAtIndex(YTPlaylist.activeRow(), ListView.Beginning)
        mediaPlayer.errorMsg = ""
    }

    Connections {
        target: video
        onStreamUrlChanged: {
            mediaPlayer.videoPlay()
        }
    }

    Connections {
        target: YTPlaylist
        onActiveRowChanged: {
            changeVideo()
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            visible: page.orientation === Orientation.Portrait
            MenuItem {
                text: qsTr("720p")
                onClicked: {
                    YT.setDefinition("720p")
                    video.loadStreamUrl()
                }
            }
            MenuItem {
                text: qsTr("480p")
                onClicked: {
                    YT.setDefinition("480p")
                    video.loadStreamUrl()
                }
            }
            MenuItem {
                text: qsTr("360p")
                onClicked: {
                    YT.setDefinition("360p")
                    video.loadStreamUrl()
                }
            }
            MenuItem {
                text: qsTr("Copy url")
                onClicked: Clipboard.text = video.getWebpage()
            }
            MenuItem {
                property bool subscribed: video.isSubscribed(video.getChannelId())
                text: subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                onClicked: {
                    YT.toggleSubscription()
                    subscribed = video.isSubscribed(video.getChannelId())
                }
            }
        }

        Column {
            anchors.fill: parent
            Row {
                id: videoPlayer
                Rectangle {
                    width: page.width
                    height: page.width/1.777777777777778
                    color: "black"

                    MediaPlayer {
                        id: mediaPlayer
                        source: video.streamUrl

                        function videoPlay() {
                            videoPlaying = true
                            if (mediaPlayer.bufferProgress == 1) {
                                mediaPlayer.play()
                            }
                        }

                        function videoPause() {
                            videoPlaying = false
                            mediaPlayer.pause()
                        }

                        property bool videoPlaying: false
                        property string errorMsg: ""

                        function nextVideo() {
                            if (!YTPlaylist.nextRowExists()) return
                            mediaPlayer.seek(0)
                            YTPlaylist.setActiveRow(YTPlaylist.nextRow())
                            video.loadStreamUrl()
                            changeVideo()
                        }

                        function prevVideo() {
                            if (!YTPlaylist.previousRowExists()) return
                            mediaPlayer.seek(0)
                            YTPlaylist.setActiveRow(YTPlaylist.previousRow())
                            video.loadStreamUrl()
                            changeVideo()
                        }

                        onPlaybackStateChanged: {
                            if (mediaPlayer.playbackState == MediaPlayer.StoppedState) {
                                app.playing = ""
                            }

                            mprisPlayer.playbackState = mediaPlayer.playbackState === MediaPlayer.PlayingState ?
                                        Mpris.Playing : mediaPlayer.playbackState === MediaPlayer.PausedState ?
                                            Mpris.Paused : Mpris.Stopped
                        }

                        onError: {
                            if ( error === MediaPlayer.ResourceError ) errorMsg = qsTr("Error: Problem with allocating resources")
                            else if ( error === MediaPlayer.ServiceMissing ) errorMsg = qsTr("Error: Media service error")
                            else if ( error === MediaPlayer.FormatError ) errorMsg = qsTr("Error: Video or Audio format is not supported")
                            else if ( error === MediaPlayer.AccessDenied ) errorMsg = qsTr("Error: Access denied to the video")
                            else if ( error === MediaPlayer.NetworkError ) errorMsg = qsTr("Error: Network error")
                            stop()
                        }

                        onBufferProgressChanged: {
                            if (videoPlaying && mediaPlayer.bufferProgress == 1) {
                                mediaPlayer.play();
                            }

                            if (mediaPlayer.bufferProgress == 0) {
                                mediaPlayer.pause();
                            }
                        }
                    }

                    VideoOutput {
                        id: videoOutput
                        anchors.fill: parent
                        width : page.width
                        height: page.width/1.777777777777778
                        source: mediaPlayer

                        Rectangle {
                            id: errorPane
                            z: 99
                            anchors.fill: parent
                            color: Theme.rgba("black", 0.8)
                            visible: mediaPlayer.errorMsg !== ""
                            Label {
                                id: errorText
                                text: mediaPlayer.errorMsg
                                visible: parent.visible
                                anchors.centerIn: parent
                                font.pointSize: Theme.fontSizeExtraLarge
                                font.family: Theme.fontFamilyHeading
                            }
                        }

                        BusyIndicator {
                            size: BusyIndicatorSize.Large
                            anchors.centerIn: parent
                            running: mediaPlayer.bufferProgress != 1
                        }

                        MouseArea {
                            anchors.fill: videoOutput
                            onClicked: {
                                _controlsVisible = !_controlsVisible
                            }
                        }

                        Label {
                            id: progress
                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                            anchors.margins: Theme.paddingLarge
                            text:  Format.formatDuration(Math.round(mediaPlayer.position/1000), Formatter.DurationShort)
                        }

                        Label {
                            id: duration
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.margins: Theme.paddingLarge
                            text: Format.formatDuration(Math.round(mediaPlayer.duration/1000), Formatter.DurationShort)
                        }

                        NumberAnimation {
                            id: showAnimation
                            targets: [progress, duration, playButton, prevButton, nextButton]
                            properties: "opacity"
                            to: 1
                            duration: 100
                        }
                        NumberAnimation {
                            id: hideAnimation
                            targets: [progress, duration, playButton, prevButton, nextButton]
                            properties: "opacity"
                            to: 0
                            duration: 100
                        }

                        IconButton {
                            id: playButton
                            enabled: opacity != 0
                            icon.source: mediaPlayer.playbackState == MediaPlayer.PlayingState ? "image://theme/icon-m-pause" : "image://theme/icon-m-play"
                            anchors.centerIn: parent
                            onClicked: mediaPlayer.playbackState == MediaPlayer.PlayingState ? mediaPlayer.videoPause() : mediaPlayer.videoPlay()
                        }

                        IconButton {
                            id: nextButton
                            enabled: opacity != 0
                            icon.source: "image://theme/icon-m-next"
                            anchors.top: playButton.top
                            anchors.left: playButton.right
                            anchors.leftMargin: page.width/4 - playButton.width/2
                            onClicked: mediaPlayer.nextVideo()
                        }

                        IconButton {
                            id: prevButton
                            enabled: opacity != 0
                            icon.source: "image://theme/icon-m-previous"
                            anchors.top: playButton.top
                            anchors.right: playButton.left
                            anchors.rightMargin: page.width/4 - playButton.width/2
                            onClicked: mediaPlayer.prevVideo()
                        }

                        Slider {
                            id: proggress
                            value: mediaPlayer.position
                            minimumValue: 0
                            maximumValue: mediaPlayer.duration
                            anchors.left: page.landscape ? proggress.right : videoOutput.left
                            anchors.right: page.landscape ? duration.left : videoOutput.right
                            anchors.bottom: videoOutput.bottom
                            anchors.bottomMargin: page.landscape ? 0 : -proggress.height/2
                            anchors.leftMargin: page.landscape ? Theme.paddingLarge : -Theme.paddingLarge*4
                            anchors.rightMargin: page.landscape ? -Theme.paddingLarge*2 : -Theme.paddingLarge*4
                            handleVisible: _controlsVisible

                            NumberAnimation on opacity {
                                id: showAnimation3
                                to: 1
                                duration: 100

                            }
                            NumberAnimation on opacity {
                                id: hideAnimation3
                                to: 0
                                duration: 100
                            }

                            onReleased: mediaPlayer.seek(proggress.value)
                        }
                    }

                    ScreenBlank {
                        suspend: mediaPlayer.playbackState == MediaPlayer.PlayingState
                    }
                }
            }
            Row {
                id: desc
                Column {
                    padding: Theme.paddingLarge
                    TextArea {
                        text: title
                        width: page.width - Theme.paddingLarge*2
                        font.pixelSize: Theme.fontSizeLarge
                        color: Theme.highlightColor
                        font.family: Theme.fontFamilyHeading
                        readOnly: true
                        textMargin: 0
                        labelVisible: false
                        wrapMode: TextEdit.WordWrap
                    }

                    Row {
                        width: parent.width
                        spacing: Theme.paddingLarge
                        Label {
                            text: author
                            font.pixelSize: Theme.fontSizeMedium
                            truncationMode: TruncationMode.Fade
                            color: Theme.primaryColor
                            bottomPadding: Theme.paddingLarge

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    YT.watchChannel(video.getChannelId())
                                    pageStack.navigateBack()
                                }
                            }
                        }

                        Label {
                            text: qsTr("%L1 views", '', viewCount).arg(viewCount)
                            font.pixelSize: Theme.fontSizeMedium
                            truncationMode: TruncationMode.Fade
                            color: Theme.secondaryColor
                            bottomPadding: Theme.paddingLarge
                        }
                    }


                    TextArea {
                        text: description
                        width: page.width - Theme.paddingLarge*2
//                        height: page.height/3
                        readOnly: true
                        textMargin: 0
                        labelVisible: false
                        wrapMode: TextEdit.WordWrap
                    }
                }
            }

            Row {
                id: playlist
                width: parent.width
                height: parent.height - videoPlayer.height - desc.height
                NumberAnimation { id: anim; target: listView; property: "contentX"; duration: 500 }
                SilicaListView {
                    id: listView
                    width: parent.width
                    height: parent.height
                    spacing: Theme.paddingMedium
                    model: YTPlaylist
                    clip: true
                    delegate: VideoElement {
                        subPage: true
                    }
                }
            }
        }
    }

    CoverActionList {
        id: coverAction
        enabled: mediaPlayer.playbackState !== MediaPlayer.StoppedState

        CoverAction {
            iconSource: mediaPlayer.playbackState == MediaPlayer.PlayingState ? "image://theme/icon-cover-pause" : "image://theme/icon-cover-play"
            onTriggered: {
                mediaPlayer.playbackState == MediaPlayer.PlayingState ? mediaPlayer.videoPause() : mediaPlayer.videoPlay()
            }
        }

        CoverAction {
            iconSource: "image://theme/icon-cover-next-song"
            onTriggered: {
                mediaPlayer.nextVideo()
            }
        }
    }

    MprisPlayer {
        id: mprisPlayer

        serviceName: "microtube"
        property string artist: author
        property string song: title
        property var playbackState: Mpris.Playing

        onArtistChanged: {
            var metadata = mprisPlayer.metadata

            metadata[Mpris.metadataToString(Mpris.Artist)] = [artist] // List of strings

            mprisPlayer.metadata = metadata
        }

        onSongChanged: {
            var metadata = mprisPlayer.metadata

            metadata[Mpris.metadataToString(Mpris.Title)] = song // String

            mprisPlayer.metadata = metadata
        }

        identity: "microtube"

        canControl: true

        canGoNext: true
        canGoPrevious: true
        canPause: true
        canPlay: true
        canSeek: true

        playbackStatus: playbackState

        loopStatus: Mpris.None
        shuffle: false
        volume: 1

        onPauseRequested: {
            console.log("pause")
            mediaPlayer.videoPause()
        }

        onPlayRequested: {
            console.log("play")
            mediaPlayer.videoPlay()
        }

        onPlayPauseRequested: {
            console.log("pauseplay")
            mediaPlayer.playbackState == MediaPlayer.PlayingState ? mediaPlayer.videoPause() : mediaPlayer.videoPlay()
        }

        onStopRequested: {
            console.log("stop")
            mediaPlayer.stop()
        }

        onNextRequested: {
            mediaPlayer.nextVideo()
        }

        onPreviousRequested: {
            mediaPlayer.prevVideo()
        }

        onSeekRequested: {
            mediaPlayer.seek(offset)
        }
    }
}
