TEMPLATE = app

QT += qml quick
CONFIG += c++11

SRC_DIR = src
DESTDIR = ../bin

SOURCES +=  \
    $${SRC_DIR}/main.cpp \
    $${SRC_DIR}/cameraimageview.cpp \
    $${SRC_DIR}/worker.cpp \
    $${SRC_DIR}/abstractcamera.cpp \
    $${SRC_DIR}/usbcamera.cpp \
    $${SRC_DIR}/imagenode.cpp \

RESOURCES += $${SRC_DIR}/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    $${SRC_DIR}/cameraimageview.h \
    $${SRC_DIR}/worker.h \
    $${SRC_DIR}/abstractcamera.h \
    $${SRC_DIR}/usbcamera.h \
    $${SRC_DIR}/imagenode.h \

INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/Cellar/opencv/2.4.11_1/include

LIBS += -L/usr/local/Cellar/opencv/2.4.11_1/lib
LIBS += -L/usr/local/lib
LIBS += -lopencv_core \
        -lopencv_highgui \
        -lopencv_objdetect \
        -lopencv_imgproc \
        -lopencv_features2d \
        -lopencv_calib3d \
        -lopencv_contrib \
        -lopencv_nonfree \
        -lopencv_features2d \
        -lopencv_flann
