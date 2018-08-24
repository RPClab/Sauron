#include <iostream>
#include "Controler.h"
#include <ctime>
#include <chrono>
#include <thread>
#include <set>
#include <fstream>

std::string getEnvVar( std::string const & key )
{
    /*if(std::getenv( key.c_str() )==nullptr) return "";
    else return std::string(std::getenv( key.c_str() ));*/
}

Json::Value OpenJSONFile(std::string envVar)
{
   /* Json::CharReaderBuilder builder;
    Json::Value obj;   // will contain the root value after parsing.
    std::string errs;
    std::string FileName=getEnvVar(envVar.c_str());
    if(FileName=="")
    {
        std::cout<<"Please add "<<envVar<<" as variable environment ! \n";
        std::exit(2);
    }
    std::ifstream ConfFile(FileName.c_str(),std::ifstream::binary);
    bool ok = Json::parseFromStream(builder,ConfFile,&obj,&errs);
    if ( !ok )
    {
        std::cout  << errs << "\n";
    }
    return obj;*/
}


class Connexion
{
public:
    void setPassword(const std::string pass){Password=pass;};
    std::string getPassword(){return Password;};
    void setUser(const std::string nam){User=nam;};
    std::string getUser(){return User;};
    void setHost(std::string hos){Host=hos;};
     std::string getHost(){return Host;};
    void setWaitingTime(int wait){WaitingTime=wait;};
    int getWaitingTime(){return WaitingTime;};
private :
    std::string User;
    std::string Password;
    std::string Host;
    int WaitingTime=0;
};


class Command
{
public:
    std::string GenerateCommand(std::string date,std::string module,std::string channel,const Value& voltage,const Value& current)
    {
        std::string com="INSERT INTO "+Database+"."+Table+" (date,module,channel,voltage,current) VALUES('"+date+"',"+module+","+channel+","+SupressUnity(voltage)+","+SupressUnity(current)+");";
        std::cout<<com<<std::endl;
        return std::move(com);
    }
    std::string getDatabase(){return Database;};
    std::string getTable(){return Table;};
    void setDatabase(std::string data){Database=data;};
    void setTable(std::string tabl){Table=tabl;};
private:
    std::string SupressUnity(const Value& value)
    {
        Value val{value};
        return val.String().erase(val.Size()-1);
    }
    std::string Database;
    std::string Table;
};

class Module
{
public:
    void setName(const std::string& na){ name=na;};
    std::string getName(){return name;};
    std::set<int> getChannels(){return channels;};
    void setChannel(int& ch){ channels.insert(ch);};
    unsigned int getNbrChannels(){return channels.size();};
    std::set<int>::iterator Begin(){return channels.begin();};
    std::set<int>::iterator End(){return channels.end();};
private:
    std::set<int> channels;
    std::string name;
};

void ExtractMonitoringHVInfos(Json::Value root,std::vector<Module>& Modules,Connexion& con,Command& com)
{
  /*  const Json::Value& monitor = root["MonitoringHV"];
    con.setWaitingTime(monitor["Waiting Time"].asInt());
    con.setUser(monitor["User"].asString());
    con.setPassword(monitor["Password"].asString());
    con.setHost(monitor["Host"].asString());
    com.setDatabase(monitor["Database"].asString());
    com.setTable(monitor["Table"].asString());
    const Json::Value& modules = monitor["Modules"]; 
    for (unsigned int j = 0; j < modules.size(); ++j)
    {
            Module module;
            module.setName( modules[j]["Name"].asString());
            const Json::Value& channels = modules[j]["Channels"]; 
            for (unsigned int k = 0; k < channels.size(); ++k)
            {
                int chan=channels[k].asInt();
                module.setChannel(chan);
            }
            Modules.push_back(std::move(module));;
    }*/
}

int main()
{
   /* Controler controller;
    std::vector<Module> Modules;
    Connexion conn;
    Command command;
    ExtractMonitoringHVInfos(OpenJSONFile("MonitoringHV"),Modules,conn,command);
    mariadb::account_ref Myaccount= mariadb::account::create(conn.getHost(),conn.getUser(),conn.getPassword());
    mariadb::connection_ref Myconnection=mariadb::connection::create(Myaccount);
    while(1)
    {
        std::time_t ti = ::time(nullptr);
        mariadb::date_time tim(ti);
        for(std::vector<Module>::iterator it=Modules.begin();it!=Modules.end();++it)
        {
            try
            {
                std::vector<Value> VoltagesMeasured=controller.Module(it->getName()).MeasureVoltage();
                std::vector<Value> CurrentsMeasured=controller.Module(it->getName()).MeasureCurrent();
                for(std::set<int>::iterator itt=it->Begin();itt!=it->End();++itt)
                {
                    std::string vol=VoltagesMeasured[*itt].String();
                    std::string cur=CurrentsMeasured[*itt].String();
                    std::string com=command.GenerateCommand(tim.str(),controller.Module(it->getName()).GetSerialNumber().String(),std::to_string(*itt),vol,cur);
                    Myconnection->execute(com);
                }
            }
            catch(const std::string& module)
            {
                std::cout<<"Module "<<module<<" not connected or not in the ModuleConfiguration file ! \n";
                std::cout<<"NOT TAKING INTO ACCOUNT THIS MODULE !!!\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(conn.getWaitingTime()));
    }
    return 0;   */
}
