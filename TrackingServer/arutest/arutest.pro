TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp


LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../cvblobs8.3_linux/release/ -lblob
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../cvblobs8.3_linux/debug/ -lblob
else:symbian: LIBS += -lblob
else:unix: LIBS += -L$$PWD/../cvblobs8.3_linux/ -lblob

INCLUDEPATH += $$PWD/../cvblobs8.3_linux
DEPENDPATH += $$PWD/../cvblobs8.3_linux

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../cvblobs8.3_linux/release/blob.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../cvblobs8.3_linux/debug/blob.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../cvblobs8.3_linux/libblob.a


INCLUDEPATH +=  $$PWD/../FotbalRobotic/libs/mosquitto-1.0.5/lib

LIBS += -lmosquitto


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../aruco-1.2.4/build/src/release/ -laruco
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../aruco-1.2.4/build/src/debug/ -laruco
else:symbian: LIBS += -laruco
else:unix: LIBS += -L$$PWD/../aruco-1.2.4/build/src/ -laruco

INCLUDEPATH += $$PWD/../aruco-1.2.4/src
DEPENDPATH += $$PWD/../aruco-1.2.4/src
