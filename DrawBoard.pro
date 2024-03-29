#-------------------------------------------------
#
# Project created by QtCreator 2017-11-02T20:39:25
#
#-------------------------------------------------

QT       += core gui
QT       += opengl


#QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DrawBoard
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWindow.cpp \
    OpenGLWindow.cpp

HEADERS += \
        MainWindow.h \
    OpenGLWindow.h \
    common.h

FORMS += \
        MainWindow.ui





RESOURCES += \
    src.qrc

win32: LIBS += -L$$PWD/glut/ -lglut32

INCLUDEPATH += $$PWD/glut
DEPENDPATH += $$PWD/glut

win32: LIBS += -L$$PWD/glut/ -lglut

INCLUDEPATH += $$PWD/glut
DEPENDPATH += $$PWD/glut
