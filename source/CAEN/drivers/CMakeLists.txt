if(EXISTS "/dev/a2818_0" AND IS_DIRECTORY "/dev/a2818_0")
    message("-- a2818 devices detected")
    string(SUBSTRING ${CMAKE_HOST_SYSTEM_VERSION} 0 4 LINUX_VERSION)
    string(COMPARE GREATER ${LINUX_VERSION} "2.5" LINUX_VERSION)
    if( ${LINUX_VERSION}  STREQUAL "0" )
    message("-- Linux version inferior to 2.6")
    set(CAEN_DRIVERS_MAKEFILE ${CMAKE_SOURCE_DIR}/source/CAEN/drivers/A2818Drv-1.20/Makefile.2.4)
    set(CAEN_DRIVERS_SCRIPT ${CMAKE_SOURCE_DIR}/source/CAEN/drivers/A2818Drv-1.20/a2818_load.2.4)
    else()
    message("-- Linux version superior to 2.6")
    set(CAEN_DRIVERS_MAKEFILE ${CMAKE_SOURCE_DIR}/source/CAEN/drivers/A2818Drv-1.20/Makefile.2.6-3.x)
    set(CAEN_DRIVERS_SCRIPT ${CMAKE_SOURCE_DIR}/source/CAEN/drivers/A2818Drv-1.20/a2818_load.2.6-3.x)
    endif()
    message("-- ${CAEN_DRIVERS_MAKEFILE}")
    message("-- ${CAEN_DRIVERS_SCRIPT}")
add_custom_target(drivers 
                  COMMAND "sudo" "make" "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/CAENUSBdrvB-1.5.2/" 
                  COMMAND "sudo" "make" "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/CAENUSBdrvB-1.5.2/" 
                  "install" 
                  COMMAND "sudo" "make" "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/CAENUSBdrvB-1.5.2/" 
                  "clean" 
                  COMMAND "sudo" "make" "--makefile=${CAEN_DRIVERS_MAKEFILE}" 
                  "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/A2818Drv-1.20/" 
	          COMMAND "sudo" "sh" "${CAEN_DRIVERS_SCRIPT}"
                  COMMAND "sudo" "make" "--makefile=${CAEN_DRIVERS_MAKEFILE}" 
                  "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/A2818Drv-1.20/" "clean"
              WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
                  )
else()
add_custom_target(drivers 
                  COMMAND "sudo" "make" "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/CAENUSBdrvB-1.5.2/" 
                  COMMAND "sudo" "make" "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/CAENUSBdrvB-1.5.2/" 
                  "install" 
                  COMMAND "sudo" "make" "--directory=${CMAKE_SOURCE_DIR}/source/CAEN/drivers/CAENUSBdrvB-1.5.2/" 
                  "clean" 
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
                  )
endif()
