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

#ifndef CAENCONNECTOR_H
#define CAENCONNECTOR_H
#include "Connector.h"
#include <cstring>
// CAEN
extern "C" 
{
    #include "CAEN/CAENComm.h"
    #include "CAEN/CAENHVWrapper.h"
    #include "CAEN/CAENVMElib.h"
    #include "CAEN/CAENVMEoslib.h"
    #include "CAEN/CAENVMEtypes.h"
}

class CAENConnector : public Connector
{
public:

    CAENConnector();

    CAENConnector(const CAENConnector& other);
    
    CAENConnector& operator=(const CAENConnector& other);
    CAENConnector& operator()(const CAENConnector& other);
    
    CAENConnector(const std::map<std::string,std::string>& params);
    
    void Initialize() override;
    
    void Connect() override;
    
    void Disconnect() override;
    
    CAENConnector* Clone() override { return new CAENConnector(*this);}
    
    void Release() override;
    
    bool IsConnected() override;
    Value SendCommand(const std::string&) override;
    
private:
    Value GetCrateMap(const std::vector<Value>& params)
    {
        std::string results;
        unsigned short	NrOfSl;
        unsigned short *SerNumList=nullptr;
        unsigned short *NrOfCh=nullptr;
        char* ModelList=nullptr;
        char* DescriptionList=nullptr;
        unsigned char	*FmwRelMinList=nullptr;
        unsigned char *FmwRelMaxList=nullptr;
        CAENHV_GetCrateMap(m_handle, &NrOfSl, &NrOfCh, &ModelList, &DescriptionList, &SerNumList,&FmwRelMinList, &FmwRelMaxList );
        if(NrOfSl-1<params[1].Int())
        {
            delete SerNumList; delete ModelList; delete DescriptionList; delete FmwRelMinList; delete FmwRelMaxList; delete NrOfCh;
            std::cout<<"Wrong slot \n";
            throw -2;
        }
		char	*m = ModelList, *d = DescriptionList;
		for( int i = 0; i < NrOfSl; i++ , m += strlen(m) + 1, d += strlen(d) + 1 )
        {
            if(params[1].Int()==i)
            {
                std::string mm=std::string(m);
                if(mm.empty()) mm=" ";
                std::string dd=std::string(d);
                if(dd.empty()) dd=" ";
                results=std::string(mm)+"*"+std::string(dd)+"*"+std::to_string(NrOfCh[i])+"*"+std::to_string(FmwRelMaxList[i])+"."+std::to_string(FmwRelMinList[i])+"*"+std::to_string(SerNumList[i]);
                break;
            }
        }
        delete SerNumList; delete ModelList; delete DescriptionList; delete FmwRelMinList; delete FmwRelMaxList; delete NrOfCh;
        return Value(results);
    }
    Value ExecComm(std::vector<Value>& params)
    {
         return CAENHV_ExecComm(m_handle,params[1].CString());
    }
    
    Value GetBdParamProp(std::vector<Value>& params)
    {
        return GetBdParamProp(params[1].UShort(),params[2].CString(),params[3].CString());
    }
    
    
    Value GetBdParamProp(const unsigned short& slot,const std::string& params, const std::string& property)
    {
         unsigned long type{0};
         CAENHV_GetBdParamProp(m_handle,slot,params.c_str(),property.c_str(),&type);
         return Value(type);
    }
    
Value GetBdParam(std::vector<Value>& params)
{
	int				temp;
	unsigned short	SlotList=params[1].UShort(); 
	float			fParValList{0};
	unsigned long	tipo;
    unsigned long lParValList{0};
	std::string ParName=params[2].CString();

	CAENHVRESULT ret = CAENHV_GetBdParamProp(m_handle,SlotList, ParName.c_str(), "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetBdParamProp: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
	
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		ret = CAENHV_GetBdParam(m_handle,1,&SlotList, ParName.c_str(),&fParValList);
	}
	else
	{
		ret = CAENHV_GetBdParam(m_handle,1,&SlotList, ParName.c_str(),&lParValList);
	}

	if( ret != CAENHV_OK ) printf("CAENHV_GetBdParam: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	else
	{
		if( tipo == PARAM_TYPE_NUMERIC ) printf("\nSlot: %2d  %s: %10.2f\n",SlotList,ParName.c_str(),fParValList); 
		else printf("\nSlot: %2d  %s: %x\n",SlotList,ParName.c_str(), lParValList); 
	}
	if(tipo == PARAM_TYPE_NUMERIC) return Value(fParValList);
    else return Value(lParValList);
}

Value GetBdParamInfo(const unsigned short& slot)
{
    std::string param{""};
    int	parNum{0};
	char* ParNameList = (char *)nullptr;
	CAENHVRESULT ret = CAENHV_GetBdParamInfo(m_handle,slot,&ParNameList);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetBdParamInfo: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
    char (*par)[MAX_PARAM_NAME]= (char(*)[MAX_PARAM_NAME])ParNameList;
	for(int i = 0 ; par[i][0] ; i++ ) parNum++;
	for(int i = 0 ; i < parNum ; i++ )
    {
        param=std::string(par[i])+", "+param;
	}
	return Value(param);
}

Value GetBdParamInfo(std::vector<Value>& params)
{
    return GetBdParamInfo(params[1].UShort());
}

    
    void setCAENHVSystemType();
    void setLinkType();
    void setArg();
    void setIP();
    void setIBusAddress();
    void setLinkNum();
    void setConetNode();
    void setVMEBaseAddress();
    void setPort();
    void setBaudRate();
    void setParity();
    void setCommData();
    void setCommStop();
    void setUsername();
    void setPassword();
    CAENHV_SYSTEM_TYPE_t m_CAENHVSystemType{SY1527};
    int m_linkType{0};
    std::string m_IP{""};
    std::string m_IBusAddress{""};
    std::string m_linkNum{""};
    std::string m_conetNode{""};
    std::string m_VMEBaseAddress{""};
    std::string m_port{""};
    std::string m_baudrate{""};
    std::string m_commData{""};
    std::string m_commStop{""};
    std::string m_parity{""};
    std::string m_username{""};
    std::string m_password{""};
    std::string m_arg{""};
    int m_handle{-1};
    int m_connected{-1};
};
#endif
