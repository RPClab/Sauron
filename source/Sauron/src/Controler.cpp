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


#include "Controler.h"
#include <cstdlib>
#include "json/json.h"
#include "serial.h"
#include <iostream>
#include <fstream>
//std::string Controler::getEnvVar( std::string const & key )
//{
   // if(std::getenv( key.c_str() )==nullptr) return "";
   // else return std::string(std::getenv( key.c_str() ));
//}

//Json::Value Controler::OpenJSONFile(std::string envVar)
//{
  /*  Json::CharReaderBuilder builder;
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
//}

//void Controler::ExtractModulesInfos(Json::Value root)
//{
    /*const Json::Value& modules = root["Modules"];
    for(unsigned int i=0; i<modules.size();++i)
    {
        std::map<std::string,std::string> param{
            {"Name",modules[i]["Name"].asString()},
            {"SN",modules[i]["SN"].asString()},
            {"Description",modules[i]["Description"].asString()},
            {"Baudrate",modules[i]["Baudrate"].asString()},
            {"Timeout",modules[i]["Timeout"].asString()},
            {"Bytesize",modules[i]["Bytesize"].asString()},
            {"Parity",modules[i]["Parity"].asString()},
            {"Stopbits",modules[i]["Stopbits"].asString()},
            {"Flowcontrol",modules[i]["Flowcontrol"].asString()},
            {"Port",""},
        };
        params.push_back(std::move(param));
    }*/
//}

//void Controler::DiscoverModules()
//{
  /* std::vector<serial::PortInfo> Devices=serial::list_ports();
   for(unsigned int i=0;i!=Devices.size();++i)
    {
        std::cout<<Devices[i].port<<std::endl;
        for(unsigned int j=0;j!=params.size();++j)
        {
            try
            {
                
                //params[j].insert(std::pair<std::string,std::string>("Port",Devices[i].port));
                params[j]["Port"]=Devices[i].port;
                isegNHSR device(params[j]);
                if(device.IsMySerialNumber(params[j]["SN"])==true)
                {
                    Modules.insert(std::pair<std::string,isegNHSR>(params[j]["Name"],std::move(device)));
                    break;
                }
            }
            catch(serial::IOException)
            {
              break;
            }
        }
    }
    for(std::map<std::string,isegNHSR>::iterator it=Modules.begin();it!=Modules.end();++it)
   {
        std::cout<<it->first<<" connected"<<std::endl;
    }*/
//}

//isegNHSR& Controler::Module(const std::string& module)
//{
    /*if(Modules.find(module)!=Modules.end()) return Modules[module];
    else
    {
        std::cout<<"Module "<<module<<" not found !\n";
        throw(module);
    }*/
//}


//Controler::Controler()
//{
    //ExtractModulesInfos(OpenJSONFile("ModulesConfFile"));
   // DiscoverModules();
//}
