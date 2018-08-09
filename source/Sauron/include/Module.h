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


#ifndef MODULE_H
#define MODULE_H
#include "Connector.h"
#include "DumbConnector.h"
#include <iostream>
#include "Measure.h"
		
class Module
{
public:
    Module():m_connector(&m_dumb){};
    Module(Connector& connector,std::map<std::string,std::string>& params):m_connector(connector.Clone())
    {
        m_params=params;
    };
    Module(Connector* connector,std::map<std::string,std::string>& params):m_connector(connector->Clone())
    {
        m_params=params;
    };
    Module(Connector& connector):m_connector(connector.Clone()){};
    Module(Connector* connector):m_connector(connector->Clone()){};
    Module(std::map<std::string,std::string>& params)
    {
        m_params=params;
    };
    void setConnector(Connector& connector)
    {
        if(m_connector!=nullptr&&m_connector->getName()!="DumbConnector")
        {
            delete m_connector;
        }
        m_connector=connector.Clone();
    }
    void setConnector(Connector* connector)
    {
        if(m_connector!=nullptr&&m_connector->getName()!="DumbConnector")
        {
            delete m_connector;
        }
        m_connector=connector->Clone();
    }
    Module& operator()(const Module& module)
    {
        m_connector=module.m_connector->Clone();
        return *this;
    }
    
    Module& operator=(const Module& module)
    {
        m_connector=module.m_connector->Clone();
        return *this;
    }
    
    void setSlot(const Value& slot)
    {
        m_slot=slot;
    }
    
    Value getSlot()
    {
        return m_slot;
    }
    
    void setConnectorParameters(const Parameters& params)
    {
        m_connector->setParameters(params);
    }
    void setParameters(const Parameters& params)
    {
        m_params=params;
    }
    void Initialize()
    {
        setName();
        setDescription();
        setSlot();
       m_connector->Initialize();
    }
    void Connect()
    {
        m_connector->Connect();
        FillInfos();
    }
    void Disconnect()
    {
        m_connector->Disconnect();
    }

    bool IsConnected()
    {
        return m_connector->IsConnected();
    }
    ~Module()
    {
       if(m_connector!=nullptr&&m_connector->getName()!="DumbConnector"&&m_connector->isCrateConnector()==false)
       {
           delete m_connector;
           m_connector=nullptr;
       }
    }
    /****************************************** WIENER FUNCTIONS
     * 
     */
    /**
     * Index of the module 
     */
   /* virtual Value getIndex()=0;
    virtual Value getIdentification()=0;
    virtual Value get5V()=0;
    virtual Value get24V()=0;
    virtual Value getTemperatureModule(const unsigned int% sensor)=0;
    virtual std::vector<Value> getTemperatures()=0;
    virtual Value getHardwareLimitVoltage()=0;
    virtual Value getHardwareLimitCurrent()=0;
    virtual Value getModuleRampSpeedVoltage()=0;
    virtual Value getModuleRampSpeedCurrent()=0;
    virtual void setModuleRampSpeedVoltage()=0;
    virtual void setModuleRampSpeedCurrent()=0;
    virtual Value getModuleStatus()=0;
    virtual Value getModuleEventStatus()=0;
    virtual clear()=0;
    /****** channels*/
    /*virtual Value getName(int);
    virtual void setName(int,string);
    virtual Value getIndex()=0;
    virtual Value getGroup()=0;
    virtual Value getStatus()=0;
    virtual Value getPolarity()=0;
    virtual Value setPolarity()=0;
    virtual Value getTemperature();
    virtual Value setSwitch()=0;
    virtual Value setVoltageRiseRate();
    virtual Value setVoltageFallRate();
    virtual Value getVoltageRiseRate();
    virtual Value getVoltageFallRate();
    virtual Value setCurrentRiseRate();
    virtual Value setCurrentFallRate();
    virtual Value getCurrentRiseRate();
    virtual Value getCurrentFallRate();
    virtual Value setSupervisionBehavior();
    virtual Value getSupervisionBehavior();
    virtual Value setSupervisionMinSenseVoltage();
    virtual Value setSupervisionMaxSenseVoltage();
    virtual Value getSupervisionMinSenseVoltage();
    virtual Value getSupervisionMaxSenseVoltage();
    virtual Value getSupervisionMaxTerminalVoltage();
    virtual Value setSupervisionMaxTerminalVoltage();
    virtual Value getSupervisionMaxCurrent();
    virtual Value setSupervisionMaxCurrent();
    virtual Value getSupervisionMaxTemperature();
    virtual Value setSupervisionMaxTemperature();
    virtual Value getConfigMaxSenseVoltage();
    virtual Value getConfigMaxTerminalVoltage();
    virtual Value getSupervisionMaxPower();
    virtual Value getConfigMaxCurrent();
    virtual Value getTripTimeMaxCurrent();
    virtual Value setTripTimeMaxCurrent();*/

