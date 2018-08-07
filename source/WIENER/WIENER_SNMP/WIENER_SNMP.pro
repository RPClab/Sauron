#-------------------------------------------------
#
# Project created by QtCreator 2012-07-26T17:46:28
#
#-------------------------------------------------

CONFIG -= QT

TARGET = WIENER_SNMP
TEMPLATE = lib

RC_FILE = resources.rc

DEFINES += WIENERNETSNMP_LIBRARY

windows {
	INCLUDEPATH += c:/usr/include
	QMAKE_LIBDIR += c:/usr/lib
	*-msvc* {
		LIBS += netsnmp.lib ws2_32.lib advapi32.lib
	}
} else {
		LIBS += -lnetsnmp
}

SOURCES += \
	WIENER_SNMP.cpp

HEADERS += \
	WIENER_SNMP.h \
	version.h

OTHER_FILES += \
	readme.txt \
	resources.rc \
	releases.txt
