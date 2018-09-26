cmake_minimum_required(VERSION 3.1)

PROJECT(PrintVoltageCurrent)

include_directories("../../include")
set(SOURCES PrintVoltageCurrent.cpp PrintVoltageCurrent.h)
add_library(PrintVoltageCurrent SHARED ${SOURCES})
target_link_libraries(PrintVoltageCurrent pthread)
install (TARGETS PrintVoltageCurrent ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors)