    /**
    * Turn on the channel ch
    */
    virtual void on(const Value& ch)=0;
    /**
    * Turn of the channel ch
    */
    virtual void off(const Value&)=0;
    /**
    * Turn on all the channels
    */
    virtual void on()
    {
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) on(i);
    }
    /**
    * Turn off all the channels
    */
    virtual void off()
    {
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) off(i);
    }
    virtual void setVoltage(const Value&,const Value&)=0;
    
    virtual void setVoltage(const Value& HV )
    {
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) setVoltage(i,HV);
    }
    virtual Value getVoltage(const Value&)=0;
    
    virtual std::vector<Value> getVoltage()
    {
        std::vector<Value> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) ret[i]=getVoltage(i);
        return std::move(ret);
    }
    
    virtual Value mesureVoltage(const Value&)=0;
    
    virtual std::vector<Value> mesureVoltage()
    {
        std::vector<Value> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) ret[i]=mesureVoltage(i);
        return std::move(ret); 
    }
    
    virtual void printSetVoltage(const Value& channel,std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<", channel "<<channel<<" set : "<<getVoltage(channel).Float()<<"V\n";
    }
    
    virtual void printSetVoltage(std::ostream& stream =std::cout)
    {
       stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) stream<<"\t-> channel "<<i<<" set : "<<getVoltage(i).Float()<<"V\n";
    }
    
    virtual void printMeasuredVoltage(const Value& channel,std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<", channel "<<channel<<" measured : "<<mesureVoltage(channel).Float()<<"V\n";
    }
    
    virtual void printMeasuredVoltage(std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) stream<<"\t-> channel "<<i<<" measured : "<<mesureVoltage(i).Float()<<"V\n";
    }
    
    virtual void printVoltage(const Value& channel,std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<", channel "<<channel<<" measured : "<<mesureVoltage(channel).Float()<<"V ["<<getVoltage(channel).Float()<<"V]\n";
    }
    
    virtual void printVoltage(std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) stream<<"\t-> channel "<<i<<" measured : "<<mesureVoltage(i).Float()<<"V ["<<getVoltage(i).Float()<<"V]\n";
    }
    
    virtual void setCurrent(const Value&,const Value&)=0;
    
    virtual void setCurrent(const Value& current)
    {
       for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) setCurrent(i,current); 
    }
    
    virtual Value getCurrent(const Value& = Value())=0;
    
    virtual std::vector<Value> getCurrent()
    {
        std::vector<Value> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) ret[i]=getCurrent(i);
        return std::move(ret); 
    }
    
        
    virtual void printSetCurrent(const Value& channel,std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<", channel "<<channel<<" set : "<<getCurrent(channel).Float()<<"A\n";
    }
    
    virtual void printSetCurrent(std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) stream<<"\t-> channel "<<i<<" set : "<<getCurrent(i).Float()<<"A\n";
    }
    
    virtual void printMeasuredCurrent(const Value& channel,std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<", channel "<<channel<<" measured : "<<mesureCurrent(channel).Float()<<"A\n";
    }
    
    virtual void printMeasuredCurrent(std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) stream<<"\t-> channel "<<i<<" measured : "<<mesureCurrent(i).Float()<<"A\n";
    }
    
    virtual void printCurrent(const Value& channel,std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<", channel "<<channel<<" measured : "<<mesureCurrent(channel).Float()<<"A ["<<getCurrent(channel).Float()<<"A]\n";
    }
    
    virtual void printCurrent(std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) stream<<"\t-> channel "<<i<<" measured : "<<mesureCurrent(i).Float()<<"A ["<<getCurrent(i).Float()<<"A]\n";
    }
    
    virtual Value mesureCurrent(const Value&)=0;
    
    virtual std::vector<Value> mesureCurrent()
    {
        std::vector<Value> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i)ret[i]=mesureCurrent(i);
        return std::move(ret); 
    }
    
    virtual std::vector<Measure> getMeasures()
    {
        std::vector<Measure> mes(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i)
        {
            mes[i].setCurrent(mesureCurrent(i));
            mes[i].setVoltage(mesureVoltage(i));
            mes[i].setModule(getName());
            mes[i].setChannel(i);
        }
        return mes;
    }
    
    virtual void printVoltageCurrent(std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) stream<<"\t-> channel "<<i<<" measured : "<<mesureVoltage(i).Float()<<"V ["<<getVoltage(i).Float()<<"V]"<<", "<<mesureCurrent(i).Float()<<"A ["<<getCurrent(i).Float()<<"A]\n";
    }
    
    
        virtual void printVoltageCurrent(const Value& channel,std::ostream& stream =std::cout)
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        stream<<"\t-> channel "<<channel<<" measured : "<<mesureVoltage(channel).Float()<<"V ["<<getVoltage(channel).Float()<<"V]"<<", "<<mesureCurrent(channel).Float()<<"A ["<<getCurrent(channel).Float()<<"A]\n";
    }
    Value getName() const
    {
        return m_name;
    }
    
    virtual Module* Clone()=0;
    virtual Module* Clone() const =0;
    
    void setName(const std::string name)
    {
        m_name=name;
    }
    
    void setSlot(const std::string slot)
    {
        m_slot=slot;
    }
    
    
    void setDescription(const std::string descr)
    {
        m_description=descr;
    }
    
    void printParameters(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover<<"Module "<<getName()<<" parameters :\n";
        m_params.printParameters(mover);
        m_connector->printParameters(mover+"\t");
    }
    void printParameters(const std::string mover)
    {
        std::cout<<mover<<"Module "<<getName()<<" parameters :\n";
        m_params.printParameters(mover);
        m_connector->printParameters(mover+"\t");
    }
    Value getNbrChannels()
    {
        return m_nbrOfChannels;
    }
