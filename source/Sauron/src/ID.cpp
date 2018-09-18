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
 
#include "ID.h"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "json/json.h"
#include "mariadb++/account.hpp"
#include "mariadb++/connection.hpp"
#include "mariadb++/time.hpp"
#include <fstream>

std::map<std::string,int> ID::m_rackIDs=std::map<std::string,int>();
std::map<std::string,std::array<int,2>> ID::m_crateIDs=std::map<std::string,std::array<int,2>>();
std::map<std::string,std::array<int,3>> ID::m_moduleIDs=std::map<std::string,std::array<int,3>>();
std::vector<std::array<std::string,3>> ID::m_moduleInfos=std::vector<std::array<std::string,3>>();
std::vector<std::array<std::string,3>> ID::m_crateInfos=std::vector<std::array<std::string,3>>();
std::vector<std::array<std::string,3>> ID::m_rackInfos=std::vector<std::array<std::string,3>>();
unsigned int ID::m_moduleNbr{0};
unsigned int ID::m_crateNbr{0};
unsigned int ID::m_rackNbr{0};
unsigned int ID::m_configID{0};

unsigned int ID::getConfigID()
{
    return m_configID;
}

void ID::printJSON(std::ostream& stream,const std::string& mover)
{
    stream<<createJSON(mover)<<std::endl;
}

void ID::writeJSON(const std::string& file)
{
    std::ofstream myfile;
    myfile.open(file.c_str());
    myfile<<createJSON("\t");
    myfile.close();
}

std::string ID::createJSON(const std::string& mover)
{
    Json::Value racks(Json::arrayValue);
    for(std::map<std::string,int>::iterator rack=m_rackIDs.begin();rack!=m_rackIDs.end();++rack)
    {
        Json::Value Jrack;
        Jrack["ID"]=rack->second;
        Jrack["Name"]=m_rackInfos[rack->second][0];
        Jrack["Description"]=m_rackInfos[rack->second][1];
        Jrack["SN"]=m_rackInfos[rack->second][2];
        Json::Value crates(Json::arrayValue);
        for(std::map<std::string,std::array<int,2>>::iterator crate=m_crateIDs.begin();crate!=m_crateIDs.end();++crate)
        {
            Json::Value Jcrate;
            Json::Value modules(Json::arrayValue);
            if(crate->second[0]==rack->second)
            {
                Jcrate["ID"]=crate->second[1];
                Jcrate["Name"]=m_crateInfos[crate->second[1]][0];
                Jcrate["Description"]=m_crateInfos[crate->second[1]][1];
                Jcrate["SN"]=m_crateInfos[crate->second[1]][2];
            }
            else continue;
            for(std::map<std::string,std::array<int,3>>::iterator module=m_moduleIDs.begin();module!=m_moduleIDs.end();++module)
            {
                if(module->second[0]==rack->second&&module->second[1]==crate->second[1])
                {
                    Json::Value Jmodule;
                    Jmodule["ID"]=module->second[2];
                    Jmodule["Name"]=m_moduleInfos[module->second[2]][0];
                    Jmodule["Description"]=m_moduleInfos[module->second[2]][1];
                    Jmodule["SN"]=m_moduleInfos[module->second[2]][2];
                    modules.append(Jmodule);
                }
            }
            Jcrate["Modules"]=modules;
            crates.append(Jcrate);
        }
        Jrack["Crates"]=crates;
        racks.append(Jrack);
    }
    Json::StreamWriterBuilder builder;
    builder["indentation"] = mover; // If you want whitespace-less output
    return Json::writeString(builder,racks);
}



void ID::addDescription(const std::string& who,const std::string& description)
{
    if(isRack(who)==true) m_rackInfos[getRackID(who)][1]=description;
    else if (isCrate(who)==true) m_crateInfos[getCrateID(who)][1]=description;
    else if (isModule(who)==true) m_moduleInfos[getModuleID(who)][1]=description;
}
    
void ID::addSerialNumber(const std::string& who,const std::string& sn)
{
    if(isRack(who)==true) m_rackInfos[getRackID(who)][2]=sn;
    else if (isCrate(who)==true) m_crateInfos[getCrateID(who)][2]=sn;
    else if (isModule(who)==true) m_moduleInfos[getModuleID(who)][2]=sn;  
}

bool ID::isRack(const std::string& who)
{
    if(m_rackIDs.find(who)!=m_rackIDs.end()) return true;
    else return false;
}

