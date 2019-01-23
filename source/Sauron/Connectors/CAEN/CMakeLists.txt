cmake_minimum_required(VERSION 3.1)

project(CAENConnector)


set(CAENlibs_repository "https://github.com/RPClab/CAENlibs.git")
set(CAENlibs_version "master")

include(ExternalProject)

# ----- CAENlibs package -----
ExternalProject_Add( 
                    CAENlibs
                    GIT_REPOSITORY ${CAENlibs_repository}
                    GIT_TAG ${CAENlibs_version}
                    GIT_PROGRESS TRUE
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCAENHVWrapper_demo=ON
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/CAENlibs-prefix
                    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/CAENlibs
                    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                    )                       



if (UNIX)
   SET(CAEN_FLAGS "-DUNIX" "-DLINUX" CACHE INTERNAL " " FORCE )
endif (UNIX)

if (WIN32)
   SET(CAEN_FLAGS "-D_WIN32" "-DWIN32" CACHE INTERNAL " " FORCE )
endif (WIN32)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
add_definitions(${CAEN_FLAGS})
include_directories("${CMAKE_INSTALL_PREFIX}/include")
include_directories("${CMAKE_INSTALL_PREFIX}/include/CAEN")
set(SOURCES CAENConnector.cpp CAENConnector.h)
link_libraries(CAENVME CAENComm caenhvwrapper pthread)
add_library(CAENConnector SHARED ${SOURCES})
target_link_libraries(CAENConnector pthread)
install (TARGETS CAENConnector ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Connectors LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Connectors RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Connectors)
add_dependencies(CAENConnector CAENlibs)
