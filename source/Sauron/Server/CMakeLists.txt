## Include headers
include_directories(include)
include_directories(${CMAKE_INSTALL_PREFIX}/include)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)


add_library( NHRSpp_lib STATIC ${SAURON_SOURCES})
link_libraries(NHRSpp_lib serial jsoncpp pthread netsnmp)

## Install executable
install(TARGETS  NHRSpp_lib ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)
find_package(Boost 1.67.0 COMPONENTS program_options system thread container)

  
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(Server Server.cpp)
  target_link_libraries(Server ${Boost_LIBRARIES} NHRSpp_lib iseg libnetsnmp.so ${CAENVME} libcaenhvwrapper.so libCAENComm.so ${OPENSSL_LIBRARIES} )
  install(TARGETS Server RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

