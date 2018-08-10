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
#include <thread>
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
    
    CAENConnector* Clone() override 
    { 
        if(isCrateConnector()==true) return this;
        else return new CAENConnector(*this);
    }
    
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


Value GetChName(std::vector<Value>& params)
{
    std::string chnames{""};
    unsigned short listaCh[2048];
    char  (*listNameCh)[MAX_CH_NAME];
    for( int n = 0; n < params[2].UShort(); n++ )
    {
        listaCh[n] = params[3+n].UShort() ;
    }
    listNameCh = (char (*)[MAX_CH_NAME]) malloc(params[2].UShort()*MAX_CH_NAME);
	CAENHVRESULT ret = CAENHV_GetChName(m_handle,params[1].UShort(),params[2].UShort(), listaCh, listNameCh);
	if( ret != CAENHV_OK )
    {
	   delete [] listNameCh;
       std::cout<<"CAENHV_GetChName: "<<CAENHV_GetError(m_handle)<<" (num. "<<ret<<")\n\n";
    }
	else
    {  
	    for(int  n = 0; n < params[2].UShort(); n++ ) chnames+=std::to_string(listaCh[n])+", "+std::string(listNameCh[n]);
    }  
    delete [] listNameCh;
    return chnames;
}
 
    Value GetChParamProp(std::vector<Value>& params)
    {
        return GetChParamProp(params[1].UShort(),params[2].UShort(),params[3].CString(),params[4].CString());
    }
    
    
    Value GetChParamProp(const unsigned short& slot,const unsigned short& channel,const std::string& params, const std::string& property)
    {
         unsigned long type{0};
         CAENHV_GetChParamProp(m_handle,slot,channel,params.c_str(),property.c_str(),&type);
         return Value(type);
    }
    
    
