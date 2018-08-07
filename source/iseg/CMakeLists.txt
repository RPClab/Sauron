include_directories(${CMAKE_SOURCE_DIR}/bin/include)
include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/source/Sauron/include)
add_library( iseg STATIC src/isegNHSR.cpp src/Parameters.cpp)
link_libraries(iseg serial pthread)
install (TARGETS iseg DESTINATION ${CMAKE_SOURCE_DIR}/bin/lib)

configure_file(include/Parameters.h ${CMAKE_SOURCE_DIR}/bin/include/iseg/Parameters.h COPYONLY)
configure_file(include/isegNHSR.h ${CMAKE_SOURCE_DIR}/bin/include/iseg/isegNHSR.h COPYONLY)
