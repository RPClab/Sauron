#include "add.h"
#include "WIENER_SNMP.h"
#include "version.h"
#include <net-snmp/net-snmp-config.h>
 #include <net-snmp/net-snmp-includes.h>
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



int snmpInit(const char* mib_path)
{
	snmp_log(LOG_DEBUG, "*** Initialise SNMP ***\n");

	init_snmp("WIENER_SNMP_DLL");
	init_mib();	
	add_mibdir(mib_path);							// init MIB processing
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
