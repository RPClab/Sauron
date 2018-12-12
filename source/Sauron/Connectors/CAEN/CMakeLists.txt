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


                        
include_directories("${CMAKE_INSTALL_PREFIX}/include")
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/CAENlibs/lib/CAENComm-1.2/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/CAENlibs/lib/CAENHVWrapper-5.82/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/CAENlibs/lib/CAENVMELib-2.50/include")

if (UNIX)
   SET(CAEN_FLAGS "-DUNIX" "-DLINUX" CACHE INTERNAL " " FORCE )
endif (UNIX)

if (WIN32)
   SET(CAEN_FLAGS "-D_WIN32" "-DWIN32" CACHE INTERNAL " " FORCE )
endif (WIN32)

set(SOURCES CAENConnector.cpp CAENConnector.h)
add_definitions(${CAEN_FLAGS})
add_library(CAENConnector SHARED ${SOURCES})
target_link_libraries(CAENConnector CAENVME CAENComm caenhvwrapper)
add_dependencies(CAENConnector CAENlibs)

message(" LLLLLL ${CMAKE_INSTALL_PREFIX}/lib/Connectors")
install (TARGETS CAENConnector ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Connectors LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Connectors RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Connectors)