protected:
    /* Main parameters */
    Connector* m_connector{nullptr};
    Parameters m_params;
    static DumbConnector m_dumb;
    /* Useful parameters */
    Value m_compagny{""};
    Value m_model{""};
    Value m_serialNumber{""};
    Value m_version{""};
    Value m_name{""};
    Value m_description{""};
    Value m_nbrOfChannels{""};
    Value m_slot{"0"};
    void setDescription()
    {
        if(m_params.hasParam("Description"))
        {
            m_description=m_params["Description"];
        }
        else
        {
            std::cout<<"Slot puuted to default value "<<m_description<<"\n";
        }
    }   
    void setName()
    {
        if(m_params.hasParam("Name"))
        {
            m_name=m_params["Name"];
        }
        else
        {
            std::string error="Name is mandatory ! \n";
            std::cout<<error;
            throw error;
        }
    }
    
    void setSlot()
    {
        if(m_params.hasParam("Slot"))
        {
            m_slot=m_params["Slot"];
        }
        else
        {
            std::cout<<"Slot puuted to default value "<<m_slot<<"\n";
        }
    }
    
    bool hasParam(std::map<std::string,std::string>& m_params, const std::string& param)
    {
        if(m_params.find(param)!=m_params.end()) return true;
        else return false;
    }
    virtual Value GoodChannelNumber(const Value& channel) const
    {
        if (m_slot.Int()==0) return channel;
        return (m_slot.Int()-1)*100+channel.UInt();
    }
    Value SendCommand(const std::string& command)
    {
        return m_connector->SendCommand(command);
    }
    
    virtual void FillInfos()=0;
};
#endif
