#-------------------------------------------------
#
# Project created by QtCreator 2018-08-02T00:16:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reminder
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
    table_date.cpp \
    remind.cpp \
    logger.cpp \
    update.cpp \
    history.cpp

HEADERS += \
        mainwindow.h \
    table_date.h \
    structers.h \
    remind.h \
    logger.h \
    update.h \
    history.h

FORMS += \
        mainwindow.ui \
    table_date.ui \
    remind.ui \
    update.ui \
    history.ui

win32:RC_ICONS += /res/clock.ico

RESOURCES +=
