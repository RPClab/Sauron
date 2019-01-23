cmake_minimum_required(VERSION 3.1)

PROJECT(RecordVoltages)

include_directories("${CMAKE_INSTALL_PREFIX}/include")
include_directories(${MariaDBClient_INCLUDE_DIRS})
set(SOURCES RecordVoltages.cpp RecordVoltages.h)
add_library(RecordVoltages SHARED ${SOURCES})
target_link_libraries(RecordVoltages ${MariaDBClient_LIBRARIES} mariadbclientpp pthread)
install (TARGETS RecordVoltages ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/Monitors)
