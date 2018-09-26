cmake_minimum_required(VERSION 3.1)

PROJECT(CAENModule)

include_directories("../../include")
set(SOURCES CAENModule.cpp CAENModule.h)
add_library(CAENModule SHARED ${SOURCES})
target_link_libraries(CAENModule  pthread )
install (TARGETS CAENModule ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Modules LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Modules RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Modules)
