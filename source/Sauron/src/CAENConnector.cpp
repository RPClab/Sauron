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

CAENConnector::CAENConnector(const CAENConnector& other):CAENConnector()
{
    m_params=other.m_params;
}

CAENConnector::CAENConnector()
{
     m_type="CAENConnector";
}



CAENConnector::CAENConnector(const std::map<std::string,std::string>& params):CAENConnector()
{
    m_params=params;
}

CAENConnector& CAENConnector::operator=(const CAENConnector& other)
{
    m_params=other.m_params;
    m_type=other.m_type;
    return *this;
}

CAENConnector& CAENConnector::operator()(const CAENConnector& other)
{
    m_params=other.m_params;
    m_type=other.m_type;
    return *this;
}


void CAENConnector::Initialize()
{
   setCAENHVSystemType();
   setLinkType();
   setArg();
}

bool CAENConnector::IsConnected()
{
    if(m_connected==0) return true;
    else return false;
}

void CAENConnector::Release()
{
    
}

void CAENConnector::Disconnect()
{
    if(IsConnected())
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


void CAENConnector::Connect()
{
    if(!IsConnected()) 
    {
        m_connected=CAENHV_InitSystem(m_CAENHVSystemType,m_linkType,(void*)m_arg.c_str(),m_username.c_str(),m_password.c_str(),&m_handle);
        std::cout<<"CAENHV_InitSystem: "<<CAENHV_GetError(m_handle)<<"(num. "<<m_connected<<")\n";
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
        setCommData();
        m_arg=m_arg+m_commData+"_";
        setCommStop();
        m_arg=m_arg+m_commStop+"_";
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
        m_port=m_params["Port"].String();
    }
    else
    {
        std::string error="Port is mandatory ! \n";
        std::cout<<error;
        throw error;
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
        std::string error="Baudrate is mandatory ! \n";
        std::cout<<error;
        throw error;
    } 
}
void CAENConnector::setParity()
{
    if(m_params.hasParam("Parity"))
    {
        m_parity=m_params["Parity"].String();
    }
    else
    {
        std::string error="Parity is mandatory ! \n";
        std::cout<<error;
        throw error;
    } 
}

void CAENConnector::setCommData()
{
    if(m_params.hasParam("CommData"))
    {
        m_commData=m_params["CommData"].String();
    }
    else
    {
        std::string error="CommData is mandatory ! \n";
        std::cout<<error;
        throw error;
    } 
}

void CAENConnector::setCommStop()
{
    if(m_params.hasParam("CommStop"))
    {
        m_commStop=m_params["CommStop"].String();
    }
    else
    {
        std::string error="CommStop is mandatory ! \n";
        std::cout<<error;
        throw error;
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






Value CAENConnector::SendCommand(const std::string& command)
{
    Value com(command);
    std::vector<Value> params=com.Tokenize("*");
    if(params[0].String()=="CAENHV_GetCrateMap") return GetCrateMap(params);
    else if(params[0].String()=="CAENHV_ExecComm") return ExecComm(params);
    else if(params[0].String()=="CAENHV_GetBdParamProp") return GetBdParamProp(params);
    else if(params[0].String()=="CAENHV_GetBdParam") return GetBdParam(params);
    else if(params[0].String()=="CAENHV_GetBdParamInfo") return GetBdParamInfo(params);
    else if(params[0].String()=="CAENHV_GetChName") return GetChName(params);
    else if(params[0].String()=="CAENHV_GetChParamProp") return GetChParamProp(params);
    else if(params[0].String()=="CAENHV_GetChParam") return GetChParam(params);
    else if(params[0].String()=="CAENHV_GetChParamInfo") return GetChParamInfo(params);
    else if(params[0].String()=="CAENHV_GetExecCommList") return GetExecCommList(params);
    else if(params[0].String()=="CAENHV_GetSysPropList") return GetSysPropList(params);
    else if(params[0].String()=="CAENHV_GetSysProp") return GetSysProp(params);
    else if(params[0].String()=="CAENHV_GetSysPropInfo") return GetSysPropInfo(params);
    else if(params[0].String()=="CAENHV_SetBdParam") return SetBdParam(params);
    else if(params[0].String()=="CAENHV_SetChName") return SetChName(params);
    else if(params[0].String()=="CAENHV_SetChParam") return SetChParam(params);
    else if(params[0].String()=="CAENHV_SetSysProp") return SetSysProp(params);
    else if(params[0].String()=="CAENHV_TestBdPresence") return TestBdPresence(params);
    else
    {
        std::cout<<"Command unknown"<<std::endl;
        throw -3;
    }
}

