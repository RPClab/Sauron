#include "RacksManager.h"
#include "Parameters.h"
#include "Measure.h"
int main()
{
    RacksManager manager;
    manager.initialize();
    //manager.printParameters();
    manager.connect();
    manager.on();
    std::cout<<manager.getNbrChannels()<<std::endl;
    //manager("Crate2").on();
    //manager("LV").setVoltage(3);
    //ID::writeJSON("./toto.json");
    //std::cout<<"Config ID : "<<ID::getConfigID()<<std::endl;
    /*manager("HV").on();
    manager("iseg1").on();
    manager("iseg2").on();
    manager("N1741A").on();*/
    manager("iseg1").setWantedVoltage(1000);
    manager("iseg2").setWantedVoltage(1000);
    manager("N1741A").setWantedVoltage(500);
   // manager.printModuleStatus();
    //std::vector<MeasuresAndSets> po=manager().getMeasuresAndSets();
    //for(unsigned int i=0;i!=po.size();++i) po[i].print();
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    //std::thread toto=manager.printVoltageCurrentthread();
    //toto.detach();
    /*for(unsigned int i=0;i!=1000;i=i+100)
    {
        
        std::cout<<i<<std::endl;
        manager.setVoltage(i);
        std::this_thread::sleep_for(std::chrono::seconds(50));
    }*/
    manager.startMonitoring("MonitorVoltages");
    manager.startMonitoring("RecordVoltages");
    manager.startMonitoring("PrintVoltageCurrent",10);
    //manager.stopMonitoring("PrintVoltageCurrent");
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10000));
    }
    manager.disconnect();
 return 0;       
}
