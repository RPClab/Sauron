cmake_minimum_required(VERSION 3.1)

PROJECT(CAENConnector)

macro( set_if_not )
    if( NOT ${ARGV0} )
        set( ${ARGN} )
    endif()
endmacro()

set_if_not( CAENlibs_repository "https://github.com/RPClab/CAENlibs.git" )
set_if_not( CAENlibs_version "master" )

include(ExternalProject)

    # ----- CAENlibs package -----
    ExternalProject_Add( 
                        CAENlibs
                        GIT_REPOSITORY ${CAENlibs_repository}
                        GIT_TAG ${CAENlibs_version}
                        GIT_PROGRESS TRUE
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/CAENlibs-prefix
                        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/CAENlibs
                        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                        LIST_SEPARATOR %
                        UPDATE_COMMAND ""
                        )                       

include_directories("../../include")

if (UNIX)
   SET(CAEN_FLAGS "-DUNIX" "-DLINUX" CACHE INTERNAL " " FORCE )
endif (UNIX)

if (WIN32)
   SET(CAEN_FLAGS "-D_WIN32" "-DWIN32" CACHE INTERNAL " " FORCE )
endif (WIN32)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/CAENlibs/lib/CAENComm-1.2/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/CAENlibs/lib/CAENHVWrapper-5.82/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/CAENlibs/lib/CAENVMELib-2.50/include")
add_definitions(${CAEN_FLAGS})
set(SOURCES CAENConnector.cpp CAENConnector.h)
add_library(CAENConnector SHARED ${SOURCES})
add_dependencies(CAENConnector CAENlibs)
target_link_libraries(CAENConnector ${CAENVMELIB} ${CAENCommLIB} ${caenhvwrapperLIB})
message("${CAENVMELIB} ${CAENCommLIB} ${caenhvwrapperLIB}")
install (TARGETS CAENConnector ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors)
