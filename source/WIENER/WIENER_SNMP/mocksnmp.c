#include "WIENER_SNMP.h"
#include "mocksnmp.h"

#include <testape.h>

typedef struct {
	char peerName[100];
	char readCommunity[100];
	char writeCommunity[100];
	int timeout;
	int retries;

	HSNMP session;

	MockSnmpError error;

} MockSnmp;
static MockSnmp m_mock;

void snmpOpen_setup(void)
{
	memset(&m_mock, 0, sizeof(m_mock));
}

void snmpOpen_setExpectedPeerName(const char * const peername)
{
	snmpStringCopy(m_mock.peerName, sizeof(m_mock.peerName), peername);
}

void snmpOpen_setExpectedReadCommunity(const char * const readCommunity)
{
	snmpStringCopy(m_mock.readCommunity, sizeof(m_mock.readCommunity), readCommunity);
}

void snmpOpen_setExpectedWriteCommunity(const char * const writeCommunity)
{
	snmpStringCopy(m_mock.writeCommunity, sizeof(m_mock.writeCommunity), writeCommunity);
}

void snmpOpen_setExpectedTimeout(int microSeconds)
{
	m_mock.timeout = microSeconds;
}

void snmpOpen_setExpectedRetries(int retries)
{
	m_mock.retries = retries;
}

void snmpOpen_setSession(HSNMP session)
{
	m_mock.session = session;
}

HSNMP snmpOpen_getSession()
{
	return m_mock.session;
}

void *snmp_sess_open(netsnmp_session *sess)
{
	VALIDATE(sess->version, SNMP_VERSION_2c);
	VALIDATE_STRING(sess->peername, m_mock.peerName);
	VALIDATE_STRING(sess->community, m_mock.readCommunity);
	VALIDATE(sess->community_len, strlen(m_mock.readCommunity));
	VALIDATE(sess->timeout, m_mock.timeout);
	VALIDATE(sess->retries, m_mock.retries);

	return m_mock.session;
}

// -------------------------------------------------

void snmpRequest_setErrorCondition(MockSnmpError error)
{
	m_mock.error = error;
}

int snmp_sess_synch_response(void *handle, netsnmp_pdu *request, netsnmp_pdu **response)
{
	netsnmp_pdu *alloc;
	netsnmp_variable_list *vars;

	UNREFERENCED_PARAMETER(handle);
	UNREFERENCED_PARAMETER(request);

	if (m_mock.error == SnmpError) {
		*response = NULL;
		return STAT_ERROR;
	}

	alloc = malloc(sizeof(netsnmp_pdu));
	memset(alloc, 0, sizeof(alloc));
	alloc->errstat = SNMP_ERR_NOERROR;
	alloc->errindex = 0;

	vars = malloc(sizeof(netsnmp_variable_list));
	memset(vars, 0, sizeof(vars));
	alloc->variables = vars;

	*response = alloc;

	if (m_mock.error == SnmpTimeout)
		return STAT_TIMEOUT;

	if (m_mock.error == SnmpErrorIndex) {
		alloc->errindex = 1;
		alloc->errstat = SNMP_ERR_NOSUCHNAME;
	} else if (m_mock.error == SnmpNoSuchInstance) {
		alloc->variables->type = SNMP_NOSUCHINSTANCE;
	} else if (m_mock.error == SnmpNoSuchObject) {
		alloc->variables->type = SNMP_NOSUCHOBJECT;
	} else if (m_mock.error == SnmpEndOfMibView) {
		alloc->variables->type = SNMP_ENDOFMIBVIEW;
	}

	return STAT_SUCCESS;
}
