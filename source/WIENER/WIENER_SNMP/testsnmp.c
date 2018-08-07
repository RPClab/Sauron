#include "WIENER_SNMP.h"
#include "mocksnmp.h"

#include <testape.h>

#ifdef _MSC_VER
/* Disable warning 4013: 'snmp_sess_init$$tah' undefined; assuming extern returning int */
#pragma warning (disable: 4013)
#endif

void test_snmpOpen_setup(const char * const peerName, HSNMP session)
{
	snmpOpen_setup();
	snmpOpen_setExpectedPeerName(peerName);
	snmpOpen_setExpectedReadCommunity("public");
	snmpOpen_setExpectedTimeout(300000);
	snmpOpen_setExpectedRetries(2);
	snmpOpen_setSession(session);
	EXPECT(snmp_sess_init);
	EXPECT(snmp_sess_open);
}

void test_snmpOpen_succeeds(void)
{
	const char peerName[] = "192.168.16.123";
	test_snmpOpen_setup(peerName, (HSNMP)123456);
	ALLOW(snmp_log);

	VALIDATE(snmpOpen(peerName), snmpOpen_getSession());
}

void test_snmpOpen_returnsError(void)
{
	const char peerName[] = "1.2.3.4";
	test_snmpOpen_setup(peerName, (HSNMP)0);
	EXPECT(snmp_error);
	ALLOW(snmp_log);

	VALIDATE(snmpOpen(peerName), snmpOpen_getSession());
}

void test_snmpClose_doesNothingIfNotOpen(void)
{
	snmpClose((HSNMP)0);
}

void test_snmpClose_callsSnmpSessionClose(void)
{
	EXPECT(snmp_sess_close);
	ALLOW(snmp_log);

	snmpClose((HSNMP)123);
}

void testRunner_snmpOpen(void)
{
	EXECUTE(test_snmpOpen_succeeds);
	EXECUTE(test_snmpOpen_returnsError);
	EXECUTE(test_snmpClose_doesNothingIfNotOpen);
	EXECUTE(test_snmpClose_callsSnmpSessionClose);
}

// ----------------------------------------------------------------------------

HSNMP test_getRequest_setup(MockSnmpError error)
{
	snmpRequest_setErrorCondition(error);

	EXPECT(snmp_pdu_create);
	EXPECT(snmp_add_null_var);
	EXPECT(snmp_free_pdu);

	ALLOW(snmp_errstring);
	ALLOW(snmp_log);
	ALLOW(snmp_sess_session); // error logging
	ALLOW(snmp_sess_perror); // error logging

	return (HSNMP)1234;
}

enum {
	MainSwitchOff = 0,
	MainSwitchOn
};

void test_getRequest_success(void)
{
	HSNMP session = test_getRequest_setup(SnmpSuccess);

	VALIDATE(getMainSwitch(session), MainSwitchOff);
	VALIDATE(snmpGetLastErrorCode(), STAT_SUCCESS);
}

void test_getRequest_error(void)
{
	HSNMP session = test_getRequest_setup(SnmpError);

	VALIDATE(getMainSwitch(session), MainSwitchOff);
	VALIDATE(snmpGetLastErrorCode(), STAT_ERROR);
}

void test_getMultipleIntegerRequest_error(void)
{
	HSNMP session = test_getRequest_setup(SnmpError);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, 0, 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), STAT_ERROR);
}

void test_getMultipleDoubleRequest_error(void)
{
	HSNMP session = test_getRequest_setup(SnmpError);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, 0, 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), STAT_ERROR);
}

void test_getRequest_timeout(void)
{
	HSNMP session = test_getRequest_setup(SnmpTimeout);

	VALIDATE(getMainSwitch(session), MainSwitchOff);
	VALIDATE(snmpGetLastErrorCode(), SNMP_USER_ERR_TIMEOUT);
}

void test_getMultipleIntegerRequest_timeout(void)
{
	HSNMP session = test_getRequest_setup(SnmpTimeout);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, 0, 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_USER_ERR_TIMEOUT);
}

void test_getMultipleDoubleRequest_timeout(void)
{
	HSNMP session = test_getRequest_setup(SnmpTimeout);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, 0, 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_USER_ERR_TIMEOUT);
}

