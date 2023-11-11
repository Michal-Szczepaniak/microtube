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

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0
import com.verdanditeam.yt 1.0

Page {
    id: settingsPage

    allowedOrientations: Orientation.All

    ConfigurationGroup {
        id: settings
        path: "/apps/microtube"

        property bool autoPlay: true
        property bool audioOnlyMode: false
        property bool developerMode: false
        property double buffer: 1.0
        property string downloadLocation: "/home/nemo/Downloads/"
        property string version: ""
        property string categoryId: "0"
        property string categoryName: "Film & Animation"
        property int maxDefinition: 1080
        property int currentRegionId: 0
        property string currentRegion: ""
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: column.height + header.height + Theme.paddingLarge

        PullDownMenu {
            MenuItem {
                text: qsTr("Diagnostics")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Diagnostics.qml"))
                }
            }
        }

        PageHeader {
            id: header
            title: qsTr("Settings")
        }

        Column {
            id: column
            spacing: Theme.paddingLarge
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            SectionHeader {
                text: qsTr("Search options")
            }

            TextSwitch {
               checked: playlistModel.safeSearch
               width: parent.width
               text: qsTr("Restricted mode (safe for kids)")
               onClicked: playlistModel.safeSearch = checked
            }

            ComboBox {
                id: regions
                width: parent.width
                label: qsTr("Region")
                currentIndex: settings.currentRegionId
                menu: ContextMenu {
                    Repeater {
                        model: [qsTr("Default"),qsTr("Algeria"),qsTr("Argentina"),qsTr("Australia"),qsTr("Belgium"),qsTr("Brazil"),qsTr("Canada"),qsTr("Chile"),qsTr("Colombia"),qsTr("Czech Republic"),qsTr("Egypt"),qsTr("France"),qsTr("Germany"),qsTr("Ghana"),qsTr("Greece"),qsTr("Hong Kong"),qsTr("Hungary"),qsTr("India"),qsTr("Indonesia"),qsTr("Ireland"),qsTr("Israel"),qsTr("Italy"),qsTr("Japan"),qsTr("Jordan"),qsTr("Kenya"),qsTr("Malaysia"),qsTr("Mexico"),qsTr("Morocco"),qsTr("Netherlands"),qsTr("New Zealand"),qsTr("Nigeria"),qsTr("Peru"),qsTr("Philippines"),qsTr("Poland"),qsTr("Russia"),qsTr("Saudi Arabia"),qsTr("Singapore"),qsTr("South Africa"),qsTr("South Korea"),qsTr("Spain"),qsTr("Sweden"),qsTr("Taiwan"),qsTr("Tunisia"),qsTr("Turkey"),qsTr("Uganda"),qsTr("United Arab Emirates"),qsTr("United Kingdom"),qsTr("Yemen"),]
                        delegate: MenuItem { text: modelData }
                    }
                }
                onCurrentItemChanged: {
                    var countries = ["","DZ","AR","AU","BE","BR","CA","CL","CO","CZ","EG","FR","DE","GH","GR","HK","HU","IN","ID","IE","IL","IT","JP","JO","KE","MY","MX","MA","NL","NZ","NG","PE","PH","PL","RU","SA","SG","ZA","KR","ES","SE","TW","TN","TR","UG","AE","GB","YE"];
                    settings.currentRegion = countries[currentIndex]
                    settings.currentRegionId = currentIndex
                }
            }

            ComboBox {
                id: defaultCategory
                width: parent.width
                label: qsTr("Default category")
                value: settings.categoryName
                currentIndex: -1
                menu: ContextMenu {
                    Repeater {
                        model: [qsTr("Now"),qsTr("Music"),qsTr("Gaming"),qsTr("Movies"),qsTr("Subscriptions")]
                        delegate: MenuItem {
                            text: modelData
                        }
                    }
                }
                onCurrentItemChanged: {
                    settings.categoryId = currentIndex
                    settings.categoryName = currentItem.text
                }
            }

            SectionHeader {
                text: qsTr("Playback options")
            }

            TextSwitch {
               checked: settings.autoPlay
               width: parent.width
               text: qsTr("Automatic change to next video")
               onClicked: {
                   settings.autoPlay = checked
               }
            }

            ComboBox {
                id: maximumResolution
                width: parent.width
                label: qsTr("Maximum resolution")
                currentIndex: -1
                onCurrentIndexChanged: {
                    settings.maxDefinition = indexToResolution(currentIndex)
                    settings.audioOnlyMode = settings.maxDefinition === 0
                }
                Component.onCompleted: currentIndex = resolutionToIndex(settings.maxDefinition)

                menu: ContextMenu {
                    MenuItem { text: "2160p" }
                    MenuItem { text: "1440p" }
                    MenuItem { text: "1080p" }
                    MenuItem { text: "720p" }
                    MenuItem { text: "480p" }
                    MenuItem { text: "360p" }
                    MenuItem { text: "240p" }
                    MenuItem { text: qsTr("Audio only") }
                }

                function indexToResolution(value) {
                    switch (value) {
                    case 0:
                        return 2160;
                    case 1:
                        return 1440;
                    case 2:
                        return 1080;
                    case 3:
                        return 720;
                    case 4:
                        return 480;
                    case 5:
                        return 360;
                    case 6:
                        return 240;
                    case 7:
                        return 0;
                    default:
                        return 1080;
                    }
                }

                function resolutionToIndex(value) {
                    switch (value) {
                    case 2160:
                        return 0;
                    case 1440:
                        return 1;
                    case 1080:
                        return 2;
                    case 720:
                        return 3;
                    case 480:
                        return 4;
                    case 360:
                        return 5;
                    case 240:
                        return 6;
                    case 0:
                        return 7;
                    }
                }
            }

            SectionHeader {
                text: qsTr("Other options")
            }

            Slider {
                id: bufferSlider
                width: parent.width
                minimumValue: 0
                maximumValue: 1
                stepSize: 0.1
                value: settings.buffer
                visible: settings.developerMode

                onValueChanged: settings.buffer = value
                label: qsTr("Buffer")
            }

            Button {
                id: testNewInstall
                anchors.horizontalCenter: parent.horizontalCenter
                visible: settings.developerMode
                onClicked: settings.version = ""
                text: qsTr("Test new install")
            }

            Button {
                id: testUpdate
                anchors.horizontalCenter: parent.horizontalCenter
                visible: settings.developerMode
                onClicked: settings.version = "1"
                text: qsTr("Test update")
            }

            TextField {
                width: parent.width
                text: settings.downloadLocation
                label: qsTr("Download location")
                onTextChanged: settings.downloadLocation = text
            }

            BackgroundItem {
                width: parent.width
                onClicked: pageStack.push(Qt.resolvedUrl("SponsorBlockSettings.qml"))

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.horizontalPageMargin
                    text: qsTr("SponsorBlock Plugin Configuration")
                }
            }

            Button {
                id: login
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: googleOAuthHelper.linked ? googleOAuthHelper.unlink() : googleOAuthHelper.link()
                enabled: (clientId.text !== "" && clientSecret.text !== "") || googleOAuthHelper.linked
                text: googleOAuthHelper.linked ? qsTr("Logout from YouTube") : qsTr("Login to YouTube")
            }

            Text {
                text: qsTr("In oder to allow login you need to provide your own OAuth2 Client Id and Secret. <a href=\"https://developers.google.com/youtube/registering_an_application\">https://developers.google.com/youtube/registering_an_application</a>")
                color: "white"
                textFormat: Text.StyledText
                font.pixelSize: Theme.fontSizeSmall
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - Theme.paddingLarge*2
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                onLinkActivated: Qt.openUrlExternally(link)
                linkColor: Theme.highlightColor
            }


            TextField {
                id: clientId
                width: parent.width
                label: qsTr("OAuth2 Client ID")
                enabled: !googleOAuthHelper.linked
                onTextChanged: googleOAuthHelper.setClientId(text)
                onFocusChanged: if (!focus) googleOAuthHelper.setClientId(text)
            }

            TextField {
                id: clientSecret
                width: parent.width
                label: qsTr("OAuth2 Client Secret")
                enabled: !googleOAuthHelper.linked
                onTextChanged: googleOAuthHelper.setClientSecret(text)
                onFocusChanged: if (!focus) googleOAuthHelper.setClientSecret(text)
            }
        }
        VerticalScrollDecorator {}
    }
}
