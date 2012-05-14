#-------------------------------------------------
#
# Project created by QtCreator 2012-05-10T14:11:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AisParserApp
TEMPLATE = app


SOURCES += main.cpp\
        aisparsermainwindow.cpp

HEADERS  += aisparsermainwindow.h

INCLUDEPATH += ..\\..\\
INCLUDEPATH += C:\\libraries\\boost\\v1_49_0
INCLUDEPATH += C:\\libraries\\oracle\\instantclient-sdk-nt-11.2.0.2.0\\include
INCLUDEPATH += C:\\libraries\\mysql\\v110\\include
INCLUDEPATH += C:\\libraries\\icode\\3rd-party\\include

LIBS += -LC:\\libraries\\boost\\v1_49_0\\stage\\lib -llibboost_timer-vc100-mt-1_49
LIBS += -LC:\\libraries\\oracle\\instantclient-sdk-nt-11.2.0.2.0\\lib\\msvc -loraocci11 -loci
LIBS += -LC:\\libraries\\mysql\\v110\\lib -lmysqlcppconn
LIBS += -LC:\\libraries\\icode\\3rd-party\\lib\\win32 -lshapelib

FORMS    += aisparsermainwindow.ui

