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

#ifndef DUMBCONNECTOR_H
#define DUMBCONNECTOR_H
#include "Connector.h"
#include <fstream> 

class DumbConnector : public Connector
{
public:
    DumbConnector();
    DumbConnector(const DumbConnector& other);
    DumbConnector& operator=(const DumbConnector& other);
    DumbConnector& operator()(const DumbConnector& other);
    DumbConnector(const std::map<std::string,std::string>& params);
    void initialize() override;
    void connect() override;
    DumbConnector* clone() override { return new DumbConnector(*this);} 
    void disconnect() override;
    void release() override;
    bool isConnected() override;
    Value buildCommand(const std::vector<Value>&) override;
private:
    void setStream();
    void setFilename();
    Value m_stream{"cout"};
    Value m_filename{""};
    std::fstream m_file;
};
#endif

