#include "isegNHSR.h"
#include <iostream>
#include<string>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>


isegNHSR::isegNHSR(std::map<std::string,std::string>& params):m_serial(params)
{
    Set();
}


bool isegNHSR::Connect()
{
    if(!m_serial.IsConnected()) 
    {
        m_serial.Connect();
    }
    return IsConnected();
}


bool isegNHSR::Set()
{
    Connect();
    bool connected= IsConnected();
    if(IsModule())
    {
        nbrChannel=NbrOfChannels().Int();
        std::vector<Value> infos=Tokenize(ID().String(),",");
        Compagny=infos[0];
        Model=infos[1];
        SerialNumber=infos[2];
        Version=infos[3];
     }
     Disconnect();
     return connected;
}

bool isegNHSR::IsConnected()
{
    if(m_serial.IsConnected())return true;
    else return false;
}

Value isegNHSR::GetSerialNumber()
{
    return SerialNumber;
}


void isegNHSR::Disconnect()
{
   if(IsConnected()) Disconnect();
}

Value isegNHSR::SendCommand(const std::string& command)
{
    return m_serial.SendCommand(command);
}

Value isegNHSR::ID()
{
    return SendCommand("*IDN?");
}

void isegNHSR::Clear()
{
    SendCommand("*CLS");
}

void isegNHSR::Reset()
{
    SendCommand("*RST"); 
}

Value isegNHSR::InstructionSet()
{
    return  SendCommand("*INSTR?"); 
}

void isegNHSR::SetVoltage(const int& channel,const double& HV)
{
    SendCommand(":VOLT "+std::to_string(HV)+",(@"+std::to_string(channel)+")");
}
    
void isegNHSR::SetVoltage(const double& HV)
{
    for(unsigned int i=0;i!=nbrChannel;++i) SetVoltage(i,HV);
}

void isegNHSR::On(const int& channel)
{
    SendCommand(":VOLT ON,(@"+std::to_string(channel)+")");
}

void isegNHSR::On()
{
    for(unsigned int i=0;i!=nbrChannel;++i) On(i);
}

void isegNHSR::Off(const int& channel)
{
    SendCommand(":VOLT OFF,(@"+std::to_string(channel)+")");
}
      
void isegNHSR::Off()
{
    for(unsigned int i=0;i!=nbrChannel;++i) Off(i);
}
 
void isegNHSR::Emergency(const int& channel)
{
   SendCommand(":VOLT EMCY OFF,(@"+std::to_string(channel)+")");  
}

void isegNHSR::Emergency()
{
    for(unsigned int i=0;i!=nbrChannel;++i)
    {
        Emergency(i);
         std::this_thread::sleep_for(std::chrono::nanoseconds(1500));
    }
}  

void isegNHSR::ClearEmergency(const int& channel) 
{
    SendCommand(":VOLT EMCY CLR,(@"+std::to_string(channel)+")");
}

void isegNHSR::ClearEmergency()
{
    for(unsigned int i=0;i!=nbrChannel;++i)
    {
        ClearEmergency(i);
        std::this_thread::sleep_for(std::chrono::nanoseconds(1500));
    }
}

void isegNHSR::SetVoltageBound(const int& channel, const double& HV)
{
    SendCommand(":VOLT:BOU "+std::to_string(HV)+",(@"+std::to_string(channel)+")");
}

void isegNHSR::SetVoltageBound(const double& HV)
{
     for(unsigned int i=0;i!=nbrChannel;++i)SetVoltageBound(i,HV);  
}
   
void isegNHSR::SetCurrent(const int& channel,const double& current)
{
    SendCommand(":CURR "+std::to_string(current)+",(@"+std::to_string(channel)+")");
}   
   
void isegNHSR::SetCurrent(const double& current)
{
    for(unsigned int i=0;i!=nbrChannel;++i)SetCurrent(i);
}

void isegNHSR::SetCurrentBound(const int& channel, const double& current)
{
    SendCommand(":CURR:BOU "+std::to_string(current)+",(@"+std::to_string(channel)+")");
}

void isegNHSR::SetCurrentBound(const double& current)
{
    for(unsigned int i=0;i!=nbrChannel;++i)SetCurrentBound(i,current);
}

void isegNHSR::ClearEvent(const int& channel)
{
    SendCommand(":EV CLEAR,(@"+std::to_string(channel)+")");
}

void isegNHSR::ClearEvent()
{
    for(unsigned int i=0;i!=nbrChannel;++i)ClearEvent(i);
}

void isegNHSR::SetTripTime(const int& channel, const int& time)
{
    SendCommand(":CONF:TRIP:TIME "+std::to_string(time)+",(@"+std::to_string(channel)+")");
}

