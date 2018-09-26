cmake_minimum_required(VERSION 3.1)

PROJECT(isegModule)

include_directories("../../include")
set(SOURCES isegModule.cpp isegModule.h)
add_library(isegModule SHARED ${SOURCES})
target_link_libraries(isegModule  pthread )
install (TARGETS isegModule ARCHIVE DESTINATION ${CMAKE_LIBRARY_PATH}/Modules LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}/Modules RUNTIME DESTINATION ${CMAKE_LIBRARY_PATH}/Modules)
