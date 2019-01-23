# dependencies
find_package(MariaDBClient REQUIRED)

## Include headers
include_directories(include)
include_directories(${CMAKE_INSTALL_PREFIX}/include)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

## Sources
file(GLOB SAURON_SOURCES src/*.cpp)

add_library( Sauron STATIC ${SAURON_SOURCES})
link_libraries(Sauron jsoncpp pthread dl)

## Install executable
install(TARGETS Sauron ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)
find_package(Boost 1.67.0 COMPONENTS program_options system thread container filesystem)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
 # add_executable(Controler Console.cpp)
 # target_link_libraries(Controler Sauron  ${CAENVME}  ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})
 # install(TARGETS Controler RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)

  
   add_executable(Test Test.cpp)
   if(CAEN_DRIVER)
   add_dependencies(Test drivers)
   endif()
  include_directories(${MariaDBClient_INCLUDE_DIRS})
  target_link_libraries(Test ${MariaDBClient_LIBRARIES} pthread ${Boost_LIBRARIES} Sauron )
  install(TARGETS Test RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
  endif()
#add_subdirectory(Wt)


find_package(ROOT)
if(ROOT_FOUND)
include_directories(${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
 add_executable(IvsU IvsU.cpp)
 if(CAEN_DRIVER)
   add_dependencies(IvsU drivers)
   endif()
 include_directories(${MariaDBClient_INCLUDE_DIRS})
  target_link_libraries(IvsU ${MariaDBClient_LIBRARIES} pthread ${ROOT_LIBRARIES} ${Boost_LIBRARIES} Sauron )
  install(TARGETS IvsU RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})


endif()

add_subdirectory(Connectors)
add_subdirectory(Monitors)
add_subdirectory(Modules)


