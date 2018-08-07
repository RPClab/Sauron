/**
@file readme.txt
@mainpage WIENER SNMP Library

A C/C++ library for MPOD crate access via SNMP
==============================================

This library allows a simple access to iseg high voltage and WIENER low voltage
modules in a WIENER MPOD crate. The WIENER MPOD is connected to the controlling
computer via Ethernet. The used network protocol is SNMP (Simple Network
Management Protocol). The open-source library NET-SNMP (http://www.net-snmp.org),
available for Microsoft Windows and Linux is used as backend.

The textfile WIENER-CRATE-MIB.txt describes the SNMP items available in the
MPOD crate and translates the textual SNMP descriptions to the SNMP Object IDs.
The MIB file must be copied to /usr/share/snmp/mibs.

This library is copyright:

	(c) 2006-2015 wiener Plein & Baus, Corp.
	(c) 2012-2015 iseg Spezialelektronik GmbH

This library was developed using:

- Windows XP SP3 (32 bit)
	- net-snmp 5.7.1 32 bit (self compiled with Visual Studio)
	- Visual Studio 2010
	- QtCreator 2.8 using the MSVC 2010 toolchain

- Windows 7 SP1 (64 bit)
	- net-snmp 5.7.1 64 bit (self compiled with Visual Studio)
	- Visual Studio 2010
	- QtCreator 2.6 using the MSVC 2010 toolchain

- Ubuntu 12.04 (32 and 64 bit)
	- net-snmp 5.4.3 (from package manager)
	- QtCreator 2.4.1 (from package manager)

Compiling net-snmp (Windows)
============================

@note Perl is needed to build net-snmp, e.g. ActivePerl (http://www.activestate.com)

- Open a Visual Studio command prompt:

		Start -> Programs -> Microsoft Visual Studio 2010 -> Visual Studio Tools ->
			Visual Studio Command Prompt (32 or 64 bit)

- change to the net-snmp directory and start build:

		cd net-snmp-5.7.1
		win32\build.bat

- choose:

		(8 - debug mode if you want to debug net-snmp)
		12 - install development files

- start build with 'f'

- After installation, it may be necessary to comment out
  line 81 in c:/usr/include/net-snmp/types.h:

		//typedef u_int socklen_t;

Compiling WIENER_SNMP with Qt Creator
-------------------------------------

Windows
-------
Although this library doesn't use Qt, a configured Qt matching the
MSVC toolchain is needed. Under Windows, Qt 4.8.2-MSVC2010 is used.

Ubuntu
------
Although this library doesn't use Qt, it is used for the build process.
So the Qt development files need to be installed along with the net-snmp
development files:

	sudo apt-get install build-essential libqt4-dev qtcreator
	sudo apt-get install libsnmp-dev snmp snmp-mibs-downloader

Library Overview
================

There are several function groups to set or get data either from a
single channel, multiple channels or from the module in a specific
slot. These function groups are listed below.

The functions snmpInit(), snmpOpen(), snmpClose() and snmpCleanup() handle
the connection to the MPOD crate.

- snmpInit() parses the WIENER-CRATE-MIB.txt and resolves the corresponding
  SNMP Object IDs.
- snmpOpen() opens a SNMP connection to the MPOD with the specified IP address.
- snmpClose() closes an previously opened connection.
- snmpCleanup() should be called after snmpClose() to do housekeeping.

The functions setMainSwitch() and getMainSwitch() allows to soft-switching the
crate on/off and reading back the current status. Please note that it is not
possible to turn power on, if the hardware power switch is in position "0".

More information about the functions can be found in the source code
documentation in WIENER_SNMP.h and in the WIENER-CRATE-MIB.TXT.

Function groups
---------------

- The getOutput... functions deliver information about a single channel:
  Measured voltage, measured current, status information or read back the
  written set values

- The setOutput... functions allow setting a voltage and a current limit
  for a single channel, turning the channel on/off and clearing the status
  registers.

- The getMultiple... functions deliver information for a consecutive range
  of channels, e.g. 24 voltage measurement values for one module.
  The maximum number of values is limited by the MPOD controllers network stack.
  Simultaneous transmission of up to 32 values was successfully tested.

- The setMultiple... functions sets a specific item for a consecutive range
  of channels, e.g. 24 output switches to turn on these channels.
  The maximum number of values is limited by the MPOD controllers network stack.
  Simultaneous transmission of up to 32 values was successfully tested.

- The getModule... functions deliver information about iseg HV modules,
  e.g. the serial number, the firmware release or the actual voltage ramp speed.

- The setModule... functions sets iseg HV module specific items, e.g. the voltage
  ramp speed.

Slot and channel mapping
------------------------

A MPOD crate consists of 10 slots for high voltage and low voltage modules.
Each slot can handle a maximum of 100 low voltage or high voltage channels.
So the parameter "channel" for all getOutput.../setOutput... functions is in
the range of:

- 0...99 for the module in slot 0,
- 100...199 for the module in slot 1 and
- 900...999 for the module in slot 9.

If the module has less than 100 channels, only the available channels are used.
For example, a 24 channel module uses the following channels:

- 0...23 in slot 0
- 100...123 in slot 1 and
- 900...923 in slot 9

*/
