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
import com.verdanditeam.sponsorblock 1.0
import Sailfish.Media 1.0
import org.nemomobile.mpris 1.0
import com.jolla.settings.system 1.0
import org.nemomobile.systemsettings 1.0
import org.nemomobile.configuration 1.0
import Nemo.Notifications 1.0
import Nemo.KeepAlive 1.2
import QtGraphicalEffects 1.0
import QtSensors 5.0
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
    property bool videoChanging: false
    property bool landscape: ( page.orientation === Orientation.Landscape || page.orientation === Orientation.LandscapeInverted )
    property bool landscapeCover: (_orientation === OrientationReading.LeftUp || _orientation === OrientationReading.RightUp) && app.videoCover && Qt.application.state === Qt.ApplicationInactive
    property int autoBrightness: -1
    property int inactiveBrightness: -1
    property int activeBrightness: -1
    property bool fillMode: false
    showNavigationIndicator: _controlsVisible
    allowedOrientations: app.videoCover && Qt.application.state === Qt.ApplicationInactive ? Orientation.Portrait : Orientation.All
    Keys.onRightPressed: mediaPlayer.seek(mediaPlayer.position + 5000)
    Keys.onLeftPressed: mediaPlayer.seek(mediaPlayer.position - 5000)
    Keys.onUpPressed: mediaPlayer.prevVideo()
    Keys.onDownPressed: mediaPlayer.nextVideo()
    property int _orientation: OrientationReading.TopUp

    DisplaySettings {
        id: displaySettings
        onBrightnessChanged: {
            if (inactiveBrightness === -1) {
                inactiveBrightness = brightness
                activeBrightness = brightness
                autoBrightness = displaySettings.autoBrightnessEnabled
                if ( landscape )
                    displaySettings.autoBrightnessEnabled = false
            }
        }
    }

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property bool autoPlay: true
        property bool audioOnlyMode: false
        property bool developerMode: false
        property double buffer: 1.0
        property string videoQuality: "360p"
        property string downloadLocation: "/home/nemo/Downloads/"
    }

    SponsorBlockPlugin {
        id: sponsorBlockPlugin
        property var parsedSkipSegments: ""
        onSkipSegmentsChanged: {
            if (skipSegments != "") {
                parsedSkipSegments = JSON.parse(skipSegments)
            }
        }

        function checkIfInsideSegment(timestamp) {
            for (var i = 0; i < parsedSkipSegments.length; i++) {
                var e = parsedSkipSegments[i];
                if (timestamp > e.segment[0] && timestamp < e.segment[1]) {
                    sponsorBlockPluginNotification.setSummary(e.category)
                    return e.segment[1];
                }
            }
        }
    }

    OrientationSensor {
        id: orientationSensor
        active: true

        onReadingChanged: {
            if (reading.orientation >= OrientationReading.TopUp && reading.orientation <= OrientationReading.RightUp) {
                _orientation = reading.orientation
            }
        }
    }

    Notification {
        id: sponsorBlockPluginNotification
        previewSummary: "Skipped"

        function setSummary(category) {
            var summary = "";

            switch (category) {
            case "intro":
                summary = "Intro Skipped";
                break;
            case "outro":
                summary = "Outro Skipped";
                break;
            case "interaction":
                summary = "Interaction Reminder Skipped";
                break;
            case "selfpromo":
                summary = "Self Promotion Skipped";
                break;
            case "music_offtopic":
                summary = "Offtopic music skipped";
                break;
            case "sponsor":
            default:
                summary = "Sponsor blocked";
            }

           previewSummary = summary;
        }
    }

    Timer {
        id: hideControlsAutomatically
        interval: 3000
        running: false
        repeat: false
        onTriggered: _controlsVisible = false
    }

    Timer {
        id: hideVolumeSlider
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            volumeSlider.visible = false
            volumeIndicator.visible = false
        }
    }

    Timer {
        id: hideBrightnessSlider
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            brightnessSlider.visible = false
            brightnessIndicator.visible = false
        }
    }

    Notification {
         id: downloadNotification

         summary: "Downloaded"
         previewSummary: summary
     }

    function showHideControls() {
        jupii.ping()

        if (_controlsVisible) {
            showAnimation.start()
            hideControlsAutomatically.restart()
            errorPane.show()
        } else {
            hideAnimation.start()
            errorPane.hide()
        }

        if ((_controlsVisible && page.landscape) || page.orientation === Orientation.Portrait) {
            showAnimation3.start()
        } else {
            hideAnimation3.start()
        }
    }

    onOrientationChanged: {
        page.landscape = ( page.orientation === Orientation.Landscape || page.orientation === Orientation.LandscapeInverted )

        if ( Qt.application.state === Qt.ApplicationActive && page.status === PageStatus.Active ) {
            if (landscape) {
                hideAnimation3.start()
                displaySettings.autoBrightnessEnabled = false
                displaySettings.brightness = activeBrightness
            } else {
                showAnimation3.start()
                displaySettings.autoBrightnessEnabled = autoBrightness
                displaySettings.brightness = inactiveBrightness
            }
        }
    }

    on_ControlsVisibleChanged: {
        showHideControls()
    }

    onTitleChanged: {
        app.playing = title
    }

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            app.videoCover = false
        } else if(status === PageStatus.Active && video !== null && video.streamUrl == "") {
            app.videoCover = true
            pacontrol.update()
            showHideControls()
            hideControlsAutomatically.restart()
            sponsorBlockPlugin.videoId = video.getId()

            if(settings.audioOnlyMode) {
                topMenu.resolutionChange("audio")
            } else {
                topMenu.resolutionChange("720p")
            }

        }
    }

    Component.onDestruction: {
        app.videoCover = false
        displaySettings.autoBrightnessEnabled = autoBrightness
        displaySettings.brightness = inactiveBrightness
    }

    function changeVideo() {
        page.videoChanging = true
        mediaPlayer.stop()
        video = app.playlistModel.qmlVideoAt(app.playlistModel.activeRow())
        video.loadStreamUrl()
        setVideoInfo()
    }

    function setVideoInfo() {
        title = video.getTitle()
        description = video.getDescription()
        viewCount = video.viewCount
        author = video.getChannelTitle()
        ChannelAggregator.videoWatched(video)
        listView.positionViewAtIndex(app.playlistModel.activeRow(), ListView.Beginning)
        mediaPlayer.errorMsg = ""
        errorPane.hide()
    }

    Connections {
        target: video
        onStreamUrlChanged: {
            if(!settings.audioOnlyMode) {
                if(videoChanging) videoChanging = false
                mediaPlayer.videoPlay()
            }
            setVideoInfo();
            description = video.getDescription().replace(/(\\n|\\r\\n)/g, '\n').replace(/\\u0026/, '&')
        }

        onAudioStreamUrlChanged: {
            if(settings.audioOnlyMode) {
                if(videoChanging) videoChanging = false
                mediaPlayer.videoPlay()
            }
            setVideoInfo();
            description = video.getDescription().replace(/(\\n|\\r\\n)/g, '\n').replace(/\\u0026/, '&')
        }
    }

    Connections {
        target: YT
        onNotifyDownloaded: {
            downloadNotification.summary = qsTr("Downloaded to") + " ~/" + video.getId() + ".mp4"
            downloadNotification.publish()
        }
    }

    Connections {
        target: app.playlistModel

        onActiveVideoChanged: {
            changeVideo()
        }
    }

    Connections {
        target: pacontrol
        onVolumeChanged: {
            volumeSlider.value = volume
        }
    }

    Connections {
        target: Qt.application
        onStateChanged: {
            if ( state === Qt.ApplicationInactive ) {
                displaySettings.autoBrightnessEnabled = autoBrightness
                displaySettings.brightness = inactiveBrightness
            } else if ( state === Qt.ApplicationActive && landscape ) {
                displaySettings.autoBrightnessEnabled = false
                displaySettings.brightness = activeBrightness
            }
        }
    }


    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            id: topMenu
            visible: page.orientation === Orientation.Portrait && Qt.application.state === Qt.ApplicationActive

            function resolutionChange(name) {
                if(name !== "audio") settings.videoQuality = name
                videoChanging = true
                mediaPlayer.stop()
                YT.setDefinition(name)
                video.loadStreamUrl()
            }

            MenuItem {
                text: qsTr("360p")
                enabled: !settings.audioOnlyMode
                onClicked: {
                    topMenu.resolutionChange("360p")
                }
            }

            MenuItem {
                text: qsTr("720p")
                enabled: !settings.audioOnlyMode
                onClicked: {
                    topMenu.resolutionChange("720p")
                }
            }

            MenuItem {
                text: qsTr("Download")
                enabled: video.streamUrl.toString() !== ""
                onClicked: {
                    console.log(video.streamUrl)
                    YT.download(video.getId(), settings.audioOnlyMode ? video.audioStreamUrl : video.streamUrl, settings.downloadLocation)
                }
            }

            MenuItem {
                text: qsTr("Load recommended videos")
                onClicked: {
                    app.playlistModel.findRecommended(video)
                    app.playlistModel.setActiveRow(0, false)
                }
            }

            MenuItem {
                text: qsTr("Copy url")
                onClicked: Clipboard.text = video.getWebpage()
            }
        }

        Column {
            anchors.fill: parent
            Row {
                id: videoPlayer
                Rectangle {
                    id: videoBackground
                    width : page.width
                    height: landscapeCover
                              ? page.width*1.6
                              : (landscape ? page.height : (settings.videoQuality === "360p" ? page.width/1.74 : page.width/1.777777777777778))
                    color: "black"

                    MediaPlayer {
                        id: mediaPlayer
                        source: settings.audioOnlyMode ? video.audioStreamUrl : video.streamUrl

                        function videoPlay() {
                            videoPlaying = true
                            if (mediaPlayer.bufferProgress == 1 || (settings.developerMode && mediaPlayer.bufferProgress > settings.buffer) || settings.audioOnlyMode) {
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
                            if (!app.playlistModel.nextRowExists()) return
                            videoChanging = true
                            mediaPlayer.stop()
                            app.playlistModel.setActiveRow(app.playlistModel.nextRow())
                        }

                        function prevVideo() {
                            if (!app.playlistModel.previousRowExists()) return
                            videoChanging = true
                            mediaPlayer.stop()
                            app.playlistModel.setActiveRow(app.playlistModel.previousRow())
                        }

                        onPlaybackStateChanged: {
                            if (mediaPlayer.playbackState == MediaPlayer.StoppedState) {
                                app.playing = ""
                                if ( settings.autoPlay && video.streamUrl !== "" && videoChanging === false )
                                    nextVideo()
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
                            nextVideo()
                        }

                        onErrorMsgChanged: errorPane.show()

                        onBufferProgressChanged: {
                            if (videoPlaying && mediaPlayer.bufferProgress == 1
                                    || (videoPlaying && settings.developerMode && mediaPlayer.bufferProgress > settings.buffer)
                                    || videoPlaying && settings.audioOnlyMode) {
                                mediaPlayer.play();
                            }

                            if (mediaPlayer.bufferProgress == 0 && !settings.audioOnlyMode) {
                                mediaPlayer.pause();
                            }
                        }

                        onPositionChanged: {
                            progressSlider.value = position
                            var segment = sponsorBlockPlugin.checkIfInsideSegment(position/1000)
                            if (segment && playbackState !== MediaPlayer.StoppedState) {
                                sponsorBlockPluginNotification.publish()
                                if (segment*1000 >= mediaPlayer.duration) {
                                    seek(mediaPlayer.duration)
                                    stop()
                                } else {
                                    seek(segment*1000)
                                }
                            }
                        }
                    }

                    VideoOutput {
                        id: videoOutput
                        width : landscapeCover ? page.width*1.6 : page.width
                        anchors.centerIn: parent
                        height: landscapeCover
                                  ? page.width
                                  : (landscape ? (page.fillMode ? page.width : page.height) : (settings.videoQuality === "360p" ? page.width/1.74 : page.width/1.777777777777778))
                        source: mediaPlayer
                        fillMode: page.fillMode ? VideoOutput.PreserveAspectCrop : VideoOutput.PreserveAspectFit
                        transform: Rotation {
                            origin.x: (page.width*1.6)/2
                            origin.y: page.width/2
                            angle: landscapeCover
                                   ? (_orientation === OrientationReading.LeftUp ? -90 : 90)
                                   : 0
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: "red"
                            transform: Rotation { origin.x: page.width/2; origin.y: page.height/2; angle: 90}
                            visible: false
                        }

                        Rectangle {
                            id: errorPane
                            anchors.fill: parent
                            property double colorOpacity: 0
                            color: Theme.rgba("black", colorOpacity)
                            Behavior on colorOpacity {
                                NumberAnimation {}
                            }
                            visible: true//mediaPlayer.errorMsg !== ""

                            function show() {
                                colorOpacity = 0.5
                            }

                            function hide() {
                                colorOpacity = 0
                            }

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
                            running: (!settings.developerMode && mediaPlayer.bufferProgress != 1) || (settings.developerMode && mediaPlayer.bufferProgress < settings.buffer)
                        }

                        Image {
                            id: thumbnail
                            anchors.fill: parent
                            source: video.getThumbnailUrl()
                            asynchronous: true
                            fillMode: Image.PreserveAspectCrop
                            visible: settings.audioOnlyMode
                        }

                        SilicaFlickable {
                            id: videoOptions
                            anchors.fill: videoOutput
                            flickableDirection: Flickable.HorizontalAndVerticalFlick
                            interactive: !landscape
                            contentHeight: this.height + Theme.itemSizeMedium
                            contentY: 0
                            clip: true

                            onMovementEnded: {
                                if (videoOptions.contentY > 0) {
                                    if (videoOptions.contentY === Theme.itemSizeMedium) {
                                        videoChanging = true
                                        mediaPlayer.stop()
                                        settings.audioOnlyMode = !settings.audioOnlyMode
                                        var quality = settings.audioOnlyMode ? "audio" : settings.videoQuality
                                        topMenu.resolutionChange(quality)
                                    }

                                    videoOptions.contentY = 0
                                }
                            }

                            Rectangle {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                height: Theme.itemSizeMedium
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightBackgroundColor, 0.0) }
                                    GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightBackgroundColor, 0.7) }
                                }
                            }

                            Image {
                                id: audioOnlyIcon
                                width: Theme.itemSizeMedium
                                height: Theme.itemSizeMedium
                                fillMode: Image.PreserveAspectFit
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: (Theme.itemSizeLarge - Theme.itemSizeMedium)
                                anchors.horizontalCenter: parent.horizontalCenter
                                source: settings.audioOnlyMode ? "qrc:///images/icon-m-audio-only-disable.svg" : "qrc:///images/icon-m-audio-only-enable.svg"
                            }

                            MouseArea {
                                id: mousearea
                                anchors.fill: parent
                                propagateComposedEvents: true
                                property int offset: page.height/20
                                property int offsetHeight: height - (offset*2)
                                property int step: offsetHeight / 10
                                property bool stepChanged: false
                                property int brightnessStep: displaySettings.maximumBrightness / 10
                                property int lambdaVolumeStep: -1
                                property int lambdaBrightnessStep: -1
                                property int currentVolume: -1

                                Timer{
                                    id: doubleClickTimer
                                    interval: 200
                                }

                                function calculateStep(mouse) {
                                    return Math.round((offsetHeight - (mouse.y-offset)) / step)
                                }

                                onReleased: {
                                    if (doubleClickTimer.running) doubleClicked(mouse)
                                    if (!doubleClickTimer.running) doubleClickTimer.start()
                                    if (!stepChanged) _controlsVisible = !_controlsVisible

                                    if ( landscape ) {
                                        flickable.flickableDirection = Flickable.VerticalFlick
                                        lambdaVolumeStep = -1
                                        lambdaBrightnessStep = -1
                                        stepChanged = false
                                    }
                                }

                                onPressed: {
                                    if ( landscape ) {
                                        pacontrol.update()
                                        flickable.flickableDirection = Flickable.HorizontalFlick
                                        lambdaBrightnessStep = lambdaVolumeStep = calculateStep(mouse)
                                    }
                                }

                                function doubleClicked(mouse) {
                                    if ( landscape ) {
                                        var newPos = null
                                        if(mouse.x < mousearea.width/2 ) {
                                            newPos = mediaPlayer.position - 5000
                                            if(newPos < 0) newPos = 0
                                            mediaPlayer.seek(newPos)
                                            backwardIndicator.visible = true
                                        } else if (mouse.x > mousearea.width/2) {
                                            newPos = mediaPlayer.position + 5000
                                            if(newPos > mediaPlayer.duration) {
                                                mediaPlayer.nextVideo()
                                                return
                                            }
                                            mediaPlayer.seek(newPos)
                                            forwardIndicator.visible = true
                                        }
                                    }
                                }

                                Connections {
                                    target: pacontrol
                                    onVolumeChanged: {
                                        mousearea.currentVolume = volume
                                        if (volume > 10) {
                                            mousearea.currentVolume = 10
                                        } else if (volume < 0) {
                                            mousearea.currentVolume = 0
                                        }
                                    }
                                }

                                onPositionChanged: {
                                    if ( landscape ) {
                                        var step = calculateStep(mouse)
                                        if((mouse.y - offset) > 0 && (mouse.y + offset) < offsetHeight && mouse.x < mousearea.width/2 && lambdaVolumeStep !== step) {
                                            pacontrol.setVolume(currentVolume - (lambdaVolumeStep - step))
                                            volumeSlider.value = currentVolume - (lambdaVolumeStep - step)
                                            lambdaVolumeStep = step
                                            volumeSlider.visible = true
                                            volumeIndicator.visible = true
                                            hideVolumeSlider.restart()
                                            pacontrol.update()
                                            stepChanged = true
                                        } else if ((mouse.y - offset) > 0 && (mouse.y + offset) < offsetHeight && mouse.x > mousearea.width/2 && lambdaBrightnessStep !== step) {
                                            var relativeStep = Math.round(displaySettings.brightness/brightnessStep) - (lambdaBrightnessStep - step)
                                            if (relativeStep > 10) relativeStep = 10;
                                            if (relativeStep < 0) relativeStep = 0;
                                            displaySettings.brightness = relativeStep * brightnessStep
                                            activeBrightness = relativeStep * brightnessStep
                                            lambdaBrightnessStep = step
                                            brightnessSlider.value = relativeStep
                                            brightnessSlider.visible = true
                                            brightnessIndicator.visible = true
                                            hideBrightnessSlider.restart()
                                            stepChanged = true
                                        }
                                    }
                                }
                            }
                        }
                    }

                    DisplayBlanking {
                        preventBlanking: mediaPlayer.playbackState == MediaPlayer.PlayingState
                    }

                    Row {
                        id: volumeIndicator
                        anchors.centerIn: parent
                        visible: false
                        spacing: Theme.paddingLarge

                        Image {
                            width: Theme.itemSizeLarge
                            height: Theme.itemSizeLarge
                            source: "image://theme/icon-m-speaker-on"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Label {
                            text: (mousearea.currentVolume * 10) + "%"
                            font.pixelSize: Theme.fontSizeHuge
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Row {
                        id: brightnessIndicator
                        anchors.centerIn: parent
                        visible: false
                        spacing: Theme.paddingLarge

                        Image {
                            width: Theme.itemSizeLarge
                            height: Theme.itemSizeLarge
                            source: "image://theme/icon-m-light-contrast"
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Label {
                            text: (Math.round(displaySettings.brightness/mousearea.brightnessStep) * 10) + "%"
                            font.pixelSize: Theme.fontSizeHuge
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Row {
                        id: backwardIndicator
                        anchors.centerIn: parent
                        visible: false
                        spacing: -Theme.paddingLarge*2

                        Image {
                            id: prev1
                            width: Theme.itemSizeLarge
                            height: Theme.itemSizeLarge
                            anchors.verticalCenter: parent.verticalCenter
                            fillMode: Image.PreserveAspectFit
                            source: "image://theme/icon-cover-play"

                            transform: Rotation{
                                angle: 180
                                origin.x: prev1.width/2
                                origin.y: prev1.height/2
                            }
                        }
                        Image {
                            id: prev2
                            width: Theme.itemSizeLarge
                            height: Theme.itemSizeLarge
                            anchors.verticalCenter: parent.verticalCenter
                            fillMode: Image.PreserveAspectFit
                            source: "image://theme/icon-cover-play"

                            transform: Rotation{
                                angle: 180
                                origin.x: prev2.width/2
                                origin.y: prev2.height/2
                            }
                        }

                        Timer {
                            id: hideBackward
                            interval: 300
                            onTriggered: backwardIndicator.visible = false
                        }

                        onVisibleChanged: if (backwardIndicator.visible) hideBackward.start()
                    }

                    Row {
                        id: forwardIndicator
                        anchors.centerIn: parent
                        visible: false
                        spacing: -Theme.paddingLarge*2

                        Image {
                            width: Theme.itemSizeLarge
                            height: Theme.itemSizeLarge
                            anchors.verticalCenter: parent.verticalCenter
                            fillMode: Image.PreserveAspectFit
                            source: "image://theme/icon-cover-play"

                        }
                        Image {
                            width: Theme.itemSizeLarge
                            height: Theme.itemSizeLarge
                            anchors.verticalCenter: parent.verticalCenter
                            fillMode: Image.PreserveAspectFit
                            source: "image://theme/icon-cover-play"
                        }

                        Timer {
                            id: hideForward
                            interval: 300
                            onTriggered: forwardIndicator.visible = false
                        }

                        onVisibleChanged: if (forwardIndicator.visible) hideForward.start()
                    }

                    NumberAnimation {
                        id: showAnimation
                        targets: [progress, duration, playButton, prevButton, nextButton, fillModeButton, castButton]
                        properties: "opacity"
                        to: 1
                        duration: 100
                    }
                    NumberAnimation {
                        id: hideAnimation
                        targets: [progress, duration, playButton, prevButton, nextButton, fillModeButton, castButton]
                        properties: "opacity"
                        to: 0
                        duration: 100
                    }

                    IconButton {
                        id: playButton
                        visible: opacity != 0
                        icon.source: mediaPlayer.playbackState == MediaPlayer.PlayingState ? "image://theme/icon-m-pause" : "image://theme/icon-m-play"
                        anchors.centerIn: parent
                        onClicked: mediaPlayer.playbackState == MediaPlayer.PlayingState ? mediaPlayer.videoPause() : mediaPlayer.videoPlay()
                    }

                    IconButton {
                        id: nextButton
                        visible: opacity != 0
                        icon.source: "image://theme/icon-m-next"
                        anchors.top: playButton.top
                        anchors.left: playButton.right
                        anchors.leftMargin: page.width/4 - playButton.width/2
                        onClicked: mediaPlayer.nextVideo()
                    }

                    IconButton {
                        id: prevButton
                        visible: opacity != 0
                        icon.source: "image://theme/icon-m-previous"
                        anchors.top: playButton.top
                        anchors.right: playButton.left
                        anchors.rightMargin: page.width/4 - playButton.width/2
                        onClicked: mediaPlayer.prevVideo()
                    }

                    IconButton {
                        id: castButton
                        visible: opacity != 0 && landscape && jupii.found
                        icon.source: "qrc:///images/icon-m-cast.svg"
                        width: Theme.itemSizeExtraSmall
                        height: width
                        anchors.right: fillModeButton.left
                        anchors.top: parent.top
                        anchors.margins: Theme.paddingMedium
                        icon.width: width
                        icon.height: width
                        onClicked: jupii.addUrlOnceAndPlay(video.streamUrl.toString(), video.getWebpage(), title, author, (settings.audioOnlyMode ? 1 : 2), "microtube", "/usr/share/icons/hicolor/172x172/apps/microtube.png")
                    }

                    IconButton {
                        id: fillModeButton
                        visible: opacity != 0 && landscape
                        icon.source: page.fillMode ? "qrc:///images/icon-m-scale-to-16-9.svg" : "qrc:///images/icon-m-scale-to-21-9.svg"
                        width: Theme.itemSizeExtraSmall
                        height: width
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: Theme.paddingMedium
                        icon.width: width
                        icon.height: width
                        onClicked: page.fillMode = !page.fillMode
                    }

                    Slider {
                        id: volumeSlider
                        visible: false
                        x: page.width - height
                        y: page.height
                        width: page.height
                        minimumValue: 0
                        maximumValue: 10
                        transform: Rotation { angle: -90}
                        enabled: false

                        Behavior on opacity {
                            PropertyAction {}
                        }
                    }

                    Slider {
                        id: brightnessSlider
                        visible: false
                        x: 0
                        y: page.height
                        width: page.height
                        transform: Rotation { angle: -90}
                        enabled: false
                        maximumValue: 10
                        minimumValue: 0

                        Behavior on opacity {
                            PropertyAction {}
                        }
                    }

                    Label {
                        id: progress
                        width: Theme.itemSizeExtraSmall
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.margins: Theme.paddingLarge
                        text: Format.formatDuration(Math.round(mediaPlayer.position/1000), ((mediaPlayer.duration/1000) > 3600 ? Formatter.DurationLong : Formatter.DurationShort))
                    }

                    Label {
                        id: duration
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: Theme.paddingLarge
                        text: Format.formatDuration(Math.round(mediaPlayer.duration/1000), ((mediaPlayer.duration/1000) > 3600 ? Formatter.DurationLong : Formatter.DurationShort))
                    }

                    Slider {
                        id: progressSlider
                        value: mediaPlayer.position
                        valueText: down ? Format.formatDuration(Math.round(value/1000), ((value/1000) > 3600 ? Formatter.DurationLong : Formatter.DurationShort)) : ""
                        minimumValue: 0
                        maximumValue: mediaPlayer.duration
                        anchors.bottom: if (landscape && opacity == 0) videoBackground.top; else videoBackground.bottom
                        x: landscape ? progress.width : - Theme.paddingLarge * 4
                        width: landscape ? parent.width - progress.width - duration.width : parent.width + Theme.paddingLarge * 8
                        anchors.bottomMargin: page.landscape ? 0 : (down ? -height/3 : -height/2)
                        handleVisible: _controlsVisible || down

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

                        onReleased: mediaPlayer.seek(progressSlider.value)
                    }
                }
            }

            TextArea {
                id: videoCoverTitle
                text: title
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.paddingLarge
                font.pixelSize: Theme.fontSizeHuge
                color: Theme.highlightColor
                font.family: Theme.fontFamilyHeading
                readOnly: true
                textMargin: 0
                labelVisible: false
                wrapMode: TextEdit.WordWrap
                visible: app.videoCover && Qt.application.state === Qt.ApplicationInactive
            }

            Row {
                id: playlist
                width: parent.width
                height: parent.height - videoPlayer.height
                NumberAnimation { id: anim; target: listView; property: "contentY"; duration: 500 }
                SilicaFlickable {
                    id: playlistFlickable
                    width: parent.width
                    height: playlist.height
                    contentHeight: (page.height - videoPlayer.height) + videoTitle.height + authorViews.height + videoDescription.height /*+ comments.height*/ + progress.height/2 + Theme.paddingLarge
                    clip: true
                    property int oldContentHeight: 0
                    onContentHeightChanged: {
                        if (oldContentHeight !== 0 && contentY >= parseInt(oldContentHeight - (page.height - videoPlayer.height))) {
                            var diff = contentHeight - oldContentHeight
                            playlistFlickable.contentY += diff
                            oldContentHeight = JSON.parse(JSON.stringify(contentHeight));
                        } else {
                            oldContentHeight = JSON.parse(JSON.stringify(contentHeight));
                        }
                    }

                    Column {
                        width: parent.width
                        padding: Theme.paddingLarge
                        visible: !(app.videoCover && Qt.application.state === Qt.ApplicationInactive)
                        Label {
                            id: videoTitle
                            text: title
                            width: page.width - Theme.paddingLarge*2
                            font.pixelSize: app.videoCover && Qt.application.state === Qt.ApplicationInactive ? Theme.fontSizeHuge : Theme.fontSizeLarge
                            color: Theme.highlightColor
                            font.family: Theme.fontFamilyHeading
                            wrapMode: TextEdit.WordWrap
                        }

                        Item {
                            height: Theme.paddingLarge
                            width: 1
                        }

                        Row{
                            width: parent.width
                            Column {
                                id: authorViews
                                width: parent.width/2
                                spacing: Theme.paddingSmall
                                Label {
                                    text: author
                                    width: parent.width
                                    font.pixelSize: Theme.fontSizeMedium
                                    truncationMode: TruncationMode.Fade
                                    color: Theme.primaryColor

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            pageStack.navigateBack(PageStackAction.Immediate)
                                            pageStack.push(Qt.resolvedUrl("Channel.qml"), {channel: YT.getChannel(video.getChannelId())})
                                        }
                                    }
                                }

                                Label {
                                    text: video.viewCount
                                    font.pixelSize: Theme.fontSizeMedium
                                    truncationMode: TruncationMode.Fade
                                    color: Theme.secondaryColor
                                    bottomPadding: Theme.paddingLarge
                                }
                            }

                            Row {
                                width: parent.width/2
                                rightPadding: Theme.paddingLarge*2
                                layoutDirection: Qt.RightToLeft

                                Button {
                                    id: subscribeButton
                                    preferredWidth: Theme.itemSizeHuge
                                    property bool subscribed: YT.getChannel(video.getChannelId()).isSubscribed
                                    text: subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                                    onClicked: {
                                        var channel = YT.getChannel(video.getChannelId())
                                        channel.isSubscribed ? channel.unsubscribe() : channel.subscribe()
                                    }
                                }

                                IconButton {
                                    icon.source: "image://theme/icon-m-share"
                                    onClicked: pageStack.push(Qt.resolvedUrl("components/SharePage.qml"), {videoUrl: video.getWebpage(), videoTitle: title})
                                }
                            }
                        }


                        LinkedLabel {
                            id: videoDescription
                            plainText: description
                            width: page.width - Theme.paddingLarge*2
                            color: palette.secondaryColor
                            wrapMode: TextEdit.WordWrap
                        }

//                        CommentsButton {
//                            id: comments
//                            text: qsTr("Comments")
//                            width: parent.width - Theme.paddingLarge

//                            onClicked: pageStack.push(Qt.resolvedUrl("Comments.qml"), {videoId: video.getId()})
//                        }

                        SilicaFastListView {
                            id: listView
                            width: parent.width - Theme.paddingLarge
                            height: page.height - videoPlayer.height
                            maximumFlickVelocity: 9999
                            spacing: Theme.paddingMedium
                            model: app.playlistModel
                            clip: true
                            interactive: playlistFlickable.contentY >= parseInt(playlistFlickable.contentHeight - (page.height - videoPlayer.height))
                            delegate: VideoElement {
                                id: delegate
                                subPage: true
                            }
                        }
                    }
                }
            }
        }
    }

    Jupii {
        id: jupii
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
            mediaPlayer.videoPause()
        }

        onPlayRequested: {
            mediaPlayer.videoPlay()
        }

        onPlayPauseRequested: {
            mediaPlayer.playbackState == MediaPlayer.PlayingState ? mediaPlayer.videoPause() : mediaPlayer.videoPlay()
        }

        onStopRequested: {
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
