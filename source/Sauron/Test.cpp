#include "RacksManager.h"
#include "Parameters.h"
#include "Measure.h"
int main()
{
    RacksManager& manager = RacksManager::instance();
    manager.initialize();
    manager.connect();
    manager.on();
   // manager("iseg1").setWantedVoltage(1000);
   // manager("iseg2").setWantedVoltage(1000);
   // manager("N1741A").setWantedVoltage(500);
   // manager.startMonitoring("MonitorVoltages");
   // manager.startMonitoring("RecordVoltages");
    manager.startMonitoring("PrintVoltageCurrent");
   // manager.startMonitoring("MonitorEvents");
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10000));
    }
    manager.disconnect();
 return 0;       
}