void test_getRequest_noSuchInstance(void)
{
	HSNMP session = test_getRequest_setup(SnmpNoSuchInstance);

	VALIDATE(getMainSwitch(session), MainSwitchOff);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleIntegerRequest_noSuchInstance(void)
{
	HSNMP session = test_getRequest_setup(SnmpNoSuchInstance);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, 0, 1);
	VALIDATE(result->size, 1);
	VALIDATE(result->errorCode[0], SNMP_NOSUCHINSTANCE);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleDoubleRequest_noSuchInstance(void)
{
	HSNMP session = test_getRequest_setup(SnmpNoSuchInstance);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, 0, 1);
	VALIDATE(result->size, 1);
	VALIDATE(result->errorCode[0], SNMP_NOSUCHINSTANCE);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleIntegerRequest_errorIndex(void)
{
	HSNMP session = test_getRequest_setup(SnmpErrorIndex);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, 0, 1);
	VALIDATE(result->size, 0); // TODO should size be increased if a request fails?
	VALIDATE(result->errorCode[0], SNMP_ERR_NOSUCHNAME);
}

void test_getMultipleDoubleRequest_errorIndex(void)
{
	HSNMP session = test_getRequest_setup(SnmpErrorIndex);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, 0, 1);
	VALIDATE(result->size, 0); // TODO should size be increased if a request fails?
	VALIDATE(result->errorCode[0], SNMP_ERR_NOSUCHNAME);
}

void test_getRequest_noSuchObject(void)
{
	HSNMP session = test_getRequest_setup(SnmpNoSuchObject);

	VALIDATE(getMainSwitch(session), MainSwitchOff);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHOBJECT);
}

void test_getRequest_endOfMibView(void)
{
	HSNMP session = test_getRequest_setup(SnmpEndOfMibView);

	VALIDATE(getMainSwitch(session), MainSwitchOff);
	VALIDATE(snmpGetLastErrorCode(), SNMP_ENDOFMIBVIEW);
}

void testRunner_errorHandling(void)
{
	EXECUTE(test_getRequest_success);
	EXECUTE(test_getRequest_error);
	EXECUTE(test_getMultipleIntegerRequest_error);
	EXECUTE(test_getMultipleDoubleRequest_error);
	EXECUTE(test_getRequest_timeout);
	EXECUTE(test_getMultipleIntegerRequest_timeout);
	EXECUTE(test_getMultipleDoubleRequest_timeout);
	EXECUTE(test_getRequest_noSuchInstance);
	EXECUTE(test_getMultipleIntegerRequest_noSuchInstance);
	EXECUTE(test_getMultipleDoubleRequest_noSuchInstance);
	EXECUTE(test_getMultipleIntegerRequest_errorIndex);
	EXECUTE(test_getMultipleDoubleRequest_errorIndex);
	EXECUTE(test_getRequest_noSuchObject);
	EXECUTE(test_getRequest_endOfMibView);
}

// ============================================================================

HSNMP test_getMultipleRequest_setup(MockSnmpError error)
{
	snmpRequest_setErrorCondition(error);

	return (HSNMP)1234;
}

void test_getMultipleIntegerRequest_firstIndexWrong(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, -1, 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleIntegerRequest_lastIndexWrong(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, MaxChannelsPerCrate - 1, 2);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleIntegerRequest_sizeToSmall(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, 0, 0);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleIntegerRequest_sizeToBig(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpIntegerBuffer *result = getMultipleOutputStatuses(session, 0, MaxChannelsPerSlot + 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleDoubleRequest_firstIndexWrong(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, -1, 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleDoubleRequest_lastIndexWrong(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, MaxChannelsPerCrate - 1, 2);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleDoubleRequest_sizeToSmall(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, 0, 0);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void test_getMultipleDoubleRequest_sizeToBig(void)
{
	HSNMP session = test_getMultipleRequest_setup(SnmpSuccess);

	SnmpDoubleBuffer *result = getMultipleOutputCurrents(session, 0, MaxChannelsPerSlot + 1);
	VALIDATE(result->size, 0);
	VALIDATE(snmpGetLastErrorCode(), SNMP_NOSUCHINSTANCE);
}

void testRunner_multipleRequest(void)
{
	EXECUTE(test_getMultipleIntegerRequest_firstIndexWrong);
	EXECUTE(test_getMultipleIntegerRequest_lastIndexWrong);
	EXECUTE(test_getMultipleIntegerRequest_sizeToSmall);
	EXECUTE(test_getMultipleIntegerRequest_sizeToBig);
	EXECUTE(test_getMultipleDoubleRequest_firstIndexWrong);
	EXECUTE(test_getMultipleDoubleRequest_lastIndexWrong);
	EXECUTE(test_getMultipleDoubleRequest_sizeToSmall);
	EXECUTE(test_getMultipleDoubleRequest_sizeToBig);
}
