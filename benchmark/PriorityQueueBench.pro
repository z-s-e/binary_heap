QT       += testlib
QT       -= gui

TARGET = tst_priorityqueuebench
CONFIG   += console c++11
CONFIG   -= app_bundle

QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE += -O3

TEMPLATE = app

SOURCES += \
    qptrlistadaptor.cpp \
    stdptrpqadaptor.cpp \
    stdvalpqadaptor.cpp \
    libuvheapadaptor.cpp \
    myheapadaptor.cpp \
    tst_priorityqueuebench.cpp
HEADERS += timerdata.h \
    stdptrpqadaptor.h \
    additional.h \
    stdvalpqadaptor.h \
    heap-inl.h \
    qptrlistadaptor.h \
    myheapadaptor.h \
    libuvheapadaptor.h \
    ../binary_heap.h
INCLUDEPATH += ..
DEFINES += SRCDIR=\\\"$$PWD/\\\"
