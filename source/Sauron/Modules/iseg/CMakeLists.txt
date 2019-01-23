cmake_minimum_required(VERSION 3.1)

PROJECT(isegModule)

include_directories("${CMAKE_INSTALL_PREFIX}/include")
set(SOURCES isegModule.cpp isegModule.h)
add_library(isegModule SHARED ${SOURCES})
target_link_libraries(isegModule  pthread )
install (TARGETS isegModule ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules)
