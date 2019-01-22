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


#ifndef CRATE_H
#define CRATE_H
#include "Module.h"
#include <thread>
#include "Parameters.h"
#include <iostream>
#include <map>

class Crate 
{
public:
    Crate():m_connector(&m_dumb){};
    Crate(Parameters & param,Connector& connector):m_connector(connector.clone())
    {
        m_params=param;
    };
    Crate(Parameters& param,Connector* connector):m_connector(connector->clone())
    {
        m_params=param;
    };
    Crate(Parameters& param)
    {
        m_params=param;
    };
    void setParam(Parameters& param)
    {
        m_params=param;
    }
    Crate(Connector& connector):m_connector(connector.clone()){};
    Crate(Connector* connector):m_connector(connector->clone()){};
    void Initialize()
    {
            setName();
            setDescription();
            setRack();
            if(m_connector!=nullptr)
            {
                m_connector->initialize();
            }
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
            it->second->initialize();
            }
    }
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
    Crate& operator()(const Crate& module)
    {
        m_connector=module.m_connector->clone();
        return *this;
    }
    void addModule(Module* module)
    {
        module->initialize();
        if(module->getName().String()=="")
        {
            std::cout<<"Module must have a name !"<<std::endl;
            throw -1;
        }
        else if(m_modules.find(module->getName().String())!=m_modules.end())
        {
            std::cout<<"A module with name "<<module->getName()<<" exists ! Names must be uniques !"<<std::endl;
            throw -1;
        }
        else
        {
            m_modules.insert(std::pair<std::string,Module*>(module->getName().String(),module->clone()));
        }
    }
    void addModule(const std::string& name,Module* module)
    {
        m_modules.insert(std::pair<std::string,Module*>(name,module->clone()));
    }
    Crate& operator=(const Crate& module)
    {
        m_connector=module.m_connector->clone();
        return *this;
    }
    ~Crate()
    {
       if(m_connector!=nullptr&&m_connector->getName()!="DumbConnector")
       {
           delete m_connector;
           m_connector=nullptr;
       }
       for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
       {
           delete it->second;
           it->second=nullptr;
       }
       m_modules.clear();
    }
    
    //ON
    void on(const std::string& who,const Value& channel)
    {
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                if(channel.String()=="")it->second->on();
                else it->second->on(channel);
            }
        }
        else if (hasModule(who))
        {
            if(channel.String()=="")m_modules[who]->on();
            else m_modules[who]->on(channel);
        }
    }
    //OFF
    void off(const std::string& who,const Value& channel)
    {
        
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                if(channel.String()=="")it->second->off();
                else it->second->off(channel);
            }
        }
        else if (hasModule(who))
        {
            if(channel.String()=="")m_modules[who]->off();
            else m_modules[who]->off(channel);
        }
    }
    //SET VOLTAGE
    void setVoltage(const std::string& who,const Value& channel,const Value& voltage)
    {
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                if(channel.String()=="")it->second->setVoltage(voltage);
                else it->second->setVoltage(channel,voltage);
            }
        }
        else if (hasModule(who))
        {
            if(channel.String()=="")m_modules[who]->setVoltage(voltage);
            else m_modules[who]->setVoltage(channel,voltage);
        }
    }
    //GET VOLTAGE
    virtual std::vector<VoltageSet> getVoltage(const std::string& who,const Value& channel)
    {
        std::vector<VoltageSet> ret;
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                std::vector<VoltageSet> rett;
                if(channel.String()=="")rett=it->second->getVoltage();
                else rett.push_back(it->second->getVoltage(channel));
                for(unsigned int i=0;i!=rett.size();++i)
                {
                    rett[i].getPosition().setCrate(getName());
                    if(getRack()!="")rett[i].getPosition().setRack(getRack());
                }
                ret.insert(ret.end(), rett.begin(), rett.end());
            }
        }
        else if (hasModule(who))
        {
            std::vector<VoltageSet> rett;
            if(channel.String()=="")rett=m_modules[who]->getVoltage();
            else ret.push_back(m_modules[who]->getVoltage(channel));
            for(unsigned int i=0;i!=rett.size();++i)
            {
                rett[i].getPosition().setCrate(getName());
                if(getRack()!="")rett[i].getPosition().setRack(getRack());
            }
    
            ret.insert(ret.end(), rett.begin(), rett.end());
        }
        return std::move(ret);
    }
    //GET VOLTAGE WANTED
    virtual std::vector<VoltageWanted> getWantedVoltage(const std::string& who,const Value& channel)
    {
        std::vector<VoltageWanted> ret;
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                std::vector<VoltageWanted> rett;
                if(channel.String()=="")rett=it->second->getWantedVoltage();
                else rett.push_back(it->second->getWantedVoltage(channel));
                for(unsigned int i=0;i!=rett.size();++i)
                {
                    rett[i].getPosition().setCrate(getName());
                    if(getRack()!="")rett[i].getPosition().setRack(getRack());
                }
                ret.insert(ret.end(), rett.begin(), rett.end());
            }
        }
        else if (hasModule(who))
        {
            std::vector<VoltageWanted> rett;
            if(channel.String()=="")rett=m_modules[who]->getWantedVoltage();
            else ret.push_back(m_modules[who]->getWantedVoltage(channel));
               for(unsigned int i=0;i!=rett.size();++i)
                {
                    rett[i].getPosition().setCrate(getName());
                    if(getRack()!="")rett[i].getPosition().setRack(getRack());
                }
            ret.insert(ret.end(), rett.begin(), rett.end());
        }
        return std::move(ret);
    }
    
    //SET VOLTAGE WANTED
    void setWantedVoltage(const std::string& who,const Value& channel,const Value& voltage)
    {
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                if(channel.String()=="")it->second->setWantedVoltage(voltage);
                else it->second->setWantedVoltage(channel,voltage);
            }
        }
        else if (hasModule(who))
        {
            if(channel.String()=="")m_modules[who]->setWantedVoltage(voltage);
            else m_modules[who]->setWantedVoltage(channel,voltage);
        }
    }
    
    
    // GET CURRENT MEASURED
    virtual std::vector<CurrentMeasured> getCurrentMeasured(const std::string& who,const Value& channel)
    {
        std::vector<CurrentMeasured> ret;
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                std::vector<CurrentMeasured> rett;
                if(channel.String()=="")rett=it->second->mesureCurrent();
                else rett.push_back(it->second->mesureCurrent(channel));
                for(unsigned int i=0;i!=rett.size();++i)
                {
                    rett[i].getPosition().setCrate(getName());
                    if(getRack()!="")rett[i].getPosition().setRack(getRack());
                }
                ret.insert(ret.end(), rett.begin(), rett.end());
            }
        }
        else if (hasModule(who))
        {
            std::vector<CurrentMeasured> rett;
            if(channel.String()=="")rett=m_modules[who]->mesureCurrent();
            else ret.push_back(m_modules[who]->mesureCurrent(channel));
            for(unsigned int i=0;i!=rett.size();++i)
            {
                rett[i].getPosition().setCrate(getName());
                if(getRack()!="")rett[i].getPosition().setRack(getRack());
            }
    
            ret.insert(ret.end(), rett.begin(), rett.end());
        }
        return std::move(ret);
    }
    
    
    
    
    void Connect()
    {
        if(m_connector!=nullptr)
        {
           /* try
            {*/
                m_connector->connect();
           /* }
            catch (...)
            {
                if(m_connector!=nullptr) delete m_connector;
                m_connector=nullptr;
            }*/
        }
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           try
           {
                it->second->connect();
           }
           catch(...)
           {
               std::cout<<"Problem connecting to module "<<it->first<<" erasing it !"<<std::endl;
               delete it->second;
               m_modules.erase(it);
           }
        }
    }
    
    void Disconnect()
    {
        if(m_connector!=nullptr)
        {
            m_connector->disconnect();
        }
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->disconnect();
        }
    }
        
    void setIsMonitored(const Value& name,const Value& channel,const bool& mon)
    {
        if(m_modules.find(name.String())!=m_modules.end()) m_modules[name.String()]->setIsMonitored(channel.UInt(),mon);
    }
    
    void printVoltageCurrent(const Value& channel,std::ostream& stream =std::cout)
    {
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->printVoltageCurrent(channel,stream);
        }
    }
    void printVoltageCurrent(std::ostream& stream =std::cout)
    {
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->printVoltageCurrent(stream);
        }
    }
    void setModuleParameters(const std::string& name,const Parameters& params)
    {
        m_modules[name]->setParameters(params);
    }
    void setModuleConnectorParameters(const std::string& name,const Parameters& params)
    {
        m_modules[name]->setConnectorParameters(params);
    }
    void setModuleConnector(const std::string& name,Connector* connector)
    {
        m_modules[name]->setConnector(connector);
    }
    void printParameters(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<mover<<"Crate "<<getName()<<" parameters :\n";
        m_params.printParameters(mover);
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        it->second->printParameters(stream,mover+"\t");
    }
    void printParameters(const std::string mover="")
    {
        std::cout<<mover<<"Crate "<<getName()<<" parameters :\n";
        m_params.printParameters(mover);
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        it->second->printParameters(mover+"\t");
    }
    std::vector<MeasuresAndSets> getMeasuresAndSets(const std::string& who,const Value& channel)
    {
        std::vector<MeasuresAndSets>mes;
        mes.reserve(getNbrChannels());
        if(isInRack(who)||isCrate(who)||who=="")
        {
            for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
            {
                std::vector<MeasuresAndSets>mes2;
                if(channel.String()=="")mes2=it->second->getMeasuresAndSets();
                else mes2.push_back(it->second->getMeasuresAndSets(channel));
                for(unsigned int i=0;i!=mes2.size();++i)
                {
                    mes2[i].getPosition().setCrate(getName());
                    if(getRack()!="")mes2[i].getPosition().setRack(getRack());
                }
                mes.insert(mes.begin(),mes2.begin(),mes2.end());
            }
        }
        else if (hasModule(who))
        {
            std::vector<MeasuresAndSets>mes2;
            if(channel.String()=="")mes2=m_modules[who]->getMeasuresAndSets();
            else mes2.push_back(m_modules[who]->getMeasuresAndSets(channel));
            for(unsigned int i=0;i!=mes2.size();++i)
            {
                    mes2[i].getPosition().setCrate(getName());
                    if(getRack()!="")mes2[i].getPosition().setRack(getRack());
            }
            mes.insert(mes.begin(),mes2.begin(),mes2.end());
        }
        return mes;
    }
    Value getName()
    {
        return m_name;
    }
    Value getRack()
    {
        return m_rack;
    }
    void setName()
    {
        if(m_params.hasParam("Name"))
        {
            m_name=m_params["Name"];
        }
        else
        {
            std::cout<<"Crate Name is mandatory and must be unique \n";
            throw -1;
        }
    }
    void setRack()
    {
        if(m_params.hasParam("Rack"))
        {
            m_rack=m_params["Rack"];
        }
        else
        {
            std::cout<<"Rack for crate will be set to "<<m_rack<<"\n";
        }
    }
    void setDescription()
    {
        if(m_params.hasParam("Description"))
        {
            m_description=m_params["Description"];
        }
        else
        {
            std::cout<<"Description for crate will be blank !\n";
        }
    }
    void printModuleStatus()
    {
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           std::cout<<"Module "<<it->second->getName()<<" : "<<std::endl;
           it->second->getModuleStatus().print(1);
        }
    }
    
    void printChannelsStatus()
    {
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           std::cout<<"Module "<<it->second->getName()<<" : "<<std::endl;
           for(unsigned int j=0;j!=it->second->getNbrChannels().UInt();++j)
           {
               std::cout<<"Channel "<<j<<" : "<<std::endl;
               it->second->getStatus(j).print(1);
           }
        }
    }
    
    unsigned int getNbrChannels(const Value& who=Value(""))
    {
        unsigned int nbr=0;
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           if(who==it->second->getName()||who.String()==""||who==m_name) nbr+=it->second->getNbrChannels().UInt();
        }        
        return nbr;
    }
    Value getSerialNumberModule(const std::string& module)
    {
        return m_modules[module]->getSerialNumber();
    }
    Value getSerialNumber()
    {
        return m_serialNumber;
    }
    std::vector<Value> getModuleNames()
    {
        std::vector<Value> names;
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
            names.push_back(it->first);
        }
        return std::move(names);
    }
private:
    bool isInRack(const Value& rack)
    { 
        if(m_rack==rack.String()) return true;
        else return false;
    }
    bool isCrate(const Value& crate)
    {
        if(m_name==crate.String()) return true;
        else return false;
    }
    bool hasModule(const Value& Module)
    {
        if(m_modules.find(Module.String())!=m_modules.end()) return true;
        else return false;
    }
    std::map<std::string,Module*> m_modules;
    Value m_name{""};
    Value m_description{""};
    Value m_version{""};
    Value m_constructor{""};
    Value m_serialNumber{""};
    virtual void FillInfos(){};
    Connector* m_connector{nullptr};
    static DumbConnector m_dumb;
    Parameters m_params;
    Value m_rack{""};
};
#endif
