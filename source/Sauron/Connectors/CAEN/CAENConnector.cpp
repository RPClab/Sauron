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
            throw -1;
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

