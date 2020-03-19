TARGET = simple_download
TEMPLATE = app
INCLUDEPATH += ../../ ../../include

QT += core network

SOURCES += main.cpp ../../src/QEasyDownloader.cc
HEADERS += ../../include/QEasyDownloader.hpp
