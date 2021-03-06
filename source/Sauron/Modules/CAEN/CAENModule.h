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


#ifndef CAENMODULE_H
#define CAENMODULE_H
#include "Module.h"
#include <string>
#include "CAENUtils.h"
class CAENModule : public Module
{
public:
    CAENModule():Module(){};
    CAENModule(Connector* connector):Module(connector){};
    CAENModule(Connector& connector):Module(connector){};
    
    //ON
    void on(const Value& channel)
    {
        command("CAENHV_SetChParam",m_slot.String(),"Pw",channel.String(),"1");
    }
    //OFF
    void off(const Value& channel)
    {
        command("CAENHV_SetChParam",m_slot.String(),"Pw",channel.String(),"0");
    }
    //SET VOLTAGE
    void setVoltage(const Value& channel,const Value& HV)
    {
       command("CAENHV_SetChParam",m_slot.String(),"VSet",channel.String(),HV.String());
    }
    //GET VOLTAGE
    VoltageSet getVoltage(const Value& channel)
    {
        Value a=command("CAENHV_GetChParam",m_slot.String(),"VSet",channel.String());
        Value exp=getProperty(channel,"VSet","Exp");
        //Value exp=command("CAENHV_GetChParamProp",m_slot.String(),channel.String(),"VSet","Exp");
        Value vol=a.String()+"e"+exp.String();
        VoltageSet ret;
        ret.setVoltage(vol);
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //MEASURE VOLTAGE
    VoltageMeasured mesureVoltage(const Value& channel)
    {
        Value a= command("CAENHV_GetChParam",m_slot.String(),"VMon",channel.String());
        Value exp=getProperty(channel,"VMon","Exp");
       // Value exp=command("CAENHV_GetChParamProp",m_slot.String(),channel.String(),"VMon","Exp");
        Value vol=a.String()+"e"+exp.String();
        VoltageMeasured ret;
        ret.setMeasuredVoltage(vol);
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //SET CURRENT
    void setCurrent(const Value& channel,const Value& current)
    {
       command("CAENHV_SetChParam",m_slot.String(),"ISet",channel.String(),current.String());
    }
    //GET CURRENT
    virtual CurrentSet getCurrent(const Value& channel)
    {
        Value a=command("CAENHV_GetChParam",m_slot.String(),"ISet",channel.String());
        //Value exp=command("CAENHV_GetChParamProp",m_slot.String(),channel.String(),"ISet","Exp");
        Value exp=getProperty(channel,"ISet","Exp");
        Value vol=a.String()+"e"+exp.String();
        CurrentSet ret;
        ret.setCurrent(vol);
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //MEASURE CURRENT
    virtual CurrentMeasured mesureCurrent(const Value& channel)
    {
       Value a;
       Value exp;
       if(channelHasParameter(channel,"IMon")==true)
       {
            a= command("CAENHV_GetChParam",m_slot.String(),"IMon",channel.String());
            exp=getProperty(channel,"IMon","Exp");
           // exp=command("CAENHV_GetChParamProp",m_slot.String(),channel.String(),"IMon","Exp");
       }
       else
       {
            Value HL=command("CAENHV_GetChParam",m_slot.String(),"ImonRange",channel.String());
            if(HL.String()=="1")
            {
                a= command("CAENHV_GetChParam",m_slot.String(),"IMonL",channel.String());
                //exp=command("CAENHV_GetChParamProp",m_slot.String(),channel.String(),"IMonL","Exp");
                exp=getProperty(channel,"IMonL","Exp");
            }
            else
            {
                 a= command("CAENHV_GetChParam",m_slot.String(),"IMonH",channel.String());
               // exp=command("CAENHV_GetChParamProp",m_slot.String(),channel.String(),"IMonH","Exp");
                 exp=getProperty(channel,"IMonH","Exp");
            }
       }
        Value vol=a.String()+"e"+exp.String();
        CurrentMeasured ret;
        ret.setMeasuredCurrent(vol);
        ret.getPosition().setChannel(channel);
        ret.getPosition().setModule(m_name);
        return ret;
    }
    //GET CHANNEL STATUS
    virtual Status getStatus(const Value& channel)
    {
        Value status=command("CAENHV_GetChParam",m_slot.String(),"ChStatus",channel.String());
        return m_channelStatus(status.LLong());
    };
    //GET MODULE STATUS 
    virtual Status getModuleStatus()
    {
        Value status=command("CAENHV_GetBdParam",m_slot.String(),"Alarm");
        return m_moduleStatus(status.LLong());
    };
    //SET EMERGENCY  
    virtual void setEmergency(const Value & channel )
    {
        command("CAENHV_SetChParam",m_slot.String(),"PDwn",channel.String(),"0");
    }
    
    
    CAENModule* clone() { return new CAENModule(*this);}
    CAENModule* clone() const { return new CAENModule(*this);} 
private:
    std::vector<std::map<Value,ParProp>> m_paramsChannels;
    void setChannelStatusBits()
    {
         m_channelStatus
         ("ON","On",0,1)
         ("RUP","Channel Ramp UP",1,1)
         ("RDW","Channel Ramp DOWN",2,1)
         ("OVC","IMON >= ISET",3,1)
         ("OVV","VMOV > VSET + 250V",4,1)
         ("UNV","VMON < VSET - 250V",5,1)
         ("MAXV","VOUT in MAXV protection",6,1)
         ("TRIP","Ch OFF via TRIP (Imon >= Iset during TRIP)",7,1)
         ("OVP","Power Max, Power Out > 1.7W",8,1)
         ("OVT","TEMP > 105C",9,1)
         ("DIS","Ch disabled (REMOTE Mode and Switch on OFF position)",10,1)
         ("KILL","Ch in KILL via front panel",11,1)
         ("ILK","Ch in INTERLOCK via front panel",12,1)
         ("NOCAL","Calibration Error",13,1);
    }
    void setModuleStatusBits()
    {
         m_moduleStatus
         ("CH0","Ch0 in Alarm status",0,1)
         ("CH1","Ch1 in Alarm status",1,1)
         ("CH2","Ch1 in Alarm status",2,1)
         ("CH3","Ch1 in Alarm status",3,1)
         ("PWFAIL","Board in POWER FAIL",4,1)
         ("OVP","Board in OVER POWER",5,1)
         ("HVCKFAIL","Internal HV Clock Fail (!=200+-10kHz)",6,1);
    }
    
    void fillParamsChannels()
    {
        for(int i=0;i!=m_nbrOfChannels.Int();++i)
        {
            Value list= command("CAENHV_GetChParamInfo",m_slot.String(),i);
            std::vector<Value> param = list.Tokenize(", ");
            std::map<Value,ParProp> p;
            for(unsigned int j=0;j!=param.size();++j)
            {
                Value par=command("CAENHV_GetChParamProp",m_slot.String(),i,param[j],"All");
                ParProp par2(par);
                p.insert(std::pair<Value,ParProp>(param[j],par2));
            }
            m_paramsChannels.push_back(p);
        }
    }
    
    bool channelHasParameter(const Value& channel,const std::string& param)
    {
        if(m_paramsChannels[channel.UInt()].find(param)!=m_paramsChannels[channel.UInt()].end()) return true;
        return false;
    }
    
    ParProp getParameter(const Value& channel,const Value& param)
    {
        if(m_paramsChannels[channel.UInt()].find(param)!=m_paramsChannels[channel.UInt()].end()) return m_paramsChannels[channel.UInt()][param];
        else
        {
            std::cout<<"Parameter "<<param.String()<<" unknown !"<<std::endl;
            throw -1;
        }
    }
    
    Value getProperty(const Value& channel,const std::string& param,const std::string& property)
    {
        static const int PARAM_TYPE_NUMERIC{0};
        static const int PARAM_TYPE_ONOFF{1};
        ParProp respond=getParameter(channel,param);
        if(property=="Type") return respond.getType();
        else if (property=="Mode") return respond.getMode();
        else if(respond.getType() == PARAM_TYPE_NUMERIC)
        {
            if(property=="MinVal") return respond.getMinVal();
            else if(property=="MaxVal") return respond.getMaxVal();
            else if(property=="Unit") return respond.getUnit();
            else if(property=="Exp") return respond.getExp();
            else
            {
                throw -1;
            }
        }
        else if(respond.getType() == PARAM_TYPE_ONOFF)
        {
            if(property=="OnState") return respond.getOnState();
            else if(property=="OffState") return respond.getOffState();
            else
            {
                throw -1;
            }
        }
        else
        {
            std::cout<<"What !!"<<std::endl;
            throw -1;
        } 
    }
    
    void fillInfos()
    {
        std::vector<Value> infos=ID().Tokenize("*");
        m_nbrOfChannels=infos[2];
        m_model=infos[0];
        m_serialNumber=infos[4];
        m_version=infos[3];
        fillParamsChannels();
    }
    Value ID()
    { 
        Value ret=command("CAENHV_GetCrateMap",m_slot.String());
        if(ret.Size()==0)
        {
            throw -3;
        }
        else return ret;
    }
};

class CAENModuleDriver : public ModuleDriver
{
public:
    CAENModuleDriver() : ModuleDriver("CAEN", Module::getPluginVersion()) {}
    Module* create() {return new CAENModule();}
};
#endif
