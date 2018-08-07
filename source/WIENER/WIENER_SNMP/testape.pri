windows {
	# remove additional Qmake defines
	DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

	*-msvc* {
		# remove manifest options
		CONFIG -= embed_manifest_exe

		# remove additional compiler flags (like -MDd)
		QMAKE_CFLAGS_DEBUG = -Zi
		QMAKE_CXXFLAGS_DEBUG = -Zi
		QMAKE_CFLAGS_RELEASE = -Zi
		QMAKE_CXXFLAGS_RELEASE = -Zi

		# remove the remaining manifest stuff
		QMAKE_LFLAGS_EXE =

		# additional libraries
		LIBS += testape.lib

		# linker command
		QMAKE_LINK = testape link
	}
} else {
	# additional libraries
	LIBS += -ltestape

	# linker command
	QMAKE_LINK = testape gcc
}
