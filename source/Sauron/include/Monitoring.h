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
#ifndef MONITORING_h
#define MONITORING_h
#include "Parameters.h"
#include <chrono>
#include <thread>
#include <string>
#include <atomic>
#include "RacksManager.h"

class Monitoring 
{
public:
    Monitoring(RacksManager* rackmanager,const std::string& name,const std::string& description):m_racksmanager(rackmanager),m_name(name),m_description(description){}
    Monitoring(RacksManager* rackmanager,const std::string& name, const std::string& description,const Parameters& params):m_racksmanager(rackmanager),m_name(name),m_description(description),m_params(params){}
    Monitoring(RacksManager* rackmanager,const std::string& name,const Parameters& params):m_racksmanager(rackmanager),m_name(name),m_params(params){}
    Monitoring(RacksManager* rackmanager,const std::string& name):m_racksmanager(rackmanager),m_name(name){}
    Monitoring(const std::string& name, const std::string& description):m_name(name),m_description(description){}
    Monitoring(const std::string& name, const std::string& description,const Parameters& params):m_name(name),m_description(description),m_params(params){}
    Monitoring(const std::string& name,const Parameters& params):m_name(name),m_params(params){}
    Monitoring(const std::string& name):m_name(name){}
    const Parameters& getParameters() const {return m_params;} 
    void setParameters(const Parameters& params)
    {
        m_params=params;
    }
    void setRacksManager(RacksManager* rackmanager) { m_racksmanager=rackmanager;}
    void setName(const Value& name){m_name=name.String();}
    void setName(const std::string& name){m_name=name;}
    void setDescription(const Value& description){m_description=description.String();}
    void setDescription(const std::string& description){m_description=description;}
    std::string getDescription() const {return m_description;}
    std::string getName() const {return m_name;}
    virtual void function()=0;
    virtual ~Monitoring()
    {
            if(m_racksmanager!=nullptr) m_racksmanager=nullptr;
    }
    void setTime(const unsigned int& time){m_time=time;}
    int getTime(){return m_time;}
    void start()
    { 
        m_monitoring=std::thread(&Monitoring::loop,this);
        m_monitoring.detach();
        m_started=true;
    }
    bool isStarted(){return m_started;}
    bool isStoped(){return !m_started;}
    void stop(){ m_stopMonitoring=true;}
protected:
    std::atomic_bool m_stopMonitoring{false};
    bool m_started=false;
    Monitoring()=delete;
    void loop();
    unsigned int m_time{5};
    Parameters m_params;
    std::thread m_monitoring;
    RacksManager* m_racksmanager{nullptr};
    std::string m_name{""};
    std::string m_description{""};
};
#endif
