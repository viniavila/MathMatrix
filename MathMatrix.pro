#-------------------------------------------------
#
# Project created by QtCreator 2016-12-16T08:31:34
#
#-------------------------------------------------

QT       -= gui

TARGET = MathMatrix
TEMPLATE = lib

DEFINES += MATHMATRIX_LIBRARY

SOURCES += mathmatrix.cpp

HEADERS += mathmatrix.h\
        mathmatrix_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
