#****************************************************************************
#*
#* Copyright (C) 2010 Speedovation
#* Contact: Speedovation Lab (info@speedovation.com)
#* Author : Yash
#* License : Apache License 2.0
#*
#* KineticWing IDE CrashHandler
#* This file is part of the core classes of the KiWi Editor (IDE)
#*
#* Author: Yash Bhardwaj
#*
#* All rights are reserved.
#****************************************************************************/


QT += core widgets gui sql network uitools concurrent

TARGET = KiWiCrashReporter
VERSION = 2.0.8
TEMPLATE = app

INCLUDEPATH +=  $$PWD 

DEPENDPATH +=  $${INCLUDEPATH}

#Build directory to keep things clean
CONFIG(debug, debug|release) {
    MOC_DIR = "$$OUT_PWD/tmp/crash/debug/.moc"
    OBJECTS_DIR =  "$$OUT_PWD/tmp/crash/debug/.obj"
    UI_DIR =  "$$OUT_PWD/tmp/debug/crash/.ui"
    RCC_DIR =  "$$OUT_PWD/tmp/debug/crash/.qrc"
}
CONFIG(release, debug|release) {
    MOC_DIR = "$$OUT_PWD/tmp/release/crash/.moc"
    OBJECTS_DIR =  "$$OUT_PWD/tmp/release/crash/.obj"
    UI_DIR =  "$$OUT_PWD/tmp/release/crash/.ui"
    RCC_DIR =  "$$OUT_PWD/tmp/release/crash/.qrc"
}



RC_FILE = ../../theme/appico.rc

RESOURCES += \
    ../../theme/resource.qrc

macx{

    cache()

    CONFIG -= app_bundle

}

linux{
     LIBS += -lGL #-lpython2.7
}


#CONFIG +=   static release

#CONFIG += c++11
QMAKE_CXXFLAGS+= -std=c++11
QMAKE_LFLAGS +=  -std=c++11


SOURCES += \
    main.cpp\
     $$PWD/CrashReporterWidget.cpp \
    HttpRequestWorker.cpp \

HEADERS  += \
    $$PWD/../../../version.h \
    $$PWD/CrashReporterWidget.h \
    HttpRequestWorker.h

FORMS += \
    $$PWD/CrashReporterWidget.ui


