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

#ifndef MONITORVOLTAGES_h
#define MONITORVOLTAGES_h
#include "Monitoring.h"
#include "json/json.h"
#include "mariadb++/account.hpp"
#include "mariadb++/connection.hpp"
#include "mariadb++/time.hpp"
class MonitorVoltages : public Monitoring
{
public:
    MonitorVoltages(RacksManager* rackmanager):Monitoring(rackmanager,"MonitorVoltages")
    {
        Json::Value root=OpenJSONFile("MonitoringHV");
        ExctractData(root);
        ConnectToDatabase();
    }
    void function()
    {
        std::string command="SELECT * FROM "+m_params["Database"].String()+"."+m_params["Table"].String()+" WHERE date=(SELECT max(date) FROM  "+m_params["Database"].String()+"."+m_params["Table"].String()+");";
        //std::cout<<command<<std::endl;
        mariadb::result_set_ref result = Myconnection->query(command);
        result->set_row_index(0);
        Parameters slow;
        std::time_t ti;
        for(unsigned int i=0;i!=result->column_count();++i)
        {
            switch(result->column_type(i))
            {
                case mariadb::value::type::date_time :
                {
                    mariadb::date_time timee(result->get_date_time(i));
                    ti=timee.mktime();
                    slow.addParameter(result->column_name(i),std::to_string(ti));
                    break;
                }
                case mariadb::value::type::unsigned8 :
                {
                    slow.addParameter(result->column_name(i),std::to_string(result->get_unsigned8(i)));
                    break;
                }
                case mariadb::value::type::double64 :
                {
                    slow.addParameter(result->column_name(i),std::to_string(result->get_double(i)));
                    break;
                }
                
            }
        }
        std::chrono::seconds::rep now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
        if(now-ti>=m_params["MaxDelay"].UInt())
        {
            std::cout<<"Slowcontrol was taken "<<now-ti<<"s ago. It,s much bigger than maximal duration asked ("<<m_params["MaxDelay"].UInt()<<"s) !\n";
        }
        slow.printParameters();
    }
private :
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
    void ExctractData(const Json::Value& params)
    {
        std::vector<std::string> id = params["MonitoringHV"].getMemberNames();
        for(unsigned int i=0;i!=id.size();++i)
        {
            std::cout<<id[i]<<" "<<params["MonitoringHV"][id[i]].asString()<<std::endl;
            m_params.addParameter(id[i],params["MonitoringHV"][id[i]].asString());
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
};
#endif

