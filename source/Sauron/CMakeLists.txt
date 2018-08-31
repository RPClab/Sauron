# dependencies
find_package(MariaDBClient REQUIRED)

## Include headers
include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/bin/include)

link_directories(${PROJECT_SOURCE_DIR}/bin/lib)

## Sources
file(GLOB SAURON_SOURCES src/*.cpp)

add_library( Sauron STATIC ${SAURON_SOURCES})
link_libraries(Sauron serial jsoncpp pthread netsnmp)

## Install executable
install(TARGETS Sauron ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/bin/lib LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/bin/lib)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)
find_package(Boost 1.67.0 COMPONENTS program_options system thread container)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(Controler Console.cpp)
  target_link_libraries(Controler Sauron libnetsnmp.so ${CAENVME} libcaenhvwrapper.so libCAENComm.so ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})
  install(TARGETS Controler RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endif()

  
   add_executable(Test Test.cpp)
   if(CAEN_DRIVER)
   add_dependencies(Test drivers)
   endif()
  include_directories(${MariaDBClient_INCLUDE_DIRS})
  target_link_libraries(Test ${MariaDBClient_LIBRARIES} pthread mariadbclientpp libnetsnmp.so ${CAENVME} libcaenhvwrapper.so libCAENComm.so ${OPENSSL_LIBRARIES} Sauron )
  install(TARGETS Test RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)

