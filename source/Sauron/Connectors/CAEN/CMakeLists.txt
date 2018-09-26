cmake_minimum_required(VERSION 3.1)

PROJECT(CAENConnector)

 add_definitions(-DUNIX -DLINUX)

math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
if( ${BITS}  STREQUAL "64" )
set(FOLDER "x64")
message("-- 64 bits system")
else()
set(FOLDER "x86")
message("-- 32 bits system")
endif()

include_directories("../../include")
include_directories("lib/CAENComm-1.2/include")
include_directories("lib/CAENHVWrapper-5.82/include")
include_directories("lib/CAENVMELib-2.50/include")

find_library(CAENVMELIB NAMES libCAENVME.so.2.50 PATHS "lib/CAENVMELib-2.50/lib/${FOLDER}" NO_DEFAULT_PATH)
find_library(CAENCommLIB NAMES libCAENComm.so.1.2.0 PATHS "lib/CAENComm-1.2/lib/${FOLDER}" NO_DEFAULT_PATH)

if( ${BITS}  STREQUAL "64" ) 

link_directories("lib/CAENVMELib-2.50/lib/${FOLDER}" "lib/CAENComm-1.2/lib/${FOLDER}" "lib/CAENHVWrapper-5.82/lib/${FOLDER}")


find_library(caenhvwrapperLIB NAMES libcaenhvwrapper.so.5.82 PATHS "lib/CAENHVWrapper-5.82/lib/${FOLDER}" NO_DEFAULT_PATH)
else()
link_directories("lib/CAENVMELib-2.50/lib/${FOLDER}" "lib/CAENComm-1.2/lib/${FOLDER}" "lib/CAENHVWrapper-5.82/lib")
find_library(caenhvwrapperLIB NAMES libcaenhvwrapper.so.5.82 PATHS "lib/CAENHVWrapper-5.82/lib" NO_DEFAULT_PATH)
endif()

if(${CAENVMELIB} STREQUAL "CAENVMELIB-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'CAENVME' library")
endif()

message(STATUS "Found 'CAENVME' at: ${CAENVMELIB}")

if(${CAENCommLIB} STREQUAL "CAENCommLIB-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'CAENComm' library")
endif()

message(STATUS "Found 'CAENComm' at: ${CAENCommLIB}")


if(${caenhvwrapperLIB} STREQUAL "caenhvwrapperLIB-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'caenhvwrapper' library")
endif()

message(STATUS "Found 'caenhvwrapper' at: ${caenhvwrapperLIB}")



set(SOURCES CAENConnector.cpp CAENConnector.h)
add_library(CAENConnector SHARED ${SOURCES})
#target_link_libraries(CAENConnector ${CAENVMELIB} ${CAENCommLIB} ${caenhvwrapperLIB})
install (TARGETS CAENConnector ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Connectors)

if(CAENHVWrapper_demo)
add_executable(MainWrapp CAENHVWrapper-5.82/HVWrapperDemo/MainWrapp.c CAENHVWrapper-5.82/HVWrapperDemo/console.c CAENHVWrapper-5.82/HVWrapperDemo/CmdWrapp.c )
target_link_libraries (MainWrapp ${caenhvwrapper} pthread dl ncurses m )
install (TARGETS MainWrapp DESTINATION ${CMAKE_SOURCE_DIR}/bin)
endif()

if(CAEN_DRIVER)
add_subdirectory(drivers)
endif()
