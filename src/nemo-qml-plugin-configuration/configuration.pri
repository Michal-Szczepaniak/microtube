QT *= core qml

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

HEADERS += \
    $$PWD/src/configurationgroup.h \
    $$PWD/src/configurationvalue.h

SOURCES += \
    $$PWD/src/configurationgroup.cpp \
    $$PWD/src/configurationvalue.cpp

CONFIG += link_pkgconfig
PKGCONFIG += mlite5