/** @file */

#ifndef WIENER_SNMP_H
#define WIENER_SNMP_H

#if defined(_MSC_VER) && !defined(TESTS)
#	define EXPORT _declspec(dllexport)
#else
#	define EXPORT
#endif

#define WIN32_LEAN_AND_MEAN

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <stdlib.h>

typedef void *HSNMP;   ///< SNMP handle (like FILE)

#define SNMP_USER_ERR_TIMEOUT		(MAX_SNMP_ERR + 1)	///< The SNMP request timed out

#ifdef __cplusplus
	extern "C" {
#endif

// DLL functions

EXPORT int    snmpStringCopy(char *dest, size_t len, const char *source);

EXPORT void   snmpSetStdErrLog(void);
EXPORT void   snmpSetFileLog(const char * const fileName);
EXPORT unsigned int snmpGetVersion(void);
EXPORT const char *snmpGetVersionString(void);

EXPORT int    snmpInit(void);
EXPORT void   snmpCleanup(void);
EXPORT void   snmpSetReadCommunityName(const char * const readCommunityName);
EXPORT void   snmpSetWriteCommunityName(const char * const writeCommunityName);

EXPORT HSNMP  snmpOpen(const char * const ipAddress);
EXPORT void   snmpClose(HSNMP session);
EXPORT char  *snmpGetLastError(void);
EXPORT int    snmpGetLastErrorCode(void);

// System Functions
EXPORT char *getSysDescr(HSNMP session);
EXPORT int  getMainSwitch(HSNMP session);
EXPORT int  setMainSwitch(HSNMP session, int value);
EXPORT int  getMainStatus(HSNMP session);
EXPORT int  getVmeReset(HSNMP session);
EXPORT int  setVmeReset(HSNMP session);
EXPORT int  getIpStaticAddress(HSNMP session);
EXPORT int  setIpStaticAddress(HSNMP session, int value);
EXPORT char *getPsSerialNumber(HSNMP session);

// Output Counts
EXPORT int  getOutputNumber(HSNMP session);
EXPORT int  getOutputGroups(HSNMP session);

// Output Channel Information
EXPORT int     getOutputGroup(HSNMP session, int channel);
EXPORT int     getOutputStatus(HSNMP session, int channel);
EXPORT int     getChannelStatus(HSNMP m_sessmp, int channel);
EXPORT double  getOutputSenseMeasurement(HSNMP session, int channel);
EXPORT double  getOutputTerminalMeasurement(HSNMP session, int channel);
EXPORT double  getCurrentMeasurement(HSNMP session, int channel);
EXPORT int     getTemperatureMeasurement(HSNMP session, int channel);
EXPORT int     setChannelSwitch(HSNMP session, int channel, int value);
EXPORT int     setOutputSwitch(HSNMP session, int channel, int value);
EXPORT int     getChannelSwitch(HSNMP session, int channel);
EXPORT int     getOutputSwitch(HSNMP session, int channel);
EXPORT int     setHighVoltageGroupsSwitch(HSNMP session, int value);
EXPORT int     getHighVoltageGroupsSwitch(HSNMP session);
EXPORT int     setLowVoltageGroupsSwitch(HSNMP session, int value);
EXPORT int     getLowVoltageGroupsSwitch(HSNMP session);
EXPORT double  getOutputVoltage(HSNMP session, int channel);
EXPORT double  setOutputVoltage(HSNMP session, int channel, double value);
EXPORT double  getOutputCurrent(HSNMP session, int channel);
EXPORT double  setOutputCurrent(HSNMP session, int channel, double value);
EXPORT double  getOutputRiseRate(HSNMP session, int channel);
EXPORT double  setOutputRiseRate(HSNMP session, int channel, double value);
EXPORT double  getOutputFallRate(HSNMP session, int channel);
EXPORT double  setOutputFallRate(HSNMP session, int channel, double value);
EXPORT double  getOutputCurrentRiseRate(HSNMP session, int channel);
EXPORT double  setOutputCurrentRiseRate(HSNMP session, int channel, double value);
EXPORT double  getOutputCurrentFallRate(HSNMP session, int channel);
EXPORT double  setOutputCurrentFallRate(HSNMP session, int channel, double value);
EXPORT int     getOutputSupervisionBehavior(HSNMP session, int channel);
EXPORT int     setOutputSupervisionBehavior(HSNMP session, int channel, int value);
EXPORT double  getOutputSupervisionMinSenseVoltage(HSNMP session, int channel);
EXPORT double  setOutputSupervisionMinSenseVoltage(HSNMP session, int channel, double value);
EXPORT double  getOutputSupervisionMaxSenseVoltage(HSNMP session, int channel);
EXPORT double  setOutputSupervisionMaxSenseVoltage(HSNMP session, int channel, double value);
EXPORT double  getOutputSupervisionMaxTerminalVoltage(HSNMP session, int channel);
EXPORT double  setOutputSupervisionMaxTerminalVoltage(HSNMP session, int channel, double value);
EXPORT double  getOutputSupervisionMaxCurrent(HSNMP session, int channel);
EXPORT double  setOutputSupervisionMaxCurrent(HSNMP session, int channel, double value);
EXPORT int     setOutputSupervisionMaxTemperature(HSNMP session, int channel, int value);
EXPORT double  getOutputConfigMaxSenseVoltage(HSNMP session, int channel);
EXPORT double  getOutputConfigMaxTerminalVoltage(HSNMP session, int channel);
EXPORT double  getOutputConfigMaxCurrent(HSNMP session, int channel);
EXPORT double  getOutputSupervisionMaxPower(HSNMP session, int channel);
EXPORT int     getOutputTripTimeMaxCurrent(HSNMP session, int channel);
EXPORT int     setOutputTripTimeMaxCurrent(HSNMP session, int channel, int delay);

// Sensor Information functions
EXPORT int  getSensorNumber(HSNMP session);
EXPORT int  getSensorTemp(HSNMP session, int sensor);
EXPORT int  getSensorWarningTemperature(HSNMP session, int sensor);
EXPORT int  setSensorWarningTemperature(HSNMP session, int sensor, int value);
EXPORT int  getSensorFailureTemperature(HSNMP session, int sensor);
EXPORT int  setSensorFailureTemperature(HSNMP session, int sensor, int value);

// Power Supply Detail Functions
EXPORT int    getPsOperatingTime(HSNMP session);
EXPORT double getPsAuxVoltage(HSNMP session, int auxIndex);
EXPORT double getPsAuxCurrent(HSNMP session, int auxIndex);

// Fan Tray Functions
EXPORT int  getFanOperatingTime(HSNMP session);
EXPORT int  getFanAirTemperature(HSNMP session);
EXPORT int  getFanSwitchOffDelay(HSNMP session);
EXPORT int  setFanSwitchOffDelay(HSNMP session, int value);
EXPORT int  getFanNominalSpeed(HSNMP session);
EXPORT int  setFanNominalSpeed(HSNMP session, int value);
EXPORT int  getFanNumberOfFans(HSNMP session);
EXPORT int  getFanSpeed(HSNMP session, int fan);

// Module functions
EXPORT char  *getModuleDescription(HSNMP session, int slot);
EXPORT double getModuleSupply24(HSNMP session, int slot);
EXPORT double getModuleSupply5(HSNMP session, int slot);
EXPORT double getModuleAuxTemperature(HSNMP session, int slot, int index);
EXPORT double getModuleHardwareLimitVoltage(HSNMP session, int slot);
EXPORT double getModuleHardwareLimitCurrent(HSNMP session, int slot);
EXPORT double getModuleRampSpeedVoltage(HSNMP session, int slot);
EXPORT double setModuleRampSpeedVoltage(HSNMP session, int slot, double value);
EXPORT double getModuleRampSpeedCurrent(HSNMP session, int slot);
EXPORT double setModuleRampSpeedCurrent(HSNMP session, int slot, double value);
EXPORT int    getModuleStatus(HSNMP session, int slot);
EXPORT int    getModuleEventStatus(HSNMP session, int slot);
EXPORT int    setModuleDoClear(HSNMP session, int slot);

/**
 * @enum MaximumCrateValues
 */
typedef enum MaximumCrateValues {
	MaxModuleAuxSupplies = 2,     ///< The maximum number of module auxiliary supply measurements
	MaxModuleAuxTemperatures = 4, ///< The maximum number of module temperature sensors (iseg HV only)
	MaxCommunities = 4,           ///< The maximum number of SNMP community names
	MaxFans = 6,                  ///< The maximum number of fans per crate
	MaxPsAuxSupplies = 8,         ///< The maximum number of auxiliary supplies per crate
	MaxSlotsPerCrate = 10,        ///< The maximum number of slots per crate
	MaxSensors = 12,              ///< The maximum number of sensors
	MaxChannelsPerPacket = 32,    ///< The maximum number of channels in one get/set request packet
	MaxChannelsPerSlot = 100,     ///< The maximum number of channels per slot
	MaxChannelsPerCrate = 1000    ///< The maximum number of channels per crate
} MaximumCrateValues;

/**
 * @struct SnmpDoubleBuffer
 * This struct is used to pass double parameters to setMultiple... functions
 * and for the result value of getMultiple.../setMultiple... functions.
 */
typedef struct SnmpDoubleBuffer {
	int size;                          ///< The number of values used in value[]
	double value[MaxChannelsPerSlot];  ///< An array of max. 100 double values
	int errorCode[MaxChannelsPerSlot]; ///< An array with error codes for each value
} SnmpDoubleBuffer;

/**
 * @struct SnmpIntegerBuffer
 * This struct is used to pass integer parameters to setMultiple... functions
 * and for the result value of getMultiple.../setMultiple... functions.
*/
typedef struct SnmpIntegerBuffer {
	int size;                          ///< The number of values used in value[]
	int value[MaxChannelsPerSlot];     ///< An array of max. 100 int values
	int errorCode[MaxChannelsPerSlot]; ///< An array with error codes for each value
} SnmpIntegerBuffer;

// Functions for multiple data transfer in one SNMP packet
EXPORT SnmpIntegerBuffer *getMultipleChannelStatuses(HSNMP session, int start, int size);
EXPORT SnmpIntegerBuffer *getMultipleOutputStatuses(HSNMP session, int start, int size);
EXPORT SnmpIntegerBuffer *getMultipleOutputSwitches(HSNMP session, int start, int size);
EXPORT SnmpIntegerBuffer *setMultipleOutputSwitches(HSNMP session, int start, SnmpIntegerBuffer *values);
EXPORT SnmpDoubleBuffer *getMultipleOutputVoltages(HSNMP session, int start, int size);
EXPORT SnmpDoubleBuffer *setMultipleOutputVoltages(HSNMP session, int start, SnmpDoubleBuffer *values);
EXPORT SnmpDoubleBuffer *getMultipleMeasurementTerminalVoltages(HSNMP session, int start, int size);
EXPORT SnmpDoubleBuffer *getMultipleOutputMeasurementTerminalVoltages(HSNMP session, int start, int size);
EXPORT SnmpDoubleBuffer *getMultipleOutputConfigMaxTerminalVoltages(HSNMP session, int start, int size);
EXPORT SnmpDoubleBuffer *getMultipleOutputCurrents(HSNMP session, int start, int size);
EXPORT SnmpDoubleBuffer *setMultipleOutputCurrents(HSNMP session, int start, SnmpDoubleBuffer *values);
EXPORT SnmpDoubleBuffer *getMultipleMeasurementCurrents(HSNMP session, int start, int size);
EXPORT SnmpDoubleBuffer *getMultipleOutputMeasurementCurrents(HSNMP session, int start, int size);
EXPORT SnmpDoubleBuffer *getMultipleOutputConfigMaxCurrents(HSNMP session, int start, int size);
EXPORT SnmpIntegerBuffer *getMultipleOutputTripTimeMaxCurrents(HSNMP session, int start, int size);
EXPORT SnmpIntegerBuffer *setMultipleOutputTripTimeMaxCurrents(HSNMP session, int start, SnmpIntegerBuffer *values);
EXPORT SnmpIntegerBuffer *getMultipleOutputSupervisionBehaviors(HSNMP session, int start, int size);
EXPORT SnmpIntegerBuffer *setMultipleOutputSupervisionBehaviors(HSNMP session, int start, SnmpIntegerBuffer *values);

#ifdef __cplusplus
	}
#endif

#endif
