cmake_minimum_required(VERSION 3.1)

PROJECT(MonitorVoltages)


include_directories("../../include")
include_directories(${MariaDBClient_INCLUDE_DIRS})
set(SOURCES MonitorVoltages.cpp MonitorVoltages.h)
add_library(MonitorVoltages SHARED ${SOURCES})
target_link_libraries(MonitorVoltages pthread ${MariaDBClient_LIBRARIES} mariadbclientpp pthread)
install (TARGETS MonitorVoltages ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Monitors)
