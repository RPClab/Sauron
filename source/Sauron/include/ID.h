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

#ifndef ID_H
#define ID_H
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <array>
class ID 
{
public :
    static ID& instance() 
    {
        static ID S;
        return S;
    }
    void addDescription(const std::string& who,const std::string& description);
    void addSerialNumber(const std::string& who,const std::string& sn);
    void printJSON(std::ostream& stream=std::cout,const std::string& mover="");
    void writeJSON(const std::string& file); 
    std::string createJSON(const std::string& mover="");
    bool isRack(const std::string& who);
    bool isCrate(const std::string& who);
    bool isModule(const std::string& who);
    void addRack(const std::string& rack);
    void addCrate(const std::string& rack,const std::string& crate);
    void addModule(const std::string& rack,const std::string& crate,const std::string& module);
    void createRackID(const std::string& rack);
    void createCrateID(const std::string& crate);
    void createModuleID(const std::string& module);
    unsigned int getModuleID(const std::string& module);
    unsigned int getCrateID(const std::string& crate);
    unsigned int getRackID(const std::string& rack);
    std::string getModuleName(const unsigned int& module);
    std::string getCrateName(const unsigned int& crate);
    std::string getRackName(const unsigned int& rack);
    unsigned int getNbrOfModules();
    unsigned int getNbrOfCrates();
    unsigned int getNbrOfRacks();
    unsigned int getConfigID();
    void setConfigID(const unsigned int&);
    void print(std::ostream& stream=std::cout,const std::string& mover="");
private :
    ID(){};
    ~ID(){};
    ID(const ID& other)=delete; // copy constructor
    ID(ID&& other)=delete; // move constructor
    ID& operator=(const ID& other)=delete; // copy assignment
    ID& operator=(ID&& other)=delete; // move assignment
    std::map<std::string,int> m_rackIDs;
    std::vector<std::array<std::string,3>> m_rackInfos;
    std::map<std::string,std::array<int,2>> m_crateIDs;
    std::vector<std::array<std::string,3>> m_crateInfos;
    std::map<std::string,std::array<int,3>> m_moduleIDs;
    std::vector<std::array<std::string,3>> m_moduleInfos;
    unsigned int m_moduleNbr;
    unsigned int m_crateNbr;
    unsigned int m_rackNbr;
    unsigned int m_configID;
};
#endif
