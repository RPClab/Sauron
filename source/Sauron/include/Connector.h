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

#ifndef CONNECTOR_H
#define CONNECTOR_H
#include "Value.h"
#include <string>
#include <map>
#include "Parameters.h"

class Connector
{
public:
    Connector();
    Connector(const Parameters& params);
    virtual ~Connector();
    virtual void initialize()=0;
    void setParameters(const Parameters& params);
    void printParameters(std::ostream& stream=std::cout,const std::string& mover="" );
    void printParameters(const std::string& mover);
    virtual Value sendCommand(const std::string&)=0;
    std::size_t getNbrParamaters();
    void clearParameters();
    std::string getName();
    bool isCrateConnector();
    void isCrateConnector(const bool bol);
    virtual void connect()=0; 
    virtual void disconnect()=0;
    virtual void release()=0;
    virtual bool isConnected()=0;
    virtual Connector* clone()=0;
protected:
    bool m_isCrateConnector{false};
    Parameters m_params;
    std::string m_name{"VirtualConnector"};
};
#endif
