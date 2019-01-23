########################################################
# Top-level CMakeLists.txt to build Sauron
# @author LAGARDE Francois
cmake_minimum_required(VERSION 3.1)
########################################################

# set the C++ standard to C++ 17
set(CMAKE_CXX_STANDARD 17)

project(sauron)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)
SET(CMAKE_CXX_FLAGS  "-fPIC ${CMAKE_CXX_FLAGS}" )
SET(CMAKE_EXE_LINKER_FLAGS  "-fPIC ${CMAKE_EXE_LINKER_FLAGS}" )

set(CMAKE_INSTALL_PREFIX "")

if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)
else()
endif()

set(CMAKE_INSTALL_PREFIX "")

if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)
else()
endif()


macro( set_if_not )
    if( NOT ${ARGV0} )
        set( ${ARGN} )
    endif()
endmacro()

set_if_not( jsoncpp_repository "https://github.com/RPClab/jsoncpp" )
set_if_not( jsoncpp_version "master" )
set_if_not( mariadbpp_repository "https://github.com/RPClab/mariadbpp" )
set_if_not( mariadbpp_version "master" )

include(ExternalProject)

option(CAEN_DRIVER "Compile and install CAEN drivers" OFF)
option(CLIENT "Build the client" OFF)
option(SERVER "Build the server" OFF)
option(SAURON "Build the library" ON)

if(CLIENT)
    add_subdirectory(source/Sauron/Client)
endif()

if(SERVER)
    set(SAURON ON)
    add_subdirectory(source/Sauron/Server)
endif()

if(SAURON)
    # dependencies
    find_package(MariaDBClient REQUIRED)
  #  find_package(OpenSSL REQUIRED) 

    # ----- jsoncpp package -----
    ExternalProject_Add( 
                        jsoncpp
                        GIT_REPOSITORY ${jsoncpp_repository}
                        GIT_TAG ${jsoncpp_version}
                        GIT_PROGRESS TRUE
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-fPIC"
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp-prefix
                        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp
                        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                        LIST_SEPARATOR %
                        UPDATE_COMMAND ""
                        )

    # ----- mariadb++ package -----
    ExternalProject_Add( 
                        mariadbpp
                        GIT_REPOSITORY ${mariadbpp_repository}
                        GIT_TAG ${mariadbpp_version}
                        GIT_PROGRESS TRUE
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-fPIC"
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/mariadbpp-prefix
                        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/mariadbpp
                        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                     LIST_SEPARATOR %
                        UPDATE_COMMAND ""
                        )
    add_subdirectory(source/Sauron)
endif()
