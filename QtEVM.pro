#-------------------------------------------------
#
# Project created by QtCreator 2014-01-07T13:31:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = excutable
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    WindowHelper.cpp \
    VideoProcessor.cpp \
    SpatialFilter.cpp \
    MotionDialog.cpp

HEADERS  += mainwindow.h \
    WindowHelper.h \
    VideoProcessor.h \
    SpatialFilter.h \
    MotionDialog.h

FORMS    += mainwindow.ui \
    MotionDialog.ui

RESOURCES += \
    myResources.qrc

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv \
                fftw3
}
Win32 {
INCLUDEPATH += C:\OpenCV2.2\include\
LIBS += -LC:\OpenCV2.2\lib \
    -lopencv_core220 \
    -lopencv_highgui220 \
    -lopencv_imgproc220 \
    -lopencv_features2d220 \
    -lopencv_calib3d220
}
