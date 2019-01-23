cmake_minimum_required(VERSION 3.1)

PROJECT(MonitorEvents)

include_directories("${CMAKE_INSTALL_PREFIX}/include")
set(SOURCES MonitorEvents.cpp MonitorEvents.h)
add_library(MonitorEvents SHARED ${SOURCES})
target_link_libraries(MonitorEvents pthread)
install (TARGETS MonitorEvents ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Modules/Monitors LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors)
