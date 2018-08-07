 #ifndef ADD_H
 #define ADD_H
 
 #if defined(_MSC_VER) && !defined(TESTS)
 #	define EXPORT _declspec(dllexport)
 #else
 #	define EXPORT
 #endif

 #define WIN32_LEAN_AND_MEAN
 
 #include <net-snmp/net-snmp-config.h>
 #include <net-snmp/net-snmp-includes.h>

	 EXPORT int    snmpInit(const char*);
	 

 #endif
