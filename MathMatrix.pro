#-------------------------------------------------
#
# Project created by QtCreator 2016-12-16T08:31:34
#
#-------------------------------------------------

QT       -= gui

win32:TARGET = MathMatrix
unix:TARGET = mathmatrix
TEMPLATE = lib
CONFIG += c++11

DEFINES += MATHMATRIX_LIBRARY

SOURCES += mathmatrix.cpp

HEADERS += \
    mathmatrix_global.h \
    mathmatrix.h

OTHER_FILES += \
    LICENSE \
    README.md

unix {
    target.path = /usr/lib
    INSTALLS += target
}
