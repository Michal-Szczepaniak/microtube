project(microtube CXX)
cmake_minimum_required(VERSION 3.5)

find_package(Qt5 COMPONENTS Core Network Qml Gui Quick DBus Sql OpenGL OpenGLExtensions SystemInfo REQUIRED)
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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(o2_SOURCE vendor/o2/src)

add_subdirectory(vendor/SortFilterProxyModel)

set( o2_SOURCES
    ${o2_SOURCE}/o2.cpp
    ${o2_SOURCE}/o2pollserver.cpp
    ${o2_SOURCE}/o2reply.cpp
    ${o2_SOURCE}/o2replyserver.cpp
    ${o2_SOURCE}/o2requestor.cpp
    ${o2_SOURCE}/o2simplecrypt.cpp
    ${o2_SOURCE}/o0jsonresponse.cpp
    ${o2_SOURCE}/o0settingsstore.cpp
    ${o2_SOURCE}/o0baseauth.cpp
    ${o2_SOURCE}/o2gft.cpp
    ${o2_SOURCE}/o2google.cpp
    ${o2_SOURCE}/o2googledevice.cpp
)

set( o2_HEADERS
    ${o2_SOURCE}/o2.h
    ${o2_SOURCE}/o2pollserver.h
    ${o2_SOURCE}/o2reply.h
    ${o2_SOURCE}/o2replyserver.h
    ${o2_SOURCE}/o2requestor.h
    ${o2_SOURCE}/o0abstractstore.h
    ${o2_SOURCE}/o0baseauth.h
    ${o2_SOURCE}/o0export.h
    ${o2_SOURCE}/o0globals.h
    ${o2_SOURCE}/o0jsonresponse.h
    ${o2_SOURCE}/o0requestparameter.h
    ${o2_SOURCE}/o0settingsstore.h
    ${o2_SOURCE}/o0simplecrypt.h
    ${o2_SOURCE}/o2gft.h
    ${o2_SOURCE}/o2google.h
    ${o2_SOURCE}/o2googledevice.h
)

file( GLOB_RECURSE SOURCES src/*.cpp )
file( GLOB_RECURSE HEADERS src/*.h )
add_executable(microtube
    ${SOURCES}
    ${HEADERS}
    $<TARGET_OBJECTS:SortFilterProxyModel>
    ${o2_SOURCES}
    ${o2_HEADERS}
    qml/resources/resources.qrc
    )
target_compile_definitions(microtube PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

target_include_directories(microtube PRIVATE
    $<BUILD_INTERFACE:
    src
    ${GLIB_INCLUDE_DIRS}
    ${DBUSGLIB_INCLUDE_DIRS}
    ${SAILFISH_INCLUDE_DIRS}
    ${AUDIORESOURCE_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${GSTREAMER_PLAYER_INCLUDE_DIRS}
    ${GSTREAMER_VIDEO_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${o2_SOURCE}
>)
target_link_libraries(microtube
    Qt5::Quick
    Qt5::DBus
    Qt5::Sql
    Qt5::OpenGL
    Qt5::OpenGLExtensions
    Qt5::SystemInfo
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
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

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
file(GLOB_RECURSE QML_FILES qml/*.qml)
add_custom_target(distfiles
    SOURCES
        microtube.desktop
        qml/microtube.qml
        js/channelInfo.js
        js/channelVideos.js
        js/search.js
        js/trending.js
        js/videoInfo.js
        js/basicVideoInfo.js
        js/comments.js
        js/commentReplies.js
        js/package.json
        microtube-url.desktop
        rpm/microtube.spec
        ${QML_FILES}
        ${TsFiles})

file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
${CMAKE_BINARY_DIR}/microtube:bin
")

