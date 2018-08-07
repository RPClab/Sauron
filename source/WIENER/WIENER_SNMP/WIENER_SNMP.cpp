// WIENER SNMP basic SNMP library to Demonstrate C-Access to WIENER-Crates via SNMP
// modified for LabView import 04/23/06, Andreas Ruben
//
// The path to the Net-SNMP include files (default /usr/include) must be added to the
// include file search path!
// The following libraries must be included:
// netsnmp.lib ws2_32.lib
// The path to the Net-SNMP library must be added to the linker files.
// /usr/lib
// path for the WIENER MIB file (mibdirs) c:/usr/share/snmp/mibs

#include "WIENER_SNMP.h"
#include "version.h"

#ifdef _MSC_VER
#define strdup _strdup
#define vsnprintf vsprintf_s
#define strPrintf sprintf_s
#else
#define strPrintf snprintf
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (void)(P)
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

static const char WienerMibFileName[] = "WIENER-CRATE-MIB";
static const char DefaultReadCommunity[] = "public";  ///< default community name for read operations
static const char DefaultWriteCommunity[] = "guru";   ///< default community name for write operation

static char *m_readCommunity = (char *)DefaultReadCommunity;
static char *m_writeCommunity = (char *)DefaultWriteCommunity;

/**
 * @brief The SnmpObject class is used internally to resolve OIDs and for the SNMP calls.
 */
class SnmpObject {
public:
	oid    id[MAX_OID_LEN]; ///< The resolved SNMP OID
	size_t len;             ///< The resolved OIDs length in byte
	char   desc[100];       ///< The OIDs textual representation, e.g. "sysDescr.0"
};

static SnmpObject moduleIndex[MaxSlotsPerCrate];
static SnmpObject moduleDescription[MaxSlotsPerCrate];
static SnmpObject moduleSupply[MaxModuleAuxSupplies][MaxSlotsPerCrate];
static SnmpObject moduleHardwareLimitVoltage[MaxSlotsPerCrate];
static SnmpObject moduleHardwareLimitCurrent[MaxSlotsPerCrate];
static SnmpObject moduleRampSpeedVoltage[MaxSlotsPerCrate];
static SnmpObject moduleRampSpeedCurrent[MaxSlotsPerCrate];
static SnmpObject moduleStatus[MaxSlotsPerCrate];
static SnmpObject moduleEventStatus[MaxSlotsPerCrate];
static SnmpObject moduleDoClear[MaxSlotsPerCrate];
static SnmpObject moduleAuxiliaryMeasurementTemperature[MaxModuleAuxTemperatures][MaxSlotsPerCrate];

static SnmpObject sysDescr;
static SnmpObject sysMainSwitch;
static SnmpObject sysStatus;
static SnmpObject sysVmeSysReset;
static SnmpObject outputNumber;
static SnmpObject groupsNumber;
static SnmpObject highVoltageGroupsSwitch;
static SnmpObject lowVoltageGroupsSwitch;
static SnmpObject ipStaticAddress;

static SnmpObject outputName[MaxChannelsPerCrate];
static SnmpObject outputIndex[MaxChannelsPerCrate];
static SnmpObject outputGroup[MaxChannelsPerCrate];
static SnmpObject outputStatus[MaxChannelsPerCrate];
static SnmpObject outputMeasurementSenseVoltage[MaxChannelsPerCrate];
static SnmpObject outputMeasurementTerminalVoltage[MaxChannelsPerCrate];
static SnmpObject outputMeasurementCurrent[MaxChannelsPerCrate];
static SnmpObject outputMeasurementTemperature[MaxChannelsPerCrate];
static SnmpObject outputSwitch[MaxChannelsPerCrate];
static SnmpObject outputVoltage[MaxChannelsPerCrate];
static SnmpObject outputCurrent[MaxChannelsPerCrate];
static SnmpObject outputVoltageRiseRate[MaxChannelsPerCrate];
static SnmpObject outputVoltageFallRate[MaxChannelsPerCrate];
static SnmpObject outputCurrentRiseRate[MaxChannelsPerCrate];
static SnmpObject outputCurrentFallRate[MaxChannelsPerCrate];
static SnmpObject outputSupervisionBehavior[MaxChannelsPerCrate];
static SnmpObject outputSupervisionMinSenseVoltage[MaxChannelsPerCrate];
static SnmpObject outputSupervisionMaxSenseVoltage[MaxChannelsPerCrate];
static SnmpObject outputSupervisionMaxTerminalVoltage[MaxChannelsPerCrate];
static SnmpObject outputSupervisionMaxCurrent[MaxChannelsPerCrate];
static SnmpObject outputSupervisionMaxTemperature[MaxChannelsPerCrate];
static SnmpObject outputConfigMaxSenseVoltage[MaxChannelsPerCrate];
static SnmpObject outputConfigMaxTerminalVoltage[MaxChannelsPerCrate];
static SnmpObject outputConfigMaxCurrent[MaxChannelsPerCrate];
static SnmpObject outputSupervisionMaxPower[MaxChannelsPerCrate];
static SnmpObject outputTripTimeMaxCurrent[MaxChannelsPerCrate];

static SnmpObject sensorNumber;
static SnmpObject sensorTemperature[MaxSensors];
static SnmpObject sensorWarningThreshold[MaxSensors];
static SnmpObject sensorFailureThreshold[MaxSensors];

static SnmpObject psSerialNumber;
static SnmpObject psOperatingTime;
static SnmpObject psDirectAccess;
static SnmpObject fanNumberOfFans;
static SnmpObject fanOperatingTime;
static SnmpObject fanSerialNumber;
static SnmpObject fanAirTemperature;
static SnmpObject fanSwitchOffDelay;
static SnmpObject fanNominalSpeed;
static SnmpObject fanSpeed[MaxFans];

static SnmpObject psAuxVoltage[MaxPsAuxSupplies];
static SnmpObject psAuxCurrent[MaxPsAuxSupplies];

static SnmpObject snmpCommunityName[MaxCommunities];

static double snmpSetDouble(HSNMP session, const SnmpObject &object, double value);
static double snmpGetDouble(HSNMP session, const SnmpObject &object);
static int    snmpSetInt(HSNMP session, const SnmpObject &object, int value);
static int    snmpGetInt(HSNMP session, const SnmpObject &object);
static char  *snmpGetString(HSNMP session, const SnmpObject &object);

static char m_stringBuffer[1024] = { 0 };
static char m_snmpLastErrorString[1024] = { 0 };
static int  m_errorCode = 0;

static SnmpDoubleBuffer m_doubleBuffer;
static SnmpIntegerBuffer m_integerBuffer;

#ifdef _MSC_VER

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(ul_reason_for_call);
	UNREFERENCED_PARAMETER(lpReserved);

	return TRUE;
}

#endif

/**
 * @brief Platform independent secure string copy.
 *
 * Copies maximum \a len bytes to the buffer \a dest.
 * @since 1.2
 * @param dest buffer to hold the string copy
 * @param len sizeof(buffer)
 * @param source
 * @return
 */
int snmpStringCopy(char *dest, size_t len, const char *source)
{
#ifdef _MSC_VER
	strcpy_s(dest, len, source);
#else
	strncpy(dest, source, len);
	dest[len - 1] = 0;
#endif

	return 0;
}

/**
 * @brief Simple logging function with printf-like usage.
 * @internal
 * @param priority
 * @param format
 */
static void sysLog(int priority, const char *format, ...)
{
	UNREFERENCED_PARAMETER(priority);

	va_list vaPrintf;
	va_start(vaPrintf, format);
	vprintf(format, vaPrintf);
	putchar('\n');

	// store errors in snmpLastErrorBuffer, which can be read by snmpGetLastError()
	if (priority == LOG_ERR)
		vsnprintf(m_snmpLastErrorString, sizeof(m_snmpLastErrorString), format, vaPrintf);

	va_end(vaPrintf);
}

// Helper functions

/**
 * @brief Resolves the OID from the textual SNMP description
 * and stores the OID in object.
 * @internal
 * @param node The name to resolve, e.g. "sysMainSwitch"
 * @param object A pointer to SnmpObject to store the resolved OID
 * @return true on success, false otherwise
 */
static int getNode(const char * const node, SnmpObject *object)
{
	object->len = MAX_OID_LEN;
	if (!get_node(node, object->id, &object->len)) {
		snmp_log(LOG_ERR, "OID %s not found!\n", node);
		return false;
	}

	snmpStringCopy(object->desc, sizeof(object->desc), node);

	return true;
}

/**
 * @brief Resolves the OID from the textual SNMP description
 * with appended index and stores the OID in object.
 * @internal
 * @param nodeBase The node base name, e.g. "outputSwitch"
 * @param index The node index, e.g. 100 for "outputSwitch.100"
 * @param object A pointer to SnmpObject to store the resolved OID
 * @return true on success, false otherwise
 */
static int getIndexNode(const char * const nodeBase, int index, SnmpObject *object)
{
	char node[100];

	strPrintf(node, sizeof(node), "%s.%i", nodeBase, index);

	return getNode(node, object);
}

/**
 * @brief Activates logging on stderr console.
 * @since 1.1
 * @note This is the default setting.
 */
void snmpSetStdErrLog(void)
{
	snmp_enable_stderrlog();
}

/**
 * @brief Activates logging to the file filename.
 * @since 1.1
 * @param fileName The full path to the file where all log
 * information should go to.
 * @note If the specified file already exists,
 * new log information is appended.
 */
void snmpSetFileLog(const char * const fileName)
{
	snmp_enable_filelog(fileName, 1);
}

/**
 * @brief Returns the library four digit version number as unsigned int value.
 *
 * This allows to check for a specific version number.
 * @since 1.1
 * @return The version number as unsigned int value, e.g. 0x01010000
 */
unsigned int snmpGetVersion(void)
{
	const uint32_t version[] = { VER_FILEVERSION };

	return (version[0] << 24) + (version[1] << 16) + (version[2] << 8) + version[3];
}

/**
 * @brief Returns the library four digit version number as null-terminated string.
 *
 * The digits are separated by dots.
 * @since 1.1
 * @return The version number, e.g. "1.1.0.3"
 */
const char *snmpGetVersionString(void)
{
	return VER_FILEVERSION_STR;
}

/**
 * @brief SNMP Initialization.
 *
 * Resolves all needed OIDs from the MIB file and prepares the SNMP communication.
 * The actual connection to a MPOD crate is done with snmpOpen().
 * @return true on success, false otherwise (e.g. if an OID could not be resolved)
 */
int snmpInit(void)
{
	snmp_log(LOG_DEBUG, "*** Initialise SNMP ***\n");

	init_snmp("WIENER_SNMP_DLL");
	init_mib();								// init MIB processing
	if (!read_module(WienerMibFileName)) {		// read specific mibs
		snmp_log(LOG_ERR, "Unable to load SNMP MIB file \"%s\"\n", WienerMibFileName);
		return false;
	}
	snmp_log(LOG_DEBUG, "*** Translate OIDs ***\n");

	// Translate System OIDS
	getNode("sysDescr.0", &sysDescr); // FIXME: doesn't work atm in Linux

	if (
		(!getNode("sysMainSwitch.0",      &sysMainSwitch)) ||
		(!getNode("sysStatus.0",          &sysStatus)) ||
		(!getNode("sysVmeSysReset.0",     &sysVmeSysReset)) ||
		(!getNode("outputNumber.0",       &outputNumber)) ||
		(!getNode("groupsNumber.0",       &groupsNumber)) ||
		(!getNode("groupsSwitch.64",      &highVoltageGroupsSwitch)) ||
		(!getNode("groupsSwitch.128",     &lowVoltageGroupsSwitch)) ||
		(!getNode("ipStaticAddress.0",    &ipStaticAddress)) ||
//		(!getNode("psFirmwareVersion.0",  &psFirmwareVersion)) ||
		(!getNode("psSerialNumber.0",     &psSerialNumber)) ||
		(!getNode("psOperatingTime.0",    &psOperatingTime)) ||
		(!getNode("psDirectAccess.0",     &psDirectAccess)) ||
		(!getNode("sensorNumber.0",       &sensorNumber)) ||
//		(!getNode("fanFirmwareVersion.0", &fanFirmwareVersion)) ||
		(!getNode("fanSerialNumber.0",    &fanSerialNumber)) ||
		(!getNode("fanOperatingTime.0",   &fanOperatingTime)) ||
		(!getNode("fanAirTemperature.0",  &fanAirTemperature))||
		(!getNode("fanSwitchOffDelay.0",  &fanSwitchOffDelay)) ||
		(!getNode("fanNominalSpeed.0",    &fanNominalSpeed)) ||
		(!getNode("fanNumberOfFans.0",    &fanNumberOfFans))
	) {
		return false;
	}

	// Translate module and channel information OIDs
	for (int slot = 0; slot < MaxSlotsPerCrate; ++slot) {

		if (
			(!getIndexNode("moduleIndex", slot + 1, &moduleIndex[slot])) ||
			(!getIndexNode("moduleDescription", slot + 1, &moduleDescription[slot])) ||
			(!getIndexNode("moduleAuxiliaryMeasurementVoltage0", slot + 1, &moduleSupply[0][slot])) ||
			(!getIndexNode("moduleAuxiliaryMeasurementVoltage1", slot + 1, &moduleSupply[1][slot])) ||
			(!getIndexNode("moduleAuxiliaryMeasurementTemperature0", slot + 1, &moduleAuxiliaryMeasurementTemperature[0][slot])) ||
			(!getIndexNode("moduleAuxiliaryMeasurementTemperature1", slot + 1, &moduleAuxiliaryMeasurementTemperature[1][slot])) ||
			(!getIndexNode("moduleAuxiliaryMeasurementTemperature2", slot + 1, &moduleAuxiliaryMeasurementTemperature[2][slot])) ||
			(!getIndexNode("moduleAuxiliaryMeasurementTemperature3", slot + 1, &moduleAuxiliaryMeasurementTemperature[3][slot])) ||
			(!getIndexNode("moduleHardwareLimitVoltage", slot + 1, &moduleHardwareLimitVoltage[slot])) ||
			(!getIndexNode("moduleHardwareLimitCurrent", slot + 1, &moduleHardwareLimitCurrent[slot])) ||
			(!getIndexNode("moduleRampSpeedVoltage", slot + 1, &moduleRampSpeedVoltage[slot])) ||
			(!getIndexNode("moduleRampSpeedCurrent", slot + 1, &moduleRampSpeedCurrent[slot])) ||
			(!getIndexNode("moduleStatus", slot + 1, &moduleStatus[slot])) ||
			(!getIndexNode("moduleEventStatus", slot + 1, &moduleEventStatus[slot])) ||
			(!getIndexNode("moduleDoClear", slot + 1, &moduleDoClear[slot]))
		) {
			return false;
		}

		int base = MaxChannelsPerSlot * slot; // array index

		for (int channel = base; channel < base + MaxChannelsPerSlot; ++channel) {
			if (
				(!getIndexNode("outputName", channel + 1, &outputName[channel])) ||
				(!getIndexNode("outputIndex", channel + 1, &outputIndex[channel])) ||
				(!getIndexNode("outputGroup", channel + 1, &outputGroup[channel])) ||
				(!getIndexNode("outputStatus", channel + 1, &outputStatus[channel])) ||
				(!getIndexNode("outputMeasurementSenseVoltage", channel + 1, &outputMeasurementSenseVoltage[channel])) ||
				(!getIndexNode("outputMeasurementTerminalVoltage", channel + 1, &outputMeasurementTerminalVoltage[channel])) ||
				(!getIndexNode("outputMeasurementCurrent", channel + 1, &outputMeasurementCurrent[channel])) ||
				(!getIndexNode("outputMeasurementTemperature", channel + 1, &outputMeasurementTemperature[channel])) ||
				(!getIndexNode("outputSwitch", channel + 1, &outputSwitch[channel])) ||
				(!getIndexNode("outputVoltage", channel + 1, &outputVoltage[channel])) ||
				(!getIndexNode("outputCurrent", channel + 1, &outputCurrent[channel])) ||
				(!getIndexNode("outputVoltageRiseRate", channel + 1, &outputVoltageRiseRate[channel])) ||
				(!getIndexNode("outputVoltageFallRate", channel + 1, &outputVoltageFallRate[channel])) ||
				(!getIndexNode("outputCurrentRiseRate", channel + 1, &outputCurrentRiseRate[channel])) ||
				(!getIndexNode("outputCurrentFallRate", channel + 1, &outputCurrentFallRate[channel])) ||
				(!getIndexNode("outputSupervisionBehavior", channel + 1, &outputSupervisionBehavior[channel])) ||
				(!getIndexNode("outputSupervisionMinSenseVoltage", channel + 1, &outputSupervisionMinSenseVoltage[channel])) ||
				(!getIndexNode("outputSupervisionMaxSenseVoltage", channel + 1, &outputSupervisionMaxSenseVoltage[channel])) ||
				(!getIndexNode("outputSupervisionMaxTerminalVoltage", channel + 1, &outputSupervisionMaxTerminalVoltage[channel])) ||
				(!getIndexNode("outputSupervisionMaxCurrent", channel + 1, &outputSupervisionMaxCurrent[channel])) ||
//				(!getIndexNode("outputSupervisionMaxTemperature", channel + 1, &outputSupervisionMaxTemperature[channel])) ||
				(!getIndexNode("outputConfigMaxSenseVoltage", channel + 1, &outputConfigMaxSenseVoltage[channel])) ||
				(!getIndexNode("outputConfigMaxTerminalVoltage", channel + 1, &outputConfigMaxTerminalVoltage[channel])) ||
				(!getIndexNode("outputSupervisionMaxPower", channel + 1, &outputSupervisionMaxPower[channel])) ||
				(!getIndexNode("outputConfigMaxCurrent", channel + 1, &outputConfigMaxCurrent[channel])) ||
				(!getIndexNode("outputTripTimeMaxCurrent", channel + 1, &outputTripTimeMaxCurrent[channel]))
			) {
				return false;
			}
		}
	}

	for (int sensor = 0; sensor < MaxSensors; ++sensor)
		if (
			(!getIndexNode("sensorTemperature", sensor + 1, &sensorTemperature[sensor])) ||
			(!getIndexNode("sensorWarningThreshold", sensor + 1, &sensorWarningThreshold[sensor])) ||
			(!getIndexNode("sensorFailureThreshold", sensor + 1, &sensorFailureThreshold[sensor]))
		) {
			return false;
		}

	for (int name = 0; name < MaxCommunities; ++name)
		if (!getIndexNode("snmpCommunityName", name + 1, &snmpCommunityName[name]))
			return false;

	for (int fan = 0; fan < MaxFans; ++fan)
		if (!getIndexNode("fanSpeed", fan + 1, &fanSpeed[fan]))
			return false;

	for (int aux = 0; aux < MaxPsAuxSupplies; ++aux) {
		if (
			(!getIndexNode("psAuxiliaryMeasurementVoltage", aux + 1, &psAuxVoltage[aux])) ||
			(!getIndexNode("psAuxiliaryMeasurementCurrent", aux + 1, &psAuxCurrent[aux]))
		) {
			return false;
		}
	}

	snmp_log(LOG_DEBUG, "*** Initialise SNMP done ***\n");
	SOCK_STARTUP;											// only in main thread

	return true;
}

/**
 * @brief Additional cleanup. Should be called after snmpClose.
 */
void snmpCleanup(void)
{
	SOCK_CLEANUP;
}

/**
 * @brief Set a new read community name for SNMP access.
 *
 * The read community name has to match the configured read
 * community name in the MPOD crate.
 * The default read community name is "public".
 * @since 1.1
 * @note This function must be called before snmpOpen().
 * @param readCommunityName the new read community name
 */
void snmpSetReadCommunityName(const char * const readCommunityName)
{
	m_readCommunity = strdup(readCommunityName);
}

/**
 * @brief Set a new write community name for SNMP access.
 *
 * The write community name has to match the configured write
 * community name in the MPOD crate.
 * The default write community name is "guru".
 * @since 1.1
 * @note This function must be called before any write access function.
 * @param writeCommunityName the new write community name
 */
void snmpSetWriteCommunityName(const char * const writeCommunityName)
{
	m_writeCommunity = strdup(writeCommunityName);
}

/**
 * @brief Opens a SNMP session to the specified ipAddress.
 *
 * This function also sets the number of retries and the timeout value.
 * @param ipAddress A null-terminated ASCII string representation
 * of an IPv4 address, e.g. "192.168.17.101"
 * @return A handle to the opened SNMP session, which is a required
 * parameter for any further call.
 */
HSNMP snmpOpen(const char * const ipAddress)
{
	HSNMP session;
	struct snmp_session snmpSession;
	snmp_sess_init(&snmpSession);                  // structure defaults
	snmpSession.version = SNMP_VERSION_2c;
	snmpSession.peername = strdup(ipAddress);
	snmpSession.community = (u_char *)strdup(m_readCommunity);
	snmpSession.community_len = strlen(m_readCommunity);

	snmpSession.timeout = 300000;   // timeout (us)
	snmpSession.retries = 2;        // retries

	if (!(session = snmp_sess_open(&snmpSession))) {
		int liberr, syserr;
		char *errstr = 0;
		snmp_error(&snmpSession, &liberr, &syserr, &errstr);
		snmp_log(LOG_ERR, "Open SNMP session for host \"%s\": %s\n", ipAddress, errstr);
		free(errstr);
		return 0;
	}

	snmp_log(LOG_INFO, "SNMP session for host \"%s\" opened\n", ipAddress);
	return session;
}

/**
 * @brief Closes the previously opened SNMP session specified by session.
 * @param session The handle returned by snmpOpen()
 */
void snmpClose(HSNMP session)
{
	if (!session)
		return;

	if (!snmp_sess_close(session))
		snmp_log(LOG_ERR, "Close SNMP session: ERROR\n");
	else
		snmp_log(LOG_INFO, "SNMP session closed\n");
}

/**
 * @brief Returns a pointer to a descriptive string for the last
 * failed SNMP operation.
 * @return A pointer to a null-terminated error string for the last
 * failed SNMP operation.
 * @note This pointer is valid until the next string operation.
 */
char *snmpGetLastError(void)
{
	return m_snmpLastErrorString;
}

/**
 * @brief Returns the SNMP error code for the last SNMP operation.
 *
 * If the last snmp function call succeeded, this function
 * returns SNMP_ERR_NOERROR.
 * @note The additional define SNMP_USER_ERR_TIMEOUT
 * is added to the net-snmp defines.
 * @since 1.2
 * @return The SNMP error code for the last SNMP operation.
 */
int snmpGetLastErrorCode(void)
{
	return m_errorCode;
}

// System Information Functions

/**
 * @brief Returns a pointer to the MPOD controller description string.
 *
 * The pointer is valid until the next call of any string function.
 * @param session The handle returned by snmpOpen()
 * @return the MPOD controller description string, containing the
 * controller serial number and firmware releases, e.g.:
 * "WIENER MPOD (4388090, MPOD 2.1.2098.1, MPODslave 1.09, MPOD-BL 1.50 )"
 * @note This pointer is valid until the next string operation.
 */
char *getSysDescr(HSNMP session)
{
	return snmpGetString(session, sysDescr);
}

/**
 * @brief Returns the crate power on/off status.
 *
 * The result is the logical "and" between the hardware main switch
 * and the setMainSwitch function.
 * @param session The handle returned by snmpOpen()
 * @return The current on/off status of the crate:
 * - crate is powered off (0)
 * - crate is powered on (1)
 */
int getMainSwitch(HSNMP session)
{
	return snmpGetInt(session, sysMainSwitch);
}

/**
 * @brief Sets the crate main switch to on or off.
 *
 * If the hardware main switch is set to the "0" position,
 * this function always returns 0.
 * @param session The handle returned by snmpOpen()
 * @param value 0 = set off, 1 = set on
 * @return The new on/off status of the crate.
 */
int setMainSwitch(HSNMP session, int value)
{
	return snmpSetInt(session, sysMainSwitch, value);
}

/**
 * @brief Returns a bit field with the status of the complete crate.
 * @param session The handle returned by snmpOpen()
 * @return The complete crate status.
 */
int getMainStatus(HSNMP session)
{
	return snmpGetInt(session, sysStatus);
}

/**
 * @brief Returns the VME system reset status.
 * @param session The handle returned by snmpOpen()
 * @return
 */
int getVmeReset(HSNMP session)
{
	return snmpGetInt(session, sysVmeSysReset);
}

/**
 * @brief Initiate a VME system reset.
 * @param session The handle returned by snmpOpen()
 * @return
 */
int setVmeReset(HSNMP session)
{
	return snmpSetInt(session, sysVmeSysReset, 1);
}

/**
 * @brief Returns the crates static IP address as 32 bit integer.
 * @param session The handle returned by snmpOpen()
 * @return The static IP address.
 */
int getIpStaticAddress(HSNMP session)
{
	return snmpGetInt(session, ipStaticAddress);
}

/**
 * @brief Sets a new static IP address.
 * @param session The handle returned by snmpOpen()
 * @param value The IP address as 32 bit integer
 * @return
 */
int setIpStaticAddress(HSNMP session, int value)
{
	return snmpSetInt(session, ipStaticAddress, value);
}

/**
 * @brief Returns a pointer to a string containing the MPOD controllers
 * serial number.
 *
 * @param session The handle returned by snmpOpen()
 * @return The crates serial number, e.g. "4388090".
 * @note The pointer is valid until the next call of any string function.
 */
char *getPsSerialNumber(HSNMP session)
{
	return snmpGetString(session, psSerialNumber);
}

// System Count Functions

/**
 * @brief Returns the total number of output channels in the crate.
 * @param session The handle returned by snmpOpen()
 * @return The total number of output channels
 */
int getOutputNumber(HSNMP session)
{
	return snmpGetInt(session, outputNumber);
}

/**
 * @brief getOutputGroups
 * @param session The handle returned by snmpOpen()
 * @return
 */
int getOutputGroups(HSNMP session)
{
	return snmpGetInt(session, groupsNumber);
}

// Output Channel Information

/**
 * @brief getOutputGroup
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
int getOutputGroup(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, outputGroup[channel]);
}

/**
 * @brief Returns the channel outputStatus register.
 * @note This function is deprecated. Use getOutputStatus() instead.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The channels outputStatus register
 */
int getChannelStatus(HSNMP session, int channel)
{
	return getOutputStatus(session, channel);
}

/**
 * @brief Returns the channel outputStatus register.
 * @since 1.1
 * @note For iseg HV modules, the outputStatus register combines
 * bits from the modules Channel Status and Channel Event Status register.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The channels outputStatus register
 */
int getOutputStatus(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, outputStatus[channel]);
}

/**
 * @brief Returns the measured output sense voltage in Volt.
 * @note This function is only valid for WIENER LV modules.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The channels measured sense voltage int Volt.
 */
double getOutputSenseMeasurement(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputMeasurementSenseVoltage[channel]);
}

/**
 * @brief Returns the measured output terminal voltage for channel in Volt.
 * @note For iseg HV modules, this items corresponds to the
 * Voltage Measurement (Vmeas) register.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The measured output terminal voltage in Volt.
 */
double getOutputTerminalMeasurement(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputMeasurementTerminalVoltage[channel]);
}

/**
 * @brief Returns the measured output current for channel in Ampere.
 * @note For iseg HV modules, this items corresponds to the
 * Current Measurement (Imeas) register.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The measured output measurement current in Ampere.
 */
double getCurrentMeasurement(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputMeasurementCurrent[channel]);
}

/**
 * @brief Returns the measured temperature for channel in degree Celsius.
 * @note Only WIENER Low Voltage modules have a channel-wise temperature
 * measurement. For iseg HV modules, use getModuleAuxTemperature().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The measured output temperature in Degree Celsius.
 */
int getTemperatureMeasurement(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, outputMeasurementTemperature[channel]);
}

/**
 * @brief Change the state of the channel.
 * @note This function is deprecated. Use setOutputSwitch() instead.
 * @param session The handle returned by snmpOpen()
 * @param channel The channel in the range of (0...::MaxChannelsPerCrate - 1)
 * @param value
 * - set channel off (0)
 * - set channel on (1)
 * - reset channel emergency off (2)
 * - set channel emergency off (3)
 * - clear channel events (10)
 * @return
 */
int setChannelSwitch(HSNMP session, int channel, int value)
{
	return setOutputSwitch(session, channel, value);
}

/**
 * @brief Change the state of the channel.
 * @since 1.1
 * @note For iseg HV modules, this item corresponds to the
 * Channel Control register.
 * @param session The handle returned by snmpOpen()
 * @param channel The channel in the range of (0...::MaxChannelsPerCrate - 1)
 * @param value
 * - set channel off (0)
 * - set channel on (1)
 * - reset channel emergency off (2)
 * - set channel emergency off (3)
 * - clear channel events (10)
 * @return
 */
int setOutputSwitch(HSNMP session, int channel, int value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetInt(session, outputSwitch[channel], value);
}

/**
 * @brief Returns the state of the channel.
 * @note This function is deprecated. Use getOutputSwitch() instead.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 * - set channel off (0)
 * - set channel on (1)
 * - reset channel emergency off (2)
 * - set channel emergency off (3)
 * - clear channel events (10)
 */
int getChannelSwitch(HSNMP session, int channel)
{
	return getOutputSwitch(session, channel);
}

/**
 * @brief Returns the state of the channel.
 * @since 1.1
 * @note For iseg HV modules, this item corresponds to the
 * Channel Control register.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 * - set channel off (0)
 * - set channel on (1)
 * - reset channel emergency off (2)
 * - set channel emergency off (3)
 * - clear channel events (10)
 */
int getOutputSwitch(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, outputSwitch[channel]);
}

/**
 * @brief setHighVoltageGroupsSwitch
 * @param session The handle returned by snmpOpen()
 * @param value
 * @return
 */
int setHighVoltageGroupsSwitch(HSNMP session, int value)
{
	return snmpSetInt(session, highVoltageGroupsSwitch, value);
}

/**
 * @brief getHighVoltageGroupsSwitch
 * @param session The handle returned by snmpOpen()
 * @return
 */
int getHighVoltageGroupsSwitch(HSNMP session)
{
	return snmpGetInt(session, highVoltageGroupsSwitch);
}

/**
 * @brief setLowVoltageGroupsSwitch
 * @param session The handle returned by snmpOpen()
 * @param value
 * @return
 */
int setLowVoltageGroupsSwitch(HSNMP session, int value)
{
	return snmpSetInt(session, lowVoltageGroupsSwitch, value);
}

/**
 * @brief getLowVoltageGroupsSwitch
 * @param session The handle returned by snmpOpen()
 * @return
 */
int getLowVoltageGroupsSwitch(HSNMP session)
{
	return snmpGetInt(session, lowVoltageGroupsSwitch);
}

/**
 * @brief Returns the demanded output voltage for channel.
 * @note For iseg HV modules, this item corresponds to the
 * Voltage Set (Vset) register.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The demanded output voltage in Volt.
 */
double getOutputVoltage(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputVoltage[channel]);
}

/**
 * @brief Sets the demanded output voltage for channel.
 * @note For iseg HV modules, this item corresponds to the
 * Voltage Set (Vset) register.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @param value the demanded output voltage in Volt.
 * @return The demanded output voltage in Volt.
 */
double setOutputVoltage(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputVoltage[channel], value);
}

/**
 * @brief Returns the demanded maximum output current for channel.
 * @note For iseg HV modules, this item corresponds to the
 * Current Set (Iset) register.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The demanded output current in Ampere.
 */
double getOutputCurrent(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputCurrent[channel]);
}

/**
 * @brief Sets the demanded maximum output current for channel.
 * @note For iseg HV modules, this item corresponds to the
 * Current Set (Iset) register.
 * @param session The handle returned by snmpOpen()
 * @param channel The channel in the range of (0...::MaxChannelsPerCrate - 1)
 * @param value The demanded ouput current in Ampere
 * @return The demanded maximum output current in Ampere.
 */
double setOutputCurrent(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputCurrent[channel], value);
}

/**
 * @brief Returns the channel voltage rise rate in Volt/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use getModuleRampSpeedVoltage().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputRiseRate(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputVoltageRiseRate[channel]);
}

/**
 * @brief Sets the channel voltage rise rate in Volt/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use setModuleRampSpeedVoltage().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @param value
 * @return
 */
double setOutputRiseRate(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputVoltageRiseRate[channel], value);
}

/**
 * @brief Returns the channel voltage fall rate in Volt/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use getModuleRampSpeedVoltage().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputFallRate(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputVoltageFallRate[channel]);
}

/**
 * @brief Sets the channel voltage fall rate in Volt/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use setModuleRampSpeedVoltage().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @param value
 * @return
 */
double setOutputFallRate(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputVoltageFallRate[channel], value);
}

