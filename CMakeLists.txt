project(microtube CXX)
cmake_minimum_required(VERSION 3.5)

find_package(Qt5 COMPONENTS Core Network Qml Gui Quick DBus OpenGL OpenGLExtensions REQUIRED)
find_package(DBus1 REQUIRED)
find_package(PkgConfig)

pkg_search_module(SAILFISH sailfishapp REQUIRED)
pkg_search_module(GLIB glib-2.0 REQUIRED)
pkg_search_module(DBUSGLIB dbus-glib-1 REQUIRED)
pkg_search_module(AUDIORESOURCE audioresource-qt REQUIRED)
pkg_search_module(GSTREAMER gstreamer-1.0 REQUIRED)
pkg_search_module(GSTREAMER_PLAYER gstreamer-player-1.0 REQUIRED)
pkg_search_module(GSTREAMER_VIDEO gstreamer-video-1.0 REQUIRED)
pkg_search_module(EGL egl REQUIRED)
pkg_search_module(OPENGL_EXTENSIONS Qt5OpenGLExtensions REQUIRED)
pkg_search_module(NEMO_GSTREAMER_INTERFACES nemo-gstreamer-interfaces-1.0 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_CXX_STANDARD 20)

file( GLOB_RECURSE SOURCES src/*.cpp )
file( GLOB_RECURSE HEADERS src/*.h )
add_executable(microtube
    ${SOURCES}
    ${HEADERS}
    qml/resources/resources.qrc
    )
target_compile_definitions(microtube PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

target_include_directories(microtube PRIVATE
    $<BUILD_INTERFACE:
    ${GLIB_INCLUDE_DIRS}
    ${DBUSGLIB_INCLUDE_DIRS}
    ${SAILFISH_INCLUDE_DIRS}
    ${AUDIORESOURCE_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${GSTREAMER_PLAYER_INCLUDE_DIRS}
    ${GSTREAMER_VIDEO_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
>)
target_link_libraries(microtube
    Qt5::Quick
    Qt5::DBus
    Qt5::OpenGL
    Qt5::OpenGLExtensions
    ${DBus1_LDFLAGS}
    ${GLIB_LDFLAGS}
    ${DBUSGLIB_LDFLAGS}
    ${SAILFISH_LDFLAGS}
    ${AUDIORESOURCE_LDFLAGS}
    ${GSTREAMER_LDFLAGS}
    ${GSTREAMER_PLAYER_LDFLAGS}
    ${GSTREAMER_VIDEO_LDFLAGS}
    ${EGL_LDFLAGS}
    ${OPENGL_EXTENSIONS_STATIC_LDFLAGS}
    ${NEMO_GSTREAMER_INTERFACES_LDFLAGS}
)
target_compile_options(microtube PUBLIC
    ${EGL_CFLAGS_OTHER}
)

install(TARGETS microtube
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml
    DESTINATION share/microtube
)
install(DIRECTORY js
    DESTINATION share/microtube
)
install(DIRECTORY translations
    DESTINATION share/microtube
    FILES_MATCHING PATTERN "*.qm"
)
install(FILES microtube.desktop
    DESTINATION share/applications
)
install(FILES icons/86x86/microtube.png
    DESTINATION share/icons/hicolor/86x86/apps
)
install(FILES icons/108x108/microtube.png
    DESTINATION share/icons/hicolor/108x108/apps
)
install(FILES icons/128x128/microtube.png
    DESTINATION share/icons/hicolor/128x128/apps
)
install(FILES icons/172x172/microtube.png
    DESTINATION share/icons/hicolor/172x172/apps
)

FILE(GLOB TsFiles "translations/*.ts")
add_custom_target(distfiles
    SOURCES
        microtube.desktop
        qml/microtube.qml
        js/channelInfo.js
        js/channelVideos.js
        js/search.js
        js/trending.js
        js/videoInfo.js
        microtube-url.desktop
        qml/cover/CoverPage.qml
        qml/pages/Channel.qml
        qml/pages/Comments.qml
        qml/pages/Filters.qml
        qml/pages/InstallDialog.qml
        qml/pages/SponsorBlockSettings.qml
        qml/pages/SubscriptionsImport.qml
        qml/pages/UpdateDialog.qml
        qml/pages/components/APIKeyDialog.qml
        qml/pages/components/CenteredLabel.qml
        qml/pages/components/Comment.qml
        qml/pages/components/CommentsButton.qml
        qml/pages/components/Jupii.qml
        qml/pages/components/VideoElement_copy.qml
        qml/pages/About.qml
        qml/pages/Main.qml
        qml/pages/Subscriptions.qml
        qml/pages/SubscriptionsImport.qml
        qml/pages/Settings.qml
        qml/pages/VideoPlayer.qml
        qml/pages/components/VideoElement.qml
        qml/pages/components/CategoryElement.qml
        qml/pages/components/SilicaFastListView.qml
        qml/pages/components/helpers.js
        qml/pages/components/private/BoundsBehavior.qml
        qml/pages/components/private/FastScrollAnimation.js
        qml/pages/components/private/FastScrollAnimation.qml
        qml/pages/components/private/QuickScroll.qml
        qml/pages/components/private/QuickScrollArea.qml
        qml/pages/components/private/QuickScrollButton.qml
        qml/pages/components/private/HighlightImage.qml
        rpm/microtube.spec
        ${TsFiles})

file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
${CMAKE_BINARY_DIR}/microtube:bin
")

