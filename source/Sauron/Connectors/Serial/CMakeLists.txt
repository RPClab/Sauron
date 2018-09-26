cmake_minimum_required(VERSION 3.1)

PROJECT(SerialConnector)

macro( set_if_not )
    if( NOT ${ARGV0} )
        set( ${ARGN} )
    endif()
endmacro()

set_if_not( serial_repository "https://github.com/RPClab/serial" )
set_if_not( serial_version "master" )

include(ExternalProject)

    # ----- serial package -----
    ExternalProject_Add( 
                        serial
                        GIT_REPOSITORY ${serial_repository}
                        GIT_TAG ${serial_version}
                        GIT_PROGRESS TRUE
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-fPIC"
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/serial-prefix
                        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/serial
                        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                        LIST_SEPARATOR %
                        UPDATE_COMMAND ""
                        )


include_directories("../../include")
set(SOURCES SerialConnector.cpp SerialConnector.h)
link_libraries(serial)
add_library(SerialConnector SHARED ${SOURCES})
target_link_libraries(SerialConnector pthread)
install (TARGETS SerialConnector ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors)