/**
 * @brief Returns the channel current rise rate in Ampere/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use getModuleRampSpeedCurrent().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputCurrentRiseRate(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputCurrentRiseRate[channel]);
}

/**
 * @brief Sets the channel current rise rate in Ampere/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use setModuleRampSpeedCurrent().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @param value
 * @return
 */
double setOutputCurrentRiseRate(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputCurrentRiseRate[channel], value);
}

/**
 * @brief Returns the channel current fall rate in Ampere/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use getModuleRampSpeedCurrent().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputCurrentFallRate(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputCurrentFallRate[channel]);
}

/**
 * @brief Sets the channel current fall rate in Ampere/second.
 *
 * @note This function is for WIENER LV only.
 * For iseg HV modules, use setModuleRampSpeedCurrent().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @param value
 * @return
 */
double setOutputCurrentFallRate(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputCurrentFallRate[channel], value);
}

/**
 * @brief Returns a bit field packed into an integer
 * which define the behavior of the output channel or power supply after failures.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
int getOutputSupervisionBehavior(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, outputSupervisionBehavior[channel]);
}

/**
 * @brief Set the behavior of the output channel or power supply after failures.
 *
 * For each supervision value, a two-bit field exists.
 * The enumeration of this value (..L+..H*2) is:
 * - WIENER LV devices
 *   - 0           ignore the failure
 *   - 1           switch off this channel
 *   - 2           switch off all channels with the same group number
 *   - 3           switch off the complete crate.
 * - iseg HV devices
 *   - 0           ignore the failure
 *   - 1           switch off this channel by ramp down the voltage
 *   - 2           switch off this channel by a emergencyOff
 *   - 3           switch off the whole board of the HV module by emergencyOff.
 *
 * The position of the bit fields in the integer value are:
 * - Bit 0, 1:   outputFailureMinSenseVoltage
 * - Bit 2, 3:   outputFailureMaxSenseVoltage
 * - Bit 4, 5:   outputFailureMaxTerminalVoltage
 * - Bit 6, 7:   outputFailureMaxCurrent
 * - Bit 8, 9:   outputFailureMaxTemperature
 * - Bit 10, 11: outputFailureMaxPower
 * - Bit 12, 13: outputFailureInhibit
 * - Bit 14, 15: outputFailureTimeout
 * @param session The handle returned by snmpOpen()
 * @param channel The channel (0...::MaxChannelsPerCrate - 1) for which the behaviour should be set
 * @param value The 16 bit integer with bits set according the preceding table.
 * @return
 */
int setOutputSupervisionBehavior(HSNMP session, int channel, int value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetInt(session, outputSupervisionBehavior[channel], value );
}

/**
 * @brief getOutputSupervisionMinSenseVoltage
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputSupervisionMinSenseVoltage(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputSupervisionMinSenseVoltage[channel]);
}

/**
 * @brief setOutputSupervisionMinSenseVoltage
 * @param session The handle returned by snmpOpen()
 * @param channel
 * @param value
 * @return
 */
double setOutputSupervisionMinSenseVoltage(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputSupervisionMinSenseVoltage[channel], value);
}

/**
 * @brief getOutputSupervisionMaxSenseVoltage
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputSupervisionMaxSenseVoltage(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputSupervisionMaxSenseVoltage[channel]);
}

/**
 * @brief setOutputSupervisionMaxSenseVoltage
 * @param session The handle returned by snmpOpen()
 * @param channel
 * @param value
 * @return
 */
double setOutputSupervisionMaxSenseVoltage(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputSupervisionMaxSenseVoltage[channel], value);
}

/**
 * @brief If the measured voltage at the power supply output terminals is above this value,
 * the power supply performs the function defined by setOutputSupervisionBehavior().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The maximum terminal voltage in Volt
 */
double getOutputSupervisionMaxTerminalVoltage(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputSupervisionMaxTerminalVoltage[channel]);
}

/**
 * @brief If the measured voltage at the power supply output terminals is above this value,
 * the power supply performs the function defined by setOutputSupervisionBehavior().
 * @param session The handle returned by snmpOpen()
 * @param channel the channel (0...::MaxChannelsPerCrate - 1) to set the max. terminal voltage
 * @param value The maximum terminal voltage in Volt
 * @return
 */
double setOutputSupervisionMaxTerminalVoltage(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputSupervisionMaxTerminalVoltage[channel], value);
}

/**
 * @brief If the measured current is above this value, the power supply
 * performs the function defined by setOutputSupervisionBehavior().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The maximum output current in Ampere
 */
double getOutputSupervisionMaxCurrent(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputSupervisionMaxCurrent[channel]);
}

/**
 * @brief If the measured current is above this value, the power supply
 * performs the function defined by setOutputSupervisionBehavior().
 * @param session The handle returned by snmpOpen()
 * @param channel The channel (0...::MaxChannelsPerCrate - 1) to set the max. current
 * @param value The maximum current in Ampere
 * @return
 */
double setOutputSupervisionMaxCurrent(HSNMP session, int channel, double value)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, outputSupervisionMaxCurrent[channel], value );
}

/**
 * @brief If the measured module temperature is above this value, the power
 * supply performs the function defined by setOutputSupervisionBehavior().
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return The maximum temperature in degree Celsius
 */
int getOutputSupervisionMaxTemperature(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, outputSupervisionMaxTemperature[channel]);
}

/**
 * @brief Returns the maximum sense voltage for channel.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputConfigMaxSenseVoltage(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputConfigMaxSenseVoltage[channel]);
}

/**
 * @brief Returns the maximum terminal voltage for channel.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputConfigMaxTerminalVoltage(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputConfigMaxTerminalVoltage[channel]);
}

/**
 * @brief Returns the maximum output current for channel.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputConfigMaxCurrent(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputConfigMaxCurrent[channel]);
}

/**
 * @brief getOutputSupervisionMaxPower
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return
 */
double getOutputSupervisionMaxPower(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, outputSupervisionMaxPower[channel]);
}

/**
 * @brief Returns the time span for the delayed trip function.
 * @param session The handle returned by snmpOpen()
 * @param channel The requested channel (0...::MaxChannelsPerCrate - 1)
 * @return the trip delay time (0...4000 ms)
 */
int getOutputTripTimeMaxCurrent(HSNMP session, int channel)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, outputTripTimeMaxCurrent[channel]);
}

/**
 * @brief Defines a time span for the delayed trip function.
 * @param session The handle returned by snmpOpen()
 * @param channel The channel (0...::MaxChannelsPerCrate - 1) for which to set the delayed trip
 * @param delay The trip delay time (0...4000 ms)
 * @return
 */
int setOutputTripTimeMaxCurrent(HSNMP session, int channel, int delay)
{
	if (channel < 0 || channel >= MaxChannelsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetInt(session, outputTripTimeMaxCurrent[channel], delay);
}

// Sensor Information functions

int getSensorNumber(HSNMP session)
{
	return snmpGetInt(session, sensorNumber);
}

int getSensorTemp(HSNMP session, int sensor)
{
	if (sensor < 0 || sensor > MaxSensors) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, sensorTemperature[sensor]);
}

int getSensorWarningTemperature(HSNMP session, int sensor)
{
	if (sensor < 0 || sensor > MaxSensors) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, sensorWarningThreshold[sensor]);
}

int setSensorWarningTemperature(HSNMP session, int sensor, int value)
{
	if (sensor < 0 || sensor > MaxSensors) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetInt(session, sensorWarningThreshold[sensor], value);
}

int getSensorFailureTemperature(HSNMP session, int sensor)
{
	if (sensor < 0 || sensor > MaxSensors) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, sensorFailureThreshold[sensor]);
}

int setSensorFailureTemperature(HSNMP session, int sensor, int value)
{
	if (sensor < 0 || sensor > MaxSensors) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetInt(session, sensorFailureThreshold[sensor], value);
}

// Power Supply specific Functions.

/**
 * @brief Returns the crates operating time in seconds.
 * @param session The handle returned by snmpOpen()
 * @return The crates operating time in seconds.
 */
int getPsOperatingTime(HSNMP session)
{
	return snmpGetInt(session, psOperatingTime);
}

double getPsAuxVoltage(HSNMP session, int auxIndex)
{
	if ( (auxIndex < 0) || (auxIndex >= MaxPsAuxSupplies) ) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, psAuxVoltage[auxIndex]);
}

