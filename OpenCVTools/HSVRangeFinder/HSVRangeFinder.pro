#-------------------------------------------------
#
# Project created by QtCreator 2012-10-28T19:51:50
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = HSVRangeFinder
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann

