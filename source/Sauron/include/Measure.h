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


#ifndef MEASURE_H
#define MEASURE_H
#include "Value.h"
#include <string>
#include <iostream>
#include <map>
class Measure 
{
public:
    void setVoltage(const Value& voltage)
    {
        m_voltage=voltage;
    }
    void setCurrent(const Value& current)
    {
        m_current=current;
    }
    void setChannel(const Value& channel)
    {
        m_channel=channel.UInt();
    }
    void setModule(const Value& moduleName)
    {
        if(m_moduleNbrMap.find(moduleName.String())==m_moduleNbrMap.end()) add(moduleName,"Module");
        m_module=m_moduleNbrMap[moduleName.String()];
    }
    void setCrate(const Value& crateName)
    {
        if(m_crateNbrMap.find(crateName.String())==m_crateNbrMap.end()) add(crateName,"Crate");
        m_crate=m_crateNbrMap[crateName.String()];
    }
    void setRack(const Value& rackName)
    {
        if(m_rackNbrMap.find(rackName.String())==m_rackNbrMap.end()) add(rackName,"Rack");
        m_rack=m_rackNbrMap[rackName.String()];
    }
    void print(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover<<"Rack : "<<m_rack<<", Crate : "<<m_crate<<", Module : "<<m_module<<", Channel : "<<m_channel<<", Voltage : "<<m_voltage.Float()<<"V, Current : "<<m_current.Float()<<"A\n";
    }
private:
    void add(const Value& moduleName,const std::string& type)
    {
        if(type=="Module")
        {
            m_moduleNbrMap.insert({moduleName.String(),m_moduleNbr});
            m_moduleNbr++;
        }
        else if(type=="Crate")
        {
            m_crateNbrMap.insert({moduleName.String(),m_crateNbr});
            m_crateNbr++;
        }
        else if (type=="Rack")
        {
            m_rackNbrMap.insert({moduleName.String(),m_rackNbr});
            m_rackNbr++;
        }
    }
    static std::map<std::string,unsigned int> m_moduleNbrMap;
    static std::map<std::string,unsigned int> m_crateNbrMap;
    static std::map<std::string,unsigned int> m_rackNbrMap;
    static unsigned int m_moduleNbr;
    static unsigned int m_crateNbr;
    static unsigned int m_rackNbr;
    Value m_voltage{""};
    Value m_current{""};
    unsigned int m_channel{0};
    unsigned int m_module{0};
    unsigned int m_crate{0};
    unsigned int m_rack{0};
};
#endif
