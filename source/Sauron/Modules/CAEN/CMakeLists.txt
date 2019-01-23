cmake_minimum_required(VERSION 3.1)

PROJECT(CAENModule)

include_directories("${CMAKE_INSTALL_PREFIX}/include")
set(SOURCES CAENModule.cpp CAENModule.h)
add_library(CAENModule SHARED ${SOURCES})
target_link_libraries(CAENModule  pthread )
install (TARGETS CAENModule ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules)
