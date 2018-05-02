#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T19:32:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = QKalib
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
        mainwindow.cpp \
    dialogvermuestras.cpp \
    dialogconfigurarcalibracion.cpp

HEADERS += \
        mainwindow.h \
    dialogvermuestras.h \
    dialogconfigurarcalibracion.h

FORMS += \
        mainwindow.ui \
    dialogvermuestras.ui \
    dialogconfigurarcalibracion.ui

win32
{
LIBS += -lopencv_core341 -lopencv_photo341\
         -lopencv_highgui341 -lopencv_videoio341\
        -lopencv_imgproc341 -lopencv_features2d341\
        -lopencv_calib3d341 -lopencv_imgcodecs341\

win32:RC_ICONS += QK.ico
}

unix
{
LIBS += -lopencv_core -lopencv_photo\
        -lopencv_ts -lopencv_highgui -lopencv_videoio\
        -lopencv_imgproc -lopencv_features2d\
        -lopencv_calib3d -lopencv_imgcodecs\
}

RESOURCES += \
    iconos.qrc
