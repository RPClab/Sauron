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

#include "DumbConnector.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

DumbConnector::DumbConnector(const DumbConnector& other):DumbConnector()
{
    m_params=other.m_params;
}

DumbConnector::DumbConnector()
{
    m_name="DumbConnector";
}

DumbConnector::DumbConnector(const std::map<std::string,std::string>& params):DumbConnector()
{
    m_params=params;
}

DumbConnector& DumbConnector::operator=(const DumbConnector& other)
{
    DumbConnector ret(other);
    return *this;
}

DumbConnector& DumbConnector::operator()(const DumbConnector& other)
{
    m_params=other.m_params;
    return *this;
}


void DumbConnector::setStream()
{
    if(m_params.hasParam("Stream"))
    {
        if(m_params["Stream"]=="cout") m_stream="cout";
        else if (m_params["Stream"]=="file") m_stream="fstream";
        else if (m_params["Stream"]=="clog") m_stream="clog";
        else 
        {
            std::cout<<"Stream can be only cout cerr and file !"<<std::endl;
            std::cout<<"Stream setted to default value "<<m_stream<<" !\n";
        }
    }
    else
    {
        std::cout<<"Stream setted to default value "<<m_stream<<" !\n";
    } 
}    
  
void DumbConnector::setFilename()
{
    if(m_params.hasParam("Filename"))
    {
        m_filename=m_params["Filename"];
    }
    else
    {
        std::cout<<"Filename should be setted !\n";
        throw -1;
    } 
} 
    

void DumbConnector::initialize()
{
    setStream();
    if(m_stream=="fstream")
    {
        setFilename();
    }
}

bool DumbConnector::isConnected()
{
    if(m_stream=="fstream")
    {
        return m_file.is_open();
    }
    else return true;
}

void DumbConnector::release()
{
    
}

void DumbConnector::disconnect()
{
    if (m_stream=="fstream") m_file.close();
}


void DumbConnector::connect()
{
    if(!isConnected()&&m_stream=="fstream") 
    {
        m_file.open(m_filename.CString(),std::ios_base::in | std::ios_base::app);
    }
}

Value DumbConnector::sendCommand(const std::string& command)
{
    if(m_stream=="fstream") m_file<<command<<"\n";
    else if (m_stream=="cout") std::cout<<command<<"\n";
    else if (m_stream=="clog") std::clog<<command<<"\n";
    return Value("");
}

