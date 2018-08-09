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
    void on()
    {
        std::lock_guard<std::mutex> lock(mutexx);
       // if(block==false)
       // {
          //  block=true;
            for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
            {
                itt->second->on();
            }
         //   block=false;
       // }
       // else on();
    }
    void off()
    {
        std::lock_guard<std::mutex> lock(mutexx);
       // if(block==false)
       // {
       //     block=true;
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->off();
        }
      //  block=false;
       //  }
        // else off();
        
    }
    void disconnect()
    {
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->Disconnect();
        }
    }
    void connect()
    {
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->Connect();
        }
    }
    void printVoltageCurrent()
    {
        std::lock_guard<std::mutex> lock(mutexx);
       // if(block==false)
       // {
       //     block=true;
        for(std::map<std::string,Crate*>::iterator itt=m_racks.begin();itt!=m_racks.end();++itt)
        {
                itt->second->printVoltageCurrent();
        }
       //         block=false;
       //  }

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
    
    std::vector<Measure> getMeasures()
    {
        std::vector<Measure>mes;
        mes.reserve(getNbrChannels());
        for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it)
        {
            std::vector<Measure> mes2=it->second->getMeasures();
            std::move(mes2.begin(),mes2.end(), std::inserter(mes,mes.end()));
        }
        return std::move(mes);
    }
    
    
private :
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
};
#endif
