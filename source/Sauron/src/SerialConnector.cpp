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

#include "SerialConnector.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>

SerialConnector::SerialConnector(const SerialConnector& other):SerialConnector()
{
    m_params=other.m_params;
}

SerialConnector::SerialConnector()
{
     m_type="SerialConnector";
}



SerialConnector::SerialConnector(const std::map<std::string,std::string>& params):SerialConnector()
{
    m_params=params;
}

SerialConnector& SerialConnector::operator=(const SerialConnector& other)
{
    m_params=other.m_params;
    m_type=other.m_type;
    return *this;
}

SerialConnector& SerialConnector::operator()(const SerialConnector& other)
{
    m_params=other.m_params;
    m_type=other.m_type;
    return *this;
}


void SerialConnector::Initialize()
{
    setPort();
    setBaudRate();
    setTimeout();
    setBytesize();
    setParity();
    setStopbits();
    setFlowcontrol();
}

bool SerialConnector::IsConnected()
{
    return m_serial.isOpen();
}

void SerialConnector::Release()
{
    
}

void SerialConnector::Disconnect()
{
    if(IsConnected()) m_serial.close();
}


void SerialConnector::Connect()
{
    if(!IsConnected()) 
    {
            m_serial.open();
    }
    else
    {
        std::cout<<"Yet connected"<<std::endl;
    }
}

void SerialConnector::setPort()
{
    if(m_params.hasParam("Port"))
    {
        m_port=m_params["Port"].String();
        m_serial.setPort(m_port);
    }
    else
    {
        
        std::string error="Port is mandatory ! \n";
        std::cout<<error;
        throw error;
    }
}

void SerialConnector::setBaudRate()
{
    if(m_params.hasParam("Baudrate"))
    {
        m_baudrate=static_cast<uint32_t>(m_params["Baudrate"].Double());
    }
    else
    {
        std::cout<<"Baudrate setted to default value "<<m_baudrate<<" !\n";
    }
    m_serial.setBaudrate(m_baudrate);
}

void SerialConnector::setTimeout()
{
    if(m_params.hasParam("Timeout"))
    {
        m_timeout=serial::Timeout::simpleTimeout(m_params["Timeout"].Int());
    }
    else
    {
        std::cout<<"Timeout setted to default value !\n";
    }
    m_serial.setTimeout(m_timeout);
}

void SerialConnector::setBytesize()
{
    if(m_params.hasParam("Bytesize"))
    {
        if(m_params["Bytesize"]=="8") m_bytesize=serial::bytesize_t::eightbits;
        else if (m_params["Bytesize"]=="7") m_bytesize=serial::bytesize_t::sevenbits;
        else if (m_params["Bytesize"]=="6") m_bytesize=serial::bytesize_t::sixbits;
        else if (m_params["Bytesize"]=="5") m_bytesize=serial::bytesize_t::fivebits;
        else 
        {
            std::cout<<"Bytesize can be only 8, 7, 6 or 5 !"<<std::endl;
            std::cout<<"Bytesize setted to default value "<<m_bytesize<<" !\n";
        }
    }
    else
    {
        std::cout<<"Bytesize setted to default value "<<m_bytesize<<" !\n";
    }
    m_serial.setBytesize(m_bytesize);
}

void SerialConnector::setParity()
{
    if(m_params.hasParam("Parity"))
    {
        if(m_params["Parity"]=="none") m_parity=serial::parity_t::parity_none;
        else if (m_params["Parity"]=="odd")m_parity=serial::parity_t::parity_odd;
        else if (m_params["Parity"]=="even")m_parity=serial::parity_t::parity_even;
        else if (m_params["Parity"]=="mark")m_parity=serial::parity_t::parity_mark;
        else if (m_params["Parity"]=="space")m_parity=serial::parity_t::parity_space;
        else 
        {
            std::cout<<"Parity can be only none, odd, even, mark or space"<<std::endl;
            std::cout<<"Parity setted to default value "<<m_parity<<" !\n";
        }
    }
    else
    {
        std::cout<<"Parity setted to default value "<<m_parity<<" !\n";
    }
    m_serial.setParity(m_parity);
}

void SerialConnector::setStopbits()
{
    if(m_params.hasParam("Stopbits"))
    {
        if(m_params["Stopbits"]=="1") m_stopbits=serial::stopbits_t::stopbits_one;
        else if (m_params["Stopbits"]=="2") m_stopbits=serial::stopbits_t::stopbits_two;
        else if (m_params["Stopbits"]=="1.5") m_stopbits=serial::stopbits_t::stopbits_one_point_five;
        else 
        {
            std::cout<<"Stopbits can be only 1, 1.5 or 2"<<std::endl;
            std::cout<<"Stopbits setted to default value "<<m_stopbits<<" !\n";
        }
    }
    else
    {
        std::cout<<"Stopbits setted to default value "<<m_stopbits<<" !\n";
    }
    m_serial.setStopbits(m_stopbits);
}

void SerialConnector::setFlowcontrol()
{
    if(m_params.hasParam("Flowcontrol"))
    {
        if(m_params["Flowcontrol"]=="none") m_flowcontrol=serial::flowcontrol_t::flowcontrol_none;
        else if (m_params["Flowcontrol"]=="software") m_flowcontrol=serial::flowcontrol_t::flowcontrol_software;
        else if (m_params["Flowcontrol"]=="hardware") m_flowcontrol=serial::flowcontrol_t::flowcontrol_hardware;
        else 
        {
            std::cout<<"Flowcontrol can be only 1, 1.5 or 2"<<std::endl;
            std::cout<<"Parity setted to default value "<<m_flowcontrol<<" !\n";
        }
    }
    else
    {
        std::cout<<"Stopbits setted to default value "<<m_flowcontrol<<" !\n";
    } 
    m_serial.setFlowcontrol(m_flowcontrol);
}

Value SerialConnector::SendCommand(const std::string& command)
{
        std::string respond="";
        std::string real_command=command+"\n";
        std::size_t bytes_wrote = m_serial.write(real_command);
        if(bytes_wrote==real_command.size()) 
        {
            respond=m_serial.read(bytes_wrote+2000);
            if(respond.size()==bytes_wrote) return std::string();
            respond=respond.erase(0,bytes_wrote);
        }
        return Value(respond);
}

