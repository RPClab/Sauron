cmake_minimum_required(VERSION 3.1)

PROJECT(MPODModule)

include_directories("${CMAKE_INSTALL_PREFIX}/bin/include")
set(SOURCES MPODModule.cpp MPODModule.h)
add_library(MPODModule SHARED ${SOURCES})
target_link_libraries(MPODModule  pthread )
install (TARGETS MPODModule ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Modules LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Modules RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Modules)
