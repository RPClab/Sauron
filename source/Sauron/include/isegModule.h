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
    void on(const Value& channel)
    {
        SendCommand(":VOLT ON,(@"+channel.String()+")");
    }
    void off(const Value& channel)
    {
        SendCommand(":VOLT OFF,(@"+channel.String()+")");
    }
    void setVoltage(const Value& channel,const Value& HV)
    {
        SendCommand(":VOLT "+HV.String()+",(@"+channel.String()+")");
    }
    Value getVoltage(const Value& channel)
    {
        return SendCommand(":READ:VOLT?(@"+channel.String()+")");
    }
    
    Value mesureVoltage(const Value& channel)
    {
       return SendCommand(":MEAS:VOLT?(@"+channel.String()+")"); 
    }
    
    void setCurrent(const Value& channel,const Value& current)
    {
        SendCommand(":CURR "+current.String()+",(@"+channel.String()+")");
    }
    
    virtual Value getCurrent(const Value& channel)
    {
        return SendCommand(":READ:CURR?(@"+channel.String()+")");
    }
    
    virtual Value mesureCurrent(const Value& channel)
    {
        return SendCommand(":MEAS:CURR?(@"+channel.String()+")");
    }
    isegModule* Clone() { return new isegModule(*this);}
    isegModule* Clone() const { return new isegModule(*this);} 
private:
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
