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
#include "Status.h"
#include "serial.h"	
class Module
{
public:
    Module():m_connector(&m_dumb){};
    Module(Connector& connector,std::map<std::string,std::string>& params):m_connector(connector.clone())
    {
        m_params=params;
    };
    Module(Connector* connector,std::map<std::string,std::string>& params):m_connector(connector->clone())
    {
        m_params=params;
    };
    Module(Connector& connector):m_connector(connector.clone()){};
    Module(Connector* connector):m_connector(connector->clone()){};
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
        m_connector=connector.clone();
    }
    void setConnector(Connector* connector)
    {
        if(m_connector!=nullptr&&m_connector->getName()!="DumbConnector")
        {
            delete m_connector;
        }
        m_connector=connector->clone();
    }
    Module& operator()(const Module& module)
    {
        m_connector=module.m_connector->clone();
        return *this;
    }
    
    Module& operator=(const Module& module)
    {
        m_connector=module.m_connector->clone();
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
    void initialize()
    {
            setName();
            setDescription();
            setSlot();
            m_connector->initialize();
    }
    void connect()
    {
        /*if(!m_connector->IsConnected())
        {*/
            try
            {
                m_connector->connect();
                fillInfos();
            }
            catch(...)
            {
                findPort();
            }
            m_WantedVoltage=std::vector<VoltageWanted>(getNbrChannels().UInt());
            setWantedVoltage(Value(""));
            setChannelStatusBits();
            setModuleStatusBits();
        //}
    }
    void disconnect()
    {
        m_connector->disconnect();
    }

    bool isConnected()
    {
        return m_connector->isConnected();
    }
    virtual ~Module()
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
    virtual Value getModuleEventStatus()=0;
    virtual clear()=0;
    /****** channels*/
    /*virtual Value getName(int);
    virtual void setName(int,string);
    virtual Value getIndex()=0;
    virtual Value getGroup()=0;
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

    //ON
    virtual void on(const Value& ch)=0;
    virtual void on()
    {
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) on(i);
    }
    //OFF
    virtual void off(const Value&)=0;
    virtual void off()
    {
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) off(i);
    }
    //SET VOLTAGE
    virtual void setVoltage(const Value&,const Value&)=0;
    virtual void setVoltage(const Value& HV )
    {
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) setVoltage(i,HV);
    }
    //GET VOLTAGE
    virtual VoltageSet getVoltage(const Value&)=0;
    virtual std::vector<VoltageSet> getVoltage()
    {
        std::vector<VoltageSet> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i)
        {
            ret[i]=getVoltage(i);
        }
        return std::move(ret);
    }
    //MEASURE VOLTAGE 
    virtual VoltageMeasured mesureVoltage(const Value&)=0;
    virtual std::vector<VoltageMeasured> mesureVoltage()
    {
        std::vector<VoltageMeasured> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) ret[i]=mesureVoltage(i);
        return std::move(ret); 
    }
    //SET CURRENT
    virtual void setCurrent(const Value&,const Value&)=0;
    virtual void setCurrent(const Value& current)
    {
       for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) setCurrent(i,current); 
    }
    //GET CURRENT
    virtual CurrentSet getCurrent(const Value& = Value())=0;
    virtual std::vector<CurrentSet> getCurrent()
    {
        std::vector<CurrentSet> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) ret[i]=getCurrent(i);
        return std::move(ret); 
    }
    //MEASURE CURRENT
    virtual CurrentMeasured mesureCurrent(const Value&)=0;
    virtual std::vector<CurrentMeasured> mesureCurrent()
    {
        std::vector<CurrentMeasured> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i)ret[i]=mesureCurrent(i);
        return std::move(ret); 
    }
    //GET CHANNEL STATUS
    virtual Status getStatus(const Value&)=0;
    virtual std::vector<Status> getStatus()
    {
        std::vector<Status> ret;
        ret.reserve(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i)ret[i]=getStatus(i);
        return std::move(ret); 
    }
    //GET MODULE STATUS
    virtual Status getModuleStatus()=0;
    //SET CHANNEL EMERGENCY 
    virtual void setEmergency(const Value&)=0;
    //SET CHANNELS EMERGENCY
    virtual void setEmergency()
    {
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) setEmergency(i);
    }
    
    virtual void printSetVoltage(const Value& channel,std::ostream& stream =std::cout,const std::string mover="")
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        getVoltage(channel).print(stream,mover);
    }
    
    virtual void printSetVoltage(std::ostream& stream =std::cout,const std::string mover="")
    {
       stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) getVoltage(i).print(stream,mover);
    }
    
   /* virtual void printMeasuredVoltage(const Value& channel,std::ostream& stream =std::cout)
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
    }*/
    
    
    
    virtual std::vector<MeasuresAndSets> getMeasuresAndSets()
    {
        std::vector<MeasuresAndSets> mes(m_nbrOfChannels.UInt());
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i)
        {
            mes[i](Measures(mesureCurrent(i),mesureVoltage(i)),Sets(getVoltage(i),getCurrent(i)));
        }
        return mes;
    }
    
    virtual MeasuresAndSets getMeasuresAndSets(const Value& channel)
    {
        MeasuresAndSets mes(Measures(mesureCurrent(channel),mesureVoltage(channel)),Sets(getVoltage(channel),getCurrent(channel)));
        return std::move(mes);
    }
    
    
    
    
    virtual void printVoltageCurrent(std::ostream& stream =std::cout, const std::string mover="")
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        for(unsigned int i=0;i!=m_nbrOfChannels.UInt();++i) getMeasuresAndSets(i).printVoltageCurrent(stream,mover);
    }
    
    
    virtual void printVoltageCurrent(const Value& channel,std::ostream& stream =std::cout, const std::string mover="")
    {
        stream<<"Module "<<m_name<<" "<<((m_slot.Int()!=-1) ? Value("(slot "+m_slot.String()+")") : Value())<<" :\n";
        getMeasuresAndSets(channel).printVoltageCurrent(stream,mover);
    }
    Value getName() const
    {
        return m_name;
    }
    
    virtual Module* clone()=0;
    virtual Module* clone() const =0;
    
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
    
    virtual void setChannelStatusBits()
    {
        std::cout<<"setChannelStatusBits not implemented in Module for "<<m_compagny<<"  "<<m_model<<" !\n";
        std::cout<<"It will be more difficult to acces the status !"<<std::endl;
    }
    
        virtual void setModuleStatusBits()
    {
        std::cout<<"setModuleStatusBits not implemented in Module for "<<m_compagny<<"  "<<m_model<<" !\n";
        std::cout<<"It will be more difficult to acces the status !"<<std::endl;
    }
    void setWantedVoltage(const Value& channel,const Value& HV)
    {
         m_WantedVoltage[channel.UInt()].getPosition().setChannel(channel);
         m_WantedVoltage[channel.UInt()].getPosition().setModule(m_name);
         m_WantedVoltage[channel.UInt()].setWantedVoltage(HV);
    }
    void setWantedVoltage(const Value& HV)
    {
        for(unsigned int i=0;i!=m_WantedVoltage.size();++i)
        {
            m_WantedVoltage[i].getPosition().setChannel(i);
            m_WantedVoltage[i].getPosition().setModule(m_name);
            m_WantedVoltage[i].setWantedVoltage(HV);
        }
    }
    void setIsMonitored(const unsigned int& i,const bool& mon)
    {
        m_WantedVoltage[i].setIsMonitored(mon);
    }
    VoltageWanted getWantedVoltage(const unsigned int& i)
    {
        return m_WantedVoltage[i];
    }
    VoltageWanted getWantedVoltage(const Value& i)
    {
        return m_WantedVoltage[i.UInt()];
    }
    std::vector<VoltageWanted> getWantedVoltage()
    {
        return m_WantedVoltage;
    }
    Value getSerialNumber()
    {
        return m_serialNumber;
    }
