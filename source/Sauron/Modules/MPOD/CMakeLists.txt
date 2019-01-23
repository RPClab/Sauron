cmake_minimum_required(VERSION 3.1)

PROJECT(MPODModule)

include_directories("${CMAKE_INSTALL_PREFIX}/include")
set(SOURCES MPODModule.cpp MPODModule.h)
add_library(MPODModule SHARED ${SOURCES})
target_link_libraries(MPODModule  pthread )
install (TARGETS MPODModule ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules)
