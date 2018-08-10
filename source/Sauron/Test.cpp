#include "RacksManager.h"
#include "Parameters.h"
#include "Measure.h"
int main()
{
    RacksManager manager;
    manager.Initialize();
    manager.printParameters();
    manager.connect();
    manager.on();
    std::cout<<manager.getNbrChannels()<<std::endl;
    std::thread toto=manager.printVoltageCurrentthread();
    while(1)
    {
        manager.on();
        std::this_thread::sleep_for(std::chrono::seconds(60));
        manager.off();
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
   manager.printVoltageCurrentthread().join();
   manager.disconnect();
 return 0;       
}
