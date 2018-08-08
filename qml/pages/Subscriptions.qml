import QtQuick 2.0
import Sailfish.Silica 1.0
import "components"

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        PageHeader {
            id: header
            title: qsTr("Subscriptions")
        }

        SilicaListView {
            id: listView
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            model: ListModel {
                ListElement {
                    name: "Apple"
                }
                ListElement {
                    name: "Orange"
                }
                ListElement {
                    name: "Banana"
                }
            }
            delegate: VideoElement {}
        }
    }
}
