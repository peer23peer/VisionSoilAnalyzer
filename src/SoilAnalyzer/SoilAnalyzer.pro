#-------------------------------------------------
#
# Project created by QtCreator 2015-08-08T18:57:27
#
#-------------------------------------------------

QT       += core gui concurrent
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
@
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
@

TARGET = SoilAnalyzer
TEMPLATE = lib
VERSION = 1.0.0

DEFINES += SOILANALYZER_LIBRARY

SOURCES += \
    soilsettings.cpp \
    sample.cpp \
    particle.cpp \
    analyzer.cpp

HEADERS +=\
    soilsettings.h \
    sample.h \
    particle.h \
    analyzer.h \
    soilanalyzerexception.h \
    soilanalyzer.h \
    lab_t_archive.h \
    soilanalyzertypes.h

#opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include

#boost
DEFINES += BOOST_ALL_DYN_LINK
INCLUDEPATH += /usr/include/boost

contains(QT_ARCH, x86_64) {
    LIBS += -L/usr/lib/x86_64-linux-gnu/ -lboost_serialization -lboost_iostreams
}
contains(QT_ARCH, arm) {
    LIBS += -L/usr/lib/arm-linux-gnueabihf/ -lboost_serialization -lboost_iostreams
}

#Zlib
LIBS += -L/usr/local/lib -lz
INCLUDEPATH += /usr/local/include

unix:!macx: LIBS += -L$$PWD/../../build/install/ -lSoilMath

INCLUDEPATH += $$PWD/../SoilMath
DEPENDPATH += $$PWD/../SoilMath

unix:!macx: LIBS += -L$$PWD/../../build/install/ -lSoilVision
INCLUDEPATH += $$PWD/../SoilVision
DEPENDPATH += $$PWD/../SoilVision

#MainLib

target.path = $PWD/../../../build/install
INSTALLS += target


