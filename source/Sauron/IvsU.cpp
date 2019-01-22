#include "RacksManager.h"
#include "Parameters.h"
#include "Measure.h"
#include "TGraph.h"
#include <vector>
#include "TCanvas.h"

const int NbrChamber=4;
const int Wait=12;

int main()
{
    TCanvas can;
    std::vector<TGraph*> Graphs;
    for(std::size_t i=0;i!=NbrChamber;++i)
    {
        Graphs.push_back(new TGraph);
        Graphs[i]->SetTitle(("I vs Ucorrected for chamber "+std::to_string(i+1)).c_str());
    }
    RacksManager& manager = RacksManager::instance();
    manager.initialize();
    manager.connect();
    manager("HV").on();
    manager("HV").setVoltage(0);
    //manager.startMonitoring("PrintVoltageCurrent");
    for(unsigned int i=0;i!=3500;i+=100)
    {
        static int point=0;
        manager("HV").setVoltage(i);;
        for(unsigned int h=0;h!=Wait;++h)
        {
            std::cout<<h*1.0/Wait*100<<"%"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::vector<MeasuresAndSets> data=manager.getMeasuresAndSets();
        for(std::size_t j=0;j!=NbrChamber;++j)
        {
            Graphs[j]->SetPoint(point,i,data[j].getMeasuredCurrent().Double());
            std::cout<<data[j].getMeasuredVoltage().String()<<"V : "<<data[j].getMeasuredCurrent().String()<<"A"<<std::endl;
            //can.cd();
            //Graphs[j]->Draw();
            //can.SaveAs((std::to_string(j)+".png").c_str());
        }
    }
   // manager("HV").on();
   // manager("iseg1").setWantedVoltage(1000);
   // manager("iseg2").setWantedVoltage(1000);
   // manager("N1741A").setWantedVoltage(500);
   // manager.startMonitoring("MonitorVoltages");
   // manager.startMonitoring("RecordVoltages");
    
   // manager.startMonitoring("MonitorEvents");
    manager.disconnect();
    for(std::size_t i=0;i!=NbrChamber;++i)
    {
        can.cd();
        Graphs[i]->Draw();
        can.SaveAs((std::to_string(i)+".png").c_str());
        delete Graphs[i];
    }
 return 0;       
}
