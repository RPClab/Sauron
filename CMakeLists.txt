########################################################
# Top-level CMakeLists.txt to build Sauron
# @author LAGARDE Francois
cmake_minimum_required(VERSION 3.1)
########################################################

# set the C++ standard to C++ 17
set(CMAKE_CXX_STANDARD 17)

project(sauron)

macro( set_if_not )
  if( NOT ${ARGV0} )
    set( ${ARGN} )
  endif()
endmacro()

option(CAEN_DRIVER "Compile and install CAEN drivers" OFF)

set_if_not( jsoncpp_repository "https://github.com/RPClab/jsoncpp" )
set_if_not( jsoncpp_version "master" )
set_if_not( serial_repository "https://github.com/RPClab/serial" )
set_if_not( serial_version "master" )
set_if_not( mariadbpp_repository "https://github.com/RPClab/mariadbpp" )
set_if_not( mariadbpp_version "master" )
set_if_not( netsnmp_repository "https://github.com/RPClab/net-snmp.git" )
set_if_not( netsnmp_version "master" )

include(ExternalProject)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)

# ----- serial package -----
ExternalProject_Add( serial
    GIT_REPOSITORY ${serial_repository}
    GIT_TAG ${serial_version}
    GIT_PROGRESS TRUE
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/serial-prefix
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/serial
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    LIST_SEPARATOR %
    UPDATE_COMMAND ""
)


# ----- jsoncpp package -----
ExternalProject_Add( jsoncpp
    GIT_REPOSITORY ${jsoncpp_repository}
    GIT_TAG ${jsoncpp_version}
    GIT_PROGRESS TRUE
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp-prefix
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    LIST_SEPARATOR %
    UPDATE_COMMAND ""
)

# ----- mariadb++ package -----
ExternalProject_Add( mariadbpp
    GIT_REPOSITORY ${mariadbpp_repository}
    GIT_TAG ${mariadbpp_version}
    GIT_PROGRESS TRUE
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/mariadbpp-prefix
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/mariadbpp
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
    LIST_SEPARATOR %
    UPDATE_COMMAND ""
)

# ----- net-snmp package -----
ExternalProject_Add(netsnmp
                    GIT_REPOSITORY ${netsnmp_repository}
                    GIT_TAG ${netsnmp_version}
                    GIT_PROGRESS TRUE
                    UPDATE_DISCONNECTED TRUE
                    BUILD_ALWAYS FALSE
                    SOURCE_DIR          "${CMAKE_CURRENT_BINARY_DIR}/netsnmp"
                    BINARY_DIR          "${CMAKE_CURRENT_BINARY_DIR}/netsnmp"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
)

ExternalProject_Add_Step(netsnmp copyCMakeLists
  COMMAND "cp" "${CMAKE_SOURCE_DIR}/source/net-snmp/CMakeLists.txt" "${CMAKE_CURRENT_BINARY_DIR}/netsnmp/CMakeLists.txt"
  COMMENT "Moving CMakeLists"   
  DEPENDEES download
  DEPENDERS build
  ALWAYS 1)




set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wl,-rpath=${CMAKE_SOURCE_DIR}/bin/lib/" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_SOURCE_DIR}/bin/lib/" )

#find_library(libnetsnmp libnetsnmp.so PATHS ${CMAKE_SOURCE_DIR}/bin/lib/ )
#find_library(libnetsnmpagent libnetsnmpagent.so PATHS ${CMAKE_SOURCE_DIR}/bin/lib/)
#find_library(libnetsnmphelpers libnetsnmphelpers.so PATHS ${CMAKE_SOURCE_DIR}/bin/lib/)
#find_library(libnetsnmpmibs libnetsnmpmibs.so PATHS ${CMAKE_SOURCE_DIR}/bin/lib/)

# dependencies
find_package(MariaDBClient REQUIRED)
find_package(OpenSSL REQUIRED) 
set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/bin/lib)

add_definitions(-DUNIX -DLINUX)
add_subdirectory(source/CAEN)
add_subdirectory(source/Sauron)
add_subdirectory(source/net-snmp)



#link_directories(${CMAKE_SOURCE_DIR}/bin/lib)
#include_directories(${CMAKE_SOURCE_DIR}/bin/include)
#add_executable(Main main.cpp)
#target_link_libraries (Main pthread dl ${caenhvwrapper} WIENER_SNMP )
#install (TARGETS Main DESTINATION ${CMAKE_SOURCE_DIR}/bin)
