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
/**
 * Pure virtual class to take care of the connection to the module/crate.
 */
class Connector
{
public:
    /**
     * Default constructor
     */
    Connector();

    /**
     * Constructor with params map
     *
     * @param params 
     */
    Connector(const Parameters& params);

    /**
     * Destructor
     */
    virtual ~Connector();
    
    /**
     * To prepare the connection
     */
    virtual void Initialize()=0;
    
    virtual void setParameters(const Parameters& params)
    {
        m_params=params;
    }
    
    void printParameters(std::ostream& stream=std::cout,const std::string& mover="" )
    {
        std::cout<<mover<<"Connector type : "<<getName()<<", parameters : \n";
        m_params.printParameters(stream,mover);
    }
    void printParameters(const std::string& mover)
    {
        std::cout<<mover<<"Connector type : "<<getName()<<", parameters : \n";
        m_params.printParameters(mover);
    }
    
    virtual Value SendCommand(const std::string&)=0;
    
    std::size_t getNbrParamaters(){return m_params.size();}
    
    void ClearParameters(){m_params.clear();}
    
    virtual std::string getName(){return m_type;}
    
    bool isCrateConnector() {return m_IsCrateConnector;}
    void isCrateConnector(const bool bol) {m_IsCrateConnector=bol;}
        virtual void Connect()=0; 
    
    virtual void Disconnect()=0;
    
    virtual void Release()=0;
    
    virtual bool IsConnected()=0;
    
    virtual Connector* Clone()=0;
    
    
    
protected:
    bool m_IsCrateConnector{false};
    Parameters m_params;
    std::string m_type{"VirtualConnector"};
};
#endif
