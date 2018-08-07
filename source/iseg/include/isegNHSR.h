#ifndef isegNHSR_H
#define isegNHSR_H
#include <string>
#include "SerialConnector.h"
#include "Value.h"
#include <vector>
#include <bitset>
class isegNHSR
{
public:
    isegNHSR(){};
    isegNHSR(std::map<std::string,std::string>& params);
    bool Set();
    bool Connect();
    bool IsConnected();
    void Disconnect();
    //Common instruction set 
    Value ID();
    void Clear();
    void Reset();
    Value InstructionSet();
    //iseg SCPI instruction set 
    //:VOLT voltage
    void SetVoltage(const int&,const double&);
    void SetVoltage(const double&);
    void On(const int&);
    void On();
    void Off(const int&);
    void Off();
    void Emergency(const int&);
    void Emergency();
    void ClearEmergency(const int&);
    void ClearEmergency();
    void SetVoltageBound(const int&,const double&);
    void SetVoltageBound(const double&);
    //:CURR current
    void SetCurrent(const int&,const double&);
    void SetCurrent(const double&);
    void SetCurrentBound(const int&,const double&);
    void SetCurrentBound(const double&);
    //:EV
    void ClearEvent(const int& );
    void ClearEvent();
    //NOTFORNOW void SetEvent(const int&, const std::string word){};
    //:CONF
    void SetTripTime(const int&, const int&);
    void SetTripTime(const int&);
    Value GetTripTime(const int&);
    void SetTripAction(const int&,const int&);
    void SetTripAction(const int&);
    Value GetTripAction(const int&);
    //NOTFORNOW void SetInhibitAction(const int&,const int&);
    //NOTFORNOW void SetInhibitAction(const int&);
    Value GetInhibitAction(const int&);
    //// Only for some devices
    //NOTFORNOW void SetOutputMode(const int &,const int&){};
    //NOTFORNOW std::string GetOutputMode(const int&){};
    //NOTFORNOW void SetPolarity(const int&, const int&){};
    //NOTFORNOW std::string GetPolarity(const int&){};
    //////////////////////////
   /* void SetVCTCoef(const int&, const int&);
    std::string GetVCTCoef(const int&);
    //:READ*/
    //NOTFORNOW std::string GetTemperature(const int&);
    Value GetVoltage(const int&);
    std::vector<Value> GetVoltage();
   /* std::string GetVoltageLimit(const int&);
    std::string GetVoltageNominal(const int&);*/
    /// Only for some devices
    //NOTFORNOW std::string GetVoltageMode(const int&){};
    //////////////////////////////////////
    /*std::string GetVoltageBound(const int&);
    std::string GetVoltageOn(const int&);
    std::string GetVoltageEmergency(const int&);*/
    Value GetCurrent(const int&);
    std::vector<Value> GetCurrent();
    /*std::string GetCurrentLimit(const int&);
   /* std::string GetCurrentNominal(const int&);
    ///Only for some devices
    //NOTFORNOW std::string GetCurrentMode(const int&){};
    ////////////////////////
    std::string GetCurrentBound(const int&);
    std::string GetVoltageRamp(const int&);
    std::string GetCurrentRamp(const int&);
    
    std::string GetChannelControl(const int&);
    std::string GetChannelStatus(const int&);
    std::string GetEventStatus(const int&);
    std::string GetEventMask(const int&);*/
    //:MEAS
    Value MeasureVoltage(const int&);
    std::vector<Value> MeasureVoltage();
    Value MeasureCurrent(const int&);
    std::vector<Value> MeasureCurrent();
    //:CONF
    /*void SetVoltageRamp(const int&,const double&);
    void SetCurrentRamp(const int&, const double&);
    void SetAverageStep(const int&);
    std::string GetAverageStep();
    void EnableKillFunction();
    void DisableKillFunction();
    bool KillFunctionIsEnabled();
    void ClearEventStatus();
    void SetEventMask(std::string);
    void SetEventChannelMask(std::string);
    void SetCANAddress(const int&);
    std::string GetCANAdress();
    void SetBitrate(const int&);
    std::string GetBitRate();
    std::string GetModuleVoltageLimit();
    std::string GetModuleCurrentLimit();
    std::string GetModuleVoltageRamp();
    std::string GetModuleCurrentRamp();
    std::string GetModuleControlWord();
    std::string GetModuleStatusWord();
    std::string GetEventStatusWord();
    std::string GetEventMaskWord();
    std::string GetChannelStatusWord();
    std::string GetChannelMaskWord();
    std::string GetModuleSupply24V();
    std::string GetModuleSupplym24V();
    std::string GetModuleSupply5V();
    std::string GetModuleTemperature();*/
    int getNbrChannels(){return  nbrChannel;};
    Value GetSerialNumber();
    bool IsMySerialNumber(const std::string);
    bool IsModule();
    Value SendCommand(const std::string& command);
   /* void ConfigurationModeOn(const int&);
    void ConfigurationModeOff(const int&);
    std::string IsInConfigurationMode();*/
private:
    SerialConnector  m_serial;
    bool IsAModule{false};
    Value NbrOfChannels();
    Value Compagny={""};
    Value Model={""};
    Value SerialNumber={""};
    Value Version={""};

    std::vector<Value> Tokenize(const std::string& str, const std::string& delimiters = ",");
    int nbrChannel={0};
    Value CleanString(std::string);
    std::bitset<16>ChannelStatus;
    std::bitset<16>ModuleStatus;
};
#endif
