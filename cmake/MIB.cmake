function(CHECK_MIB)
    if(EXISTS "${CMAKE_SOURCE_DIR}/build/netsnmp/mibs/SNMPv2-SMI.txt")
        if(EXISTS "${CMAKE_SOURCE_DIR}/conf/MIB/SNMPv2-SMI.txt")
        else()
            configure_file(${CMAKE_SOURCE_DIR}/build/netsnmp/mibs/SNMPv2-SMI.txt ${CMAKE_SOURCE_DIR}/conf/MIB/SNMPv2-SMI.txt COPYONLY)
        endif()
        if(EXISTS "${CMAKE_SOURCE_DIR}/SNMPv2-TC.txt")  
        else()
            configure_file(${CMAKE_SOURCE_DIR}/build/netsnmp/mibs/SNMPv2-TC.txt ${CMAKE_SOURCE_DIR}/conf/MIB/SNMPv2-TC.txt COPYONLY)
        endif()
    endif()
endfunction(CHECK_MIB)


