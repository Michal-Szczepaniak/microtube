import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    id: listItem
    onClicked: pageStack.push(Qt.resolvedUrl("../VideoPlayer.qml"))
    Label {
        text: name

    }
}
