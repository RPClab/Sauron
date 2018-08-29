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

#include "Parameters.h"
#include "json/json.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "PrintVoltageCurrent.h"
#include "MonitorVoltages.h"


RacksManager::RacksManager()
{
    Json::Value root=openJSONFile("RacksConfFile");
    extractInfos(root);
    plugMonitor(new PrintVoltageCurrent);
    plugMonitor(new MonitorVoltages);
};

std::map<std::string,Monitoring*> RacksManager::m_monitoring=std::map<std::string,Monitoring*>();
 
void RacksManager::plugMonitor(Monitoring* monitoring)
{
    m_monitoring[monitoring->getName()]=monitoring;
    m_monitoring[monitoring->getName()]->setRacksManager(this);
}

RacksManager::~RacksManager()
{
    for(std::map<std::string,Connector*>::iterator it=m_connectors.begin();it!=m_connectors.end();++it) delete it->second;
    for(std::map<std::string,Module*>::iterator it=m_modules.begin();it!=m_modules.end();++it) delete it->second;
    for(std::map<std::string,Crate*>::iterator it=m_racks.begin();it!=m_racks.end();++it) delete it->second;
    for(std::map<std::string,Monitoring*>::iterator it=m_monitoring.begin();it!=m_monitoring.end();++it)delete it->second;
}                                        
                                                  
std::string RacksManager::getEnvVar(const std::string & key )
{
    if(std::getenv( key.c_str() )==nullptr) return "";
    else return std::string(std::getenv( key.c_str()));
}

Json::Value RacksManager::openJSONFile(const std::string & envVar)
{
    Json::CharReaderBuilder builder;
    Json::Value obj;   // will contain the root value after parsing.
    std::string errs{""};
    std::string FileName=getEnvVar(envVar.c_str());
    if(FileName=="")
    {
        std::cout<<"Please add "<<envVar<<" as variable environment ! \n";
        std::exit(2);
    }
    std::ifstream ConfFile(FileName.c_str(),std::ifstream::binary);
    if ( !Json::parseFromStream(builder,ConfFile,&obj,&errs) )
    {
        std::cout  << errs << "\n";
    }
    return obj;
}

void RacksManager::FillConnectorCrateInfos(const Json::Value& json,Parameters& params)
{
    
    std::vector<std::string> id_connector = json.getMemberNames();
    if(keyExists(id_connector,"Type")!=true)
    {
        std::cout<<"Connector type is mandatory !"<<std::endl;
        throw -2;
    }
    else
    {
        for (std::vector<std::string>::iterator itt=id_connector.begin();itt!=id_connector.end();++itt) 
        {
            params.addParameter(*itt,json[*itt].asString());
        }
    }
}


void RacksManager::FillModuleConnectorInfos(const Json::Value& json,std::map<std::string,Parameters>& c_params)
{
    std::vector<std::string> id_connector = json["Connector"].getMemberNames();
    if(keyExists(id_connector,"Type")!=true)
    {
        std::cout<<"Connector type is mandatory !"<<std::endl;
        throw -2;
    }
    else
    {
        for (std::vector<std::string>::iterator itt=id_connector.begin();itt!=id_connector.end();++itt) 
        {
            c_params[json["Name"].asString()].addParameter(*itt,json["Connector"][*itt].asString());
        }
    }  
}

void RacksManager::FillModuleInfos(const Json::Value& json,std::map<std::string,Parameters>& m_params,std::map<std::string,Parameters>& c_params)
{
    for(unsigned int module=0;module!=json.size();++module)
    {
        if(keyExistsAndValueIsUnique(json[module],"Name")!=true)
        {
            std::cout<<"Module's name is mandatory and must be unique !"<<std::endl;
            throw -1;
        }
        std::vector<std::string> id_module = json[module].getMemberNames();
        for (std::vector<std::string>::iterator ot=id_module.begin();ot!=id_module.end();++ot) 
        {
            if(*ot=="Connector")
            {
                FillModuleConnectorInfos(json[module],c_params);
            }
            else
            {
                m_params[json[module]["Name"].asString()].addParameter(*ot,json[module][*ot].asString());
            }
        }
    }
}

void RacksManager::extractInfos(const Json::Value& root)
{
    Parameters crate_infos;
    Parameters connector_infos_crate;
    std::map<std::string,Parameters> module_infos;
    std::map<std::string,Parameters> connector_infos_modules;
    const Json::Value& crates = root["Crates"]; 
    for (unsigned int i = 0; i < crates.size(); i++)
    {
        crate_infos.clear();
        connector_infos_crate.clear();
        module_infos.clear();
        connector_infos_modules.clear();
        if(keyExists(crates[i],"Type")!=true)
        {
            std::cout<<"Crate type is mandatory !"<<std::endl;
            throw -2;
        }
        std::vector<std::string> crate_params = crates[i].getMemberNames();
        if(keyExistsAndValueIsUnique(crates[i],"Name")!=true)
        {
            std::cout<<"Crate's name is mandatory and must be unique !"<<std::endl;
            throw -1;
        }
        for (std::vector<std::string>::iterator it=crate_params.begin();it!=crate_params.end();++it) 
        {
            if(*it=="Connector")
            {
                FillConnectorCrateInfos(crates[i]["Connector"],connector_infos_crate);
            }
            else if(*it=="Modules")
            {
                FillModuleInfos(crates[i]["Modules"],module_infos,connector_infos_modules);
            }
            else
            {
                crate_infos.addParameter(*it,crates[i][*it].asString());
            }
        }
        constructCrate(crate_infos,connector_infos_crate,module_infos,connector_infos_modules);
    }
}


