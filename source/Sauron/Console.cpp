#include "Connector.h"

#include "iseg/isegNHSR.h"
#include <iostream>
#include "serial.h"
#include <boost/program_options.hpp>
#include <string>
#include <regex>
#include <set>
#include <functional>
using namespace std::placeholders;
namespace po = boost::program_options;
using namespace std;

/*std::function<void(isegNHSR&,const int&,const double&)> Command(std::string comm)
{
   if(comm=="ON") return std::bind<void (isegNHSR::*)(const int&)>(&isegNHSR::On,_1,_2);
   if(comm=="OFF") return std::bind<void (isegNHSR::*)(const int&)>(&isegNHSR::Off,_1,_2);
   if(comm=="VOL") return std::bind<void (isegNHSR::*)(const int&,const double&)>(&isegNHSR::SetVoltage,_1,_2,_3);
   if(comm=="VOLBOUND") return std::bind<void (isegNHSR::*)(const int&,const double&)>(&isegNHSR::SetVoltageBound,_1,_2,_3);
   if(comm=="CURBOUND") return std::bind<void (isegNHSR::*)(const int&,const double&)>(&isegNHSR::SetCurrentBound,_1,_2,_3);
   if(comm=="TRIPTIME") return std::bind<void (isegNHSR::*)(const int&,const int&)>(&isegNHSR::SetTripTime,_1,_2,_3);
   if(comm=="CURRENT") return std::bind<void (isegNHSR::*)(const int&,const double&)>(&isegNHSR::SetCurrent,_1,_2,_3);
   if(comm=="EMERGENCY") return std::bind<void (isegNHSR::*)(const int&)>(&isegNHSR::Emergency,_1,_2);
   if(comm=="CLEAREMERGENCY") return std::bind<void (isegNHSR::*)(const int&)>(&isegNHSR::ClearEmergency,_1,_2);
   if(comm=="TRIPACTION") return std::bind<void (isegNHSR::*)(const int&,const int&)>(&isegNHSR::SetTripAction,_1,_2,_3);
   else throw -1;
}*/


//void ExecuteCommand(const std::string& commande,Controler& m,const std::string& modules,const std::set<std::string>& cha,const double& value)
//{
   /* if(modules=="all")
    {
        for(std::map<std::string,isegNHSR>::iterator it=m.Begin();it!=m.End();++it)
        {
            if(cha.size()==0)for(unsigned int y=0;y!=it->second.getNbrChannels();++y)Command(commande)(it->second,y,value);
            else for(std::set<std::string>::iterator itt=cha.begin();itt!=cha.end();++itt) Command(commande)(it->second,std::stoi(*itt),value);
        }
    }
    else
    {
            if(cha.size()==0) for(unsigned int y=0;y!=m.Module(modules).getNbrChannels();++y)Command(commande)(m.Module(modules),y,value);
            else for(std::set<std::string>::iterator it=cha.begin();it!=cha.end();++it) Command(commande)(m.Module(modules),std::stoi(*it),value);
    }*/
//}

/*std::set<std::string> ParseCommand(const std::vector<std::string>& buf)
{
    std::set<std::string> ret;
    regex num_regex("^[\\+]?([0-9]*|\\.?[0-9]+)$|(([0-9])(-[0-9]))");
    for(auto token : buf)
    {
        if (regex_search(token, num_regex))
        {
           if(token.find("-")==std::string::npos)ret.insert(token);
           else
           {
                std::string firs=token.substr(token.find("-")+1);
                token=token.erase(token.find("-"));
                int debut=std::stoi(token);
                int fin=std::stoi(firs)+1;
                //std::cout<<firs<<"  "<<token<<std::endl;
                for(unsigned int y=debut;y!=fin;++y) ret.insert(std::to_string(y));
           }
        }
    }
    return std::move(ret);
}*/

//std::vector<std::string> Tokenize(const std::string& str, const std::string& delimiters = ",")
//{
    /*std::vector<std::string> tokens;
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first non-delimiter.
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) 
    {
        // Found a token, add it to the vector."INSERT INTO "
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next non-delimiter.
        pos = str.find_first_of(delimiters, lastPos);
    }
    return std::move(tokens);*/
//}



int main(int ac, char* av[])
{
  /*  //params
    std::string modules{"all"};
    std::string channels{"all"};
    std::string lss{"all"};
    std::set<std::string> cha;
    std::string volts;
    std::string currents;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("module,m", po::value<string>(&modules)->default_value("all"), "Select the module(s)")
    ("channel,c", po::value<string>(&channels)->default_value("all"), "Select the channel(s)")
    ("ON","Turning ON")
    ("CLEAR","Clear errors")
    ("OFF","Turning OFF")
    ("voltage,v", po::value<string>(&volts), "Set Voltage (V)")
    ("current,i", po::value<string>(&currents), "Set Current (mA)")
    ("ls",po::value<string>(&lss), "Print status")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    
    Controler m;
    if (vm.count("channel")) 
    {
      //  cha=ParseCommand(Tokenize(channels));
    }
    if (vm.count("help")) 
    {
        //cout << desc << "\n";
       // return 1;
    }
    if (vm.count("ON")) 
    {
      // ExecuteCommand("ON",m,modules,cha,0.);
    }
    else if (vm.count("OFF")) 
    {
       //ExecuteCommand("OFF",m,modules,cha,0.);
    }
    else if (vm.count("CLEAR")) 
    {
       for(std::map<std::string,isegNHSR>::iterator it=m.Begin();it!=m.End();++it)
        {
            it->second.Clear();
        }
    }
    else if (vm.count("ls")) 
    {
       if(lss=="all")
        {    
            for(std::map<std::string,isegNHSR>::iterator it=m.Begin();it!=m.End();++it)
            {
                std::vector<Value> VoltagesSet=it->second.GetVoltage();
                std::vector<Value> CurrentsSet=it->second.GetCurrent();
                std::vector<Value> VoltagesMeasured=it->second.MeasureVoltage();
                std::vector<Value> CurrentsMeasured=it->second.MeasureCurrent();
                std::cout<<"Module "<<it->first<<" : \n";
                for(unsigned int i=0;i!=VoltagesSet.size();++i)std::cout<<"Channel "<<i<<" : "<<VoltagesMeasured[i]<<" ("<<VoltagesSet[i]<<") : "<<CurrentsMeasured[i]<<" ("<<CurrentsSet[i]<<")\n";
            }
        }
        else
        {
            std::vector<Value> VoltagesSet=m.Module(lss).GetVoltage();
            std::vector<Value> CurrentsSet=m.Module(lss).GetCurrent();
            std::vector<Value> VoltagesMeasured=m.Module(lss).MeasureVoltage();
            std::vector<Value> CurrentsMeasured=m.Module(lss).MeasureCurrent();
            std::cout<<"Module "<<lss<<" : "<<std::endl;
            for(unsigned int i=0;i!=VoltagesSet.size();++i)std::cout<<"Channel "<<i<<" : "<<VoltagesMeasured[i]<<" ("<<VoltagesSet[i]<<") : "<<CurrentsMeasured[i]<<" ("<<CurrentsSet[i]<<")\n";
        }
    }
    if (vm.count("voltage")) 
    {
      // ExecuteCommand("VOL",m,modules,cha,std::stod(volts));
    }
    if (vm.count("current")) 
    {
      //  ExecuteCommand("CURRENT",m,modules,cha,std::stod(currents)/1000);
    }
    return 0;*/
} 
