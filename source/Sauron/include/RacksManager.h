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

#ifndef RACKSMANAGER_H
#define RACKSMANEGER_H
#include <string>
#include <mutex>
#include <map>
#include "json/json.h"
#include "Crate.h"
#include <set>
#include <chrono>
#include <thread>
#include <functional>
#include "Kernel.h"
class Monitoring;

class RacksManager 
{
public: 
    static RacksManager& instance() 
    {
        static RacksManager S;
        return S;
    }
    void plugMonitor(Monitoring*);
    void initialize();
    void printParameters(std::ostream& stream=std::cout,const std::string mover="")
    {
        stream<<"Parameters : \n";
        for(std::set<std::string>::iterator it=getListRacks().begin();it!=getListRacks().end();++it)
        {
            stream<<"Rack "<<*it<<" : \n";
            for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
            {
                if(itt->second->getRack()==*it) itt->second->printParameters(stream,"\t");
            }
        }
    }
    
    //ON
    void on()
    {
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
            itt->second->on(m_who,m_channel);
        }
    }
    //OFF
    void off()
    {
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
            itt->second->off(m_who,m_channel);
        }
    }
    //SET VOLTAGE WANTED
    void setWantedVoltage(const Value& voltage)
    {
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->setWantedVoltage(m_who,m_channel,voltage);
        }
    }
    
    //SET VOLTAGE
    void setVoltage(const Value& voltage)
    {
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->setVoltage(m_who,m_channel,voltage);
        }
    }
    //GET VOLTAGE
    std::vector<VoltageSet> getVoltage()
    {
        std::vector<VoltageSet> ret;
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                std::vector<VoltageSet> rett=itt->second->getVoltage(m_who,m_channel);
                ret.insert(ret.end(),rett.begin(),rett.end());
        }
        return std::move(ret);
    }
    // GET WANTED VOLTAGE   
    std::vector<VoltageWanted> getWantedVoltage()
    {
        std::vector<VoltageWanted> ret;
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                std::vector<VoltageWanted> rett=itt->second->getWantedVoltage(m_who,m_channel);
                ret.insert(ret.end(),rett.begin(),rett.end());
        }
        return std::move(ret);
    }
    
    // GET CURRENT MEASURED
    std::vector<CurrentMeasured> getCurrentMeasured()
    {
        std::vector<CurrentMeasured> ret;
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                std::vector<CurrentMeasured> rett=itt->second->getCurrentMeasured(m_who,m_channel);
                ret.insert(ret.end(),rett.begin(),rett.end());
        }
        return std::move(ret);
    }
    
    
    void Release();
    
    void disconnect()
    {
        if(isConnected==true)
        {
            Release();
            std::lock_guard<std::mutex> lock(mutexx);
            for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
            {
                itt->second->Disconnect();
            }
            isInitialized=false;
            isConnected=false;
        }
    }
    void connect()
    {
        if(isInitialized==false)
        {
            std::cout<<"Initialize first !!"<<std::endl;
        }
        else if(isConnected==false&&isInitialized==true)
        {
            std::lock_guard<std::mutex> lock(mutexx);
            for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
            {
                itt->second->Connect();
            }
            fillSerialNumbers();
            isConnected=true;
        }
    }
    void printVoltageCurrent()
    {
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->printVoltageCurrent();
        }
    }
    unsigned int getNbrChannels()
    {
        unsigned int nbr=0;
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
            nbr+=itt->second->getNbrChannels(m_who);
        }
        return nbr;
    }
    void startMonitoring(const std::string& name,unsigned int time=0);

    void stopMonitoring(const std::string& name);

    
    void printModuleStatus()
    {
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
           it->second->printModuleStatus();
        }
    }
    
    void printChannelsStatus()
    {
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
           it->second->printChannelsStatus();
        }
    }
    
    std::vector<MeasuresAndSets> getMeasuresAndSets()
    {
        std::vector<MeasuresAndSets>mes;
        mes.reserve(getNbrChannels());
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
            std::vector<MeasuresAndSets> mes2=it->second->getMeasuresAndSets(m_who,m_channel);
            mes.insert(mes.end(),mes2.begin(),mes2.end());
        }
        return mes;
    }
    RacksManager& operator()(const std::string& who="",const std::string& channel="")
    {
        m_who=who;
        m_channel=channel;
        return *this;
    }
    RacksManager& operator()(const std::string& who,const int& channel)
    {
        m_who=who;
        m_channel=channel;
        return *this;
    }
    
    void setIsMonitored(const Value& name,const Value& channel,const bool& mon)
    {
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
            it->second->setIsMonitored(name,channel,mon);
        }
    }

