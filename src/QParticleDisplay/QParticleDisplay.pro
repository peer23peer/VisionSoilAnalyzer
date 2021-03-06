#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T22:02:49
#
#-------------------------------------------------

QT       += core gui concurrent
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QParticleDisplay
TEMPLATE = lib
CONFIG += shared
VERSION = 1.3.4

SOURCES += qparticledisplay.cpp

HEADERS  += qparticledisplay.h

FORMS    += qparticledisplay.ui

unix:!macx: LIBS += -L$$PWD/../../build/install/ -lpictureflow-qt

INCLUDEPATH += $$PWD/../pictureflow-qt
DEPENDPATH += $$PWD/../pictureflow-qt

#MainLib
unix {
    target.path = $PWD/../../../build/install
    INSTALLS += target
}

unix:!macx: LIBS += -L$$PWD/../../build/install/ -lSoilAnalyzer

INCLUDEPATH += $$PWD/../SoilAnalyzer
DEPENDPATH += $$PWD/../SoilAnalyzer

unix:!macx: LIBS += -L$$PWD/../../build/install/ -lSoilMath

INCLUDEPATH += $$PWD/../SoilMath
DEPENDPATH += $$PWD/../SoilMath

unix:!macx: LIBS += -L$$PWD/../../build/install/ -lQOpenCVQT

INCLUDEPATH += $$PWD/../QOpenCVQT
DEPENDPATH += $$PWD/../QOpenCVQT

unix:!macx: LIBS += -L$$PWD/../../build/install/ -lSoilVision
INCLUDEPATH += $$PWD/../SoilVision
DEPENDPATH += $$PWD/../SoilVision

#opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include
