#include "RacksManager.h"
#include "Parameters.h"
#include "Measure.h"
int main()
{
    RacksManager manager;
    manager.Initialize();
    //manager.printParameters();
    manager.connect();
    manager.off();
    //std::cout<<manager.getNbrChannels()<<std::endl;
    manager("LV",1).on();
    manager("Crate2").on();
    manager("LV","2").setVoltage(3);
    manager("Crate2").setVoltage(4000);
    manager("Crate2",0).setVoltage(3000);
    //manager.printModuleStatus();
    //std::vector<MeasuresAndSets> po=manager().getMeasuresAndSets();
    //for(unsigned int i=0;i!=po.size();++i) po[i].print();
    /*manager.setVoltage("Crate2",1000);
    manager.setVoltage("Crate1",7);
    manager.setVoltage("Crate1",0,5);
    manager.setVoltage("HV",1500);
    manager.setVoltage("Crate2",0,2000);*/
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    //std::thread toto=manager.printVoltageCurrentthread();
    //toto.detach();
    /*for(unsigned int i=0;i!=1000;i=i+100)
    {
        
        std::cout<<i<<std::endl;
        manager.setVoltage(i);
        std::this_thread::sleep_for(std::chrono::seconds(50));
    }*/
    manager.startMonitoring("MonitorVoltages",2);
  //  manager.stopMonitoring("PrintVoltageCurrent");
    /*    for(unsigned int i=0;i!=100;i++)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<i<<std::endl;
    }*/
    std::this_thread::sleep_for(std::chrono::seconds(1000));
    manager.disconnect();
 return 0;       
}
