cmake_minimum_required(VERSION 3.1)

PROJECT(MonitorEvents)

include_directories("../../include")
set(SOURCES MonitorEvents.cpp MonitorEvents.h)
add_library(MonitorEvents SHARED ${SOURCES})
target_link_libraries(MonitorEvents pthread)
install (TARGETS MonitorEvents ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors)
