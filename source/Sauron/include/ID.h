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

class ID 
{
public :
    static void add(const std::string& Name,const std::string& type);
    static void print(std::ostream& stream=std::cout,const std::string& mover="" );
    static unsigned int getModuleID(const std::string& module);
    static unsigned int getCrateID(const std::string& crate);
    static unsigned int getRackID(const std::string& rack);
    static std::string getModuleName(const unsigned int& module);
    static std::string getCrateName(const unsigned int& crate);
    static std::string getRackName(const unsigned int& rack);
    unsigned int static getNbrOfModules();
    unsigned int static getNbrOfCrates();
    unsigned int static getNbrOfRacks();
private :
    static std::map<std::string,unsigned int> m_moduleIDs;
    static std::vector<std::string> m_moduleNames;
    static std::map<std::string,unsigned int> m_crateIDs;
    static std::vector<std::string> m_crateNames;
    static std::map<std::string,unsigned int> m_rackIDs;
    static std::vector<std::string> m_rackNames;
    static unsigned int m_moduleNbr;
    static unsigned int m_crateNbr;
    static unsigned int m_rackNbr;
};
#endif
