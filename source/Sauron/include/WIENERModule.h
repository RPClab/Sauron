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


#ifndef WIENERMODULE_H
#define WIENERMODULE_H
#include "Module.h"
#include <string>

class WIENERModule : public Module
{
public:
    WIENERModule():Module(){};
    WIENERModule(Connector* connector):Module(connector){};
    WIENERModule(Connector& connector):Module(connector){};
    //ON
    void on(const Value& channel)
    {
        command("***SEND***outputSwitch.u"+goodChannelNumber(channel).String()+"***VALUE***1");
    }
    //OFF
    void off(const Value& channel)
    {
        command("***SEND***outputSwitch.u"+goodChannelNumber(channel).String()+"***VALUE***0");
    }
    //SET VOLTAGE
    void setVoltage(const Value& channel,const Value& HV)
    {
        command("***SEND***outputVoltage.u"+goodChannelNumber(channel).String()+"***VALUE***"+HV.String());
    }
    //GET VOLTAGE
    VoltageSet getVoltage(const Value& channel)
    {
        VoltageSet ret;
        ret.setVoltage(command("***RECEIVE***outputVoltage.u"+goodChannelNumber(channel).String()));
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //MEASURE VOLTAGE
    VoltageMeasured mesureVoltage(const Value& channel)
    {
        VoltageMeasured ret;
        ret.setMeasuredVoltage(command("***RECEIVE***outputMeasurementTerminalVoltage.u"+goodChannelNumber(channel).String()));
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //SET CURRENT
    void setCurrent(const Value& channel,const Value& current)
    {
        command("***SEND***outputCurrent.u"+goodChannelNumber(channel).String()+"***VALUE***"+goodChannelNumber(current).String());
    }
    //GET CURRENT
    virtual CurrentSet getCurrent(const Value& channel)
    {
        CurrentSet ret;
        ret.setCurrent(command("***RECEIVE***outputCurrent.u"+goodChannelNumber(channel).String()));
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //MEASURE CURRENT 
    virtual CurrentMeasured mesureCurrent(const Value& channel)
    {
        CurrentMeasured ret;
        ret.setMeasuredCurrent(command("***RECEIVE***outputMeasurementCurrent.u"+goodChannelNumber(channel).String()));
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //GET CHANNEL STATUS
    virtual Status getStatus(const Value& channel)
    {
        Value status=command("***RECEIVE***outputStatus.u"+goodChannelNumber(channel).String());
        return m_channelStatus(status.LLong());
        return 0;
    };
    //GET MODULE STATUS 
    virtual Status getModuleStatus()
    {
        Value status=command("***RECEIVE***moduleStatus."+m_slot.String());
        return m_moduleStatus(status.LLong());
    };
    //SET EMERGENCY  
    virtual void setEmergency(const Value & channel )
    {
        command("***SEND***outputSwitch.u"+goodChannelNumber(channel).String()+"***VALUE***3");
    }
    
    WIENERModule* clone() { return new WIENERModule(*this);}
    WIENERModule* clone() const { return new WIENERModule(*this);} 
private:
    void setModuleStatusBits()
    {
         if(m_compagny=="iseg")//SHOULD BE HIGH VOLTAGE MPOD manual version 2.8 page 36
         {
            m_moduleStatus
            ("FA","Module has reached state fine adjustment",0,1)
            ("LI","Module is in state live insertion",2,1)
            ("NS","Hardware failure detected (consult iseg support)",4,1)
            ("HLVG","Hardware limit voltage in proper range, using for HV distributor modules with current mirror only",5,1)
            ("IE","Input error in connection with module access",6,1)
            ("NSE","All channels without any failure",8,1)
            ("NR","All channels stable, no ramp active",9,1)
            ("SLG","Safety loop is closed",10,1)
            ("EA","Any event is active and mask is set",11,1)
            ("G","Module state is good",12,1)
            ("SG","Power supply is good",13,1)
            ("TG","Module temperature is good",14,1)
            ("KE","Module state is kill enable",15,1);
         }
    }
    void setChannelStatusBits()
    {
            //MPOD manual version 2.8 page 30
            m_channelStatus
            ("ON","output channel is on",0,1)
            ("I","external (hardware-)inhibit of the output channel",1,1)
            ("FMinSV","Sense voltage is too low",2,1)
            ("FMaxSV","Sense voltage is too high",3,1)
            ("FMTV","Terminal voltage is too high",4,1)
            ("FMC","Current is too high",5,1)
            ("FMT","Head sink temperature is too high",6,1)
            ("FMP","Output power is too high",7,1)
            ("FT","Communication timeout",9,1)
            ("CL","Current limiting is active (constant current mode)",10,1)
            ("RU","Output voltage is increasing",11,1)
            ("RD","Output voltage is decreasing",12,1)
            ("EK","EnableKill is active",13,1)
            ("EO","EmergencyOff event is active",14,1)
            ("Adj","Fine adjustment is working",15,1)
            ("CV","Voltage control (constant voltage mode)",16,1)
            ("LCR","Channel is in low current measurement range",17,1)
            ("CBE","Output current is out of bounds",18,1)
            ("FCL","Hardware current limit (EHS)/ trip (EDS,EBS) was exceeded",19,1);
    }
    void fillInfos()
    {
        std::vector<Value> infos=ID().Tokenize(", ");
        m_nbrOfChannels=infos[2];
        m_compagny=infos[0];
        m_model=infos[1];
    }
    Value ID()
    {
       Value ret=command("***RECEIVE***moduleDescription."+m_slot.String());
       if(ret.Size()==0)
       {
           throw -3;
       }
       else return ret;
    }
};
#endif
