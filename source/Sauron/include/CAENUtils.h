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
#ifndef CAENUTILS_H
#define CAENUTILS_H
#include "Value.h"
#include <string>
extern "C" 
{
    #include "CAEN/CAENComm.h"
    #include "CAEN/CAENHVWrapper.h"
    #include "CAEN/CAENVMElib.h"
    #include "CAEN/CAENVMEoslib.h"
    #include "CAEN/CAENVMEtypes.h"
}
 
 
class UglyCParProp
{
public:
    unsigned long Type{0};
    unsigned long Mode{0};
    float MinVal{0};
    float MaxVal{0};
    unsigned short	Unit{0};
    short Exp{0};
    std::string OnState{'\0',30};
    std::string OffState{'\0',30};
};
 
class ParProp
{
public:
    ParProp(){};
    ParProp(const UglyCParProp& par)
    {
        m_ugly=std::move(par);
    }
    ParProp(Value value)
    {
        std::vector<Value> val=value.Tokenize(", ");
        m_ugly.Type=val[0].ULong();
        m_ugly.Mode=val[1].ULong();
        if(m_ugly.Type==PARAM_TYPE_NUMERIC)
        {
            m_ugly.MinVal=val[2].Float();
            m_ugly.MaxVal=val[3].Float();
            m_ugly.Unit=val[4].UShort();
            m_ugly.Exp=val[5].Short();
        }
        else if (m_ugly.Type == PARAM_TYPE_ONOFF)
        {
            m_ugly.OnState=val[2].String();
            m_ugly.OffState=val[3].String();
        }
    }
    Value toValue()
    {
        std::string valu=std::to_string(m_ugly.Type)+", "+std::to_string(m_ugly.Mode);
        if(m_ugly.Type==PARAM_TYPE_NUMERIC)
        {
            valu+=", "+std::to_string(m_ugly.MinVal);
            valu+=", "+std::to_string(m_ugly.MaxVal);
            valu+=", "+std::to_string(m_ugly.Unit);
            valu+=", "+std::to_string(m_ugly.Exp);
        }
        else if(m_ugly.Type == PARAM_TYPE_ONOFF)
        {
            valu+=", "+m_ugly.OnState;
            valu+=", "+m_ugly.OffState;
        }
        return Value(valu);
    }
    //Ugly to mimic C++ style with C
    void print(std::ostream& stream=std::cout)
    {
        stream<<"Type : "<<m_ugly.Type<<"\n";
        stream<<"Mode : "<<m_ugly.Mode<<"\n";
        if(m_ugly.Type==PARAM_TYPE_NUMERIC)
        {
            stream<<"MinVal : "<<m_ugly.MinVal<<"\n";
            stream<<"MaxVal : "<<m_ugly.MaxVal<<"\n";
            stream<<"Unit : "<<m_ugly.Unit<<"\n";
            stream<<"Exp : "<<m_ugly.Exp<<"\n";
        }
        else if (m_ugly.Type == PARAM_TYPE_ONOFF)
        {
            stream<<"OnState : "<<m_ugly.OnState<<"\n";
            stream<<"OffState : "<<m_ugly.OffState<<"\n";
        }
    }
    unsigned long getType() const
    {
        return m_ugly.Type;
    }
    unsigned long getMode() const
    {
        return m_ugly.Mode;
    }
    float getMinVal()
    {
        return m_ugly.MinVal;
    }
    float getMaxVal()
    {
        return m_ugly.MaxVal;
    }
    short getExp()
    {
        return m_ugly.Exp;
    }
    unsigned short getUnit()
    {
        return m_ugly.Unit;
    }
    std::string getOnState()
    {
        return m_ugly.OnState;
    }
    std::string getOffState()
    {
        return m_ugly.OffState;
    }
private:
    UglyCParProp m_ugly;
};
#endif
