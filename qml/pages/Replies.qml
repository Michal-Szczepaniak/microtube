import QtQuick 2.0
import Sailfish.Silica 1.0
import com.verdanditeam.yt 1.0
import "components"

Page {
    id: page
    allowedOrientations: Orientation.All

    property var repliesContinuation: null

    CommentsModel {
        id: commentsModel
    }

    onStatusChanged: if (status === PageStatus.Active) commentsModel.loadRepliesForComment(repliesContinuation)

    SilicaListView {
        id: listView
        anchors.fill: parent

        header: PageHeader {
            id: header
            title: qsTr("Replies")
        }

        model: commentsModel

        BusyIndicator {
            running: commentsModel.busy
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
        }

        delegate: Comment { }
    }
}