Value GetChParam(std::vector<Value>& params)
{
    std::string respond{""};
	float fParValList{0};
	unsigned long	tipo{0};
    unsigned long lParValList{0};
    unsigned short channel =params[3].UShort();
	CAENHVRESULT ret = CAENHV_GetChParamProp(m_handle,params[1].UShort(),params[3].UShort(),params[2].CString(), "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetChParamProp: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		ret = CAENHV_GetChParam(m_handle,params[1].UShort(),params[2].CString(),1,&channel,&fParValList);
	}
	else
	{
		ret = CAENHV_GetChParam(m_handle,params[1].UShort(),params[2].CString(),1,&channel,&lParValList);
	}

	if( ret != CAENHV_OK ) printf("CAENHV_GetChParam: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	else
	{
        if( tipo == PARAM_TYPE_NUMERIC )
        {
            respond=std::to_string(fParValList);
        }
        else
        {
            respond=std::to_string(lParValList);
        }
	}
    return respond;
}
 
Value GetChParamInfo(std::vector<Value>& params)
{
    std::string param{""};
    int	parNum{0};
	char* ParNameList = (char *)nullptr;
	CAENHVRESULT ret = CAENHV_GetChParamInfo(m_handle,params[1].UShort(),params[2].UShort(),&ParNameList,&parNum);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetBdParamInfo: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
    char (*par)[MAX_PARAM_NAME]= (char(*)[MAX_PARAM_NAME])ParNameList;
	for(int i = 0 ; i < parNum ; i++ )
    {
        param=std::string(par[i])+", "+param;
	}
	return Value(param);
}

Value GetChParamInfo(const unsigned short& slot,const unsigned short& channel)
{
    std::string param{""};
    int	parNum{0};
	char* ParNameList = (char *)nullptr;
	CAENHVRESULT ret = CAENHV_GetChParamInfo(m_handle,slot,channel,&ParNameList,&parNum);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetBdParamInfo: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
    char (*par)[MAX_PARAM_NAME]= (char(*)[MAX_PARAM_NAME])ParNameList;
	for(int i = 0 ; i < parNum ; i++ )
    {
        param=std::string(par[i])+", "+param;
	}
	return Value(param);
}
 
Value GetExecCommList(std::vector<Value>& params)
{
	unsigned short	NrOfExec;
	std::string d{""};
    char *c=nullptr;
	char *ExecList = (char *)nullptr;
	CAENHVRESULT ret = CAENHV_GetExecCommList(m_handle, &NrOfExec, &ExecList);
                                                 
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetExecCommList: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
	else
	{
		c = ExecList;
		for(int i = 0; i < NrOfExec; i++, c += strlen(c) + 1 )   
		{
			d=std::string(c)+", "+d;
		}
		if( ExecList != nullptr) delete ExecList;
	} 
	return d;
}
 
void keepAliveFunction()
{
    std::vector<Value> dumb;
    while(1)
    {
        GetSysPropList(dumb);
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
}
 
 
Value GetSysPropList(std::vector<Value>& params)
{
    std::string retour{""};
	unsigned short	NrOfProp{0};
	char			*p{nullptr};
	char			*PropList{nullptr};
	CAENHVRESULT ret = CAENHV_GetSysPropList(m_handle, &NrOfProp, &PropList);                                                 
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetSysPropList: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
	else
	{
		p = PropList;    
		for(int i = 0 ; i < NrOfProp ; i++, p += strlen(p) + 1 )   
		{
            retour=std::string(p)+", "+retour;
		}
		if( PropList != nullptr) delete PropList;
	}
    return retour;
}
 
Value GetSysProp(std::vector<Value>& params)
{
    unsigned long type{0};
    CAENHV_GetSysProp(m_handle,params[1].CString(),&type);
    return Value(type);
}

Value GetSysPropInfo(std::vector<Value>& params)
{
    unsigned int mode{0};
    unsigned type{0};
    std::string param{""};
	CAENHVRESULT ret = CAENHV_GetSysPropInfo(m_handle,params[1].CString(),&mode,&type);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetSysPropInfo: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
    param=std::to_string(mode)+", "+std::to_string(type);
	return Value(param);
}

Value SetBdParam(std::vector<Value>& params)
{
	float fParVal=params[3].Float();
    unsigned short slot=params[1].UShort();
	unsigned long	tipo{0};
    unsigned long lParVal=params[3].ULong();
	CAENHVRESULT ret = CAENHV_GetBdParamProp(m_handle,params[1].UShort(),params[2].CString(), "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetBdParamProp: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		ret = CAENHV_SetBdParam(m_handle,1,&slot,params[2].CString(),&fParVal);
	}
	else
	{
		ret = CAENHV_SetBdParam(m_handle,1,&slot,params[2].CString(),&lParVal);
	}
	if(ret != CAENHV_OK)printf("CAENHV_SetBdParam: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
    return ret;
}

Value SetChName(std::vector<Value>& params)
{
    unsigned short channel=params[2].UShort();
	CAENHVRESULT ret = CAENHV_SetChName(m_handle,params[1].UShort(),1,&channel,params[3].CString());
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_SetChName: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
    return ret;
}

Value SetChParam(std::vector<Value>& params)
{
    float fParVal=params[4].Float();
	unsigned long	tipo{0};
    unsigned long lParVal=params[4].ULong();
    unsigned short channel=params[3].UShort();
	CAENHVRESULT ret = CAENHV_GetChParamProp(m_handle,params[1].UShort(),channel,params[2].CString(), "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetChParamProp: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
	}
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		ret = CAENHV_SetChParam(m_handle,params[1].UShort(),params[2].CString(),1,&channel, &fParVal);
	}
	else
	{
		ret = CAENHV_SetChParam(m_handle,params[1].UShort(),params[2].CString(),1,&channel, &lParVal);
	}
	if(ret != CAENHV_OK) printf("CAENHV_SetChParam: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
    return ret;
}

Value SetSysProp(std::vector<Value>& params)
{
	unsigned Mode{0};
    unsigned Type{0};
	CAENHVRESULT ret = CAENHV_GetSysPropInfo(m_handle,params[1].CString(), &Mode, &Type);
	if( ret != CAENHV_OK )
	{
		printf("CAENHV_GetSysPropInfo: %s (num. %d)\n\n",CAENHV_GetError(m_handle), ret);
	}
	switch( Type )
	{
        case SYSPROP_TYPE_STR: 
        {
            std::string set1=params[2].CString();
            ret = CAENHV_SetSysProp(m_handle,params[1].CString(),&set1);
            break;
        }
        case SYSPROP_TYPE_REAL:
        {
            float set2=params[2].Float();
            ret = CAENHV_SetSysProp(m_handle,params[1].CString(),&set2); 
            break;
        }
        case SYSPROP_TYPE_UINT2:
        {
            unsigned short set3=params[2].UShort();
            ret = CAENHV_SetSysProp(m_handle,params[1].CString(),&set3); 
            break;
        }
        case SYSPROP_TYPE_UINT4:
        {
            unsigned long set4=params[2].ULong();
            ret = CAENHV_SetSysProp(m_handle,params[1].CString(),&set4); 
            break;
        }
        case SYSPROP_TYPE_INT2:
        {
            short set5=params[2].Short();
            ret = CAENHV_SetSysProp(m_handle,params[1].CString(),&set5); 
            break;
        }
        case SYSPROP_TYPE_INT4:
        {
            long set6=params[2].Long();
            ret = CAENHV_SetSysProp(m_handle,params[1].CString(),&set6); 
            break;
        }
        case SYSPROP_TYPE_BOOLEAN:
        {
            unsigned set7=params[2].UInt();
            ret = CAENHV_SetSysProp(m_handle,params[1].CString(),&set7);
            break;
        }
	}
    if(ret != CAENHV_OK) printf("CAENHV_SetSysProp: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
    return Value(ret);
}

Value TestBdPresence(std::vector<Value>& params)
{
    unsigned short NrOfCh{0};
    unsigned short serNumb{0};
    unsigned char fmwMax{0};
    unsigned char fmwMin{0};
    //std::string Model{""};
    //std::string* mdl=&Model;
    //std::string Descr{""};
    //std::string* des=&Descr;
    char Model[15], Descr[80];
    char *mdl,*des;
    mdl=(char*)Model;
    des=(char*)Descr;
	CAENHVRESULT ret = CAENHV_TestBdPresence(m_handle,params[1].UShort(), &NrOfCh,&mdl,&des, &serNumb, &fmwMin, &fmwMax);
	if( ret != CAENHV_OK )
    {
        printf("CAENHV_TestBdPreset: %s (num. %d)\n\n", CAENHV_GetError(m_handle), ret);
    }
    std::string results=std::string(Model)+", "+std::string(Descr)+", "+std::to_string(NrOfCh)+", "+std::to_string(fmwMax)+"."+std::to_string(fmwMin)+", "+std::to_string(serNumb);
    return Value(0);
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
    std::thread m_keepAlive;
};
#endif
