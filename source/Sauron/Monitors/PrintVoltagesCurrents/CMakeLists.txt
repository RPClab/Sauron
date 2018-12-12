cmake_minimum_required(VERSION 3.1)

PROJECT(PrintVoltageCurrent)

include_directories("${CMAKE_INSTALL_PREFIX}/include")
set(SOURCES PrintVoltageCurrent.cpp PrintVoltageCurrent.h)
add_library(PrintVoltageCurrent SHARED ${SOURCES})
target_link_libraries(PrintVoltageCurrent pthread)
install (TARGETS PrintVoltageCurrent ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors)
