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


#include "RacksManager.h"
#include "json/json.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

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

void RacksManager::extractInfos(const Json::Value& root)
{
    std::map<std::string,std::string> crate_infos;
    std::map<std::string,std::string> connector_infos_crate;
    std::map<std::string,std::map<std::string,std::string>> module_infos;
    std::map<std::string,std::map<std::string,std::string>> connector_infos_modules;
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
        std::vector<std::string> id = crates[i].getMemberNames();
        if(keyExistsAndValueIsUnique(crates[i],"Name")!=true)
        {
            std::cout<<"Crate's name is mandatory and must be unique !"<<std::endl;
            throw -1;
        }
        else
        {
            for (std::vector<std::string>::iterator it=id.begin();it!=id.end();++it) 
            {
                if(*it=="Connector")
                {
                    std::vector<std::string> id_connector = crates[i]["Connector"].getMemberNames();
                    if(keyExists(id_connector,"Type")!=true)
                    {
                        std::cout<<"Connector type is mandatory !"<<std::endl;
                        throw -2;
                    }
                    else
                    {
                        for (std::vector<std::string>::iterator itt=id_connector.begin();itt!=id_connector.end();++itt) 
                        {
                            connector_infos_crate.insert({*itt,crates[i]["Connector"][*itt].asString()});
                        }
                    }
                }
                else if(*it=="Modules")
                {
                    for(unsigned int module=0;module!=crates[i]["Modules"].size();++module)
                    {
                        if(keyExistsAndValueIsUnique(crates[i]["Modules"][module],"Name")!=true)
                        {
                            std::cout<<"Module's name is mandatory and must be unique !"<<std::endl;
                            throw -1;
                        }
                        std::vector<std::string> id_module = crates[i]["Modules"][module].getMemberNames();
                        for (std::vector<std::string>::iterator ot=id_module.begin();ot!=id_module.end();++ot) 
                        {
                            if(*ot=="Connector")
                            {
                                std::vector<std::string> id_connector = crates[i]["Modules"][module]["Connector"].getMemberNames();
                                if(keyExists(id_connector,"Type")!=true)
                                {
                                    std::cout<<"Connector type is mandatory !"<<std::endl;
                                    throw -2;
                                }
                                else
                                {
                                    for (std::vector<std::string>::iterator itt=id_connector.begin();itt!=id_connector.end();++itt) 
                                    {
                                        connector_infos_modules[crates[i]["Modules"][module]["Name"].asString()].insert({*itt,crates[i]["Modules"][module]["Connector"][*itt].asString()});

                                    }
                                }
                            }
                            else
                            {
                               module_infos[crates[i]["Modules"][module]["Name"].asString()].insert({*ot,crates[i]["Modules"][module][*ot].asString()});
                            }
                        }
                    }
                }
                else
                {
                    crate_infos.insert({*it,crates[i][*it].asString()});
                    
                }
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
            return !keyExists(values[key],id[key].asString());
        }
    }
}

void RacksManager::constructCrate(std::map<std::string,std::string>& crate_infos,
                   std::map<std::string,std::string>& connector_infos_crate,
                   std::map<std::string,std::map<std::string,std::string>>& module_infos,
                   std::map<std::string,std::map<std::string,std::string>>& connector_infos_modules)
{
  m_racks[crate_infos["Name"]]=new Crate(crate_infos);
  Connector* connector_crate=nullptr;
  if(m_connectors.find(connector_infos_crate["Type"])!=m_connectors.end())
  {
      connector_crate=m_connectors[connector_infos_crate["Type"]]->Clone();
      connector_crate->setParameters(connector_infos_crate);
  }
  else if(m_connectors.find(connector_infos_crate["Type"])!=m_connectors.end()&&connector_infos_crate.size()!=0)
  {
    std::cout<<"Connector type : "<<connector_infos_crate["Type"]<<" unknown"<<std::endl;
  }
  for(std::map<std::string,std::map<std::string,std::string>>::iterator itt=module_infos.begin();itt!=module_infos.end();++itt)
  {
    if(m_modules.find(itt->second["Type"])!=m_modules.end())
    {
        m_racks[crate_infos["Name"]]->addModule(itt->first,m_modules[module_infos[itt->first]["Type"]]);
        m_racks[crate_infos["Name"]]->setModuleParameters(itt->first,module_infos[itt->first]);
        if(connector_infos_modules.find(itt->first)==connector_infos_modules.end())
        {
            if(connector_crate==nullptr)
            {
                std::cout<<"Error : or crate and/or Module should have a connector attached \n";
                throw -3;
            }
            else
            {
                m_racks[crate_infos["Name"]]->setModuleConnector(itt->first,connector_crate);
            }
        }
        else
        {
            m_racks[crate_infos["Name"]]->setModuleConnector(itt->first,m_connectors[connector_infos_modules[itt->first]["Type"]]->Clone());
            m_racks[crate_infos["Name"]]->setModuleConnectorParameters(itt->first,connector_infos_modules[itt->first]);
        }
    }
    else
    {
        m_racks[crate_infos["Name"]]->addModule(itt->first,m_modules[crate_infos["Type"]]);
        m_racks[crate_infos["Name"]]->setModuleParameters(itt->first,module_infos[itt->first]);
        if(connector_infos_modules.find(itt->first)==connector_infos_modules.end())
        {
            if(connector_crate==nullptr)
            {
                std::cout<<"Error : or crate and/or Module should have a connector attached \n";
                throw -3;
            }
            else
            {
                m_racks[crate_infos["Name"]]->setModuleConnector(itt->first,connector_crate);
            }
        }
        else
        {
            m_racks[crate_infos["Name"]]->setModuleConnector(itt->first,m_connectors[connector_infos_modules[itt->first]["Type"]]->Clone());
            m_racks[crate_infos["Name"]]->setModuleConnectorParameters(itt->first,connector_infos_modules[itt->first]);
        }
    }
  }
}

