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

#include "Connector.h"

Connector::Connector()
{

}

std::size_t Connector::getNbrParamaters()
{
    return m_params.size();
}
    
void Connector::ClearParameters()
{
    m_params.clear();
}
    
std::string Connector::getName()
{
    return m_name;
}
    
bool Connector::isCrateConnector() 
{
    return m_IsCrateConnector;
}
    
void Connector::isCrateConnector(const bool bol) 
{
    m_IsCrateConnector=bol;
}

Connector::Connector(const Parameters& params)
{
    m_params=params;
}

Connector::~Connector()
{
    m_params.clear();
}

void Connector::setParameters(const Parameters& params)
{
    m_params=params;
}
    
void Connector::printParameters(std::ostream& stream,const std::string& mover)
{
    std::cout<<mover<<"Connector type : "<<getName()<<", parameters : \n";
    m_params.printParameters(stream,mover);
}
    
void Connector::printParameters(const std::string& mover)
{
    std::cout<<mover<<"Connector type : "<<getName()<<", parameters : \n";
    m_params.printParameters(mover);
}
