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
import Sailfish.Share 1.0
import "components"
import "components/helpers.js" as Helpers

Page {
    id: page
    property string videoIdToPlay
    property bool subscribed: false
    property bool _controlsVisible: true
    property bool videoChanging: false
    property bool landscape: ( page.orientation === Orientation.Landscape || page.orientation === Orientation.LandscapeInverted )
    property bool landscapeCover: (_orientation === OrientationReading.LeftUp || _orientation === OrientationReading.RightUp) && app.videoCover && Qt.application.state === Qt.ApplicationInactive
    property int autoBrightness: -1
    property int inactiveBrightness: -1
    property int activeBrightness: -1
    property bool fillMode: false
    property bool playlistMode: false
    showNavigationIndicator: _controlsVisible
    allowedOrientations: app.videoCover && Qt.application.state === Qt.ApplicationInactive ? Orientation.Portrait : Orientation.All
    Keys.onRightPressed: videoPlayer.seek(videoPlayer.position + 5000)
    Keys.onLeftPressed: videoPlayer.seek(videoPlayer.position - 5000)
    Keys.onUpPressed: videoPlayer.prevVideo()
    Keys.onDownPressed: videoPlayer.nextVideo()
    property int _orientation: OrientationReading.TopUp

    DisplaySettings {
        id: displaySettings
        onBrightnessChanged: {
            if (inactiveBrightness === -1) {
                inactiveBrightness = brightness
                activeBrightness = brightness
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
        property int maxDefinition: 1080
        property string downloadLocation: StandardPaths.download
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

    ChannelHelper {
        id: channelHelper
    }

    VideoHelper {
        id: videoHelperYupii

        onGotVideoInfo: {
            jupii.addUrlOnceAndPlay(videoHelperYupii.videoUrl, videoHelperYupii.currentVideo.url, videoHelperYupii.currentVideo.title, videoHelperYupii.currentVideo.author.name, videoHelperYupii.currentVideo.description, 4, "microtube", "/usr/share/icons/hicolor/172x172/apps/microtube.png")
        }
    }

    VideoHelper {
        id: videoHelper

        onSubtitleChanged: {
            var playing = videoPlayer.state === VideoPlayer.StatePlaying
            if (playing) videoPlayer.pause()
            videoPlayer.subtitle = videoHelper.subtitle
            if (playing) videoPlayer.play()
        }

        onGotVideoInfo: {
            videoPlayer.setAudioOnlyMode(settings.audioOnlyMode)
            videoPlayer.videoSource = videoHelper.videoUrl
            videoPlayer.audioSource = videoHelper.audioUrl
            if (videoChanging) videoChanging = false
            videoPlayer.play()
            sponsorBlockPlugin.videoId = videoHelper.currentVideo.videoId
            currentPlaylist.loadRecommendedVideos(videoHelper.currentVideo.url)
            videoHelper.markAsWatched()

            if (googleOAuthHelper.linked) {
                googleOAuthHelper.getRating(videoHelper.currentVideo.videoId)
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

    ShareAction {
        id: shareAction
        resources: [ videoHelper.currentVideo.url ]
    }

    function showHideControls() {
        jupii.ping()

        if (_controlsVisible) {
            showAnimation.start()
            hideControlsAutomatically.restart()
            dimPane.show()
        } else {
            hideAnimation.start()
            dimPane.hide()
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
                displaySettings.brightness = activeBrightness
            } else {
                showAnimation3.start()
                displaySettings.brightness = inactiveBrightness
            }
        }
    }

    on_ControlsVisibleChanged: {
        showHideControls()
    }

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            app.videoCover = false
        } else if(status === PageStatus.Active && videoHelper.videoUrl == "") {
            videoHelper.loadVideoUrl(videoIdToPlay, settings.maxDefinition)
            app.videoCover = true
            pacontrol.update()
            showHideControls()
            hideControlsAutomatically.restart()
        }
    }

    Component.onDestruction: {
        app.videoCover = false
        displaySettings.brightness = inactiveBrightness
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
                displaySettings.brightness = inactiveBrightness
            } else if ( state === Qt.ApplicationActive && landscape ) {
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

            MenuItem {
                text: qsTr("Download")
                enabled: videoHelper.currentVideo.url !== ""
                onClicked: {
                    var endsWithXml = /\/$/;
                    var path = settings.downloadLocation;
                    if (!endsWithXml.test(path)) path += '/';
                    var PATTERN = /[a-zA-Z\s]/;
                    var filename = videoHelper.currentVideo.title.replace(/\W/g, '')
                    if (filename === "") filename = "download"
                    path += filename;
                    path += '.mp4';
                    videoDownloader.download(videoHelper.currentVideo.url, path)
                }
            }

            MenuItem {
                text: qsTr("Copy url")
                onClicked: Clipboard.text = videoHelper.currentVideo.url
            }
        }

        Column {
            anchors.fill: parent
            Row {
                id: videoPlayerRow
                Rectangle {
                    id: videoBackground
                    width : page.width
                    height: landscapeCover
                              ? page.width*1.6
                              : (landscape ? page.height : (settings.videoQuality === "360p" ? page.width/1.74 : page.width/1.777777777777778))
                    color: "black"

                    VideoPlayer {
                        id: videoPlayer
                        width : landscapeCover ? page.width*1.6 : page.width
                        anchors.centerIn: parent
                        height: landscapeCover
                                  ? page.width
                                  : (landscape ? (page.fillMode ? page.width : page.height) : (settings.videoQuality === "360p" ? page.width/1.74 : page.width/1.777777777777778))


                        transform: Rotation {
                            origin.x: (page.width*1.6)/2
                            origin.y: page.width/2
                            angle: landscapeCover
                                   ? (_orientation === OrientationReading.LeftUp ? -90 : 90)
                                   : 0
                        }

                        Behavior on width { PropertyAnimation { duration: pinchArea.pinching ? 250 : 0 } }
                        Behavior on height { PropertyAnimation { duration: pinchArea.pinching ? 250 : 0 } }

                        onStateChanged: {
                            if (state === VideoPlayer.StatePaused) {
                                console.log("Video paused", settings.autoPlay, videoHelper.videoUrl !== "", videoChanging === false)
                            } else if (state === VideoPlayer.StateStopped) {
                                app.playing = ""
                                console.log("Video stopped", settings.autoPlay, videoHelper.videoUrl !== "", videoChanging === false)
                                if (settings.autoPlay && videoHelper.videoUrl !== "" && videoChanging === false)
                                    nextVideo()
                            } else if (state === VideoPlayer.StatePlaying) {
                                videoPlayer.setPlaybackSpeed(playbackSpeedSlider.value)
                                showHideControls()
                            }

                            mprisPlayer.playbackState = state === VideoPlayer.StatePlaying ?
                                        Mpris.Playing : state === VideoPlayer.StatePaused ?
                                            Mpris.Paused : Mpris.Stopped
                        }

                        onPositionChanged: {
                            progressSlider.value = position
                            var segment = sponsorBlockPlugin.checkIfInsideSegment(position/1000)
                            if (segment && state !== VideoPlayer.StateStopped && !sponsorBlockTimeout.running) {
                                sponsorBlockPluginNotification.publish()
                                seek((segment+1.000)*1000.0)
                                sponsorBlockTimeout.start()
                            }
                        }

                        Timer {
                            id: sponsorBlockTimeout
                            interval: 500
                            repeat: false
                        }

                        function nextVideo() {
                            videoChanging = true
                            videoPlayer.stop()
                            if (playlistMode) {
                                app.playlistModel.nextVideo();
                            } else {
                                videoHelper.loadVideoUrl(currentPlaylist.getIdAt(0), settings.maxDefinition)
                            }
                        }

                        function prevVideo() {
                            if (!app.playlistModel.previousRowExists()) return
                            videoChanging = true
                            videoPlayer.stop()
                            app.playlistModel.setActiveRow(app.playlistModel.previousVideo())
                        }

                        Label {
                            id: subtitles
                            text: videoPlayer.displaySubtitle
                            width: videoPlayerRow.width - Theme.paddingLarge*2
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.weight: Font.Bold
                            color: "white"
                            visible: videoPlayer.subtitle !== ""
                            style: Text.Outline
                            styleColor: "black"
                            anchors.bottom: videoPlayer.bottom
                            anchors.bottomMargin: Theme.paddingMedium + (landscape ? (videoPlayer.height - Screen.width)/2 : 0)
                            anchors.horizontalCenter: videoPlayer.horizontalCenter
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: "red"
                            transform: Rotation { origin.x: page.width/2; origin.y: page.height/2; angle: 90}
                            visible: false
                        }

                        Rectangle {
                            id: dimPane
                            anchors.fill: parent
                            property double colorOpacity: 0
                            color: Theme.rgba("black", colorOpacity)
                            Behavior on colorOpacity {
                                NumberAnimation {}
                            }
                            visible: true

                            function show() {
                                colorOpacity = 0.5
                            }

                            function hide() {
                                colorOpacity = 0
                            }

                            Label {
                                id: errorText
                                visible: parent.visible
                                anchors.centerIn: parent
                                font.pointSize: Theme.fontSizeExtraLarge
                                font.family: Theme.fontFamilyHeading
                            }
                        }

                        BusyIndicator {
                            size: BusyIndicatorSize.Large
                            anchors.centerIn: parent
                            running: videoPlayer.state === VideoPlayer.StateBuffering
                        }

                        Image {
                            id: thumbnail
                            anchors.fill: parent
                            source: videoHelper.currentVideo.bigThumbnail
                            asynchronous: true
                            fillMode: Image.PreserveAspectCrop
                            visible: settings.audioOnlyMode
                        }

                        PinchArea {
                            id: pinchArea
                            anchors.fill: parent
                            enabled: true
                            property bool pinching: false
                            onPinchUpdated: {
                                if (pinch.scale < 0.9) {
                                    page.fillMode = false
                                } else if (pinch.scale > 1.5) {
                                    page.fillMode = true
                                }
                            }
                            onPinchStarted: pinching = true
                            onPinchFinished: pinching = false
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
                                            newPos = videoPlayer.position - 5000
                                            if(newPos < 0) newPos = 0
                                            videoPlayer.seek(newPos)
                                            backwardIndicator.visible = true
                                        } else if (mouse.x > mousearea.width/2) {
                                            newPos = videoPlayer.position + 5000
                                            if(newPos > videoPlayer.duration) {
                                                videoPlayer.nextVideo()
                                                return
                                            }
                                            videoPlayer.seek(newPos)
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
                        preventBlanking: videoPlayer.state === VideoPlayer.StatePlaying
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
                        targets: [progress, duration, playButton, prevButton, nextButton, castButton, subsButton, playbackSpeedButton]
                        properties: "opacity"
                        to: 1
                        duration: 100
                    }
                    NumberAnimation {
                        id: hideAnimation
                        targets: [progress, duration, playButton, prevButton, nextButton, castButton, subsButton, playbackSpeedButton, playbackSpeedSlider]
                        properties: "opacity"
                        to: 0
                        duration: 100
                    }

                    IconButton {
                        id: playButton
                        visible: opacity != 0
                        icon.source: videoPlayer.state === VideoPlayer.StatePlaying ? "image://theme/icon-m-pause" : "image://theme/icon-m-play"
                        anchors.centerIn: parent
                        onClicked: videoPlayer.state === VideoPlayer.StatePlaying ? videoPlayer.pause() : videoPlayer.play()
                    }

                    IconButton {
                        id: nextButton
                        visible: opacity != 0 && playlistMode
                        icon.source: "image://theme/icon-m-next"
                        anchors.top: playButton.top
                        anchors.left: playButton.right
                        anchors.leftMargin: page.width/4 - playButton.width/2
                        onClicked: videoPlayer.nextVideo()
                    }

                    IconButton {
                        id: prevButton
                        visible: opacity != 0 && playlistMode
                        icon.source: "image://theme/icon-m-previous"
                        anchors.top: playButton.top
                        anchors.right: playButton.left
                        anchors.rightMargin: page.width/4 - playButton.width/2
                        onClicked: videoPlayer.prevVideo()
                    }

                    IconButton {
                        id: castButton
                        visible: opacity != 0 && landscape && jupii.connected
                        icon.source: "qrc:///images/icon-m-cast.svg"
                        width: Theme.itemSizeExtraSmall
                        height: width
                        anchors.right: subsButton.left
                        anchors.top: parent.top
                        anchors.margins: Theme.paddingMedium
                        icon.width: width
                        icon.height: width
                        onClicked: videoHelperYupii.loadVideoUrl(videoHelper.currentVideo.videoId, "720", true)
                    }

                    IconButton {
                        id: subsButton
                        visible: opacity != 0 && landscape
                        icon.source: "qrc:///images/icon-m-closed-captions-text.svg"
                        width: Theme.itemSizeExtraSmall
                        height: width
                        anchors.right: playbackSpeedButton.left
                        anchors.top: parent.top
                        anchors.margins: Theme.paddingMedium
                        icon.width: width
                        icon.height: width
                        onClicked: pageStack.push(Qt.resolvedUrl("components/SubtitlesDialog.qml"), {videoHelper: videoHelper})
                    }

                    IconButton {
                        id: playbackSpeedButton
                        visible: opacity != 0 && landscape && !playbackSpeedSlider.visible
                        icon.source: "image://theme/icon-m-timer"
                        width: Theme.itemSizeExtraSmall
                        height: width
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: Theme.paddingMedium
                        icon.width: width
                        icon.height: width
                        onClicked: {
                            playbackSpeedSlider.opacity = 1.0
                            playbackSpeedSliderTimer.start()
                        }

                        Timer {
                            id: playbackSpeedSliderTimer
                            interval: 3000
                            repeat: false
                            onTriggered: playbackSpeedSlider.opacity = 0
                        }
                    }

                    Slider {
                        id: playbackSpeedSlider
                        visible: opacity !== 0
                        opacity: 0
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.topMargin: width - height + Theme.paddingMedium*2
                        anchors.rightMargin: -(width) + height + Theme.paddingMedium
                        width: Theme.itemSizeHuge*2
                        value: 1.0
                        minimumValue: 0.25
                        maximumValue: 2.0
                        stepSize: 0.25
                        transform: Rotation { angle: -90 }
                        enabled: visible
                        onDownChanged: if (down) {
                                           playbackSpeedSliderTimer.stop()
                                       } else {
                                           playbackSpeedSliderTimer.start()
                                           videoPlayer.setPlaybackSpeed(value)
                                       }

                        Behavior on opacity {
                            PropertyAction {}
                        }
                    }

                    Label {
                        anchors.top: parent.top
                        anchors.topMargin: playbackSpeedSlider.width - playbackSpeedSlider.height - Theme.paddingMedium*2
                        anchors.left: playbackSpeedSlider.left
                        anchors.leftMargin: playbackSpeedSlider.height/2 - width/2
                        text: playbackSpeedSlider.value
                        opacity: playbackSpeedSlider.opacity
                        visible: opacity !== 0
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
                        text: Format.formatDuration(Math.round(videoPlayer.position/1000), ((videoPlayer.duration/1000) > 3600 ? Formatter.DurationLong : Formatter.DurationShort))
                    }

                    Label {
                        id: duration
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: Theme.paddingLarge
                        text: Format.formatDuration(Math.round(videoPlayer.duration/1000), ((videoPlayer.duration/1000) > 3600 ? Formatter.DurationLong : Formatter.DurationShort))
                    }

                    Slider {
                        id: progressSlider
                        value: videoPlayer.position
                        valueText: down ? Format.formatDuration(Math.round(value/1000), ((value/1000) > 3600 ? Formatter.DurationLong : Formatter.DurationShort)) : ""
                        minimumValue: 0
                        maximumValue: videoPlayer.duration
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

                        onReleased: videoPlayer.seek(progressSlider.value)
                    }
                }
            }

            TextArea {
                id: videoCoverTitle
                text: videoHelper.currentVideo.title
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
                height: parent.height - videoPlayerRow.height
                NumberAnimation { id: anim; target: listView; property: "contentY"; duration: 500 }
                SilicaFlickable {
                    id: playlistFlickable
                    width: parent.width
                    height: playlist.height
                    contentHeight: (page.height - videoPlayerRow.height) + videoTitle.height + authorViews.height + videoDescription.height + comments.height + progress.height/2 + interactionRow.height + Theme.paddingLarge*2
                    clip: true
                    property int oldContentHeight: 0
                    onContentHeightChanged: {
                        if (oldContentHeight !== 0 && contentY >= parseInt(oldContentHeight - (page.height - videoPlayerRow.height))) {
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
                            text: videoHelper.currentVideo.title
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
                                width: parent.width
                                spacing: Theme.paddingSmall
                                anchors.verticalCenter: parent.verticalCenter
                                Label {
                                    text: videoHelper.currentVideo.author.name
                                    width: parent.width
                                    font.pixelSize: Theme.fontSizeMedium
                                    truncationMode: TruncationMode.Fade
                                    color: Theme.primaryColor

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            pageStack.navigateBack(PageStackAction.Immediate)
                                            pageStack.push(Qt.resolvedUrl("Channel.qml"), {channelId: videoHelper.currentVideo.author.id })
                                        }
                                    }
                                }

                                Label {
                                    text: qsTr("%1 views").arg(Helpers.parseViews(videoHelper.currentVideo.viewCount))
                                    font.pixelSize: Theme.fontSizeMedium
                                    truncationMode: TruncationMode.Fade
                                    color: Theme.secondaryColor
                                    bottomPadding: Theme.paddingLarge
                                }
                            }
                        }

                        Row {
                            id: interactionRow
                            height: Theme.itemSizeLarge
                            width: parent.width

                            Column {
                                id: likeColumn
                                width: Math.max(likeLabel.width, likeIcon.width)
                                height: parent.height
                                anchors.verticalCenter: parent.verticalCenter

                                IconButton {
                                    id: likeIcon
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    icon.source: googleOAuthHelper.rating == "like" ? "image://theme/icon-m-like" : "image://theme/icon-m-outline-like"
                                    enabled: googleOAuthHelper.linked
                                    onClicked: {
                                        if (googleOAuthHelper.rating == "like") {
                                            googleOAuthHelper.rate(videoHelper.currentVideo.videoId, "none")
                                        } else {
                                            googleOAuthHelper.rate(videoHelper.currentVideo.videoId, "like")
                                        }
                                    }
                                }

                                Label {
                                    id: likeLabel
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: Helpers.parseViews(videoHelper.currentVideo.likes)
                                    font.pixelSize: Theme.fontSizeMedium
                                    truncationMode: TruncationMode.Fade
                                    color: Theme.secondaryColor
                                }
                            }

                            Item {
                                height: 1
                                width: Theme.paddingMedium
                            }


                            Column {
                                id: dislikeColumn
                                width: Math.max(dislikeLabel.width, dislikeIcon.width)
                                height: parent.height
                                anchors.verticalCenter: parent.verticalCenter

                                IconButton {
                                    id: dislikeIcon
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    icon.rotation: 180
                                    icon.source: googleOAuthHelper.rating == "dislike" ? "image://theme/icon-m-like" : "image://theme/icon-m-outline-like"
                                    enabled: googleOAuthHelper.linked
                                    onClicked: {
                                        if (googleOAuthHelper.rating == "dislike") {
                                            googleOAuthHelper.rate(videoHelper.currentVideo.videoId, "none")
                                        } else {
                                            googleOAuthHelper.rate(videoHelper.currentVideo.videoId, "dislike")
                                        }
                                    }
                                }

                                Label {
                                    id: dislikeLabel
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: qsTr("Dislike");
                                    font.pixelSize: Theme.fontSizeMedium
                                    truncationMode: TruncationMode.Fade
                                    color: Theme.secondaryColor
                                }
                            }

                            Item {
                                height: 1
                                width: parent.width - likeColumn.width - dislikeColumn.width - shareButton.width - subscribeButton.width - Theme.paddingLarge*2
                            }

                            IconButton {
                                id: shareButton
                                anchors.verticalCenter: parent.verticalCenter
                                icon.source: "image://theme/icon-m-share"
                                onClicked: shareAction.trigger()
                            }

                            Button {
                                id: subscribeButton
                                anchors.verticalCenter: parent.verticalCenter
                                preferredWidth: Theme.itemSizeHuge
                                property bool subscribed: channelHelper.isSubscribed(videoHelper.currentVideo.author.id)
                                text: subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                                onClicked: {
                                    var authorId = videoHelper.currentVideo.author.id
                                    channelHelper.isSubscribed(authorId) ? channelHelper.unsubscribe(authorId) : channelHelper.subscribe(authorId)
                                    subscribed = channelHelper.isSubscribed(authorId)
                                }
                            }
                        }

                        Item {
                            height: Theme.paddingLarge
                            width: 1
                        }

                        LinkedLabel {
                            id: videoDescription
                            plainText: videoHelper.currentVideo.description
                            width: page.width - Theme.paddingLarge*2
                            color: palette.secondaryColor
                            wrapMode: TextEdit.WordWrap
                        }

                        CommentsButton {
                            id: comments
                            text: qsTr("Comments")
                            width: parent.width - Theme.paddingLarge

                            onClicked: pageStack.push(Qt.resolvedUrl("Comments.qml"), {videoId: videoHelper.currentVideo.videoId})
                        }

                        YtPlaylist {
                            id: currentPlaylist
                        }

                        SilicaFastListView {
                            id: listView
                            width: parent.width - Theme.paddingLarge
                            height: page.height - videoPlayerRow.height
                            maximumFlickVelocity: 9999
                            spacing: Theme.paddingMedium
                            model: currentPlaylist
                            clip: true
                            interactive: playlistFlickable.contentY >= parseInt(playlistFlickable.contentHeight - (page.height - videoPlayerRow.height))
                            delegate: VideoElement {
                                id: delegate

                                onClicked: {
                                    videoPlayer.stop()
                                    videoHelper.loadVideoUrl(id, settings.maxDefinition)
                                }
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
        enabled: videoPlayer.playbackState !== videoPlayer.StoppedState

        CoverAction {
            iconSource: videoPlayer.playbackState == videoPlayer.PlayingState ? "image://theme/icon-cover-pause" : "image://theme/icon-cover-play"
            onTriggered: {
                videoPlayer.playbackState == videoPlayer.PlayingState ? videoPlayer.videoPause() : videoPlayer.videoPlay()
            }
        }

        CoverAction {
            iconSource: "image://theme/icon-cover-next-song"
            onTriggered: {
                videoPlayer.nextVideo()
            }
        }
    }

    MprisPlayer {
        id: mprisPlayer

        serviceName: "microtube"
        property string artist: videoHelper.currentVideo.author.name
        property string song: videoHelper.currentVideo.title
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
            videoPlayer.videoPause()
        }

        onPlayRequested: {
            videoPlayer.videoPlay()
        }

        onPlayPauseRequested: {
            videoPlayer.playbackState == videoPlayer.PlayingState ? videoPlayer.videoPause() : videoPlayer.videoPlay()
        }

        onStopRequested: {
            videoPlayer.stop()
        }

        onNextRequested: {
            videoPlayer.nextVideo()
        }

        onPreviousRequested: {
            videoPlayer.prevVideo()
        }

        onSeekRequested: {
            videoPlayer.seek(offset)
        }
    }
}
