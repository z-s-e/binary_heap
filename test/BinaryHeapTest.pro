QT       += testlib

QT       -= gui

TARGET = tst_binaryheaptest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_binaryheaptest.cpp
HEADERS += ../binary_heap.h
INCLUDEPATH += ..

DEFINES += SRCDIR=\\\"$$PWD/\\\"
