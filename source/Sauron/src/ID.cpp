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

std::map<std::string,unsigned int> ID::m_moduleIDs=std::map<std::string,unsigned int>();
std::map<std::string,unsigned int> ID::m_crateIDs=std::map<std::string,unsigned int>();
std::map<std::string,unsigned int> ID::m_rackIDs=std::map<std::string,unsigned int>();
std::vector<std::string> ID::m_moduleNames=std::vector<std::string>();
std::vector<std::string> ID::m_crateNames=std::vector<std::string>();
std::vector<std::string> ID::m_rackNames=std::vector<std::string>();
unsigned int ID::m_moduleNbr{0};
unsigned int ID::m_crateNbr{0};
unsigned int ID::m_rackNbr{0};

void ID::add(const std::string& Name,const std::string& type)
{
    if(type=="Module")
    {
        if(m_moduleIDs.find(Name)==m_moduleIDs.end())
        {    
            m_moduleIDs.insert({Name,m_moduleNbr});
            m_moduleNames.push_back(Name);
            m_moduleNbr++;
        }
    }
    else if(type=="Crate")
    {
        if(m_crateIDs.find(Name)==m_crateIDs.end())
        {    
            m_crateIDs.insert({Name,m_crateNbr});
            m_crateNames.push_back(Name);
            m_crateNbr++;
        }
    }
    else if (type=="Rack")
    {
        if(m_rackIDs.find(Name)==m_rackIDs.end())
        {
            m_rackIDs.insert({Name,m_rackNbr});
            m_rackNames.push_back(Name);
            m_rackNbr++;
        }
    }
}

void ID::print(std::ostream& stream,const std::string& mover)
{
    stream<<mover<<"ID(s) : "<<std::endl;
    stream<<mover<<"\tRack ID(s) : "<<std::endl;
    for(std::size_t i=0;i!=m_rackNames.size();++i)
    {
        stream<<mover<<"\t\t*"<<m_rackNames[i]<<" : "<<i<<std::endl;
    }
    stream<<mover<<"\tCrate ID(s) : "<<std::endl;
    for(std::size_t i=0;i!=m_crateNames.size();++i)
    {
        stream<<mover<<"\t\t*"<<m_crateNames[i]<<" : "<<i<<std::endl;
    }
    stream<<mover<<"\tModule ID(s) : "<<std::endl;
    for(std::size_t i=0;i!=m_moduleNames.size();++i)
    {
        stream<<mover<<"\t\t*"<<m_moduleNames[i]<<" : "<<i<<std::endl;
    }
}

unsigned int ID::getModuleID(const std::string& module)
{
    return  m_moduleIDs[module];
}

unsigned int ID::getCrateID(const std::string& crate)
{
    return m_crateIDs[crate];
}
    
unsigned int ID::getRackID(const std::string& rack)
{
    return m_rackIDs[rack];
}

std::string ID::getModuleName(const unsigned int& module) 
{
    return  m_moduleNames[module];
}
    
std::string ID::getCrateName(const unsigned int& crate)
{
    return m_crateNames[crate];
}
    
std::string ID::getRackName(const unsigned int& rack)
{
    return m_rackNames[rack];
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
