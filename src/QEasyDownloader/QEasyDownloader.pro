# BSD-3 Clause.
# Copyright (C) 2018 Antony Jr.
# -------------------------------
#  QMake support for QEasyDownloader.
# -------------------------------

TEMPLATE = lib
CONFIG += shared_and_static build_all release
TARGET = QEasyDownloader
INCLUDEPATH += . include
DEFINES += QT_DEPRECATED_WARNINGS

QT += core concurrent network

# Inputs 
SOURCES += src/QEasyDownloader.cc
HEADERS += include/QEasyDownloader.hpp