double getPsAuxCurrent(HSNMP session, int auxIndex)
{
	if ( (auxIndex < 0) || (auxIndex >= MaxPsAuxSupplies) ) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, psAuxCurrent[auxIndex]);
}

// Fan Tray Functions

int getFanOperatingTime(HSNMP session)
{
	return snmpGetInt(session, fanOperatingTime);
}

int getFanAirTemperature(HSNMP session)
{
	return snmpGetInt(session, fanAirTemperature);
}

int getFanSwitchOffDelay(HSNMP session)
{
	return snmpGetInt(session, fanSwitchOffDelay);
}

int setFanSwitchOffDelay(HSNMP session, int value)
{
	return snmpSetInt(session, fanSwitchOffDelay, value);
}

/**
 * @brief Returns the crates fan rotation speed in revolutions per minute.
 * @param session The handle returned by snmpOpen()
 * @return The crates fan rotation speed in revolutions per minute.
 */
int getFanNominalSpeed(HSNMP session)
{
	return snmpGetInt(session, fanNominalSpeed);
}

/**
 * @brief Sets the crates fan rotation speed in revolutions per minute.
 * @param session The handle returned by snmpOpen()
 * @param value The desired revolution in the range of 1200..3600.
 * 0 is also allowed and turns off the crates fans.
 * @return
 */
int setFanNominalSpeed(HSNMP session, int value)
{
	return snmpSetInt(session, fanNominalSpeed, value );
}

int getFanNumberOfFans(HSNMP session)
{
	return snmpGetInt(session, fanNumberOfFans);
}

int getFanSpeed(HSNMP session, int fan)
{
	if (fan < 0 || fan > MaxFans) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, fanSpeed[fan]);
}

/**
 * @brief Returns a pointer to the module description string.
 *
 * The pointer is valid until the next call of any string function.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return A string with the following contents, separated by comma and space:
 * - The module vendor ("iseg" or "WIENER")
 * - The module type name
 * - The channel number
 * - The module serial number (optional)
 * - The module firmware release (optional)
 *
 * Example: "iseg, E24D1, 24, 715070, 5.14"
 */
char *getModuleDescription(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetString(session, moduleDescription[slot]);
}

/**
 * @brief Returns the measured value of the modules +24 Volt line.
 * @note This function is for iseg HV modules only.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The measured +24 Volt line voltage in Volt.
 */
double getModuleSupply24(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, moduleSupply[0][slot]);
}

/**
 * @brief Returns the measured value of the modules +5 Volt line.
 * @note This function is for iseg HV modules only.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The measured +5 Volt line voltage in Volt.
 */
double getModuleSupply5(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, moduleSupply[1][slot]);
}

/**
 * @brief Returns the measured value of one of the modules temperature sensors.
 *
 * @note This function is for iseg HV modules only.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @param index The temperature sensor index (0...3)
 * @note Most modules only have one temperature sensor at index 0.
 * @return
 */
double getModuleAuxTemperature(HSNMP session, int slot, int index)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	if (index < 0 || index >= MaxModuleAuxTemperatures) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, moduleAuxiliaryMeasurementTemperature[index][slot]);
}

/**
 * @brief Returns the modules hardware voltage limit in percent.
 *
 * @note This function is for iseg HV modules only.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The modules hardware voltage limit in percent (2...102)
 */
double getModuleHardwareLimitVoltage(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, moduleHardwareLimitVoltage[slot]);
}

/**
 * @brief Returns the modules hardware current limit in percent.
 *
 * @note This function is for iseg HV modules only.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The modules hardware current limit in percent (2...102)
 */
double getModuleHardwareLimitCurrent(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, moduleHardwareLimitCurrent[slot]);
}

/**
 * @brief Returns the modules voltage ramp speed in percent.
 *
 * @note This function is for iseg HV modules only.
 *
 * iseg modules have one common ramp speed for all channels.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The modules voltage ramp speed in percent
 */
double getModuleRampSpeedVoltage(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, moduleRampSpeedVoltage[slot]);
}

/**
 * @brief Sets the modules voltage ramp speed in percent.
 *
 * @note This function is for iseg HV modules only.
 *
 * iseg modules have one common ramp speed for all channels.
 * @param session The handle returned by snmpOpen()
 * @param slot the modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @param value The new voltage ramp speed in percent
 * @note For most modules, the range is 0.001...20 percent.
 * @return The new voltage ramp speed in percent
 */
double setModuleRampSpeedVoltage(HSNMP session, int slot, double value)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, moduleRampSpeedVoltage[slot], value);
}

/**
 * @brief Returns the modules current ramp speed in percent.
 *
 * @note This function is for iseg HV modules with constant
 * current regulation only.
 *
 * iseg modules have one common ramp speed for all channels.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The modules current ramp speed in percent
 */
double getModuleRampSpeedCurrent(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetDouble(session, moduleRampSpeedCurrent[slot]);
}

/**
 * @brief Sets the modules current ramp speed in percent.
 *
 * @note This function is for iseg HV modules with constant
 * current regulation only.
 *
 * iseg modules have one common ramp speed for all channels.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @param value The new current ramp speed in percent
 * @return The new current ramp speed in percent
 */
double setModuleRampSpeedCurrent(HSNMP session, int slot, double value)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetDouble(session, moduleRampSpeedCurrent[slot], value);
}

/**
 * @brief Returns the value of the module status register.
 *
 * @note This function is for iseg HV modules only.
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The module status register
 */
int getModuleStatus(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, moduleStatus[slot]);
}

/**
 * @brief Returns the value of the module event status register.
 *
 * @note This function is for iseg HV modules only.
 * @param session The handle returned by snmpOpen()
 * @param slot the modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return The module event status register
 */
int getModuleEventStatus(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpGetInt(session, moduleEventStatus[slot]);
}

/**
 * @brief Clears all modules events in a specific slot.
 *
 * To clear all events in all iseg HV modules,
 * use setHighVoltageGroupsSwitch() with the parameter
 * clearEvents(10).
 * @param session The handle returned by snmpOpen()
 * @param slot The modules slot position in the crate (0...::MaxSlotsPerCrate - 1)
 * @return
 */
int setModuleDoClear(HSNMP session, int slot)
{
	if (slot < 0 || slot >= MaxSlotsPerCrate) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		return 0;
	}

	return snmpSetInt(session, moduleDoClear[slot], 1);
}

// The rest of the functions are utility functions that actually do the SNMP calls

static void logErrors(HSNMP session, struct snmp_pdu *response,
							 const SnmpObject &object, int status, const char *functionName)
{
	// FAILURE: print what went wrong!
	if (status == STAT_SUCCESS)
		snmp_log(LOG_ERR, "%s(%s): Error in packet. Reason: %s\n",
				functionName, object.desc, snmp_errstring(response->errstat));
	else
		snmp_sess_perror("snmpget", snmp_sess_session(session));
}

static int getIntegerVariable(struct variable_list *vars, int *errorCode = 0)
{
	if (errorCode)
		*errorCode = SNMP_ERR_NOERROR;

	if (vars->type == ASN_BIT_STR || vars->type == ASN_OCTET_STR) {
		int value = 0;
		for (size_t i = 0; i < vars->val_len && i < sizeof(int); ++i)
			value |= (vars->val.bitstring[i] << (i * 8));
		return value;
	} if (vars->type == ASN_OPAQUE_FLOAT)
		return (int)*vars->val.floatVal;
	else if (vars->type == ASN_OPAQUE_DOUBLE)
		return (int)*vars->val.doubleVal;
	else if (vars->type == ASN_INTEGER)
		return *vars->val.integer;
	else if (vars->type == ASN_OCTET_STR)
		return *vars->val.integer;
	else if (vars->type == ASN_IPADDRESS)
		return *vars->val.integer;

	if (errorCode)
		*errorCode = vars->type;
	return 0;
}

static double getDoubleVariable(struct variable_list *vars, int *errorCode = 0)
{
	if (errorCode)
		*errorCode = SNMP_ERR_NOERROR;

	if (vars->type == ASN_OPAQUE_FLOAT)
		return *vars->val.floatVal;
	else if (vars->type == ASN_OPAQUE_DOUBLE)
		return *vars->val.doubleVal;
	else if (vars->type == ASN_INTEGER)
		return (double)*vars->val.integer;

	if (errorCode)
		*errorCode = vars->type;
	return 0.0;
}

