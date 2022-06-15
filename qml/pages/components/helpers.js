function parseViews(views) {
    if (views < 1000) {
        return views;
    } else if (views < 1000*1000) {
        return qsTr("%1K").arg(Math.floor(views/100)/10);
    } else {
        return qsTr("%1M").arg(Math.floor(views/(1000*100))/10);
    }
}
