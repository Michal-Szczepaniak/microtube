#include "yt.h"
#include "ytsearch.h"
#include "ytsinglevideosource.h"
#include <QDebug>

YT::YT(QObject *parent) : QObject(parent)
{
    playlistModel = new PlaylistModel();
}

void YT::gotStreamUrl(const QUrl &streamUrl) {
    qDebug()<<streamUrl;
    this->setStreamUrl(streamUrl);
    emit streamUrlChanged(streamUrl);
}

void YT::registerObjectsInQml(QQmlContext* context) {
    context->setContextProperty("YT",this);
    context->setContextProperty("YTPlaylist",this->playlistModel);
}

void YT::search(QString query) {
    QString q = query.simplified();

    // check for empty query
    if (q.isEmpty()) {
//        queryEdit->toWidget()->setFocus(Qt::OtherFocusReason);
        return;
    }

    SearchParams *searchParams = new SearchParams();
    searchParams->setKeywords(q);

    // go!
    this->watch(searchParams);
//    emit search(searchParams);
}

void YT::watch(SearchParams *searchParams) {
    if (!searchParams->keywords().isEmpty()) {
        if (searchParams->keywords().startsWith("http://") ||
            searchParams->keywords().startsWith("https://")) {
            QString videoId = YTSearch::videoIdFromUrl(searchParams->keywords());
            if (!videoId.isEmpty()) {
                YTSingleVideoSource *singleVideoSource = new YTSingleVideoSource(this);
                singleVideoSource->setVideoId(videoId);
                setVideoSource(singleVideoSource);
//                QTime tstamp = YTSearch::videoTimestampFromUrl(searchParams->keywords());
//                pauseTime = QTime(0, 0).msecsTo(tstamp);
                return;
            }
        }
    }
    YTSearch *ytSearch = new YTSearch(searchParams);
    ytSearch->setAsyncDetails(true);
    connect(ytSearch, SIGNAL(gotDetails()), playlistModel, SLOT(emitDataChanged()));
    setVideoSource(ytSearch);
}

void YT::setVideoSource(VideoSource *videoSource, bool addToHistory, bool back) {
    Q_UNUSED(back);
    stopped = false;
//    errorTimer->stop();

    // qDebug() << "Adding VideoSource" << videoSource->getName() << videoSource;

    if (addToHistory) {
        int currentIndex = getHistoryIndex();
        if (currentIndex >= 0 && currentIndex < history.size() - 1) {
            while (history.size() > currentIndex + 1) {
                VideoSource *vs = history.takeLast();
                if (!vs->parent()) {
                    qDebug() << "Deleting VideoSource" << vs->getName() << vs;
                    delete vs;
                }
            }
        }
        history.append(videoSource);
    }

#ifdef APP_EXTRA
    if (history.size() > 1)
        Extra::slideTransition(playlistView->viewport(), playlistView->viewport(), back);
#endif

    playlistModel->setVideoSource(videoSource);

//    QSettings settings;
//    if (settings.value("manualplay", false).toBool()) {
//        videoAreaWidget->showPickMessage();
//    }

//    sidebar->showPlaylist();
//    sidebar->getRefineSearchWidget()->setSearchParams(getSearchParams());
//    sidebar->hideSuggestions();
//    sidebar->getHeader()->updateInfo();

//    SearchParams *searchParams = getSearchParams();
//    bool isChannel = searchParams && !searchParams->channelId().isEmpty();
//    playlistView->setClickableAuthors(!isChannel);
}

void YT::searchAgain() {
    VideoSource *currentVideoSource = playlistModel->getVideoSource();
    setVideoSource(currentVideoSource, false);
}

SearchParams *YT::getSearchParams() {
    VideoSource *videoSource = playlistModel->getVideoSource();
    if (videoSource && videoSource->metaObject()->className() == QLatin1String("YTSearch")) {
        YTSearch *search = qobject_cast<YTSearch *>(videoSource);
        return search->getSearchParams();
    }
    return 0;
}

const QString &YT::getCurrentVideoId() {
    return currentVideoId;
}

int YT::getHistoryIndex() {
    return history.lastIndexOf(playlistModel->getVideoSource());
}
