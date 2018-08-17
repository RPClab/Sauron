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
#include <vector>

class Position
{
public:
    bool operator==(const Position& pos) const
    {
        if(pos.getChannel()==m_channel&&pos.getModule()==m_module&&pos.getCrate()==m_crate&&pos.getRack()==m_rack) return true;
        else return false;
    }
    bool operator!=(const Position& pos) const
    {
        return !(pos==*this);
    }
    void setChannel(const Value& channel)
    {
        m_channel=channel.UInt();
    }
    void setModule(const Value& moduleName)
    {
        add(moduleName,"Module");
        m_module=m_moduleToNbrMap[moduleName.String()];
    }
    void setCrate(const Value& crateName)
    {
        add(crateName,"Crate");
        m_crate=m_crateToNbrMap[crateName.String()];
    }
    void setRack(const Value& rackName)
    {
        add(rackName,"Rack");
        m_rack=m_rackToNbrMap[rackName.String()];
    }
    int getChannel() const {return m_channel;}
    int getModule() const {return m_module;}
    int getCrate() const {return m_crate;}
    int getRack() const {return m_rack;}
    std::string getModuleName(const int& nbr) const 
    {
        return  m_nbrToModuleMap[nbr];
    }
    std::string getCrateName(const int& nbr) const
    {
        return m_nbrToCrateMap[nbr];
    }
    std::string getRackName(const int& nbr) const
    {
        return m_nbrToRackMap[nbr];
    }
    void  print(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover;
        (m_rack!=-1&&getRackName(getRack())!="") ? (stream<<"Rack : "<<getRack()<<" ("<<getRackName(getRack())<<") , ") : (stream<<"") ;
        (m_crate!=-1) ? (stream<<"Crate : "<<getCrate()<<" ("<<getCrateName(getCrate())<<") , ") : (stream<<"");
        (m_module!=-1) ? (stream<<"Module : "<<getModule()<<" ("<<getModuleName(getModule())<<") , ") : (stream<<"");
        (m_channel !=-1) ? (stream<<"Channel : "<<getChannel()<<", ") : (stream<<"");
        stream<<"\n";
    }
private:
    friend std::ostream & operator<<(std::ostream &os, const Position& Pos);
    static std::map<std::string,unsigned int> m_moduleToNbrMap;
    static std::vector<std::string> m_nbrToModuleMap;
    static std::map<std::string,unsigned int> m_crateToNbrMap;
    static std::vector<std::string> m_nbrToCrateMap;
    static std::map<std::string,unsigned int> m_rackToNbrMap;
    static std::vector<std::string> m_nbrToRackMap;
    static unsigned int m_moduleNbr;
    static unsigned int m_crateNbr;
    static unsigned int m_rackNbr;
    int m_channel{-1};
    int m_module{-1};
    int m_crate{-1};
    int m_rack{-1};
    void add(const Value& Name,const std::string& type)
    {
        if(type=="Module")
        {
            if(m_moduleToNbrMap.find(Name.String())==m_moduleToNbrMap.end())
            {    
                m_moduleToNbrMap.insert({Name.String(),m_moduleNbr});
                m_nbrToModuleMap.push_back(Name.String());
                m_moduleNbr++;
            }
        }
        else if(type=="Crate")
        {
            if(m_crateToNbrMap.find(Name.String())==m_crateToNbrMap.end())
            {    
                m_crateToNbrMap.insert({Name.String(),m_crateNbr});
                m_nbrToCrateMap.push_back(Name.String());
                m_crateNbr++;
            }
        }
        else if (type=="Rack")
        {
            if(m_rackToNbrMap.find(Name.String())==m_rackToNbrMap.end())
            {
                m_rackToNbrMap.insert({Name.String(),m_rackNbr});
                m_nbrToRackMap.push_back(Name.String());
                m_rackNbr++;
            }
        }
    }
};



class VoltageMeasured 
{
public:
    void printPosition(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<std::endl;
    }
    void setMeasuredVoltage(const Value& voltage)
    {
        m_measuredVoltage=voltage;
    }
    Value getMeasuredVoltage() const
    {
        return m_measuredVoltage;
    }
    Position getPosition() const
    {
        return m_pos;
    }
    Position& getPosition()
    {
        return m_pos;
    }
private:
    Value m_measuredVoltage{""};
    Position m_pos;
};

