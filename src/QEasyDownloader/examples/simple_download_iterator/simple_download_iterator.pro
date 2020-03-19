TARGET = simple_download_iterator
TEMPLATE = app
INCLUDEPATH += ../../ ../../include

QT += core network

SOURCES += main.cpp ../../src/QEasyDownloader.cc
HEADERS += ../../include/QEasyDownloader.hpp
