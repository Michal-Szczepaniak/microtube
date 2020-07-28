---
id: AddingQEasyDownloader
title: Add QEasyDownloader to Your Project
sidebar_label: Adding QEasyDownloader to Your Project
---

## Using QEasyDownloader as library

Using **QEasyDownloader** as a library is always the best choice , Why ? Because if you add the    
source directly to your project , Everytime you use **QEasyDownloader** , The compiler compiles   
the whole library over and over. To avoid this , We compile **QEasyDownloader** once as a static
and a shared library and then include it in our Qt App.

To use **QEasyDownloader** as a library , You must use this directory structure.   
Other directory structures may work but this is the efficient one.

```
 -MyCoolApplication
  --libs
     ---QEasyDownloader
     ---libs.pro
  --src
     ---main.cpp
     ---mainwindow.cpp
     ---src.pro
  --MyCoolApplication.pro
```

### The Library Subdir Project file (libs.pro)

This is where you keep all third party libraries including **QEasyDownloader**.
Just add a **git submodule** or execute the steps mentioned in the **Installation**   
in the **libs** directory of your project folder.



```
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = QEasyDownloader
INCLUDEPATH += QEasyDownloader \
	       QEasyDownloader/include
```


### Your Source's Project file (src.pro)

```
TEMPLATE = app
TARGET = ../MyCoolApplication
QT += core concurrent gui # Everything else you need.
LIBS += -l../libs/QEasyDownloader/libQEasyDownloader.a # you can also use .so , its auto-generated.
INCLUDEPATH += . .. ../libs/QEasyDownloader \
               ../libs/QEasyDownloader/include
SOURCES += main.cpp mainwindow.cpp # All your source files.
# Add your header files too..
```

### Your Main Project file ( MyCoolApplication.pro )

```
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libs \ # Always use this order
	  src
INCLUDEPATH += . libs/QEasyDownloader \
               libs/QEasyDownloader/include
```

### Including QEasyDownloader in your Source

Whenever you want to use **QEasyDownloader** , you just need to include it!

```
#include <QEasyDownloader>
```


Thats it , All you have to do is to build your project with **qmake**.   
like this in your **Project Folder**.   

``` $ mkdir build; cd build ; qmake .. ; make -j$(nproc) ```


## Building QEasyDownloader with CMake

If you whish to build **QEasyDownloader** with CMake instead of qmake then you can do   
so by just adding the QEasyDownloader **directory** as a **sub-directory** in CMakeLists.txt.

refer the **CMake docs** for more information on building a Qt Project. For reference   
you can take a look at QEasyDownloader's **CMakeLists.txt**.


## Directly Adding QEasyDownloader to your Project

Since **QEasyDownloader** is a simple library you can also directly use it.

|	        |            				                             |		
|-----------|--------------------------------------------------------|
|   Header: | #include < QEasyDownloader >                             |
|   qmake:  | QT += core concurrent	       	                         |
|	        | HEADERS += QEasyDownloader/include/QEasyDownloader.hpp |
|           | SOURCES += QEasyDownloader/src/QEasyDownloader.cc      |
| Inherits: | QObject			                                     |

**QEasyDownloader** is just a header and a source and all you have to do after installation is to add   
it in your **.pro** file ( **Qt Project file** ).

### Qt Project file (**.pro**)

Append these **lines** to your **Qt Project file**.

```
QT += core concurrent
INCLUDEPATH += QEasyDownloader QEasyDownloader/include
HEADERS += QEasyDownloader/include/QEasyDownloader.hpp
SOURCES += QEasyDownloader/src/QEasyDownloader.cc
```

### Including QEasyDownloader in your Source

Whenever you want to use **QEasyDownloader** , you just need to include it!

```
#include <QEasyDownloader>
```

