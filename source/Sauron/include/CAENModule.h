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

class CAENModule : public Module
{
public:
    CAENModule():Module(){};
    CAENModule(Connector* connector):Module(connector){};
    CAENModule(Connector& connector):Module(connector){};
    void on(const Value& channel)
    {
       // SendCommand();
    }
    void off(const Value& channel)
    {
       // SendCommand();
    }
    void setVoltage(const Value& channel,const Value& HV)
    {
       // SendCommand();
    }
    Value getVoltage(const Value& channel)
    {
       // return SendCommand();
    }
    
    Value mesureVoltage(const Value& channel)
    {
       //return SendCommand();
    }
    
    void setCurrent(const Value& channel,const Value& current)
    {
       // SendCommand();
    }
    
    virtual Value getCurrent(const Value& channel)
    {
        //return SendCommand();
    }
    
    virtual Value mesureCurrent(const Value& channel)
    {
       // return SendCommand();
    }
    
    
    
    CAENModule* Clone() { return new CAENModule(*this);}
    CAENModule* Clone() const { return new CAENModule(*this);} 
private:
    std::vector<Value> m_BDParamsList;
    void setBDParamsLists()
    {
        Value params=SendCommand("CAENHV_GetBdParamInfo*"+m_slot.String());
        m_BDParamsList=params.Tokenize(", ");
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
        std::cout<<SendCommand("CAENHV_GetBdParamInfo*"+m_slot.String())<<std::endl;
        return SendCommand("CAENHV_GetCrateMap*"+m_slot.String());
    }
};
#endif
