QT += gui
QT += core
QT += network
QT += widgets

INCLUDEPATH += $$PWD/include

SOURCES += \
    $$PWD/src/Updater.cpp \
    $$PWD/src/Downloader.cpp \
    $$PWD/src/QSimpleUpdater.cpp

HEADERS += \
    $$PWD/include/QSimpleUpdater.h \
    $$PWD/src/Updater.h \
    $$PWD/src/Downloader.h

FORMS += $$PWD/src/Downloader.ui
RESOURCES +=
