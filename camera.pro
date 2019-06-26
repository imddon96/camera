#-------------------------------------------------
#
# Project created by QtCreator 2019-06-09T14:28:36
#
#-------------------------------------------------

QT       += core gui network

#DEFINES += QT_NO_WARNING_OUTPU T\
#                 QT_NO_DEBUG_OUTPUT

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = camera
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    v4l2_example.c \
    realtimeshow.cpp \
    videorecoder.cpp \
    powerpiont.cpp \
    qttoopencv.cpp \
    videoplayer2.cpp

HEADERS  += widget.h \
    v4l2_example.h \
    realtimeshow.h \
    videorecoder.h \
    powerpiont.h \
    qttoopencv.h \
    huffman.h \
    videoplayer2.h

FORMS    += widget.ui

#LIBS += /usr/local/lib/lib* \
#/usr/local/ffmpeg/lib/lib* \
#/usr/local/x264/lib/libx264.so.157

INCLUDEPATH += /opt/webcam/ffmpeg/include \
/usr/local/include/x264/ \
#/usr/local/include/ffmpeg/libavutil

LIBS += /opt/arm/opencv/opencv3-arm/lib/lib*
LIBS += /opt/webcam/ffmpeg/lib/lib*
LIBS += /opt/webcam/x264/lib/libx264.so.157
