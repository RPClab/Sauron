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
#include "SerialConnector.h"
#include "SNMPConnector.h"
#include "CAENConnector.h"
#include "Crate.h"
#include "isegModule.h"
#include "WIENERModule.h"
#include "CAENModule.h"
#include <set>
#include <chrono>
#include <thread>
#include <functional>

class Monitoring;

class RacksManager 
{
public: 
    RacksManager();
    ~ RacksManager();
    void plugMonitor(Monitoring*);
    void initialize()
    {
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it) it->second->Initialize();
    }
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
    
    void Release();
    
    void disconnect()
    {
        Release();
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
            itt->second->Disconnect();
        }
    }
    void connect()
    {
        std::lock_guard<std::mutex> lock(mutexx);
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->Connect();
        }
        fillSerialNumbers();
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
            nbr+=itt->second->getNbrChannels();
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
    void fillSerialNumbers()
    {
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
            ID::addSerialNumber(it->first,it->second->getSerialNumber().String());
            std::vector<Value> a= it->second->getModuleNames();
            for(unsigned int i=0;i!=a.size();++i)
            {
                ID::addSerialNumber(a[i].String(),it->second->getSerialNumberModule(a[i].String()).String());
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
    static std::map<std::string,Monitoring*> m_monitoring;
    bool keyExists(const Json::Value&,const std::string& string);
    bool keyExists(const std::vector<std::string>,const std::string& string);
    bool keyExistsAndValueIsUnique(const Json::Value& id,const std::string & key);
    void constructCrate(Parameters&,Parameters&,std::map<std::string,Parameters>&,std::map<std::string,Parameters>&);
    void extractInfos(const Json::Value& root);
    std::map<std::string,Crate*> m_racks;
    std::string getEnvVar(const std::string & key );
    Json::Value openJSONFile(const std::string& envVar);
    std::map<std::string,Connector*> m_connectors{{"VCP",new SerialConnector},{"SNMP",new SNMPConnector},{"CAEN",new CAENConnector}};
    std::map<std::string,Module*> m_modules{{"WIENER",new WIENERModule},{"iseg",new isegModule},{"CAEN",new CAENModule}};
    std::string m_who={""};
    Value m_channel={""};
};
#endif
