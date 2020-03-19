# BSD-3 Clause.
# Copyright (C) 2018 Antony Jr.
# -------------------------------
#  CMake Support for QEasyDownloader.
# -------------------------------

CMAKE_MINIMUM_REQUIRED( VERSION 3.2)
project(QEasyDownloader)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Find the QtCore library.
find_package(Qt5Core)
find_package(Qt5Concurrent)
find_package(Qt5Network)

# Include Directories.
include_directories(.)
include_directories(include)

add_library(QEasyDownloader src/QEasyDownloader.cc include/QEasyDownloader.hpp)
target_link_libraries(QEasyDownloader PUBLIC Qt5::Core Qt5::Concurrent Qt5::Network)
target_include_directories(QEasyDownloader PUBLIC . include)
