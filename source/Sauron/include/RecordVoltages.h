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

#ifndef RECORDVOLTAGES_h
#define RECORDVOLTAGES_h
#include "Monitoring.h"
#include "json/json.h"
#include "mariadb++/account.hpp"
#include "mariadb++/connection.hpp"
#include "mariadb++/time.hpp"
#include <cmath>

class RecordVoltages : public Monitoring
{
public:
    RecordVoltages():Monitoring("RecordVoltages")
    {
        Json::Value root=OpenJSONFile("RecordHV");
        ExtractData(root);
        ConnectToDatabase();
    }
    RecordVoltages(RacksManager* rackmanager):Monitoring(rackmanager,"RecordVoltages")
    {
        Json::Value root=OpenJSONFile("RecordHV");
        ExtractData(root);
        ConnectToDatabase();
    }
    void function()
    {
        m_racksmanager->operator()("","");
        std::vector<MeasuresAndSets> data=m_racksmanager->getMeasuresAndSets();
        std::vector<VoltageWanted> wanted= m_racksmanager->getWantedVoltage();
        std::time_t ti= ::time(nullptr);
        mariadb::date_time tim(ti);
        for(std::size_t i=0;i!=data.size();++i)
        {
            int o=0;
            for(std::size_t j=0;j!=wanted.size();++j)
            {
                if(data[i].getPosition()==wanted[j].getPosition()) o=j;
            }
            std::string values="\"";
            values+=std::to_string(m_measureNbr);
            values+="\",\""+tim.str()+"\",\""+std::to_string(ID::getConfigID());
            values+="\",\""+std::to_string(data[i].getPosition().getRack());
            values+="\",\""+std::to_string(data[i].getPosition().getCrate());
            values+="\",\""+std::to_string(data[i].getPosition().getModule());
            values+="\",\""+std::to_string(data[i].getPosition().getChannel());
            values+="\",\""+wanted[o].getWantedVoltage().String();
            values+="\",\""+data[i].getSetVoltage().String();
            values+="\",\""+data[i].getMeasuredVoltage().String();
            values+="\",\""+data[i].getSetCurrent().String();
            values+="\",\""+data[i].getMeasuredCurrent().String();
            values+="\",\""+std::to_string(wanted[o].isMonitored());
            values+="\",\"0\"";
            std::string command=std::string("INSERT INTO "+m_params["HVDatabase"].String()+"."+m_params["HVTable"].String()+" (Measure,Time,ID,Rack,Crate,Module,Channel,AskedVoltage,SetVoltage,MeasuredVoltage,SetCurrent,MeasuredCurrent,HVMonitored,MonitoringProblem) VALUES (")+values+std::string(");");
           // std::cout<<command<<std::endl;
            Myconnection->execute(command);
            values="";
        }
        m_measureNbr++;
    }
private :
    void init()
    {
        sendJSON();
        std::string command="SELECT Measure FROM "+m_params["HVDatabase"].String()+"."+m_params["HVTable"].String()+" WHERE Measure=(SELECT max(Measure) FROM "+m_params["HVDatabase"].String()+"."+m_params["HVTable"].String()+");";
        mariadb::result_set_ref result = Myconnection->query(command);
        if(result->row_count()==0) m_measureNbr=0;
        else
        {
            result->set_row_index(0);
            m_measureNbr=result->get_signed32(0)+1;
        }
    }
    void sendJSON()
    {
        try
        {
            std::string json=ID::createJSON();
            std::string command=std::string("SELECT * FROM "+m_params["ConfigDatabase"].String()+"."+m_params["ConfigTable"].String()+" WHERE config LIKE '")+json+std::string("';");
            //std::cout<<command<<std::endl;
            mariadb::result_set_ref result = Myconnection->query(command);
            if(result->row_count()==0) 
            {
                std::string command2=std::string("INSERT INTO "+m_params["ConfigDatabase"].String()+"."+m_params["ConfigTable"].String()+ "(config) VALUES ('")+json+std::string("');");
                Myconnection->insert(command2);
                result=Myconnection->query(command);
            }
            result->set_row_index(0);
            ID::setConfigID(result->get_signed32(0));
        }
        catch(...)
        {
            std::cout<<"Problem in sendJSON(). Program will countinue but will not upload Config in database and configID=0 !!!"<<std::endl;
        }
    }
    
    
    
    std::string getEnvVar( std::string const & key )
    {
        if(std::getenv( key.c_str() )==nullptr) return "";
        else return std::string(std::getenv( key.c_str() ));
    }
    Json::Value OpenJSONFile(std::string envVar)
    {
        Json::CharReaderBuilder builder;
        Json::Value obj;   // will contain the root value after parsing.
        std::string errs;
        std::string FileName=getEnvVar(envVar.c_str());
        if(FileName=="")
        {
            std::cout<<"Please add "<<envVar<<" as variable environment ! \n";
        }
        std::ifstream ConfFile(FileName.c_str(),std::ifstream::binary);
        bool ok = Json::parseFromStream(builder,ConfFile,&obj,&errs);
        if ( !ok )
        {
            std::cout  << errs << "\n";
        }
        return obj;
    }
    void ExtractData(const Json::Value& params)
    {
        std::vector<std::string> id = params["Database"].getMemberNames();
        for(unsigned int i=0;i!=id.size();++i)
        {
            m_params.addParameter(id[i],params["Database"][id[i]].asString());
        }
        id = params["Options"].getMemberNames();
        for(unsigned int i=0;i!=id.size();++i)
        {
            m_params.addParameter(id[i],params["Options"][id[i]].asString());
        }
    }
    void ConnectToDatabase()
    {
        Myaccount= mariadb::account::create(m_params["Host"].String(),m_params["User"].String(),m_params["Password"].String());
        Myconnection=mariadb::connection::create(Myaccount);
        setTime(m_params["Waiting Time"].UInt());
    }
    mariadb::account_ref Myaccount;
    mariadb::connection_ref Myconnection;
    std::map<std::string,std::vector<unsigned int>> Channels;
    unsigned long long m_measureNbr{0};
};
#endif

