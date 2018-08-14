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
        SendCommand("CAENHV_SetChParam*"+m_slot.String()+"*Pw*"+channel.String()+"*1");
    }
    //OFF
    void off(const Value& channel)
    {
        SendCommand("CAENHV_SetChParam*"+m_slot.String()+"*Pw*"+channel.String()+"*0");
    }
    //SET VOLTAGE
    void setVoltage(const Value& channel,const Value& HV)
    {
       SendCommand("CAENHV_SetChParam*"+m_slot.String()+"*VSet*"+channel.String()+"*"+HV.String());
    }
    //GET VOLTAGE
    Value getVoltage(const Value& channel)
    {
        Value a=SendCommand("CAENHV_GetChParam*"+m_slot.String()+"*VSet*"+channel.String());
        Value exp=SendCommand("CAENHV_GetChParamProp*"+m_slot.String()+"*"+channel.String()+"*VSet*Exp");
        Value vol=a.String()+"e"+exp.String();
        return vol;
    }
    //MEASURE VOLTAGE
    Value mesureVoltage(const Value& channel)
    {
        Value a= SendCommand("CAENHV_GetChParam*"+m_slot.String()+"*VMon*"+channel.String());
        Value exp=SendCommand("CAENHV_GetChParamProp*"+m_slot.String()+"*"+channel.String()+"*VMon*Exp");
        Value vol=a.String()+"e"+exp.String();
        return vol;
    }
    //SET CURRENT
    void setCurrent(const Value& channel,const Value& current)
    {
       SendCommand("CAENHV_SetChParam*"+m_slot.String()+"*ISet*"+channel.String()+"*"+current.String());
    }
    //GET CURRENT
    virtual Value getCurrent(const Value& channel)
    {
        Value a=SendCommand("CAENHV_GetChParam*"+m_slot.String()+"*ISet*"+channel.String());
        Value exp=SendCommand("CAENHV_GetChParamProp*"+m_slot.String()+"*"+channel.String()+"*ISet*Exp");
        Value vol=a.String()+"e"+exp.String();
        return vol;
    }
    
    virtual Value mesureCurrent(const Value& channel)
    {
       Value a;
       Value exp;
       if(ChannelHasParameter(channel,"IMon")==true)
       {
            a= SendCommand("CAENHV_GetChParam*"+m_slot.String()+"*IMon*"+channel.String());
            exp=SendCommand("CAENHV_GetChParamProp*"+m_slot.String()+"*"+channel.String()+"*IMon*Exp");
       }
       else
       {
            Value HL=SendCommand("CAENHV_GetChParam*"+m_slot.String()+"*ImonRange*"+channel.String());
            if(HL.String()==""||HL.String()=="1")
            {
                a= SendCommand("CAENHV_GetChParam*"+m_slot.String()+"*IMonL*"+channel.String());
                exp=SendCommand("CAENHV_GetChParamProp*"+m_slot.String()+"*"+channel.String()+"*IMonL*Exp");
            }
            else
            {
                 a= SendCommand("CAENHV_GetChParam*"+m_slot.String()+"*IMonH*"+channel.String());
                exp=SendCommand("CAENHV_GetChParamProp*"+m_slot.String()+"*"+channel.String()+"*IMonH*Exp");
            }
       }
        Value vol=a.String()+"e"+exp.String();
        return vol;
    }
    
    
    
    CAENModule* Clone() { return new CAENModule(*this);}
    CAENModule* Clone() const { return new CAENModule(*this);} 
private:
    bool ChannelHasParameter(const Value& channel,const std::string& param)
    {
        Value list= SendCommand("CAENHV_GetChParamInfo*"+m_slot.String()+"*"+channel.String());
        std::vector<Value>vec= list.Tokenize(", ");
        std::vector<Value>::iterator it=find(vec.begin(),vec.end(),param);
        if(it==vec.end()) return false;
        else return true;
        
    }
    void FillInfos()
    {
        std::vector<Value> infos=ID().Tokenize("*");
        m_nbrOfChannels=infos[2];
        m_model=infos[0];
        m_serialNumber=infos[4];
        m_version=infos[3];
    }
    Value ID()
    { 
        Value ret=SendCommand("CAENHV_GetCrateMap*"+m_slot.String());
        if(ret.Size()==0)
        {
            throw -3;
        }
        else return ret;
    }
};
#endif
