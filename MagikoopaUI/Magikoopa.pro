#-------------------------------------------------
#
# Project created by QtCreator 2017-10-06T19:56:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Magikoopa
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    Filesystem/externalfile.cpp \
    Filesystem/externalfilesystem.cpp \
    Filesystem/filebase.cpp \
    Filesystem/memoryfile.cpp \
    Filesystem/plainarchive.cpp \
    Filesystem/sarcfilesystem.cpp \
    Filesystem/yaz0file.cpp \
    exheader.cpp \
    patchmaker.cpp \
    patchcompiler.cpp

HEADERS += \
        mainwindow.h \
    Filesystem/compressedfile.h \
    Filesystem/externalfile.h \
    Filesystem/externalfilesystem.h \
    Filesystem/filebase.h \
    Filesystem/filecontainer.h \
    Filesystem/filesystem.h \
    Filesystem/filesystembase.h \
    Filesystem/memoryfile.h \
    Filesystem/plainarchive.h \
    Filesystem/sarcfilesystem.h \
    Filesystem/version.h \
    Filesystem/yaz0file.h \
    exheader.h \
    patchmaker.h \
    patchcompiler.h

FORMS += \
        mainwindow.ui