protected:
    void findPort()
    {
        std::string old_port="";
        if(m_connector->getLinkType()=="VCP")
        {
            if(m_params.hasParam("SN")==false)
            {
                if(m_connector->getParameters().hasParam("Port")==false)
                {
                    std::cout<<"Connector "<<m_connector->getName()<<" is VCP !"
                    <<" You should provide or the Port connector of the module "
                    <<m_name.String()<<" or the SN of the module !"<<std::endl;
                }
                else
                {
                    std::cout<<"Something is wrong with the Connector parameters of module "<<m_name.String()<<std::endl;
                    std::cout<<"Maybe the Port as change... But you didn't provide SN for this module ! Sauron cant't help you !"<<std::endl;
                }
                throw;
            }
            else
            {
                Parameters par=m_connector->getParameters();
                old_port=par["Port"].String();
                std::vector<serial::PortInfo> inf=serial::list_ports();
                for(unsigned int i=0;i!=inf.size();++i)
                {
                    par=m_connector->getParameters();
                    par["Port"]=inf[i].port;
                    m_connector->setParameters(par);
                    m_connector->initialize();
                    try
                    {
                        m_connector->connect();
                        fillInfos();
                    }
                    catch(...){}
                    if(m_params["SN"]==m_serialNumber)
                    {
                        std::cout<<"Port "<<old_port<<" for Module "<<m_name<<" has changed to "<<par["Port"]<<std::endl;
                        break;
                    }
                }
            }
        }
    }
    std::vector<VoltageWanted> m_WantedVoltage;
    Status m_channelStatus;
    Status m_moduleStatus;
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
    virtual Value goodChannelNumber(const Value& channel) const
    {
        if (m_slot.Int()==0) return channel;
        return (m_slot.Int()-1)*100+channel.UInt();
    }
    Value command()
    {
        Value val=m_connector->buildCommand(params);
        params.clear();
        return val;
    }
    template<typename T, typename ... Args>
    Value command(T first, Args ... args) 
    {	
        params.push_back(Value(first));
        return command(args ...);
    }    
    virtual void fillInfos()=0;
    std::vector<Value> params;
};
#endif
