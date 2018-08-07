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


if(EXISTS "${CMAKE_SOURCE_DIR}/bin/lib/libnetsnmp.a" 
   AND EXISTS "${CMAKE_SOURCE_DIR}/bin/lib/libnetsnmpagent.a" 
   AND EXISTS "${CMAKE_SOURCE_DIR}/bin/lib/libnetsnmphelpers.a"
   AND EXISTS "${CMAKE_SOURCE_DIR}/bin/lib/libnetsnmpmibs.a"
   )
    add_custom_target(SNMP COMMAND "")
    CHECK_MIB()
else()
   add_custom_target(SNMP
                  COMMAND "sh" "${CMAKE_SOURCE_DIR}/build/netsnmp/configure" "--disable-agent" "--disable-applications" "--disable-manuals" "--disable-scripts" "--disable-mibs" "--prefix=${CMAKE_SOURCE_DIR}/bin" "--disable-embedded-perl" "--without-rpm"  "--with-default-snmp-version=\"2\"" "--with-sys-contact=\"\"" "--with-sys-location=\"\"" "--with-logfile=\"none\"" "--with-persistent-directory=\"\"" "--with-copy-persistent-files=\"no\"" "--disable-perl-cc-checks" "--with-defaults" "--enable-shared=\"off\""
                  COMMAND "make" "-j4"
                  COMMAND "make" "install" 
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
                  )
    CHECK_MIB()
endif()




