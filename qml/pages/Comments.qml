import QtQuick 2.0
import Sailfish.Silica 1.0
import com.verdanditeam.yt 1.0
import "components"

Page {
    id: page
    allowedOrientations: Orientation.All

    property var model: YTComments

    SilicaListView {
        id: listView
        anchors.fill: parent

        header: PageHeader {
            id: header
            title: qsTr("Comments")
        }

        model: page.model

        delegate: Comment { }
    }
}
