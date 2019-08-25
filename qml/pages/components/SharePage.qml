import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0

SharePage {
    id: page

    property string videoUrl
    property string videoTitle

    header: qsTr("Share")

    content: {
        "type": "text/x-url",
        "status": page.videoUrl,
        "linkTitle": page.videoTitle,
    }

    mimeType: "text/x-url"
}
