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
   // std::cout<<manager.getNbrChannels()<<std::endl;
    std::map<int,int> a={{1,1},{2,3}};
    Parameters b;
    b=a;
    Parameters c(a);
    //c.printParameters();
    //std::thread toto=manager.printVoltageCurrentthread();
   /* while(1)
    {
        manager.on();
        std::this_thread::sleep_for(std::chrono::seconds(60));
        manager.off();
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }*/
   std::vector<Measure> mes=manager.getMeasures();
  for(unsigned int i=0;i!=mes.size();++i) mes[i].print();
    //manager.off();
   // toto.join();
   // manager.printVoltageCurrentthread().join();
/*std::map<std::string,std::string> serial2{
            {"Name","iseg2"},
            {"SN","titi"},
            {"Description","blabla"},
            {"Port","/dev/ttyUSB0"},
            {"Baudrate" , "9600"},
            {"Timeout" , "100"},
            {"Bytesize" , "8"},
            {"Parity" , "none"},
            {"Stopbits" , "1"},
            {"Flowcontrol", "none"}
        };*/
       
/*std::map<std::string,std::string> serial1{
            {"Name","iseg1"},
            {"SN","titi"},
            {"Description","blabla"},
            {"Port","/dev/ttyUSB1"},
            {"Baudrate" , "9600"},
            {"Timeout" , "100"},
            {"Bytesize" , "8"},
            {"Parity" , "none"},
            {"Stopbits" , "1"},
            {"Flowcontrol", "none"},
            {"Slot", "0"}
        };*/
   /*   std::map<std::string,std::string> snmp1{
            {"Name","WIENER1"},
            {"MIB_path","/home/sjturpc/Desktop/Sauron/conf/MIB"},
            {"MIB_filename","WIENER-CRATE-MIB"},
            {"Description","blabla"},
            {"Version","v2c"},
            {"Baudrate" , "9600"},
            {"Timeout" , "10000"},
            {"Bytesize" , "8"},
            {"Retries" , "5"},
            {"Port" , "1"},
            {"Flowcontrol", "none"},
            {"ReadCommunity","guru"},
            {"Slot" , "0"}
        };*//*
          std::map<std::string,std::string> snmp2{
            {"Name","WIENER2"},
            {"MIB_path","/home/sjturpc/Desktop/Sauron/conf/MIB"},
            {"MIB_filename","WIENER-CRATE-MIB"},
            {"Description","blabla"},
            {"Version","v2c"},
            {"Baudrate" , "9600"},
            {"Timeout" , "10000"},
            {"Bytesize" , "8"},
            {"Retries" , "5"},
            {"Port" , "1"},
            {"Flowcontrol", "none"},
            {"ReadCommunity","guru"},
            {"Slot" , "1"}
        };*/
    //SNMPConnector snmpcon(snmp1);
   /* SNMPConnector snmpcon2(snmp2);
    
    */
    /*SerialConnector iseg1(serial1);
    SerialConnector iseg2(serial2);*/
  //  Crate crate;
   /* Module* toto=new isegModule(iseg1);
    toto->setParameters(serial1);
    toto->printParameters();*/
    /*Module* toto2=new isegModule(iseg2);
    toto2->setParameters(serial2);
    
    Module* toto3=new WIENERModule(snmpcon);
    toto3->setParameters(snmp1);
    Module* toto4=new WIENERModule(snmpcon2);
    toto4->setParameters(snmp2);
    
    crate.addModule(toto);
    crate.addModule(toto2);
    crate.addModule(toto3);
    crate.addModule(toto4);
    
    crate.Initialize();
    crate.Connect();
    crate.on();
    crate.printVoltageCurrent();
    crate.Disconnect();*/
   // Module* toto3=new isegModule(iseg1);
   // Module* toto2=new isegModule(iseg2);
   // toto3->setParameters(param);
 //   toto2->setParameters(param3);
    //toto3->Initialize();
    //toto3->Connect();
    //toto3->off();
 //   crate.addModule(toto3);
//    crate.addModule(toto2);
    //crate.Initialize();
  //  crate.Connect();
 //   crate.on();
  //  crate.Disconnect();
    
    //crate.addModule(toto);
    //crate.addModule(toto2);
   /* crate.addModule(toto3);
    toto3->getName();
    crate.Initialize();
    crate.Connect();
    crate.on();*/
    
   // wiener->Connect();
    //wiener->on();
   // wiener->setVoltage(1.5);
   // wiener->setCurrent(0.125);
  //  wiener->printVoltageCurrent();
   // std::cout<<"Slot : "<<wiener->getSlot()<<std::endl;
  //  wiener->setSlot(1);
  //  std::cout<<"Slot : "<<wiener->getSlot()<<std::endl;
  //  wiener->on();
   // wiener->printVoltageCurrent();
    //wiener->on(1);
 //   iseg->Connect();
 //   iseg2->Connect();
 /*   iseg->on();
    iseg2->on();
    iseg->setVoltage(1254.25);
    iseg->setCurrent(0.0001);
    iseg->printVoltageCurrent();
    iseg->Disconnect();
    iseg2->setVoltage(1254.25);
    iseg2->setCurrent(0.0001);
    iseg2->printVoltageCurrent();
    iseg2->Disconnect();*/
 return 0;   
    
}