static snmp_pdu *prepareSetRequestPdu(void)
{
	struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_SET);
	pdu->community = (u_char *)strdup(m_writeCommunity);
	pdu->community_len = strlen(m_writeCommunity);

	return pdu;
}

static snmp_pdu *prepareGetRequestPdu(const SnmpObject *objects, int size)
{
	struct snmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_GET);

	for (int i = 0; i < size; ++i)
		snmp_add_null_var(pdu, objects[i].id, objects[i].len); // generate request data

	return pdu;
}

static void updateSnmpErrorCode(int status, struct snmp_pdu *response)
{
	if ( (status == STAT_ERROR) || (response == NULL) ) {
		m_errorCode = STAT_ERROR;
	} else if (status == STAT_TIMEOUT) {
		m_errorCode = SNMP_USER_ERR_TIMEOUT;
	} else if (response->errstat) {
		m_errorCode = response->errstat;
	} else if (
		response->variables->type == SNMP_NOSUCHINSTANCE ||
		response->variables->type == SNMP_NOSUCHOBJECT ||
		response->variables->type == SNMP_ENDOFMIBVIEW
	) {
		m_errorCode = response->variables->type;
	} else {
		m_errorCode = SNMP_ERR_NOERROR;
	}
}

static int snmpGetInt(HSNMP session, const SnmpObject &object)
{
	int value = 0;

	struct snmp_pdu *pdu = prepareGetRequestPdu(&object, 1);

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		value = getIntegerVariable(response->variables);
	} else {
		logErrors(session, response, object, status, "snmpGetInt");
	}

	snmp_free_pdu(response);
	return value;
}

static int snmpSetInt(HSNMP session, const SnmpObject &object, int value)
{
	struct snmp_pdu *pdu = prepareSetRequestPdu();

	if (snmp_oid_compare(object.id, object.len, ipStaticAddress.id, ipStaticAddress.len) == 0)
		snmp_pdu_add_variable(pdu, object.id, object.len, ASN_IPADDRESS, (u_char *)&value, sizeof(value));
	else
		snmp_pdu_add_variable(pdu, object.id, object.len, ASN_INTEGER, (u_char *)&value, sizeof(value));

	int result = 0;
	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		result = getIntegerVariable(response->variables);
	} else {
		logErrors(session, response, object, status, "snmpSetInt");
	}

	snmp_free_pdu(response);
	return result;
}

static double snmpGetDouble(HSNMP session, const SnmpObject &object)
{
	double value = 0.0;

	struct snmp_pdu *pdu = prepareGetRequestPdu(&object, 1);

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		value = getDoubleVariable(response->variables);
	} else {
		logErrors(session, response, object, status, "snmpGetDouble");
	}

	snmp_free_pdu(response);
	return value;
}

static double snmpSetDouble(HSNMP session, const SnmpObject &object, double value)
{
	struct snmp_pdu *pdu = prepareSetRequestPdu();

	float v = (float)value;
	snmp_pdu_add_variable(pdu, object.id, object.len, ASN_OPAQUE_FLOAT, (u_char *)&v, sizeof(v));

	double result = 0.0;

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		result = getDoubleVariable(response->variables);
	} else {
		logErrors(session, response, object, status, "snmpSetDouble");
	}

	snmp_free_pdu(response);
	return result;
}

static char *snmpGetString(HSNMP session, const SnmpObject &object)
{
	struct snmp_pdu *pdu = prepareGetRequestPdu(&object, 1);

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	memset(m_stringBuffer, 0, sizeof(m_stringBuffer));

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		struct variable_list *vars = response->variables;
		if (vars->type == ASN_OCTET_STR) {
			size_t len = sizeof(m_stringBuffer) - 1;
			if (len > vars->val_len)
				len = vars->val_len;

			memcpy(m_stringBuffer, vars->val.string, len);
			m_stringBuffer[len] = 0;
		}
	} else {
		logErrors(session, response, object, status, "snmpGetString");
	}

	snmp_free_pdu(response);
	return m_stringBuffer;
}

static void appendToIntegerBuffer(struct snmp_pdu *response)
{
	struct variable_list *vars;
	for (vars = response->variables; vars; vars = vars->next_variable) {
		int errorCode;
		m_integerBuffer.value[m_integerBuffer.size] = getIntegerVariable(vars, &errorCode);
		m_integerBuffer.errorCode[m_integerBuffer.size] = errorCode;
		m_integerBuffer.size++;
	}
}

static void setIntegerBufferError(struct snmp_pdu *response)
{
	if (!response)
		return;

	int index = response->errindex - 1;
	if (index == -1)
		return;

	if (m_integerBuffer.size + index < MaxChannelsPerSlot)
		m_integerBuffer.errorCode[m_integerBuffer.size + index] = response->errstat;
}

static bool processNextGetIntegerPacket(HSNMP session, const SnmpObject *objects, int packetSize)
{
	bool result = true;
	struct snmp_pdu *pdu = prepareGetRequestPdu(objects, packetSize);

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		appendToIntegerBuffer(response);
	} else {
		setIntegerBufferError(response);
		logErrors(session, response, objects[0], status, "snmpGetMultipleInteger");
		result = false;
	}

	snmp_free_pdu(response);
	return result;
}

static SnmpIntegerBuffer *snmpGetMultipleInteger(HSNMP session, const SnmpObject *objects, int size)
{
	memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
	m_errorCode = SNMP_ERR_NOERROR;

	size = min(size, MaxChannelsPerSlot);
	while (size > 0) {
		int packetSize = min(size, MaxChannelsPerPacket);
		if (!processNextGetIntegerPacket(session, objects, packetSize))
			break;

		size -= packetSize;
		objects += packetSize;
	}

	return &m_integerBuffer;
}

static bool processNextSetIntegerPacket(HSNMP session, const SnmpObject *objects, int *values, int packetSize)
{
	bool result = true;
	struct snmp_pdu *pdu = prepareSetRequestPdu();

	for (int i = 0; i < packetSize; ++i) {
		int v = values[i];
		snmp_pdu_add_variable(pdu, objects[i].id, objects[i].len, ASN_INTEGER, (u_char *)&v, sizeof(v));
	}

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		appendToIntegerBuffer(response);
	} else {
		setIntegerBufferError(response);
		logErrors(session, response, objects[0], status, "snmpSetMultipleInteger");
		result = false;
	}

	snmp_free_pdu(response);
	return result;
}

static SnmpIntegerBuffer *snmpSetMultipleInteger(HSNMP session, const SnmpObject *objects, SnmpIntegerBuffer *values)
{
	memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
	m_errorCode = SNMP_ERR_NOERROR;

	int *val = values->value;
	int size = min(values->size, MaxChannelsPerSlot);
	while (size > 0) {
		int packetSize = min(size, MaxChannelsPerPacket);
		if (!processNextSetIntegerPacket(session, objects, val, packetSize))
			break;

		size -= packetSize;
		val += packetSize;
		objects += packetSize;
	}

	return &m_integerBuffer;
}

static void appendToDoubleBuffer(struct snmp_pdu *response)
{
	struct variable_list *vars;
	for (vars = response->variables; vars; vars = vars->next_variable) {
		int errorCode;
		m_doubleBuffer.value[m_doubleBuffer.size] = getDoubleVariable(vars, &errorCode);
		m_doubleBuffer.errorCode[m_doubleBuffer.size] = errorCode;
		m_doubleBuffer.size++;
	}
}

static void setDoubleBufferError(struct snmp_pdu *response)
{
	if (!response)
		return;

	int index = response->errindex - 1;
	if (index == -1)
		return;

	if (m_doubleBuffer.size + index < MaxChannelsPerSlot)
		m_doubleBuffer.errorCode[m_doubleBuffer.size + index] = response->errstat;
}

static bool processNextGetDoublePacket(HSNMP session, const SnmpObject *objects, int packetSize)
{
	bool result = true;
	struct snmp_pdu *pdu = prepareGetRequestPdu(objects, packetSize);

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		appendToDoubleBuffer(response);
	} else {
		setDoubleBufferError(response);
		logErrors(session, response, objects[0], status, "snmpGetMultipleDouble");
		result = false;
	}

	snmp_free_pdu(response);
	return result;
}