void isegNHSR::SetTripTime(const int& time)
{
    for(unsigned int i=0;i!=nbrChannel;++i)SetTripTime(i,time);
}

Value isegNHSR::GetTripTime(const int& channel)
{
    return SendCommand(":CONF:TRIP:TIME?,(@"+std::to_string(channel)+")");
}

void isegNHSR::SetTripAction(const int& channel,const int& action)
{
    SendCommand(":CONF:TRIP:ACT "+std::to_string(action)+",(@"+std::to_string(channel)+")");
}

void isegNHSR::SetTripAction(const int& action)
{
    for(unsigned int i=0;i!=nbrChannel;++i)SetTripAction(i,action);
}


Value isegNHSR::GetTripAction(const int& channel)
{
    return SendCommand(":CONF:TRIP:ACT?(@"+std::to_string(channel)+")");
}
/*
void isegNHSR::SetInhibitAction(const int& channel,const int& action)
{
   SendCommand(":CONF:INHI:ACT "+std::to_string(action)+",(@"+std::to_string(channel)+")"); 
}

void isegNHSR::SetInhibitAction(const int& action)
{
   for(unsigned int i=0;i!=nbrChannel;++i)SetInhibitAction(i,action); 
}
*/
Value isegNHSR::GetInhibitAction(const int& channel)
{
    return SendCommand(":CONF:INH:ACT?(@"+std::to_string(channel)+")"); 
}







    

Value isegNHSR::CleanString(std::string stri)
{
   stri.erase(std::remove_if(stri.begin(), stri.end(),[](const char & element){return std::isspace(element);}),stri.end());
   return stri;
}



std::vector<Value> isegNHSR::Tokenize(const std::string& str, const std::string& delimiters)
{
    std::vector<Value> tokens;
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first non-delimiter.
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) 
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next non-delimiter.
        pos = str.find_first_of(delimiters, lastPos);
    }
    return std::move(tokens);
}




bool isegNHSR::IsMySerialNumber(const std::string sn)
{
    if(SerialNumber==sn) return true;
    else return false;
}


Value isegNHSR::MeasureCurrent(const int& channel) 
{
 return SendCommand(":MEAS:CURR?(@"+std::to_string(channel)+")");
}

Value isegNHSR::MeasureVoltage(const int& channel)
{
    return SendCommand(":MEAS:VOLT?(@"+std::to_string(channel)+")");
}


std::vector<Value> isegNHSR::MeasureCurrent()
{
        std::vector<Value> Currents;
        Currents.reserve(NbrOfChannels().Int());
        for(unsigned int i=0;i!=nbrChannel;++i)
        {
            Currents.push_back(MeasureCurrent(i));
        }
        return std::move(Currents); 
}


std::vector<Value> isegNHSR::MeasureVoltage()
{
    std::vector<Value> Voltages;
    Voltages.reserve(NbrOfChannels().Int());
    for(unsigned int i=0;i!=nbrChannel;++i)
    {
        Voltages.push_back(MeasureVoltage(i));
    }
    return std::move(Voltages);
}


    Value isegNHSR::GetVoltage(const int& channel)
    {
        return SendCommand(":READ:VOLT?(@"+std::to_string(channel)+")");
    }

    Value isegNHSR::GetCurrent(const int& channel)
    {
        return SendCommand(":READ:CURR?(@"+std::to_string(channel)+")");
    }


std::vector<Value> isegNHSR::GetVoltage()
{
    std::vector<Value> Voltages;
    Voltages.reserve(NbrOfChannels().Int());
    for(unsigned int i=0;i!=nbrChannel;++i)
    {
        Voltages.push_back(GetVoltage(i));
    }
    return std::move(Voltages);
}


    
    std::vector<Value> isegNHSR::GetCurrent()
    {
        std::vector<Value> Currents;
        Currents.reserve(NbrOfChannels().Int());
        for(unsigned int i=0;i!=nbrChannel;++i)
        {
            Currents.push_back(GetCurrent(i));
        }
        return std::move(Currents); 
    }



/*
std::string isegNHSR::GetTemperature(const int& channel)
{
    return SendCommand(":READ:CHAN:TEMP?(@"+std::to_string(channel)+")");
}
*/

//Test if it understand what I mean ! If yes than it,s a module
bool isegNHSR::IsModule()
{
    try
    {
        //For SCPI module ID is always defined (protocol rules)
        Value test=ID();
    }
    catch (const std::out_of_range& e) 
    {
        return false;
    }
    return true;
}


Value isegNHSR::NbrOfChannels()
{
    return SendCommand(":READ:MOD:CHAN?");
}
