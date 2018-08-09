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
    Crate(Parameters & param,Connector& connector):m_connector(connector.Clone())
    {
        m_params=param;
    };
    Crate(Parameters& param,Connector* connector):m_connector(connector->Clone())
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
    Crate(Connector& connector):m_connector(connector.Clone()){};
    Crate(Connector* connector):m_connector(connector->Clone()){};
    void Initialize()
    {
        setName();
        setDescription();
        setRack();
        if(m_connector!=nullptr)
        {
            m_connector->Initialize();
        }
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->Initialize();
        }
    }
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
    Crate& operator()(const Crate& module)
    {
        m_connector=module.m_connector->Clone();
        return *this;
    }
    void addModule(Module* module)
    {
        module->Initialize();
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
            m_modules.insert(std::pair<std::string,Module*>(module->getName().String(),module->Clone()));
        }
    }
    void addModule(const std::string& name,Module* module)
    {
        m_modules.insert(std::pair<std::string,Module*>(name,module->Clone()));
    }
    Crate& operator=(const Crate& module)
    {
        m_connector=module.m_connector->Clone();
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
    
    void off()
    {
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->off();
        }
    }
        void on()
    {
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->on();
        }
    }
    
    void Connect()
    {
        if(m_connector!=nullptr)
        {
            m_connector->Connect();
        }
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->Connect();
        }
    }
    
    void Disconnect()
    {
        if(m_connector!=nullptr)
        {
            m_connector->Disconnect();
        }
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           it->second->Disconnect();
        }
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
    std::vector<Measure> getMeasures()
    {
        std::vector<std::thread>threads;
        std::vector<Measure>mes;
        mes.reserve(getNbrChannels());
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
            std::vector<Measure> mes2=it->second->getMeasures();
            for(unsigned int i=0;i!=mes2.size();++i)
            {
                mes2[i].setCrate(getName());
                mes2[i].setRack(getRack());
            }
            std::move(mes2.begin(),mes2.end(), std::inserter(mes,mes.end()));
        }
        return std::move(mes);
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
    unsigned int getNbrChannels()
    {
        unsigned int nbr=0;
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it)
        {
           nbr+=it->second->getNbrChannels().UInt();
        }
        return nbr;
    }
private:
    std::map<std::string,Module*> m_modules;
    Value m_name{""};
    Value m_description{""};
    Value m_version{""};
    Value m_constructor{""};
    virtual void FillInfos(){};
    Connector* m_connector{nullptr};
    static DumbConnector m_dumb;
    Parameters m_params;
    Value m_rack{"Unknow"};
};
#endif
