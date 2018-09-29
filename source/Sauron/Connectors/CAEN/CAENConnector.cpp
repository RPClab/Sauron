/*
 * Copyright (c) 2018 Lagarde François lagarde.at.sjtu.edu.cn
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "CAENConnector.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "Kernel.h"

#if defined(_WIN32)
#include <tchar.h>
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>
#include <cstring>
#endif

#if defined(__linux__)
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#if defined(__APPLE__)
#include <sys/param.h>
#include <stdint.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#endif


#ifdef _WIN32
#  define EXPORTIT __declspec( dllexport )
#else
#  define EXPORTIT
#endif

extern "C" EXPORTIT void register_pugg_plugin(pugg::Kernel* kernel)
{
	kernel->add_driver(new CAENConnectorDriver());
}

CAENConnector::CAENConnector(const CAENConnector& other):CAENConnector()
{
    m_params=other.m_params;
}

CAENConnector::CAENConnector()
{
     m_name="CAENConnector";
}



CAENConnector::CAENConnector(const std::map<std::string,std::string>& params):CAENConnector()
{
    m_params=params;
}

CAENConnector& CAENConnector::operator=(const CAENConnector& other)
{
    m_params=other.m_params;
    m_name=other.m_name;
    return *this;
}

CAENConnector& CAENConnector::operator()(const CAENConnector& other)
{
    m_params=other.m_params;
    m_name=other.m_name;
    return *this;
}


void CAENConnector::initialize()
{
   setCAENHVSystemType();
   setLinkType();
   setArg();
}

bool CAENConnector::isConnected()
{
    if(m_connected==0) return true;
    else return false;
}

void CAENConnector::release()
{
    
}

void CAENConnector::disconnect()
{
    if(isConnected())
    {
        m_connected=CAENHV_DeinitSystem(m_handle);
        if( m_connected == CAENHV_OK )
        std::cout<<"CAENHV_DeinitSystem: Connection closed (num. "<<m_connected<<")\n";
        else
        {
            std::cout<<"CAENHV_DeinitSystem: "<<CAENHV_GetError(m_handle)<<" (num. "<<double(m_connected)<<")\n";
        }
    }
}


void CAENConnector::connect()
{
    if(!isConnected()) 
    {
        m_connected=CAENHV_InitSystem(m_CAENHVSystemType,m_linkType,(void*)m_arg.c_str(),m_username.c_str(),m_password.c_str(),&m_handle);
        if(m_connected!=4100)std::cout<<"CAENHV_InitSystem: "<<CAENHV_GetError(m_handle)<<"(num. "<<m_connected<<")\n";
        if(m_connected!=0)
        {
            static int i=1;
            if(i>=m_ports.size()) throw std::out_of_range("All port were tested");
            m_params.changeParameter("Port",m_ports[i].port);
            ++i;
            initialize();
            connect();
        }
        if(m_CAENHVSystemType==SY1527||m_CAENHVSystemType==SY2527||m_CAENHVSystemType==SY4527||m_CAENHVSystemType==SY5527)
        {
            m_keepAlive=std::thread(&CAENConnector::keepAliveFunction,this);
            m_keepAlive.detach();
        }
    }
}


void CAENConnector::setCAENHVSystemType()
{
    if(m_params.hasParam("CAENHVSystemType"))
    {
        if(m_params["CAENHVSystemType"].String()=="SY1527") m_CAENHVSystemType=SY1527;
        else if(m_params["CAENHVSystemType"].String()=="SY2527") m_CAENHVSystemType=SY2527;
        else if(m_params["CAENHVSystemType"].String()=="SY4527") m_CAENHVSystemType=SY4527;
        else if(m_params["CAENHVSystemType"].String()=="SY5527") m_CAENHVSystemType=SY5527;
        else if(m_params["CAENHVSystemType"].String()=="N568") m_CAENHVSystemType=N568;
        else if(m_params["CAENHVSystemType"].String()=="V65XX") m_CAENHVSystemType=V65XX;
        else if(m_params["CAENHVSystemType"].String()=="N14XX") m_CAENHVSystemType=N1470;
        else if(m_params["CAENHVSystemType"].String()=="V8100") m_CAENHVSystemType=V8100;
        else if(m_params["CAENHVSystemType"].String()=="N568E") m_CAENHVSystemType=N568E;
        else if(m_params["CAENHVSystemType"].String()=="DT55XX") m_CAENHVSystemType=DT55XX;
        else if(m_params["CAENHVSystemType"].String()=="FTK") m_CAENHVSystemType=FTK;
        else if(m_params["CAENHVSystemType"].String()=="DT55XXE") m_CAENHVSystemType=DT55XXE;
        else if(m_params["CAENHVSystemType"].String()=="N1068") m_CAENHVSystemType=N1068;
        else
        {
            std::string error="Unknown CAENHVSystemType !\nAvailables are : SY1527, SY2527, SY4527, SY5527, N568, V65XX, N14XX, V8100, N568E, DT55XX, FTK, DT55XXE, N1068\n";
            std::cout<<error;
            throw error;
        }
    }
    else
    {
        
        std::string error="CAENHVSystemType is mandatory ! \n";
        std::cout<<error;
        throw error;
    }
}

void CAENConnector::setLinkType()
{
    if(m_params.hasParam("LinkType"))
    {
        if(m_params["LinkType"].String()=="TCP") m_linkType=LINKTYPE_TCPIP;
        else if(m_params["LinkType"].String()=="RS232") m_linkType=LINKTYPE_RS232;
        else if(m_params["LinkType"].String()=="CAENET") m_linkType=LINKTYPE_CAENET;
        else if(m_params["LinkType"].String()=="USB") m_linkType=LINKTYPE_USB;
        else if(m_params["LinkType"].String()=="Optical") m_linkType=LINKTYPE_OPTLINK;
        else if(m_params["LinkType"].String()=="VCP") m_linkType=LINKTYPE_USB_VCP;
        else
        {
            std::string error="Unknown LinkType !\nAvailables are : TCP, RS232, CAENET, USB, Optical, VCP\n";
            std::cout<<error;
            throw error;
        }
    }
    else
    {
        
        std::string error="LinkType is mandatory ! \n";
        std::cout<<error;
        throw error;
    }  
}

void CAENConnector::setArg()
{
    m_arg.clear();
    if(m_linkType==LINKTYPE_TCPIP
      &&
      (m_CAENHVSystemType==SY1527||m_CAENHVSystemType==SY2527||m_CAENHVSystemType==SY4527||m_CAENHVSystemType==SY5527))
    {
        setIP();
        m_arg=m_IP;
        setUsername();
        setPassword();
    }
    else if(m_linkType==LINKTYPE_TCPIP&&m_CAENHVSystemType==N568E)
    {
        setIP();
        m_arg=m_IP;
        setIBusAddress();
        m_arg=m_arg+"_"+m_IBusAddress;
    }
    else if(m_linkType==LINKTYPE_USB||m_linkType==LINKTYPE_OPTLINK)
    {
        setLinkNum();
        m_arg=m_linkNum;
        if(m_linkType==LINKTYPE_USB) m_arg+="_0";
        else
        {
            setConetNode();
            m_arg=m_arg+"_"+m_conetNode;
        }
        setVMEBaseAddress();
        m_arg=m_arg+"_"+m_VMEBaseAddress;
    }
    else if(m_linkType=LINKTYPE_USB_VCP)
    {
        setPort();
        m_arg=m_port+"_";
        setBaudRate();
        m_arg=m_arg+m_baudrate+"_";
        setBytesize();
        m_arg=m_arg+m_bytesize+"_";
        setStopbits();
        m_arg=m_arg+m_stopbits+"_";
        setParity();
        m_arg=m_arg+m_parity+"_";
        setIBusAddress();
        m_arg=m_arg+m_IBusAddress;
    }
}

void CAENConnector::setIP()
{
    if(m_params.hasParam("IP"))
    {
        m_IP=m_params["IP"].String();
    }
    else
    {
        
        std::string error="IP is mandatory ! \n";
        std::cout<<error;
        throw error;
    }  
}

void CAENConnector::setIBusAddress()
{
  if(m_params.hasParam("IBusAddress"))
    {
        m_IBusAddress=m_params["IBusAddress"].String();
    }
    else
    {
        
        std::string error="IBusAddress is mandatory ! \n";
        std::cout<<error;
        throw error;
    }    
}

void CAENConnector::setLinkNum()
{
    if(m_params.hasParam("LinkNum"))
    {
        m_linkNum=m_params["LinkNum"].String();
    }
    else
    {
        
        std::string error="LinkNum is mandatory ! \n";
        std::cout<<error;
        throw error;
    }  
}

void CAENConnector::setConetNode()
{
    if(m_params.hasParam("ConetNode"))
    {
        m_conetNode=m_params["ConetNode"].String();
    }
    else
    {
        
        std::string error="ConetNode is mandatory ! \n";
        std::cout<<error;
        throw error;
    }  
}

void CAENConnector::setVMEBaseAddress()
{
    if(m_params.hasParam("VMEBaseAddress"))
    {
        m_VMEBaseAddress=m_params["VMEBaseAddress"].String();
    }
    else
    {
        std::string error="VMEBaseAddress is mandatory ! \n";
        std::cout<<error;
        throw error;
    }  
}

void CAENConnector::setPort()
{
    if(m_params.hasParam("Port"))
    {
        std::size_t const pos=m_params["Port"].String().find_last_of('/');
        m_port=m_params["Port"].String().substr(pos+1);
    }
    else
    {
        std::cout<<"Sauron will try to find it if you have provided a SN for the Module !\n";
        m_ports=list_ports();
        std::size_t const pos=m_ports[0].port.find_last_of('/');
        m_port=m_ports[0].port.substr(pos+1);
    } 
}

void CAENConnector::setBaudRate()
{
    if(m_params.hasParam("Baudrate"))
    {
        m_baudrate=m_params["Baudrate"].String();
    }
    else
    {
        std::cout<<"Baudrate setted to default value "<<m_baudrate<<" !\n";
    }
}
void CAENConnector::setParity()
{
    if(m_params.hasParam("Parity"))
    {
        if(m_params["Parity"]=="none") m_parity="0";
        else if (m_params["Parity"]=="odd")m_parity="2";
        else if (m_params["Parity"]=="even")m_parity="1";
        else if (m_params["Parity"]=="mark")m_parity="3";
        else if (m_params["Parity"]=="space")m_parity="4";
        else 
        {
            std::cout<<"Parity can be only none, odd, even, mark or space"<<std::endl;
            std::cout<<"Parity setted to default value "<<m_parity<<" !\n";
        }
    }
    else
    {
        std::cout<<"Parity setted to default value "<<m_parity<<" !\n";
    }
}

void CAENConnector::setBytesize()
{
    if(m_params.hasParam("Bytesize"))
    {
        if(m_params["Bytesize"]=="8") m_bytesize="8";
        else if (m_params["Bytesize"]=="7") m_bytesize="7";
        else if (m_params["Bytesize"]=="6") m_bytesize="6";
        else if (m_params["Bytesize"]=="5") m_bytesize="5";
        else 
        {
            std::cout<<"Bytesize can be only 8, 7, 6 or 5 !"<<std::endl;
            std::cout<<"Bytesize setted to default value "<<m_bytesize<<" !\n";
        }
    }
    else
    {
        std::cout<<"Bytesize setted to default value "<<m_bytesize<<" !\n";
    }
}


void CAENConnector::setStopbits()
{
    if(m_params.hasParam("Stopbits"))
    {
        if(m_params["Stopbits"]=="1") m_stopbits="0";
        else if (m_params["Stopbits"]=="2") m_stopbits="2";
        else if (m_params["Stopbits"]=="1.5") m_stopbits="1";
        else 
        {
            std::cout<<"Stopbits can be only 1, 1.5 or 2"<<std::endl;
            std::cout<<"Stopbits setted to default value "<<m_stopbits<<" !\n";
        }
    }
    else
    {
        std::cout<<"Stopbits setted to default value "<<m_stopbits<<" !\n";
    }
}

void CAENConnector::setUsername()
{
    if(m_params.hasParam("Username"))
    {
        m_username=m_params["Username"].String();
    }
    else
    {
        std::string error="Username is mandatory ! \n";
        std::cout<<error;
        throw error;
    }   
}

void CAENConnector::setPassword()
{
    if(m_params.hasParam("Password"))
    {
        m_password=m_params["Password"].String();
    }
    else
    {
        std::string error="Password is mandatory ! \n";
        std::cout<<error;
        throw error;
    }  
}


#if defined(_WIN32)
using std::vector;
using std::string;

static const DWORD port_name_max_length = 256;
static const DWORD friendly_name_max_length = 256;
static const DWORD hardware_id_max_length = 256;

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo( size_needed, 0 );
	WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

vector<CAENConnector::PortInfo> CAENConnector::list_ports()
{
	vector<CAENConnector::PortInfo> devices_found;

	HDEVINFO device_info_set = SetupDiGetClassDevs(
		(const GUID *) &GUID_DEVCLASS_PORTS,
		NULL,
		NULL,
		DIGCF_PRESENT);

	unsigned int device_info_set_index = 0;
	SP_DEVINFO_DATA device_info_data;

	device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

	while(SetupDiEnumDeviceInfo(device_info_set, device_info_set_index, &device_info_data))
	{
		device_info_set_index++;

		// Get port name

		HKEY hkey = SetupDiOpenDevRegKey(
			device_info_set,
			&device_info_data,
			DICS_FLAG_GLOBAL,
			0,
			DIREG_DEV,
			KEY_READ);

		TCHAR port_name[port_name_max_length];
		DWORD port_name_length = port_name_max_length;

		LONG return_code = RegQueryValueEx(
					hkey,
					_T("PortName"),
					NULL,
					NULL,
					(LPBYTE)port_name,
					&port_name_length);

		RegCloseKey(hkey);

		if(return_code != EXIT_SUCCESS)
			continue;

		if(port_name_length > 0 && port_name_length <= port_name_max_length)
			port_name[port_name_length-1] = '\0';
		else
			port_name[0] = '\0';

		// Ignore parallel ports

		if(_tcsstr(port_name, _T("LPT")) != NULL)
			continue;

		// Get port friendly name

		TCHAR friendly_name[friendly_name_max_length];
		DWORD friendly_name_actual_length = 0;

		BOOL got_friendly_name = SetupDiGetDeviceRegistryProperty(
					device_info_set,
					&device_info_data,
					SPDRP_FRIENDLYNAME,
					NULL,
					(PBYTE)friendly_name,
					friendly_name_max_length,
					&friendly_name_actual_length);

		if(got_friendly_name == TRUE && friendly_name_actual_length > 0)
			friendly_name[friendly_name_actual_length-1] = '\0';
		else
			friendly_name[0] = '\0';

		// Get hardware ID

		TCHAR hardware_id[hardware_id_max_length];
		DWORD hardware_id_actual_length = 0;

		BOOL got_hardware_id = SetupDiGetDeviceRegistryProperty(
					device_info_set,
					&device_info_data,
					SPDRP_HARDWAREID,
					NULL,
					(PBYTE)hardware_id,
					hardware_id_max_length,
					&hardware_id_actual_length);

		if(got_hardware_id == TRUE && hardware_id_actual_length > 0)
			hardware_id[hardware_id_actual_length-1] = '\0';
		else
			hardware_id[0] = '\0';

		#ifdef UNICODE
			std::string portName = utf8_encode(port_name);
			std::string friendlyName = utf8_encode(friendly_name);
			std::string hardwareId = utf8_encode(hardware_id);
		#else
			std::string portName = port_name;
			std::string friendlyName = friendly_name;
			std::string hardwareId = hardware_id;
		#endif

		CAENConnector::PortInfo port_entry;
		port_entry.port = portName;
		port_entry.description = friendlyName;
		port_entry.hardware_id = hardwareId;

		devices_found.push_back(port_entry);
	}

	SetupDiDestroyDeviceInfoList(device_info_set);

	return devices_found;
}
#endif // #if defined(_WIN32)


#if defined(__linux__)
std::vector<std::string> CAENConnector::glob(const std::vector<std::string>& patterns)
{
    std::vector<std::string> paths_found;
	if(patterns.size() == 0) return paths_found;
    glob_t glob_results;
    int glob_retval = ::glob(patterns[0].c_str(), 0, NULL, &glob_results);
    std::vector<std::string>::const_iterator iter = patterns.begin();
    while(++iter != patterns.end())
    {
        glob_retval = ::glob(iter->c_str(), GLOB_APPEND, NULL, &glob_results);
    }
    for(int path_index = 0; path_index < glob_results.gl_pathc; path_index++)
    {
        paths_found.push_back(glob_results.gl_pathv[path_index]);
    }
    globfree(&glob_results);
    return paths_found;
}

std::string CAENConnector::basename(const std::string& path)
{
    std::size_t pos = path.rfind("/");
    if(pos == std::string::npos) return path;
    return std::string(path, pos+1, std::string::npos);
}

std::string CAENConnector::dirname(const std::string& path)
{
    std::size_t pos = path.rfind("/");
    if(pos == std::string::npos) return path;
    else if(pos == 0) return "/";
    return std::string(path, 0, pos);
}

bool CAENConnector::path_exists(const std::string& path)
{
    struct stat sb;
    if( stat(path.c_str(), &sb ) == 0 ) return true;
    return false;
}

std::string CAENConnector::realpath(const std::string& path)
{
    char* real_path = ::realpath(path.c_str(), NULL);
    std::string result;
    if(real_path != NULL)
    {
        result = real_path;
        free(real_path);
    }
    return result;
}

std::string CAENConnector::usb_sysfs_friendly_name(const std::string& sys_usb_path)
{
    unsigned int device_number = 0;
    std::istringstream( read_line(sys_usb_path + "/devnum") ) >> device_number;
    std::string manufacturer = read_line( sys_usb_path + "/manufacturer" );
    std::string product = read_line( sys_usb_path + "/product" );
    std::string serial = read_line( sys_usb_path + "/serial" );
    if( manufacturer.empty() && product.empty() && serial.empty() ) return "";
    return format("%s %s %s", manufacturer.c_str(), product.c_str(), serial.c_str() );
}

std::vector<std::string> CAENConnector::get_sysfs_info(const std::string& device_path)
{
    std::string device_name = basename( device_path );
    std::string friendly_name;
    std::string hardware_id;
    std::string sys_device_path = format( "/sys/class/tty/%s/device", device_name.c_str() );
    if( device_name.compare(0,6,"ttyUSB") == 0 )
    {
        sys_device_path = dirname( dirname( realpath( sys_device_path ) ) );
        if( path_exists( sys_device_path ) )
        {
            friendly_name = usb_sysfs_friendly_name( sys_device_path );
            hardware_id = usb_sysfs_hw_string( sys_device_path );
        }
    }
    else if( device_name.compare(0,6,"ttyACM") == 0 )
    {
        sys_device_path = dirname( realpath( sys_device_path ) );
        if( path_exists( sys_device_path ) )
        {
            friendly_name = usb_sysfs_friendly_name( sys_device_path );
            hardware_id = usb_sysfs_hw_string( sys_device_path );
        }
    }
    else
    {
        // Try to read ID string of PCI device
        std::string sys_id_path = sys_device_path + "/id";
        if( path_exists( sys_id_path ) ) hardware_id = read_line( sys_id_path );
    }
    if( friendly_name.empty() )friendly_name = device_name;
    if( hardware_id.empty() )hardware_id = "n/a";
    std::vector<std::string> result;
    result.push_back(friendly_name);
    result.push_back(hardware_id);
    return result;
}

std::string CAENConnector::read_line(const std::string& file)
{
    std::ifstream ifs(file.c_str(), std::ifstream::in);
    std::string line;
    if(ifs)
    {
        getline(ifs, line);
    }
    return line;
}

std::string CAENConnector::format(const char* format, ...)
{
    va_list ap;
    std::size_t buffer_size_bytes = 256;
    std::string result;
    char* buffer = (char*)malloc(buffer_size_bytes);
    if( buffer == NULL ) return result;
    bool done = false;
    unsigned int loop_count = 0;
    while(!done)
    {
        va_start(ap, format);
        int return_value = vsnprintf(buffer, buffer_size_bytes, format, ap);
        if( return_value < 0 )
        {
            done = true;
        }
        else if( return_value >= buffer_size_bytes )
        {
            // Realloc and try again.
            buffer_size_bytes = return_value + 1;
            char* new_buffer_ptr = (char*)realloc(buffer, buffer_size_bytes);
            if( new_buffer_ptr == NULL )
            {
                done = true;
            }
            else
            {
                buffer = new_buffer_ptr;
            }
        }
        else
        {
            result = buffer;
            done = true;
        }
        va_end(ap);
        if( ++loop_count > 5 ) done = true;
    }
    free(buffer);
    return result;
}

std::string CAENConnector::usb_sysfs_hw_string(const std::string& sysfs_path)
{
    std::string serial_number = read_line( sysfs_path + "/serial" );
    if( serial_number.length() > 0 )
    {
        serial_number = format( "SNR=%s", serial_number.c_str() );
    }
    std::string vid = read_line( sysfs_path + "/idVendor" );
    std::string pid = read_line( sysfs_path + "/idProduct" );
    return format("USB VID:PID=%s:%s %s", vid.c_str(), pid.c_str(), serial_number.c_str() );
}

std::vector<CAENConnector::PortInfo> CAENConnector::list_ports()
{
    std::vector<PortInfo> results;

    std::vector<std::string> search_globs;
    search_globs.push_back("/dev/ttyACM*");
    search_globs.push_back("/dev/ttyS*");
    search_globs.push_back("/dev/ttyUSB*");
    search_globs.push_back("/dev/tty.*");
    search_globs.push_back("/dev/cu.*");

    std::vector<std::string> devices_found = glob( search_globs );

    std::vector<std::string>::iterator iter = devices_found.begin();

    while( iter != devices_found.end() )
    {
        std::string device = *iter++;

        std::vector<std::string> sysfs_info = get_sysfs_info( device );

        std::string friendly_name = sysfs_info[0];

        std::string hardware_id = sysfs_info[1];

        PortInfo device_entry;
        device_entry.port = device;
        device_entry.description = friendly_name;
        device_entry.hardware_id = hardware_id;
        results.push_back( device_entry );
    }
    return results;
}

#endif // defined(__linux__)

#if defined(__APPLE__)
using std::string;
using std::vector;

#define HARDWARE_ID_STRING_LENGTH 128
string cfstring_to_string( CFStringRef cfstring )
{
    char cstring[MAXPATHLEN];
    string result;
    if( cfstring )
    {
        Boolean success = CFStringGetCString( cfstring,
            cstring,
            sizeof(cstring),
            kCFStringEncodingASCII );

        if( success )
            result = cstring;
    }
    return result;
}

string get_device_path( io_object_t& serial_port )
{
    CFTypeRef callout_path;
    string device_path;

    callout_path = IORegistryEntryCreateCFProperty( serial_port,
        CFSTR(kIOCalloutDeviceKey),
        kCFAllocatorDefault,
        0 );

    if (callout_path)
    {
        if( CFGetTypeID(callout_path) == CFStringGetTypeID() )
            device_path = cfstring_to_string( static_cast<CFStringRef>(callout_path) );
        CFRelease(callout_path);
    }
    return device_path;
}

string get_class_name( io_object_t& obj )
{
    string result;
    io_name_t class_name;
    kern_return_t kern_result;
    kern_result = IOObjectGetClass( obj, class_name );
    if( kern_result == KERN_SUCCESS ) result = class_name;
    return result;
}

io_registry_entry_t get_parent_iousb_device( io_object_t& serial_port )
{
    io_object_t device = serial_port;
    io_registry_entry_t parent = 0;
    io_registry_entry_t result = 0;
    kern_return_t kern_result = KERN_FAILURE;
    string name = get_class_name(device);
    // Walk the IO Registry tree looking for this devices parent IOUSBDevice.
    while( name != "IOUSBDevice" )
    {
        kern_result = IORegistryEntryGetParentEntry( device,
        kIOServicePlane,
        &parent );
        if(kern_result != KERN_SUCCESS)
        {
            result = 0;
            break;
        }
        device = parent;
        name = get_class_name(device);
    }
    if(kern_result == KERN_SUCCESS) result = device;
    return result;
}

string get_string_property( io_object_t& device, const char* property )
{
    string property_name;

    if( device )
    {
        CFStringRef property_as_cfstring = CFStringCreateWithCString (
            kCFAllocatorDefault,
            property,
            kCFStringEncodingASCII );

        CFTypeRef name_as_cfstring = IORegistryEntryCreateCFProperty(
            device,
            property_as_cfstring,
            kCFAllocatorDefault,
            0 );

        if( name_as_cfstring )
        {
            if( CFGetTypeID(name_as_cfstring) == CFStringGetTypeID() )
                property_name = cfstring_to_string( static_cast<CFStringRef>(name_as_cfstring) );

            CFRelease(name_as_cfstring);
        }

        if(property_as_cfstring) CFRelease(property_as_cfstring);
    }
    return property_name;
}

uint16_t get_int_property( io_object_t& device, const char* property )
{
    uint16_t result = 0;
    if( device )
    {
        CFStringRef property_as_cfstring = CFStringCreateWithCString (
            kCFAllocatorDefault,
            property,
            kCFStringEncodingASCII );

        CFTypeRef number = IORegistryEntryCreateCFProperty( device,
            property_as_cfstring,
            kCFAllocatorDefault,
            0 );
        if(property_as_cfstring) CFRelease(property_as_cfstring);

        if( number )
        {
            if( CFGetTypeID(number) == CFNumberGetTypeID() )
            {
                bool success = CFNumberGetValue( static_cast<CFNumberRef>(number),
                    kCFNumberSInt16Type,
                    &result );
                if( !success ) result = 0;
            }
            CFRelease(number);
        }
    }
    return result;
}

string rtrim(const string& str)
{
    string result = str;
    string whitespace = " \t\f\v\n\r";
    std::size_t found = result.find_last_not_of(whitespace);
    if (found != std::string::npos) result.erase(found+1);
    else result.clear();
    return result;
}

vector<CAENConnector::PortInfo> CAENConnector::list_ports(void)
{
    vector<CAENConnector::PortInfo> devices_found;
    CFMutableDictionaryRef classes_to_match;
    io_iterator_t serial_port_iterator;
    io_object_t serial_port;
    mach_port_t master_port;
    kern_return_t kern_result;

    kern_result = IOMasterPort(MACH_PORT_NULL, &master_port);

    if(kern_result != KERN_SUCCESS) return devices_found;

    classes_to_match = IOServiceMatching(kIOSerialBSDServiceValue);

    if (classes_to_match == NULL) return devices_found;

    CFDictionarySetValue( classes_to_match, CFSTR(kIOSerialBSDTypeKey),CFSTR(kIOSerialBSDAllTypes) );

    kern_result = IOServiceGetMatchingServices(master_port, classes_to_match, &serial_port_iterator);

    if (KERN_SUCCESS != kern_result) return devices_found;

    while ( (serial_port = IOIteratorNext(serial_port_iterator)) )
    {
        string device_path = get_device_path( serial_port );
        io_registry_entry_t parent = get_parent_iousb_device( serial_port );
        IOObjectRelease(serial_port);

        if( device_path.empty() ) continue;

        CAENConnector::PortInfo port_info;
        port_info.port = device_path;
        port_info.description = "n/a";
        port_info.hardware_id = "n/a";

        string device_name = rtrim( get_string_property( parent, "USB Product Name" ) );
        string vendor_name = rtrim( get_string_property( parent, "USB Vendor Name") );
        string description = rtrim( vendor_name + " " + device_name );
        if( !description.empty() ) port_info.description = description;

        string serial_number = rtrim(get_string_property( parent, "USB Serial Number" ) );
        uint16_t vendor_id = get_int_property( parent, "idVendor" );
        uint16_t product_id = get_int_property( parent, "idProduct" );

        if( vendor_id && product_id )
        {
            char cstring[HARDWARE_ID_STRING_LENGTH];

            if(serial_number.empty()) serial_number = "None";

            int ret = snprintf( cstring, HARDWARE_ID_STRING_LENGTH, "USB VID:PID=%04x:%04x SNR=%s",
                vendor_id,
                product_id,
                serial_number.c_str() );

            if( (ret >= 0) && (ret < HARDWARE_ID_STRING_LENGTH) ) port_info.hardware_id = cstring;
        }
        devices_found.push_back(port_info);
    }
    IOObjectRelease(serial_port_iterator);
    return devices_found;
}
#endif // defined(__APPLE__)



Value CAENConnector::buildCommand(const std::vector<Value>& command2)
{
    std::vector<Value> command=command2;
    if(command[0].String()=="CAENHV_GetCrateMap") return getCrateMap(command);
    else if(command[0].String()=="CAENHV_ExecComm") return execComm(command);
    else if(command[0].String()=="CAENHV_GetBdParamProp") return getBdParamProp(command);
    else if(command[0].String()=="CAENHV_GetBdParam") return getBdParam(command);
    else if(command[0].String()=="CAENHV_GetBdParamInfo") return getBdParamInfo(command);
    else if(command[0].String()=="CAENHV_GetChName") return getChName(command);
    else if(command[0].String()=="CAENHV_GetChParamProp") return getChParamProp(command);
    else if(command[0].String()=="CAENHV_GetChParam") return getChParam(command);
    else if(command[0].String()=="CAENHV_GetChParamInfo") return getChParamInfo(command);
    else if(command[0].String()=="CAENHV_GetExecCommList") return getExecCommList(command);
    else if(command[0].String()=="CAENHV_GetSysPropList") return getSysPropList(command);
    else if(command[0].String()=="CAENHV_GetSysProp") return getSysProp(command);
    else if(command[0].String()=="CAENHV_GetSysPropInfo") return getSysPropInfo(command);
    else if(command[0].String()=="CAENHV_SetBdParam") return setBdParam(command);
    else if(command[0].String()=="CAENHV_SetChName") return setChName(command);
    else if(command[0].String()=="CAENHV_SetChParam") return setChParam(command);
    else if(command[0].String()=="CAENHV_SetSysProp") return setSysProp(command);
    else if(command[0].String()=="CAENHV_TestBdPresence") return testBdPresence(command);
    else
    {
        std::cout<<"Command unknown"<<std::endl;
        throw -3;
    }
}