class CurrentMeasured
{
public:
    void printPosition(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<std::endl;
    }
    void setMeasuredCurrent(const Value& current)
    {
        m_measuredCurrent=current;
    }
    Value getMeasuredCurrent() const
    {
        return m_measuredCurrent;
    }
    Position getPosition() const
    {
        return m_pos;
    }
    Position& getPosition()
    {
        return m_pos;
    }
private:
    Value m_measuredCurrent{""};
    Position m_pos;
};


class VoltageSet 
{
public:
    void printPosition(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<std::endl;
    }
    void setVoltage(const Value& voltage)
    {
        m_setVoltage=voltage;
    }
    Value getSetVoltage() const
    {
        return m_setVoltage;
    }
    void print(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover<<m_pos<<" Set Voltage : "<<getSetVoltage().Float()<<"V\n";
    }
    Position getPosition() const
    {
        return m_pos;
    }
    Position& getPosition()
    {
        return m_pos;
    }
private:
    Value m_setVoltage{""};
    Position m_pos;
};

class CurrentSet : public virtual Position
{
public:
    void printPosition(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<std::endl;
    }
    void setCurrent(const Value& current)
    {
        m_setCurrent=current;
    }
    Value getSetCurrent() const
    {
        return m_setCurrent;
    }
    void print(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover<<m_pos<<" Set Current : "<<getSetCurrent().Float()<<"A\n";
    }
    Position getPosition() const 
    {
        return m_pos;
    }
    Position& getPosition()
    {
        return m_pos;
    }
private:
    Value m_setCurrent{""};
    Position m_pos;
};



class Sets
{
public:
    void printPosition(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<std::endl;
    }
    Sets(const VoltageSet& voltage,const CurrentSet& current)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setVoltage(voltage.getSetVoltage());
        setCurrent(current.getSetCurrent());
    }
    Sets(const CurrentSet& current,const VoltageSet& voltage)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setVoltage(voltage.getSetVoltage());
        setCurrent(current.getSetCurrent());
    }
    void operator()(const VoltageSet& voltage,const CurrentSet& current)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setVoltage(voltage.getSetVoltage());
        setCurrent(current.getSetCurrent());
    }
    void operator()(const CurrentSet& current,const VoltageSet& voltage)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setVoltage(voltage.getSetVoltage());
        setCurrent(current.getSetCurrent());
    }
    void setVoltage(const Value& volt)
    {
        m_setVoltage=volt;
    }
    void setCurrent(const Value& current)
    {
        m_setCurrent=current;
    }
    Value getSetVoltage() const 
    {
        return m_setVoltage;
    }
    Value getSetCurrent() const 
    {
        return m_setCurrent;
    }
    void print(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover<<m_pos<<" Set Voltage : "<<getSetVoltage().Float()<<"V, Set Current : "<<getSetCurrent().Float()<<"A\n";
    }
     Position  getPosition() const
    {
        return m_pos;
    }
    Position& getPosition()
    {
        return m_pos;
    }
private :
    Position m_pos;
    Value m_setCurrent{""};
    Value m_setVoltage{""};
};


class Measures
{
public:    
    void printPosition(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<std::endl;
    }
    Measures(const CurrentMeasured& current,const VoltageMeasured& voltage)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setMeasuredVoltage(voltage.getMeasuredVoltage());
        setMeasuredCurrent(current.getMeasuredCurrent());
    }
    Measures(const VoltageMeasured& voltage,const CurrentMeasured& current)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setMeasuredVoltage(voltage.getMeasuredVoltage());
        setMeasuredCurrent(current.getMeasuredCurrent());
    }
    void operator()(const VoltageMeasured& voltage,const CurrentMeasured& current)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setMeasuredVoltage(voltage.getMeasuredVoltage());
        setMeasuredCurrent(current.getMeasuredCurrent());
    }
    void operator()(const CurrentMeasured& current,const VoltageMeasured& voltage)
    {
        if(voltage.getPosition()!=current.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=current.getPosition();
        setMeasuredVoltage(voltage.getMeasuredVoltage());
        setMeasuredCurrent(current.getMeasuredCurrent());
    }
    void setMeasuredVoltage(const Value& volt)
    {
        m_measuredVoltage=volt;
    }
    void setMeasuredCurrent(const Value& current)
    {
        m_measuredCurrent=current;
    }
    Value getMeasuredVoltage() const 
    {
        return m_measuredVoltage;
    }
    Value getMeasuredCurrent() const
    {
        return m_measuredCurrent;
    }
    void print(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover<<m_pos<<" Measured Voltage : "<<getMeasuredVoltage().Float()<<"V, Measured Current : "<<getMeasuredCurrent().Float()<<"A\n";
    }
    Position getPosition() const
    {
        return m_pos;
    }
    Position& getPosition()
    {
        return m_pos;
    }
private:
    Position m_pos;
    Value m_measuredCurrent{""};
    Value m_measuredVoltage{""};
    
};

