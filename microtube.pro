# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = microtube

include(src/http/http.pri)
include(src/nemo-qml-plugin-configuration/configuration.pri)

CONFIG += sailfishapp

QT += sql

HEADERS += \
    src/ytsearch.h \
    src/ytstandardfeed.h \
    src/ytregions.h \
    src/ytcategories.h \
    src/ytsuggester.h \
    src/ytsinglevideosource.h \
    src/yt3listparser.h \
    src/ytchannel.h \
    src/yt3.h \
    src/ytvideo.h \
    src/video.h \
    src/videosource.h \
    src/paginatedvideosource.h \
    src/constants.h \
    src/httputils.h \
    src/datautils.h \
    src/iconutils.h \
    src/searchparams.h \
    src/suggester.h \
    src/database.h \
    src/jsfunctions.h \
    src/temporary.h \
    src/videodefinition.h \
    src/yt.h \
    src/searchmodel.h \
    src/playlistmodel.h \
    src/videomimedata.h

SOURCES += src/microtube.cpp \
    src/ytsearch.cpp \
    src/ytstandardfeed.cpp \
    src/ytregions.cpp \
    src/ytcategories.cpp \
    src/ytsuggester.cpp \
    src/ytsinglevideosource.cpp \
    src/yt3listparser.cpp \
    src/ytchannel.cpp \
    src/yt3.cpp \
    src/ytvideo.cpp \
    src/video.cpp \
    src/videosource.cpp \
    src/paginatedvideosource.cpp \
    src/constants.cpp \
    src/httputils.cpp \
    src/datautils.cpp \
    src/iconutils.cpp \
    src/searchparams.cpp \
    src/database.cpp \
    src/jsfunctions.cpp \
    src/temporary.cpp \
    src/videodefinition.cpp \
    src/yt.cpp \
    src/searchmodel.cpp  \
    src/playlistmodel.cpp \
    src/videomimedata.cpp

DISTFILES += qml/microtube.qml \
    qml/cover/CoverPage.qml \
    rpm/microtube.spec \
    translations/*.ts \
    microtube.desktop \
    qml/pages/Main.qml \
    qml/pages/Subscriptions.qml \
    qml/pages/VideoPlayer.qml \
    qml/pages/components/VideoElement.qml

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
# TRANSLATIONS += translations/microtube-de.ts
