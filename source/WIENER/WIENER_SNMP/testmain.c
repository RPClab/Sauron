#include <testape.h>

void testRunner_snmpOpen(void);
void testRunner_errorHandling(void);
void testRunner_multipleRequest(void);

void testmain(void)
{
	EXECUTE(testRunner_snmpOpen);
	EXECUTE(testRunner_errorHandling);
	EXECUTE(testRunner_multipleRequest);
}
