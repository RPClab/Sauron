set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)
find_package(Boost 1.67.0 COMPONENTS program_options system thread container)

  
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(Client Client.cpp)
  target_link_libraries(Client ${Boost_LIBRARIES} pthread )
  install(TARGETS Client RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