static SnmpDoubleBuffer *snmpGetMultipleDouble(HSNMP session, const SnmpObject *objects, int size)
{
	memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
	m_errorCode = SNMP_ERR_NOERROR;

	size = min(size, MaxChannelsPerSlot);
	while (size > 0) {
		int packetSize = min(size, MaxChannelsPerPacket);
		if (!processNextGetDoublePacket(session, objects, packetSize))
			break;

		size -= packetSize;
		objects += packetSize;
	}

	return &m_doubleBuffer;
}

static bool processNextSetDoublePacket(HSNMP session, const SnmpObject *objects, double *values, int packetSize)
{
	bool result = true;
	struct snmp_pdu *pdu = prepareSetRequestPdu();

	for (int i = 0; i < packetSize; ++i) {
		float v = (float)values[i];
		snmp_pdu_add_variable(pdu, objects[i].id, objects[i].len, ASN_OPAQUE_FLOAT, (u_char *)&v, sizeof(v));
	}

	struct snmp_pdu *response;
	int status = snmp_sess_synch_response(session, pdu, &response);
	updateSnmpErrorCode(status, response);

	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		appendToDoubleBuffer(response);
	} else {
		setDoubleBufferError(response);
		logErrors(session, response, objects[0], status, "snmpSetMultipleDouble");
		result = false;
	}

	snmp_free_pdu(response);
	return result;
}

static SnmpDoubleBuffer *snmpSetMultipleDouble(HSNMP session, const SnmpObject *objects, SnmpDoubleBuffer *values)
{
	memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
	int size = min(values->size, MaxChannelsPerSlot);
	double *val = values->value;
	m_errorCode = SNMP_ERR_NOERROR;

	while (size > 0) {
		int packetSize = min(size, MaxChannelsPerPacket);
		if (!processNextSetDoublePacket(session, objects, val, packetSize))
			break;

		size -= packetSize;
		val += packetSize;
		objects += packetSize;
	}

	return &m_doubleBuffer;
}

static bool isIndexOutOfRange(int start, int size)
{
	return start < 0 || size <= 0 || size > MaxChannelsPerSlot || start + size > MaxChannelsPerCrate;
}

/**
 * @brief Returns an array with the output status
 * for a consecutive range of channels.
 *
 * @note This function is deprecated. Use getMultipleOutputStatuses() instead.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpIntegerBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpIntegerBuffer *getMultipleChannelStatuses(HSNMP session, int start, int size)
{
	return getMultipleOutputStatuses(session, start, size);
}

/**
 * @brief Returns an array with the output status
 * for a consecutive range of channels.
 * @since 1.1
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpIntegerBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpIntegerBuffer *getMultipleOutputStatuses(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
		return &m_integerBuffer;
	}

	return snmpGetMultipleInteger(session, &outputStatus[start], size);
}

/**
 * @brief Returns an array with the outputSwitches
 * for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpIntegerBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpIntegerBuffer *getMultipleOutputSwitches(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
		return &m_integerBuffer;
	}

	return snmpGetMultipleInteger(session, &outputSwitch[start], size);
}

/**
 * @brief Sets the outputSwitch for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param values A pointer to SnmpIntegerBuffer with the list of outputSwitches.
 * @return
 */
SnmpIntegerBuffer *setMultipleOutputSwitches(HSNMP session, int start, SnmpIntegerBuffer *values)
{
	if (isIndexOutOfRange(start, values->size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
		return &m_integerBuffer;
	}

	return snmpSetMultipleInteger(session, &outputSwitch[start], values);
}

/**
 * @brief Returns the actual output voltage for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleOutputVoltages(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpGetMultipleDouble(session, &outputVoltage[start], size);
}

/**
 * @brief Sets the demanded output voltage for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param values A pointer to SnmpDoubleBuffer with the list of new outputVoltages
 * @return
 */
SnmpDoubleBuffer *setMultipleOutputVoltages(HSNMP session, int start, SnmpDoubleBuffer *values)
{
	if (isIndexOutOfRange(start, values->size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpSetMultipleDouble(session, &outputVoltage[start], values);
}

/**
 * @brief Returns an array with the measured terminal voltages
 * for a consecutive range of channels.
 * @note This function is deprecated.
 * Use getMultipleOutputMeasurementTerminalVoltages() instead.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleMeasurementTerminalVoltages(HSNMP session, int start, int size)
{
	return getMultipleOutputMeasurementTerminalVoltages(session, start, size);
}

/**
 * @brief Returns an array with the measured terminal voltages
 * for a consecutive range of channels.
 * @since 1.1
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleOutputMeasurementTerminalVoltages(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpGetMultipleDouble(session, &outputMeasurementTerminalVoltage[start], size);
}

/**
 * @brief getMultipleOutputConfigMaxTerminalVoltages
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleOutputConfigMaxTerminalVoltages(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpGetMultipleDouble(session, &outputConfigMaxTerminalVoltage[start], size);
}

/**
 * @brief Returns an array the demanded output currents
 * for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleOutputCurrents(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpGetMultipleDouble(session, &outputCurrent[start], size);
}

/**
 * @brief Sets the demanded output current for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param values A pointer to SnmpDoubleBuffer with a list of new output currents
 * @return
 */
SnmpDoubleBuffer *setMultipleOutputCurrents(HSNMP session, int start, SnmpDoubleBuffer *values)
{
	if (isIndexOutOfRange(start, values->size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpSetMultipleDouble(session, &outputCurrent[start], values);
}

/**
 * @brief Returns an array with the measured currents
 * for a consecutive range of channels.
 *
 * @note This function is deprecated.
 * Use getMultipleOutputMeasurementCurrents() instead.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleMeasurementCurrents(HSNMP session, int start, int size)
{
	return getMultipleOutputMeasurementCurrents(session, start, size);
}

/**
 * @brief Returns an array with the measured currents
 * for a consecutive range of channels.
 * @since 1.1
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleOutputMeasurementCurrents(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpGetMultipleDouble(session, &outputMeasurementCurrent[start], size);
}

/**
 * @brief Returns an array with the outputConfigMaxCurrent
 * for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpDoubleBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpDoubleBuffer *getMultipleOutputConfigMaxCurrents(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_doubleBuffer, 0, sizeof(m_doubleBuffer));
		return &m_doubleBuffer;
	}

	return snmpGetMultipleDouble(session, &outputConfigMaxCurrent[start], size);
}

/**
 * @brief Returns an array with the outputTripTimeMaxCurrent
 * for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpIntegerBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpIntegerBuffer *getMultipleOutputTripTimeMaxCurrents(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
		return &m_integerBuffer;
	}

	return snmpGetMultipleInteger(session, &outputTripTimeMaxCurrent[start], size);
}

/**
 * @brief Sets the outputTripTimeMaxCurrent for a consecutive ranges of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param values
 * @return
 */
SnmpIntegerBuffer *setMultipleOutputTripTimeMaxCurrents(HSNMP session, int start, SnmpIntegerBuffer *values)
{
	if (isIndexOutOfRange(start, values->size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
		return &m_integerBuffer;
	}

	return snmpSetMultipleInteger(session, &outputTripTimeMaxCurrent[start], values);
}

/**
 * @brief Returns an array with the outputSupervisionBehavior for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param size The number of requested channels.
 * @return A pointer to SnmpIntegerBuffer with the requested information.
 * @note This pointer is only valid until the next call of getMultiple...
 * or setMultiple... function.
 */
SnmpIntegerBuffer *getMultipleOutputSupervisionBehaviors(HSNMP session, int start, int size)
{
	if (isIndexOutOfRange(start, size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
		return &m_integerBuffer;
	}

	return snmpGetMultipleInteger(session, &outputSupervisionBehavior[start], size);
}

/**
 * @brief Sets the outputSupervisionBehavior for a consecutive range of channels.
 * @param session The handle returned by snmpOpen()
 * @param start The first channel (in the range of 0 to ::MaxChannelsPerCrate - 1).
 * 0 = slot 0, channel 0; 100 = slot 1, channel 0.
 * @param values The new outputSupervisionBehavior for the all channels starting with start.
 * @return
 */
SnmpIntegerBuffer *setMultipleOutputSupervisionBehaviors(HSNMP session, int start, SnmpIntegerBuffer *values)
{
	if (isIndexOutOfRange(start, values->size)) {
		m_errorCode = SNMP_NOSUCHINSTANCE;
		memset(&m_integerBuffer, 0, sizeof(m_integerBuffer));
		return &m_integerBuffer;
	}

	return snmpSetMultipleInteger(session, &outputSupervisionBehavior[start], values);
}
