import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    Column {
        width: parent.width

        DialogHeader { }

        TextField {
            width: parent.width
            text: YT.apiKey
            label: qsTr("Youtube API Key")
            onTextChanged: YT.apiKey = text
            labelVisible: true
            placeholderText: qsTr("API Key")
        }

        Text {
            text: qsTr("Youtube API key is required to run this application.\nTo get Youtube API key go to <a href=\"https://console.cloud.google.com\">https://console.cloud.google.com</a> and get \"YouTube Data API v3\" API key.")
            color: Theme.secondaryHighlightColor
            textFormat: Text.StyledText
            font.pixelSize: Theme.fontSizeSmall
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Theme.horizontalPageMargin*2
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            onLinkActivated: Qt.openUrlExternally(link)
            linkColor: Theme.highlightColor
            x: Theme.horizontalPageMargin
        }
    }
}
