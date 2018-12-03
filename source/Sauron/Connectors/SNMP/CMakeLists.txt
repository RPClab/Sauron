cmake_minimum_required(VERSION 3.1)

PROJECT(SNMPConnector)

macro( set_if_not )
    if( NOT ${ARGV0} )
        set( ${ARGN} )
    endif()
endmacro()

set_if_not( netsnmp_repository "https://github.com/RPClab/net-snmp.git" )
set_if_not( netsnmp_version "master" )

include(ExternalProject)

    # ----- net-snmp package -----
    ExternalProject_Add(
                        netsnmp
                        GIT_REPOSITORY ${netsnmp_repository}
                        GIT_TAG ${netsnmp_version}
                        GIT_PROGRESS TRUE
                        SOURCE_DIR          "${CMAKE_CURRENT_BINARY_DIR}/netsnmp"
                        BINARY_DIR          "${CMAKE_CURRENT_BINARY_DIR}/netsnmp"
                        LIST_SEPARATOR %
                        UPDATE_COMMAND ""
                        CONFIGURE_COMMAND "touch" "${CMAKE_CURRENT_BINARY_DIR}/netsnmp/CMakeLists.txt"
                        BUILD_COMMAND "sh" "${CMAKE_CURRENT_BINARY_DIR}/netsnmp/configure" "CFLAGS=-fPIC" "CPPFLAGS=-fPIC" "--disable-agent" "--disable-applications" "--disable-manuals" "--disable-scripts" "--disable-mibs" "--prefix=${CMAKE_SOURCE_DIR}/bin" "--disable-embedded-perl" "--without-rpm" "--with-sys-contact=\"\"" "--with-sys-location=\"\"" "--with-logfile=\"none\"" "--with-persistent-directory=\"\"" "--with-copy-persistent-files=\"no\"" "--disable-perl-cc-checks" "--with-defaults" "--enable-shared=\"on\""
                        )

                        
include(cmake/MIB.cmake)
CHECK_MIB()
include_directories("${CMAKE_INSTALL_PREFIX}/bin/include")
find_package(OpenSSL REQUIRED) 
set(SOURCES SNMPConnector.cpp SNMPConnector.h)
link_libraries(netsnmp ${OPENSSL_LIBRARIES})
add_library(SNMPConnector SHARED ${SOURCES})
target_link_libraries(SNMPConnector  pthread )
install (TARGETS SNMPConnector ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors)