bool RacksManager::keyExists(const std::vector<std::string> id,const std::string & key)
{
    std::vector<std::string>::const_iterator toto =std::find(id.begin(),id.end(),key);
    if(toto==id.end())return false;
    else return true;
}

bool RacksManager::keyExists(const Json::Value& id,const std::string & key)
{
    return keyExists(id.getMemberNames(),key);
}

bool RacksManager::keyExistsAndValueIsUnique(const Json::Value& id,const std::string & key)
{
    static std::map<std::string,std::vector<std::string>> values;
    if(keyExists(id,key)==false)
    {
        return false;
    }
    else
    {
        //key is unkmown first time is encountered
        std::map<std::string,std::vector<std::string>>::iterator it=values.find(key);
        if(it==values.end())
        {
            values[key].push_back(id[key].asString());
            return true;
        }
        else
        {
            bool exists=keyExists(values[key],id[key].asString());
            if(exists==true) return false;
            else
            {
                 values[key].push_back(id[key].asString());
                 return true;
            }
        }
    }
}

void RacksManager::constructCrate(Parameters& crate_infos,Parameters& connector_infos_crate,std::map<std::string,Parameters>& module_infos,
                   std::map<std::string,Parameters>& connector_infos_modules)
{
  m_racks[crate_infos["Name"].String()]=new Crate(crate_infos);
  Connector* connector_crate=nullptr;
  if(m_connectors.find(connector_infos_crate["Type"].String())!=m_connectors.end())
  {
      connector_crate=m_connectors[connector_infos_crate["Type"].String()]->Clone();
      connector_crate->isCrateConnector(true);
      connector_crate->setParameters(connector_infos_crate);
  }
  else if(m_connectors.find(connector_infos_crate["Type"].String())!=m_connectors.end()&&connector_infos_crate.size()!=0)
  {
    std::cout<<"Connector type : "<<connector_infos_crate["Type"]<<" unknown"<<std::endl;
  }
  for(std::map<std::string,Parameters>::iterator itt=module_infos.begin();itt!=module_infos.end();++itt)
  {
    if(m_modules.find(itt->second["Type"].String())!=m_modules.end())
    {
        m_racks[crate_infos["Name"].String()]->addModule(itt->first,m_modules[module_infos[itt->first]["Type"].String()]);
        m_racks[crate_infos["Name"].String()]->setModuleParameters(itt->first,module_infos[itt->first]);
        if(connector_infos_modules.find(itt->first)==connector_infos_modules.end())
        {
            if(connector_crate==nullptr)
            {
                std::cout<<"Error : or crate and/or Module should have a connector attached \n";
                throw -3;
            }
            else
            {
                m_racks[crate_infos["Name"].String()]->setModuleConnector(itt->first,connector_crate);
            }
        }
        else
        {
            m_racks[crate_infos["Name"].String()]->setModuleConnector(itt->first,m_connectors[connector_infos_modules[itt->first]["Type"].String()]->Clone());
            m_racks[crate_infos["Name"].String()]->setModuleConnectorParameters(itt->first,connector_infos_modules[itt->first]);
        }
    }
    else
    {
        m_racks[crate_infos["Name"].String()]->addModule(itt->first,m_modules[crate_infos["Type"].String()]);
        m_racks[crate_infos["Name"].String()]->setModuleParameters(itt->first,module_infos[itt->first]);
        if(connector_infos_modules.find(itt->first)==connector_infos_modules.end())
        {
            if(connector_crate==nullptr)
            {
                std::cout<<"Error : or crate and/or Module should have a connector attached \n";
                throw -3;
            }
            else
            {
                m_racks[crate_infos["Name"].String()]->setModuleConnector(itt->first,connector_crate);
            }
        }
        else
        {
            m_racks[crate_infos["Name"].String()]->setModuleConnector(itt->first,m_connectors[connector_infos_modules[itt->first]["Type"].String()]->Clone());
            m_racks[crate_infos["Name"].String()]->setModuleConnectorParameters(itt->first,connector_infos_modules[itt->first]);
        }
    }
  }
}

void RacksManager::Release()
{
    for(std::map<std::string,Monitoring*>::iterator it=m_monitoring.begin();it!=m_monitoring.end();++it)
    {
        it->second->stop();
    }
}

void RacksManager::startMonitoring(const std::string& name,unsigned int time)
{
    if(m_monitoring.find(name)!=m_monitoring.end()) 
    {
        if(time!=0)  m_monitoring[name]->setTime(time);
        m_monitoring[name]->start();
    }
    else
    {
        std::cout<<"Monitoring with name : "<<name<<" not loaded !! "<<std::endl;
    }
}
    
void RacksManager::stopMonitoring(const std::string& name)
{
    if(m_monitoring.find(name)!=m_monitoring.end()) 
    {
        m_monitoring[name]->stop();
    }
    else
    {
        std::cout<<"Monitoring with name : "<<name<<" not loaded !! "<<std::endl;
    }
}

