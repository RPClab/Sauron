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

#ifndef SNMPCONNECTOR_H
#define SNMPCONNECTOR_H
#include "Connector.h"
#include <memory>
class SNMPConnector : public Connector
{
public:
    SNMPConnector();
    SNMPConnector(const SNMPConnector& other);
    SNMPConnector(const std::map<std::string,std::string>& params);
    SNMPConnector& operator=(const SNMPConnector& other);
    SNMPConnector& operator()(const SNMPConnector& other);
    void initialize() override;
    void connect() override;
    void disconnect() override;
    void release() override;
    SNMPConnector* clone() override 
    { 
       /* if(isCrateConnector()==true) return this;
        else return new SNMPConnector(*this);*/
       return new SNMPConnector(*this);
    }
    bool isConnected() override;
    Value buildCommand(const std::vector<Value>&) override;
    Value receiveInfos(std::vector<Value>);
    Value sendInfos(std::vector<Value>);
private:
    void setMIBPath();
    void setMIBFilename();
    void setTimeout();
    void setRetries();
    void setVersion();
    void setReadCommunity();
    void setIP();
    char findType(Value);
    struct snmp_session * m_session;
    std::string m_MIBFilename{""};
    std::string m_MIBPath{""};
    std::string m_IP{""};
    std::string m_readCommunity{"public"};
    long m_timeout{300000};   // timeout (us)
    int  m_retries{2};        // retries
    int  m_version{1};
};

class SNMPConnectorDriver : public ConnectorDriver
{
public:
    SNMPConnectorDriver() : ConnectorDriver("SNMP", Connector::getPluginVersion()) {}
    Connector* create() {return new SNMPConnector();}
};
#endif
