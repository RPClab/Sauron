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

#include "Measure.h"
    std::map<std::string,unsigned int> Position::m_moduleToNbrMap=std::map<std::string,unsigned int>();
    std::map<std::string,unsigned int> Position::m_crateToNbrMap=std::map<std::string,unsigned int>();
    std::map<std::string,unsigned int> Position::m_rackToNbrMap=std::map<std::string,unsigned int>();
    std::vector<std::string> Position::m_nbrToModuleMap=std::vector<std::string>();
    std::vector<std::string> Position::m_nbrToCrateMap=std::vector<std::string>();
    std::vector<std::string> Position::m_nbrToRackMap=std::vector<std::string>();
    unsigned int Position::m_moduleNbr{0};
    unsigned int Position::m_crateNbr{0};
    unsigned int Position::m_rackNbr{0};

std::ostream& operator<<(std::ostream& stream, const Position& Pos)
{
        (Pos.getRack()!=-1&&Pos.getRackName()!="") ? (stream<<"Rack : "<<Pos.getRack()<<" ("<<Pos.getRackName()<<") , ") : (stream<<"") ;
        (Pos.getCrate()!=-1) ? (stream<<"Crate : "<<Pos.getCrate()<<" ("<<Pos.getCrateName()<<") , ") : (stream<<"");
        (Pos.getModule()!=-1) ? (stream<<"Module : "<<Pos.getModule()<<" ("<<Pos.getModuleName()<<") , ") : (stream<<"");
        (Pos.getChannel()!=-1) ? (stream<<"Channel : "<<Pos.getChannel()<<", ") : (stream<<"");
        return stream;
}
