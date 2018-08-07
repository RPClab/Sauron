#include "Parameters.h"
#include <string>
#include "serial.h"
#include <iostream>

void Parameters::setName(const std::string& name)
{
    Name=name;
}
    
std::string Parameters::getName()
{
    return Name;
}
    
void Parameters::setSN(const std::string& sn)
{
    SN=sn;
}

std::string Parameters::getSN()
{
    return SN;
}
    
void Parameters::setDescription(const std::string& description)
{
    Description=description;
}

std::string Parameters::getDescription()
{
    return Description;
}
    
void Parameters::setBaudrate(const unsigned int& baudrate)
{
    Baudrate=static_cast<uint32_t>(baudrate);
}

uint32_t Parameters::getBaudrate()
{
    return Baudrate;
}
    
void Parameters::setTimeout(const unsigned int& timeout)
{
    Timeout=serial::Timeout::simpleTimeout(timeout);
}
    
serial::Timeout Parameters::getTimeout()
{
    return Timeout;
}
    
void Parameters::setBytesize(const std::string& bytesize)
{
    if(bytesize=="8") Bytesize=serial::bytesize_t::eightbits;
    else if (bytesize=="7")Bytesize=serial::bytesize_t::sevenbits;
    else if (bytesize=="6")Bytesize=serial::bytesize_t::sixbits;
    else if (bytesize=="5")Bytesize=serial::bytesize_t::fivebits;
    else
    {
        std::cout<<"Bytesize can be only 8, 7, 6 or 5"<<std::endl;
        std::exit(1);
    }
}
    
serial::bytesize_t Parameters::getBytesize()
{
    return Bytesize;
}

void Parameters::setParity(const std::string& parity)
{
    if(parity=="none")Parity=serial::parity_t::parity_none;
    else if (parity=="odd")Parity=serial::parity_t::parity_odd;
    else if (parity=="even")Parity=serial::parity_t::parity_even;
    else if (parity=="mark")Parity=serial::parity_t::parity_mark;
    else if (parity=="space")Parity=serial::parity_t::parity_space;
    else
    {
        std::cout<<"Parity can be only none, odd, even, mark or space"<<std::endl;
        std::exit(1); 
    }
}

serial::parity_t Parameters::getParity()
{
    return Parity;
}

void Parameters::setStopbits(const std::string& stopbits)
{
    if(stopbits=="1") Stopbits=serial::stopbits_t::stopbits_one;
    else if (stopbits=="2") Stopbits=serial::stopbits_t::stopbits_two;
    else if (stopbits=="1.5") Stopbits=serial::stopbits_t::stopbits_one_point_five;
    else
    {
        std::cout<<"Stopbits can be only 1, 1.5 or 2"<<std::endl;
        std::exit(1);
    }
}

serial::stopbits_t Parameters::getStopbits()
{
    return Stopbits;
}

void Parameters::setFlowcontrol(const std::string& flowcontrol)
{
    if(flowcontrol=="none") Flowcontrol=serial::flowcontrol_t::flowcontrol_none;
    else if (flowcontrol=="software") Flowcontrol=serial::flowcontrol_t::flowcontrol_software;
    else if (flowcontrol=="hardware") Flowcontrol=serial::flowcontrol_t::flowcontrol_hardware;
    else
    {
        std::cout<<"Flowcontrol can be only 1, 1.5 or 2"<<std::endl;
        std::exit(1);
    }
}

serial::flowcontrol_t Parameters::getFlowcontrol()
{
    return Flowcontrol;
}

void Parameters::Print()
{
    std::cout<<"Name : "<<Name<<", SN : "<<SN<<", Description : "<<Description<<", Baudrate : "<<Baudrate<<", Bytesize : "<<Bytesize<<", Parity : "<<Parity<<", Stopbits : "<<Stopbits<<", Flowcontrol : "<<Flowcontrol<<"\n";
}