class MeasuresAndSets
{
public:
    MeasuresAndSets(){}
    void printVoltageCurrent(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<"\t-> channel "<<m_pos.getChannel()<<" measured : "<<getMeasuredVoltage().Float()<<"V ["<<getSetVoltage().Float()<<"V]"<<", "<<getMeasuredCurrent().Float()<<"A ["<<getSetCurrent().Float()<<"A]\n";
    }
    void printPosition(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<std::endl;
    }
    void print(std::ostream& stream =std::cout,const std::string& mover="")
    {
        stream<<mover<<m_pos<<getMeasuredVoltage().Float()<<"V ["<<getSetVoltage().Float()<<"V]"<<", "<<getMeasuredCurrent().Float()<<"A ["<<getSetCurrent().Float()<<"A]\n";
    }
    MeasuresAndSets(const Measures& measure,const Sets& sets)
    {
        if(measure.getPosition()!=sets.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=measure.getPosition();
        setMeasuredVoltage(measure.getMeasuredVoltage());
        setMeasuredCurrent(measure.getMeasuredCurrent());
        setVoltage(sets.getSetVoltage());
        setCurrent(sets.getSetCurrent());
    }
    MeasuresAndSets(const Sets& sets,const Measures& measure)
    {
        if(measure.getPosition()!=sets.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=measure.getPosition();
        setMeasuredVoltage(measure.getMeasuredVoltage());
        setMeasuredCurrent(measure.getMeasuredCurrent());
        setVoltage(sets.getSetVoltage());
        setCurrent(sets.getSetCurrent());
    }
    void operator()(const Measures& measure,const Sets& sets)
    {
        if(measure.getPosition()!=sets.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=measure.getPosition();
        setMeasuredVoltage(measure.getMeasuredVoltage());
        setMeasuredCurrent(measure.getMeasuredCurrent());
        setVoltage(sets.getSetVoltage());
        setCurrent(sets.getSetCurrent());
    }
    void operator()(const Sets& sets,const Measures& measure)
    {
        if(measure.getPosition()!=sets.getPosition()) throw std::logic_error("You try to add two measures from different channels");
        m_pos=measure.getPosition();
        setMeasuredVoltage(measure.getMeasuredVoltage());
        setMeasuredCurrent(measure.getMeasuredCurrent());
        setVoltage(sets.getSetVoltage());
        setCurrent(sets.getSetCurrent());
    }
    void setMeasuredVoltage(const Value& volt)
    {
        m_measuredVoltage=volt;
    }
    void setMeasuredCurrent(const Value& current)
    {
        m_measuredCurrent=current;
    }
    Value getMeasuredVoltage() const 
    {
        return m_measuredVoltage;
    }
    Value getMeasuredCurrent() const
    {
        return m_measuredCurrent;
    }
    Position getPosition() const 
    {
        return m_pos;
    }
        void setVoltage(const Value& volt)
    {
        m_setVoltage=volt;
    }
    void setCurrent(const Value& current)
    {
        m_setCurrent=current;
    }
    Value getSetVoltage() const 
    {
        return m_setVoltage;
    }
    Value getSetCurrent() const 
    {
        return m_setCurrent;
    }
    Position& getPosition()
    {
        return m_pos;
    }
private:
    Position m_pos;
    Value m_measuredCurrent{""};
    Value m_measuredVoltage{""};
    Value m_setCurrent{""};
    Value m_setVoltage{""};
};
#endif