private :
    bool isInitialized{false};
    bool isConnected{false};
    RacksManager();
    ~RacksManager();
    RacksManager(const RacksManager& other)=delete; // copy constructor
    RacksManager(RacksManager&& other)=delete; // move constructor
    RacksManager& operator=(const RacksManager& other)=delete; // copy assignment
    RacksManager& operator=(RacksManager&& other)=delete; // move assignment
    void fillSerialNumbers()
    {
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
            m_id.addSerialNumber(it->first,it->second->getSerialNumber().String());
            std::vector<Value> a= it->second->getModuleNames();
            for(unsigned int i=0;i!=a.size();++i)
            {
                m_id.addSerialNumber(a[i].String(),it->second->getSerialNumberModule(a[i].String()).String());
            }
        }
    }
    void FillConnectorCrateInfos(const Json::Value& json,Parameters& params);
    void FillModuleInfos(const Json::Value& json,std::map<std::string,Parameters>& m_params,std::map<std::string,Parameters>& c_params);
    void FillModuleConnectorInfos(const Json::Value& json,std::map<std::string,Parameters>& c_params);
    void identify(const Json::Value& json);
    std::set<std::string> getListRacks()
    {
        std::set<std::string> rackNames; 
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
            rackNames.insert(it->second->getRack().String());
        }
        return std::move(rackNames);
    }
    std::mutex mutexx;
    static std::map<std::string,std::vector<std::string>> values;
    static std::map<std::string,Monitoring*> m_monitoring;
    bool keyExists(const Json::Value&,const std::string& string);
    bool keyExists(const std::vector<std::string>,const std::string& string);
    bool keyExistsAndValueIsUnique(const Json::Value& id,const std::string & key);
    void constructCrate(Parameters&,Parameters&,std::map<std::string,Parameters>&,std::map<std::string,Parameters>&);
    void extractInfos(const Json::Value& root);
    void loadConnectors();  
    void loadMonitors();
    void loadModules();
    void m_loadGeneralConfigs()
    {
        const Json::Value& paths=openJSONFile(getEnvVar("SAURON_CONF"));
        if(keyExists(paths["Paths"],"Connectors")!=true)
            {
                std::cout<<"Connectors path variable is mandatory!"<<std::endl;
                throw -2;
            }
            else m_connectorsPath=paths["Paths"]["Connectors"].asString();
            if(keyExists(paths["Paths"],"Modules")!=true)
            {
                std::cout<<"Modules path variable is mandatory!"<<std::endl;
                throw -2;
            }
            else m_modulesPath=paths["Paths"]["Modules"].asString();
            if(keyExists(paths["Paths"],"Crates")!=true)
            {
                std::cout<<"Crates path variable is mandatory!"<<std::endl;
                throw -2;
            }
            else m_cratesPath=paths["Paths"]["Crates"].asString();
            if(keyExists(paths["Paths"],"Monitors")!=true)
            {
                std::cout<<"Monitors path variable is mandatory!"<<std::endl;
                throw -2;
            }
            else m_monitorPath=paths["Paths"]["Monitors"].asString();
            if(keyExists(paths["Paths"],"Configs")!=true)
            {
                std::cout<<"Configs path variable is mandatory!"<<std::endl;
                throw -2;
            }
            else m_configsPath=paths["Paths"]["Configs"].asString();
    }
    void loadPlugins(const std::string path);
    ID& m_id{ID::instance()};
    std::string m_connectorsPath{""};
    std::string m_modulesPath{""};
    std::string m_cratesPath{""};
    std::string m_monitorPath{""};
    std::string m_configsPath{""};
    std::map<std::string,Crate*> m_racks;
    std::string getEnvVar(const std::string & key );
    Json::Value openJSONFile(const std::string& envVar);
    std::map<std::string,Connector*> m_connectors;
    std::map<std::string,Module*> m_modules;
    std::string m_who={""};
    Value m_channel={""};
    pugg::Kernel m_kernel;
};
#endif
