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
    void on(const Value& channel)
    {
        SendCommand("***SEND***outputSwitch.u"+GoodChannelNumber(channel).String()+"***VALUE***1");
    }
    void off(const Value& channel)
    {
        SendCommand("***SEND***outputSwitch.u"+GoodChannelNumber(channel).String()+"***VALUE***0");
    }
    void setVoltage(const Value& channel,const Value& HV)
    {
        SendCommand("***SEND***outputVoltage.u"+GoodChannelNumber(channel).String()+"***VALUE***"+HV.String());
    }
    Value getVoltage(const Value& channel)
    {
        return SendCommand("***RECEIVE***outputVoltage.u"+GoodChannelNumber(channel).String());
    }
    
    Value mesureVoltage(const Value& channel)
    {
       return SendCommand("***RECEIVE***outputMeasurementTerminalVoltage.u"+GoodChannelNumber(channel).String());
    }
    
    void setCurrent(const Value& channel,const Value& current)
    {
        SendCommand("***SEND***outputCurrent.u"+GoodChannelNumber(channel).String()+"***VALUE***"+GoodChannelNumber(current).String());
    }
    
    virtual Value getCurrent(const Value& channel)
    {
        return SendCommand("***RECEIVE***outputCurrent.u"+GoodChannelNumber(channel).String());
    }
    
    virtual Value mesureCurrent(const Value& channel)
    {
        return SendCommand("***RECEIVE***outputMeasurementCurrent.u"+GoodChannelNumber(channel).String());
    }
    
    WIENERModule* Clone() { return new WIENERModule(*this);}
    WIENERModule* Clone() const { return new WIENERModule(*this);} 
private:
    void FillInfos()
    {
        std::vector<Value> infos=ID().Tokenize(", ");
        m_nbrOfChannels=infos[2];
        m_compagny=infos[0];
        m_model=infos[1];
    }
    Value ID()
    {
       return SendCommand("***RECEIVE***moduleDescription."+m_slot.String());
    }
};
#endif
