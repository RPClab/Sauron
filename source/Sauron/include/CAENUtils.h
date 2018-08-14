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
 
class ParProp
{
public:
    ParProp(){};
    ParProp(const Value& value)
    {
        std::vector<Value> val=value.Tokenize(", ");
        Type=val[0].ULong();
        Mode=val[1].ULong();
        if(Type==PARAM_TYPE_NUMERIC)
        {
            MinVal=val[2].Float();
            MaxVal=val[3].Float();
            Unit=val[4].UShort();
            Exp=val[5].Short();
        }
        else
        {
            OnState=val[2].String();
            OffState=val[3].String();
        }
    }
    //Ugly to mimic C++ style with C
    void print(std::ostream& stream=std::cout)
    {
        stream<<"Type : "<<Type<<"\n";
        stream<<"Mode : "<<Mode<<"\n";
        if(Type==PARAM_TYPE_NUMERIC)
        {
            stream<<"MinVal : "<<MinVal<<"\n";
            stream<<"MaxVal : "<<MaxVal<<"\n";
            stream<<"Unit : "<<Unit<<"\n";
            stream<<"Exp : "<<Exp<<"\n";
        }
        else
        {
            stream<<"OnState : "<<OnState<<"\n";
            stream<<"OffState : "<<OffState<<"\n";
        }
    }
    unsigned long Type{0};
    unsigned long Mode{0};
    float MinVal{0};
    float MaxVal{0};
    unsigned short	Unit{0};
    short Exp{0};
    std::string OnState{""};
    std::string OffState{""};
};
#endif
