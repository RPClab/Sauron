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

class RacksManager 
{
public: 
    RacksManager()
    {
        Json::Value root=openJSONFile("RacksConfFile");
        extractInfos(root);
    };
    ~ RacksManager()
    {
        for(std::map<std::string,Connector*>::iterator it=m_connectors.begin();it!=m_connectors.end();++it) delete it->second;
        for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it) delete it->second;
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it) delete it->second;
    }
    void Initialize()
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
    
    
    
    
    void disconnect()
    {
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
    std::thread printVoltageCurrentthread()
    {
        return std::thread(&RacksManager::loop,this);
    }
    
    
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
            std::move(mes2.begin(),mes2.end(), std::inserter(mes,mes.end()));
        }
        return std::move(mes);
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
    
private :
    void FillConnectorCrateInfos(const Json::Value& json,Parameters& params);
    void FillModuleInfos(const Json::Value& json,std::map<std::string,Parameters>& m_params,std::map<std::string,Parameters>& c_params);
    void FillModuleConnectorInfos(const Json::Value& json,std::map<std::string,Parameters>& c_params);
    void loop()
    {
        while(1)
        {
            printVoltageCurrent();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

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
