# dependencies
find_package(MariaDBClient REQUIRED)

## Include headers
include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/bin/include)

link_directories(${PROJECT_SOURCE_DIR}/bin/lib)

## Sources
file(GLOB SAURON_SOURCES src/*.cpp)

add_library( NHRSpp_lib STATIC ${SAURON_SOURCES})
#add_dependencies(NHRSpp_lib serial jsoncpp netsnmp)
link_libraries(NHRSpp_lib serial jsoncpp pthread netsnmp)

## Install executable
install(TARGETS  NHRSpp_lib ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/bin/lib LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/bin/lib)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.67.0 COMPONENTS program_options)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(Controler Console.cpp)
  target_link_libraries(Controler NHRSpp_lib iseg libnetsnmp.so ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})
  install(TARGETS Controler RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endif()



  add_executable(Monitoring Monitoring.cpp)
  include_directories(${MariaDBClient_INCLUDE_DIRS})
  target_link_libraries(Monitoring NHRSpp_lib iseg ${MariaDBClient_LIBRARIES} libnetsnmp.so ${OPENSSL_LIBRARIES} pthread mariadbclientpp)
  install(TARGETS Monitoring RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
  
   add_executable(Test Test.cpp)
   if(CAEN_DRIVER)
   add_dependencies(Test drivers)
   endif()
  include_directories(${MariaDBClient_INCLUDE_DIRS})
  target_link_libraries(Test iseg ${MariaDBClient_LIBRARIES} pthread mariadbclientpp libnetsnmp.so ${OPENSSL_LIBRARIES} NHRSpp_lib )
  install(TARGETS Test RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
  

  #add_executable(Test src/source/test.cpp)
  #target_link_libraries(Test NHRSpp_lib  pthread)
  #include_directories(${MariaDBClient_INCLUDE_DIRS})
  #install(TARGETS Test RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)

## Uncomment for example
#add_executable(toto src/source/main.cpp)
#add_dependencies(toto NHRSpp_lib)
#target_link_libraries(toto NHRSpp_lib) 
#install(TARGETS toto RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
