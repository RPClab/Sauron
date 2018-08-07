TEMPLATE = app
CONFIG += console

TARGET = WIENER_SNMP_TEST

CONFIG -= app_bundle qt

DEFINES += TESTS

windows {
	INCLUDEPATH += c:/usr/include
}

include(testape.pri)

SOURCES += \
	WIENER_SNMP.cpp \
	testmain.c \
	mocksnmp.c \
	testsnmp.c \
	testconfig.c

HEADERS += \
	WIENER_SNMP.h \
	mocksnmp.h \
	version.h
