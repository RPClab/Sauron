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

#include "Position.h"
#include <map>
#include <string>
#include <iostream>

std::ostream& operator<<(std::ostream& stream, const Position& Pos)
{
        (Pos.getRack()!=-1&&Pos.getRackName()!="") ? (stream<<"Rack : "<<Pos.getRack()<<" ("<<Pos.getRackName()<<") , ") : (stream<<"") ;
        (Pos.getCrate()!=-1) ? (stream<<"Crate : "<<Pos.getCrate()<<" ("<<Pos.getCrateName()<<") , ") : (stream<<"");
        (Pos.getModule()!=-1) ? (stream<<"Module : "<<Pos.getModule()<<" ("<<Pos.getModuleName()<<") , ") : (stream<<"");
        (Pos.getChannel()!=-1) ? (stream<<"Channel : "<<Pos.getChannel()<<", ") : (stream<<"");
        return stream;
}

bool Position::operator==(const Position& pos) const
{
    if(pos.getChannel()==m_channel&&pos.getModule()==m_module&&pos.getCrate()==m_crate&&pos.getRack()==m_rack) return true;
    else return false;
}
    
bool Position::operator!=(const Position& pos) const
{
    return !(pos==*this);
}
    
void Position::setChannel(const Value& channel)
{
    m_channel=channel.UInt();
}
    
void Position::setModule(const Value& moduleName)
{
    ID& m_id=ID::instance();
    m_module=m_id.getModuleID(moduleName.String());
}
    
void Position::setCrate(const Value& crateName)
{
    ID& m_id=ID::instance();
    m_crate=m_id.getCrateID(crateName.String());
}
    
void Position::setRack(const Value& rackName)
{
    ID& m_id=ID::instance();
    m_rack=m_id.getRackID(rackName.String());
}
    
int Position::getChannel() const 
{
    return m_channel;
}

int Position::getModule() const 
{
    return m_module;
}
    
int Position::getCrate() const 
{
    return m_crate;
}
    
int Position::getRack() const 
{
    return m_rack;
}
    
std::string Position::getModuleName() const 
{
    ID& m_id=ID::instance();
    return  m_id.getModuleName(m_module);
}
    
std::string Position::getCrateName() const
{
    ID& m_id=ID::instance();
    return m_id.getCrateName(m_crate);
}
    
std::string Position::getRackName() const
{
    ID& m_id=ID::instance();
    return m_id.getRackName(m_rack);
}
    
void  Position::print(std::ostream& stream,const std::string mover)
{
    stream<<mover;
    (m_rack!=-1&&getRackName()!="") ? (stream<<"Rack : "<<getRack()<<" ("<<getRackName()<<") , ") : (stream<<"") ;
    (m_crate!=-1) ? (stream<<"Crate : "<<getCrate()<<" ("<<getCrateName()<<") , ") : (stream<<"");
    (m_module!=-1) ? (stream<<"Module : "<<getModule()<<" ("<<getModuleName()<<") , ") : (stream<<"");
    (m_channel !=-1) ? (stream<<"Channel : "<<getChannel()<<", ") : (stream<<"");
    stream<<"\n";
}
