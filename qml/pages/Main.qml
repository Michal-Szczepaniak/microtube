import QtQuick 2.0
import Sailfish.Silica 1.0
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PullDownMenu {
            MenuItem {
                text: qsTr("Subscriptions")
                onClicked: pageStack.push(Qt.resolvedUrl("Subscriptions.qml"))
            }
        }

        PageHeader {
            id: header
            title: qsTr("Search")
        }

        SilicaListView {
            id: listView
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            header: SearchField {
                id: searchField
                width: parent.width
                placeholderText: "Search"
                onTextChanged: YT.search(searchField.text)
            }
            model: YTPlaylist
            delegate: VideoElement {}
        }
    }
}
