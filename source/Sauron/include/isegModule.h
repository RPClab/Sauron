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


#ifndef ISEGMODULE_H
#define ISEGMODULE_H
#include "Module.h"
#include <string>

class isegModule : public Module
{
public:
    isegModule():Module(){};
    isegModule(Connector* connector):Module(connector){};
    isegModule(Connector& connector):Module(connector){};
    
    //ON
    void on(const Value& channel)
    {
        SendCommand(":VOLT ON,(@"+channel.String()+")");
    }
    //OFF
    void off(const Value& channel)
    {
        SendCommand(":VOLT OFF,(@"+channel.String()+")");
    }
    //SET VOLTAGE
    void setVoltage(const Value& channel,const Value& HV)
    {
        SendCommand(":VOLT "+HV.String()+",(@"+channel.String()+")");
    }
    //GET VOLTAGE
    Value getVoltage(const Value& channel)
    {
        return SendCommand(":READ:VOLT?(@"+channel.String()+")");
    }
    //MEASURE VOLTAGE
    Value mesureVoltage(const Value& channel)
    {
       return SendCommand(":MEAS:VOLT?(@"+channel.String()+")"); 
    }
    //SET CURRENT
    void setCurrent(const Value& channel,const Value& current)
    {
        SendCommand(":CURR "+current.String()+",(@"+channel.String()+")");
    }
    //GET CURRENT
    virtual Value getCurrent(const Value& channel)
    {
        return SendCommand(":READ:CURR?(@"+channel.String()+")");
    }
    //MEASURE CURRENT
    virtual Value mesureCurrent(const Value& channel)
    {
        getStatus(channel);
        return SendCommand(":MEAS:CURR?(@"+channel.String()+")");
    }
    //GET STATUS
    Status getStatus(const Value& channel)
    {
        Value status =SendCommand(":READ:CHAN:STAT?(@"+channel.String()+")");
        return m_channelStatus(status.LLong());
    }
    //GET MODULE STATUS
    Status getModuleStatus()
    {
        Value status =SendCommand(":READ:MOD:STAT?");
        return m_moduleStatus(status.LLong());
    }
    isegModule* Clone() { return new isegModule(*this);}
    isegModule* Clone() const { return new isegModule(*this);} 
private:
    void setChannelStatusBits()//NHR and NHS devices with USB and CAN Interface version 1.18 page 12
    {
         m_channelStatus
         ("POS","Polarity of the HV (for devices with EPU only)",0,1)
         ("IERR","Input error",2,1)
         ("ON","On",3,1)
         ("RAMP","Ramping",4,1)
         ("EMCY","Emergency off without ramp",5,1)
         ("CC","Current control active (evaluation is guaranteed when no ramp is running)",6,1)
         ("CV","Voltage control active (evaluation is guaranteed when no ramp is running)",7,1)
         ("LCR","Device measures in Low Current Range (for devices with 2 current ranges only)",8,1)
         ("CBND","Current out of bounds",10,1)
         ("VBND","Voltage out of bounds",11,1)
         ("EINH","External Inhibit",12,1)
         ("TRP","Trip is set when Voltage or Current limit or Iset has been exceeded (when KillEnable=1)",13,1)
         ("CLIM","Current limit set by Imax is exceeded",14,1)
         ("VLIM","Voltage limit set by Vmax is exceeded",15,1);
    }
    void setModuleStatusBits()//NHR and NHS devices with USB and CAN Interface version 1.18 page 14
    {
         m_moduleStatus
         ("ADJ","Mode of the fine adjustement",0,1)
         ("S","Hardware failure detected (consult iseg Spezialelektronik GmbH",4,1)
         ("HVL","isHwVLgd",5,1)
         ("IERR","Input error in connection with a module access",6,1)
         ("SERR","Module without failure",8,1)
         ("RAMP","All channel stable, no ramp active",9,1)
         ("SFLP","Safety loop closed",10,1)
         ("EVNT","Any event is active and mask is set",11,1)
         ("MOD","Module in state good",12,1)
         ("SPLY","Power supply good",13,1)
         ("TMP","Module temperature good",14,1)
         ("KIL","Module state of kill enable",15,1);
    }
    void FillInfos()
    {
        m_nbrOfChannels=NbrOfChannels();
        std::vector<Value> infos=ID().Tokenize(",");
        m_compagny=infos[0];
        m_model=infos[1];
        m_serialNumber=infos[2];
        m_version=infos[3];
    }
    Value ID()
    {
        Value ret =SendCommand("*IDN?");
        if(ret.Size()==0)
        {
            throw -3;
        }
        else return ret;
    }
    Value NbrOfChannels()
    {
        return SendCommand(":READ:MOD:CHAN?");
    }
};
#endif
