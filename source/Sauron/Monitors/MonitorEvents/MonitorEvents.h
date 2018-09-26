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

#ifndef MONITOREVENTS_H
#define MONITOREVENTS_H
#include "Monitoring.h"

class MonitorEvents : public Monitoring
{
public: 
    MonitorEvents(RacksManager* rackmanager):Monitoring(rackmanager,"MonitorEvents"){}
    MonitorEvents():Monitoring("MonitorEvents"){}
    void function()
    {
        m_racksmanager->printModuleStatus();
        m_racksmanager->printChannelsStatus();
    }
};



class MonitorEventsDriver : public MonitorDriver
{
public:
    MonitorEventsDriver() : MonitorDriver("MonitorEvents", Monitoring::getPluginVersion()) {}
    Monitoring* create() {return new MonitorEvents();}
};
#endif