bool ID::isCrate(const std::string& who)
{
    if(m_crateIDs.find(who)!=m_crateIDs.end()) return true;
    else return false;
}

bool ID::isModule(const std::string& who)
{
    if(m_moduleIDs.find(who)!=m_moduleIDs.end()) return true;
    else return false;
}


void ID::createRackID(const std::string& rack)
{
    if(m_rackIDs.find(rack)==m_rackIDs.end())
    {
        m_rackIDs[rack]=m_rackNbr++;
        m_rackInfos.push_back({rack,"",""});
        m_rackNbr++;
    }
}
 
 //Just to feel consistent
void ID::addRack(const std::string& rack)
{
    createRackID(rack);
}

void ID::addCrate(const std::string& rack,const std::string& crate)
{
    std::map<std::string,std::array<int,2>>::iterator it=m_crateIDs.find(crate);
    if(it!=m_crateIDs.end())
    {    
        m_crateIDs[crate][0]=getRackID(rack);
    }
}

void ID::createCrateID(const std::string& crate)
{
    if(m_crateIDs.find(crate)==m_crateIDs.end())
    {    
        m_crateIDs.insert({crate,{-1,static_cast<int>(m_crateNbr)}});
        m_crateInfos.push_back({crate,"",""});
        m_crateNbr++;
    }
}

void ID::addModule(const std::string& rack,const std::string& crate,const std::string& module)
{
    std::map<std::string,std::array<int,3>>::iterator it=m_moduleIDs.find(module);
    if(it!=m_moduleIDs.end())
    {    
        m_moduleIDs[module][0]=getRackID(rack);
        m_moduleIDs[module][1]=getCrateID(crate);
    }
}


void ID::createModuleID(const std::string& module)
{
    if(m_moduleIDs.find(module)==m_moduleIDs.end())
    {    
        m_moduleIDs.insert({module,{-1,-1,static_cast<int>(m_moduleNbr)}});
        m_moduleInfos.push_back({module,"",""});
        m_moduleNbr++;
    }
}

void ID::print(std::ostream& stream,const std::string& mover)
{
    stream<<mover<<"ID(s) : "<<std::endl;
    for(std::map<std::string,int>::iterator rack=m_rackIDs.begin();rack!=m_rackIDs.end();++rack)
    {
        stream<<mover<<"\tRack "<<rack->first<<" ID("<<rack->second<<")"<<std::endl;
        for(std::map<std::string,std::array<int,2>>::iterator crate=m_crateIDs.begin();crate!=m_crateIDs.end();++crate)
        {
            if(crate->second[0]==rack->second)
            {
                stream<<mover<<"\t\tCrate "<<crate->first<<" ID("<<crate->second[1]<<"), ";
                stream<<"Description : "<<m_crateInfos[crate->second[1]][1]<<", ";
                stream<<"SN : "<<m_crateInfos[crate->second[1]][2]<<std::endl;
            }
            else continue;
            for(std::map<std::string,std::array<int,3>>::iterator module=m_moduleIDs.begin();module!=m_moduleIDs.end();++module)
            {
                if(module->second[0]==rack->second&&module->second[1]==crate->second[1])
                {
                    stream<<mover<<"\t\t\t "<<module->first<<" ID("<<module->second[2]<<"), ";
                    stream<<"Description : "<<m_moduleInfos[module->second[2]][1]<<", ";
                    stream<<"SN : "<<m_moduleInfos[module->second[2]][2]<<std::endl;
                }
            }
        }
    }
}

unsigned int ID::getModuleID(const std::string& module)
{
    return  m_moduleIDs[module][2];
}

unsigned int ID::getCrateID(const std::string& crate)
{
    return m_crateIDs[crate][1];
}
    
void ID::setConfigID(const unsigned int& id)
{
    m_configID=id;
}

unsigned int ID::getRackID(const std::string& rack)
{
    return m_rackIDs[rack];
}

std::string ID::getModuleName(const unsigned int& module) 
{
    return  m_moduleInfos[module][0];
}
    
std::string ID::getCrateName(const unsigned int& crate)
{
    return m_crateInfos[crate][0];
}
    
std::string ID::getRackName(const unsigned int& rack)
{
    return m_rackInfos[rack][0];
}
    
unsigned int ID::getNbrOfModules()
{ 
    return m_moduleNbr;
}
    
unsigned int ID::getNbrOfCrates()
{ 
    return m_crateNbr;
}

unsigned int ID::getNbrOfRacks()
{ 
    return m_rackNbr;
}
