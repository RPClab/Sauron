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

#ifndef STATUS_H
#define STATUS_H
#include "Value.h"
#include <string>
#include <bitset>
#include <map>
class Status 
{
public:
    Status(){};
    Status(unsigned long long status):m_status(status){};
    void setBit(const std::string& BitName,const std::string& Description,const unsigned int& position,const bool& val)
    {
        isInsertable(BitName,position);
        m_des.insert({BitName,Description});
        m_vals.insert({BitName,{position,val}});
    }
    bool is(std::string& BitName)
    {
        if(m_vals.find(BitName)==m_vals.end()) throw std::out_of_range(BitName+" not found !");
        else if(m_status[m_vals[BitName].first]==m_vals[BitName].second) return true;
        else return false;
    }
    bool isNot(std::string& BitName)
    {
        return !is(BitName);
    }
    void setStatus(unsigned long long status)
    {
        m_status=status;
    }
    Status operator()(unsigned long long status)
    {
        m_status=status;
        return *this;
    }
    bool AND(const std::string& BitName)
    {
        if(m_des.find(BitName)==m_des.end()) return true;
        else if(m_status[m_vals[BitName].first]==m_vals[BitName].second) return true;
        else return false;
    }
    bool NAND(const std::string& BitName)
    {
        if(m_des.find(BitName)==m_des.end()) return true;
        else if(m_status[m_vals[BitName].first]==m_vals[BitName].second) return false;
        else return true;
    }
    bool OR(const std::string& BitName)
    {
        if(m_des.find(BitName)==m_des.end()) return false;
        else if(m_status[m_vals[BitName].first]==m_vals[BitName].second) return true;
        else return false;
    }
    bool NOR(const std::string& BitName)
    {
        if(m_des.find(BitName)==m_des.end()) return false;
        else if(m_status[m_vals[BitName].first]==m_vals[BitName].second) return false;
        else return true; 
    }
    Status& operator()(const std::string& BitName,const std::string& Description,const unsigned int& position,const bool& val)
    {
        setBit(BitName,Description,position,val);
        return *this;
    }
    void printDescription(std::ostream& stream=std::cout)
    {
        for(std::map<std::string,std::pair<unsigned int,bool>>::iterator it=m_vals.begin();it!=m_vals.end();++it)
        {
            stream<<"["<<it->first<<"] : "<<m_des[it->first]<<"\n";
            stream<<"Position : "<<it->second.first<<", triggered if "<<it->second.second<<"\n";
        }
    }
    void print(const int& what=2,std::ostream& stream=std::cout)
    {
        for(std::map<std::string,std::pair<unsigned int,bool>>::iterator it=m_vals.begin();it!=m_vals.end();++it)
        {
            if(what==1&&(m_status[it->second.first]==it->second.second))
            {
                stream<<"["<<it->first<<"] : "<<m_des[it->first]<<"\n";
            }
            else if(what==0&&(m_status[it->second.first]!=it->second.second))
            {
                
                stream<<"["<<it->first<<"] : "<<m_des[it->first]<<"\n";
            }
            else if(what<0||what>1)
            {
                stream<<"["<<it->first<<"] : "<<m_des[it->first]<<"\n";
                stream<<"Position : "<<it->second.first<<", triggered if "<<it->second.second<<" Value : "<<m_status[it->second.first]<<"\n";
            }
        }
    }
    void printRaw()
    {
        std::cout<<m_status.to_string()<<std::endl;
    }
private:
    void isInsertable(const std::string& BitName,const unsigned int& position)
    {
        if(m_vals.find(BitName)!=m_vals.end()) throw std::out_of_range(BitName+" exists !");
        if(position>=32) throw std::out_of_range("Status can contains only 32 bits ! You ask "+BitName+" to read bit "+std::to_string(position)+" !");
    }
    std::map<std::string,std::string> m_des;
    std::map<std::string,std::pair<unsigned int,bool>> m_vals;
    std::bitset<32> m_status;
};
#endif
