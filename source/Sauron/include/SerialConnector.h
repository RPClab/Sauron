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

#ifndef SERIALCONNECTOR_H
#define SERIALCONNECTOR_H
#include "Connector.h"
#include "serial.h"

/**
 * @todo write docs
 */

class SerialConnector : public Connector
{
public:
    /**
     * Default constructor
     */
    SerialConnector();

    /**
     * Copy Constructor
     *
     * @param other TODO
     */
    SerialConnector(const SerialConnector& other);
    
    SerialConnector& operator=(const SerialConnector& other);
    SerialConnector& operator()(const SerialConnector& other);
    
    SerialConnector(const std::map<std::string,std::string>& params);
    
    void Initialize() override;
    
    void Connect() override;
    
    void Disconnect() override;
    
    SerialConnector* Clone() override 
    { 
        if(isCrateConnector()==true) return this;
        else return new SerialConnector(*this);
    }

    void Release() override;
    
    bool IsConnected() override;
    Value SendCommand(const std::string&) override;
    
private:
    void setPort();
    void setBaudRate();
    void setTimeout();
    void setBytesize();
    void setParity();
    void setStopbits();
    void setFlowcontrol();
    serial::Serial m_serial;
    std::string m_port{""};
    uint32_t m_baudrate{9600};
    serial::Timeout m_timeout{serial::Timeout()};
    serial::bytesize_t m_bytesize{serial::eightbits};
    serial::parity_t m_parity{serial::parity_none};
    serial::stopbits_t m_stopbits{serial::stopbits_one};
    serial::flowcontrol_t m_flowcontrol{serial::flowcontrol_none};
};

#endif // ISEGNHSRCONNECTOR_H
