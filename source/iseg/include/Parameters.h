#ifndef Parameters_H
#define Parameters_H
#include <string>
#include "serial.h"

class Parameters
{
public:
    void setName(const std::string&);
    std::string getName();
    void setSN(const std::string&);
    std::string getSN();
    void setDescription(const std::string&);
    std::string getDescription();
    void setBaudrate(const unsigned int&);
    uint32_t getBaudrate();
    void setTimeout(const unsigned int&);
    serial::Timeout getTimeout();
    void setBytesize(const std::string&);
    serial::bytesize_t getBytesize();
    void setParity(const std::string&);
    serial::parity_t getParity();
    void setStopbits(const std::string&);
    serial::stopbits_t getStopbits();
    void setFlowcontrol(const std::string&);
    serial::flowcontrol_t getFlowcontrol();
    void Print();
private:
    std::string Name;
    std::string SN;
    std::string Description;
    uint32_t Baudrate;
    serial::Timeout Timeout;
    serial::bytesize_t Bytesize;
    serial::parity_t Parity;
    serial::stopbits_t Stopbits;
    serial::flowcontrol_t Flowcontrol;
};
#endif
