TARGET = microtube

CONFIG += link_pkgconfig nemonotifications-qt5 sailfishapp c++17

include(vendor/vendor.pri)

CONFIG -= debug_and_release
CONFIG(debug, debug|release): {
    message(Building for debug)
}
CONFIG(release, debug|release): {
    message(Building for release)
    DEFINES *= QT_NO_DEBUG_OUTPUT
}

INCLUDEPATH += \
    $$PWD/src/QEasyDownloader/include \
    $$PWD/src

PKGCONFIG += \
    dbus-1 \
    dbus-glib-1 \
    nemonotifications-qt5

QT += sql dbus network widgets

# microtube headers
HEADERS += \
    src/mock/qrandomgenerator.h \
    src/categoriesmodel.h \
    src/comment.h \
    src/commentsmodel.h \
    src/sponsorblock.h \
    src/threadmodel.h \
    src/userfileshelper.h \
    src/yt.h \
    src/playlistmodel.h \
    src/channelmodel.h \
    src/video.h \
    src/volume/pulseaudiocontrol.h \
    src/mediaview.h \
    src/channelaggregator.h \
    src/QEasyDownloader/include/QEasyDownloader.hpp

# minitube
HEADERS += \
    src/jsfunctions.h \
    src/temporary.h \
    src/datautils.h \
    src/iconutils.h \
    src/httputils.h \
    src/constants.h \
    src/ytvideo.h \    
    src/videosource.h \
    src/mainwindow.h \
    src/ytsearch.h \
    src/ytstandardfeed.h \
    src/ytregions.h \
    src/ytcategories.h \
    src/ytsuggester.h \
    src/ytsinglevideosource.h \
    src/yt3listparser.h \
    src/videoapi.h \
    src/ytchannel.h \
    src/searchparams.h \
    src/database.h \
    src/videodefinition.h \
    src/videomimedata.h \
    src/yt3.h \
    src/suggester.h \
    src/paginatedvideosource.h \
    src/aggregatevideosource.h

# microtube sources
SOURCES += \
    src/microtube.cpp \
    src/mock/qrandomgenerator.cpp \
    src/categoriesmodel.cpp \
    src/comment.cpp \
    src/commentsmodel.cpp \
    src/sponsorblock.cpp \
    src/threadmodel.cpp \
    src/userfileshelper.cpp \
    src/yt.cpp \
    src/playlistmodel.cpp \
    src/channelmodel.cpp \
    src/video.cpp \
    src/volume/pulseaudiocontrol.cpp \
    src/channelaggregator.cpp \
    src/QEasyDownloader/src/QEasyDownloader.cc

# minitube
SOURCES += \
    src/jsfunctions.cpp \
    src/temporary.cpp \
    src/datautils.cpp \
    src/iconutils.cpp \
    src/httputils.cpp \
    src/constants.cpp \
    src/ytvideo.cpp \
    src/videosource.cpp \
    src/mainwindow.cpp \
    src/ytsearch.cpp \
    src/ytstandardfeed.cpp \
    src/ytregions.cpp \
    src/ytcategories.cpp \
    src/ytsuggester.cpp \
    src/ytsinglevideosource.cpp \
    src/yt3listparser.cpp \
    src/ytchannel.cpp \
    src/searchparams.cpp \
    src/database.cpp \
    src/videodefinition.cpp \
    src/videomimedata.cpp \
    src/yt3.cpp \
    src/paginatedvideosource.cpp \
    src/aggregatevideosource.cpp

DISTFILES += qml/microtube.qml \
    js/channelInfo.js \
    js/channelVideos.js \
    js/search.js \
    js/videoInfo.js \
    microtube-url.desktop \
    qml/cover/CoverPage.qml \
    qml/pages/Channel.qml \
    qml/pages/Comments.qml \
    qml/pages/Filters.qml \
    qml/pages/InstallDialog.qml \
    qml/pages/SponsorBlockSettings.qml \
    qml/pages/SubscriptionsImport.qml \
    qml/pages/UpdateDialog.qml \
    qml/pages/components/APIKeyDialog.qml \
    qml/pages/components/CenteredLabel.qml \
    qml/pages/components/Comment.qml \
    qml/pages/components/CommentsButton.qml \
    qml/pages/components/Jupii.qml \
    qml/pages/components/VideoElement_copy.qml \
    rpm/microtube.spec \
    translations/*.ts \
    microtube.desktop \
    qml/pages/About.qml \
    qml/pages/Main.qml \
    qml/pages/Subscriptions.qml \
    qml/pages/SubscriptionsImport.qml \
    qml/pages/Settings.qml \
    qml/pages/VideoPlayer.qml \
    qml/pages/components/VideoElement.qml \
    qml/pages/components/CategoryElement.qml \
    qml/pages/components/SilicaFastListView.qml \
    qml/pages/components/SharePage.qml \
    qml/pages/components/private/BoundsBehavior.qml \
    qml/pages/components/private/FastScrollAnimation.js \
    qml/pages/components/private/FastScrollAnimation.qml \
    qml/pages/components/private/QuickScroll.qml \
    qml/pages/components/private/QuickScrollArea.qml \
    qml/pages/components/private/QuickScrollButton.qml \
    qml/pages/components/private/HighlightImage.qml

RESOURCES += \
    qml/resources/resources.qrc

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

urlfile.files = microtube-url.desktop
urlfile.path = /usr/share/applications

INSTALLS += urlfile

jsfiles.files = \
    js/channelInfo.js \
    js/channelVideos.js \
    js/search.js \
    js/videoInfo.js \
    js/package.json
jsfiles.path = /usr/share/microtube/js/

INSTALLS += jsfiles

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/microtube-de.ts \
    translations/microtube-es.ts \
    translations/microtube-cs.ts \
    translations/microtube-hu.ts \
    translations/microtube-sv.ts \
    translations/microtube-zh_CN.ts \
    translations/microtube-pl.ts
