#ifndef MOCKSNMP_H
#define MOCKSNMP_H

void snmpOpen_setup(void);
void snmpOpen_setExpectedPeerName(const char * const peername);
void snmpOpen_setExpectedReadCommunity(const char * const readCommunity);
void snmpOpen_setExpectedWriteCommunity(const char * const writeCommunity);
void snmpOpen_setExpectedTimeout(int microSeconds);
void snmpOpen_setExpectedRetries(int retries);
void snmpOpen_setSession(HSNMP session);
HSNMP snmpOpen_getSession(void);

typedef enum MockSnmpError {
	SnmpSuccess,
	SnmpError,
	SnmpTimeout,
	SnmpNoSuchInstance,
	SnmpErrorIndex,
	SnmpNoSuchObject,
	SnmpEndOfMibView
} MockSnmpError;

void snmpRequest_setErrorCondition(MockSnmpError error);

#endif // MOCKSNMP_H
