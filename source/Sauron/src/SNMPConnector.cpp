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

#include "SNMPConnector.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <vector>
#include <bitset>
SNMPConnector::SNMPConnector(const SNMPConnector& other):SNMPConnector()
{
    m_params=other.m_params;
}

SNMPConnector::SNMPConnector()
{
    m_type="SNMPConnector";
}

SNMPConnector::SNMPConnector(const std::map<std::string,std::string>& params):SNMPConnector()
{
    m_params=params;
}

void SNMPConnector::setMIBPath()
{
    if(m_params.hasParam("MIB_path"))
    {
        m_MIBPath=m_params["MIB_path"].CString();
        add_mibdir(m_MIBPath.c_str());
    }
    else
    {
        std::cout<<"The MIB file will be searched in the default directory !\n";
    }
}

void SNMPConnector::setMIBFilename()
{
    if(m_params.hasParam("MIB_filename")) m_MIBFilename=m_params["MIB_filename"].CString();
    else 
    {
        std::cout<<"You must provide the MIB filename !\n";
        throw -1;
    }
}

void SNMPConnector::setTimeout()
{
    if(m_params.hasParam("Timeout"))
    {
        m_timeout=m_params["Timeout"].Long();
    }
    else
    {
        std::cout<<"Timeout putted to default value "<<m_timeout<<" !\n";
    }
}

void SNMPConnector::setReadCommunity()
{
    if(m_params.hasParam("ReadCommunity"))
    {
        m_readCommunity=m_params["ReadCommunity"].String();
    }
    else
    {
        std::cout<<"ReadCommunity putted to default value "<<m_readCommunity<<" !\n";
    }
}


void SNMPConnector::setRetries()
{
    if(m_params.hasParam("Retries"))
    {
        m_retries=m_params["Retries"].Int();
    }
    else
    {
        std::cout<<"Retries putted to default value "<<m_retries<<" !\n";
    }
}
    
void SNMPConnector::setVersion()
{
    if(m_params.hasParam("Version"))
    {
        if(m_params["Version"]=="v1") m_version=SNMP_VERSION_1;
        else if(m_params["Version"]=="v2c") m_version=SNMP_VERSION_2c;
        else if(m_params["Version"]=="v3") m_version=SNMP_VERSION_3;
        else
        {
            std::cout<<"Version : "<<m_params["Version"]<<" not known ! \n";
            std::cout<<"Version putted to default value "<<m_version<<" !\n";
        }
    }
    else
    {
        std::cout<<"Version putted to default value "<<m_version<<" !\n";
    }
}


void SNMPConnector::Initialize()
{
    setTimeout();
    setRetries();
    setVersion();
    setReadCommunity();
    setMIBFilename();
	init_snmp("WIENER_SNMP_DLL");
    setMIBPath();
	init_mib();	// init MIB processing      
	if (!read_module(m_MIBFilename.c_str())) 
    {   
        std::cout<<"Unable to load SNMP MIB file "<<m_MIBFilename<<" !\n";
		throw false;
	}
	SOCK_STARTUP;
}

bool SNMPConnector::IsConnected()
{
    if(m_session==nullptr) return false;
    else return true;
}

void SNMPConnector::Release()
{

}

void SNMPConnector::Disconnect()
{
    if(IsConnected())
    {    
        snmp_close(m_session);
        m_session=nullptr;
        SOCK_CLEANUP;
        std::cout<<"SNMP session for host "<<m_IP<<" closed \n";
    }
}


void SNMPConnector::Connect()
{
    if(!IsConnected())
    {
        SOCK_STARTUP;
        struct snmp_session session;
        snmp_sess_init(&session);                  // structure defaults
        session.version = m_version;
        session.peername = strdup(m_IP.c_str());
        session.community = (u_char *)strdup(m_readCommunity.c_str());
        session.community_len = strlen(m_readCommunity.c_str());
        session.timeout = m_timeout;   // timeout (us)
        session.retries = m_retries;        // retries
        if (!(m_session = snmp_open(&session))) 
        {
            int liberr=0;
            int syserr=0;
            char *errstr = nullptr;
            snmp_error(&session, &liberr, &syserr, &errstr);
            std::cout<<"Open SNMP session for host "<<m_IP<<" : "<<errstr<<"\n";
            free(errstr);
            throw 0;
        }
        std::cout<<"SNMP session for host "<<m_IP<<" opened \n";
    }
    else
    {
        std::cout<<"Yet connected"<<std::endl;
    }
}

SNMPConnector& SNMPConnector::operator=(const SNMPConnector& other)
{
    m_type=other.m_type;
    m_params=other.m_params;
    m_session=new snmp_session(*other.m_session);
    return *this;
}

SNMPConnector& SNMPConnector::operator()(const SNMPConnector& other)
{
    m_type=other.m_type;
    m_params=other.m_params;
    m_session=new snmp_session(*other.m_session);
    return *this;
}

Value SNMPConnector::ReceiveInfos(const std::string& command)
{
    netsnmp_pdu* pdu= snmp_pdu_create(SNMP_MSG_GET);
    netsnmp_pdu* response=nullptr;
    netsnmp_variable_list *vars=nullptr;
    std::size_t name_length = MAX_OID_LEN;
    oid name[MAX_OID_LEN];
    if (!snmp_parse_oid(command.c_str(),name, &name_length)) 
    {
        snmp_perror(command.c_str());
        throw -1;
    } 
    else snmp_add_null_var(pdu, name, name_length);
    bool stop=false;
    int count=1;
    Value retour;
    do
    {
        int status = snmp_synch_response(m_session, pdu, &response);
            if (status == STAT_SUCCESS) 
            {
                 if (response->errstat == SNMP_ERR_NOERROR) 
                {
                for(vars = response->variables; vars; vars = vars->next_variable) 
                {
                    if (vars->type == ASN_OCTET_STR) 
                    {
                        std::string str;
                        uint64_t toto={0};
                        retour.setPersonalType("ASN_OCTET_STR");
                        for(unsigned int i=0;i!=vars->val_len;++i)
                        {
                            str+=(vars->val.bitstring)[i];
                        }
                        std::string ret{reinterpret_cast<char*>(vars->val.string)};
                        if(ret!=str)
                        {
                            for(unsigned int i=0;i!=vars->val_len;++i)
                            {
                                std::bitset<8> p=(vars->val.bitstring)[i];
                                for(std::size_t o=0;o!=4;++o)
                                {
                                    std::bitset<1> dumb;
                                    dumb[0]=p[0];
                                    p[o]=p[7-o];
                                    p[7-o]=dumb[0];
                                }
                                toto|=p.to_ulong()<<8*i;
                            }
                            retour=toto;
                        }
                        else retour=str;
                    } 
                    else if(vars->type == ASN_BIT_STR)
                    {
                        std::string ret{reinterpret_cast<char*>(vars->val.string)};
                        ret.erase(vars->val_len);
                        retour.setPersonalType("ASN_BIT_STR");
                        std::cout<<"Toto"<<std::endl;
                        retour=ret;
                    }
                    else if (vars->type == ASN_OPAQUE_FLOAT)
                    {
                        retour.setPersonalType("ASN_OPAQUE_FLOAT");
                        retour=((float)*vars->val.floatVal);
                    }
                    else if (vars->type == ASN_OPAQUE_DOUBLE)  
                    {
                        retour.setPersonalType("ASN_OPAQUE_DOUBLE");
                        retour=((double)*vars->val.doubleVal);
                    }
                    else if (vars->type == ASN_INTEGER)
                    {
                        retour.setPersonalType("ASN_INTEGER");
                        retour=((int)*vars->val.integer);
                        
                    }
                    else if (vars->type == ASN_IPADDRESS)
                    {
                        retour.setPersonalType("ASN_IPADDRESS");
                        retour=((int)*vars->val.integer);
                    }
                }
                stop=true;
            }                       
            else 
            {
                std::cout<<"Error in packet\nReason: "<<snmp_errstring(response->errstat)<<"\n";
            }
        }
        else if (status == STAT_TIMEOUT) 
        {
            std::cout<<"Timeout: No Response from "<<m_IP<<" \n";
            stop=true;
        } 
        else 
        {                  
            snmp_sess_perror("snmpget",m_session);
            stop=true;
            throw -3;
        }    
    }
    while(stop==false);
    if (response!=nullptr) snmp_free_pdu(response);
    return Value(retour);
}

Value SNMPConnector::SendInfos(const std::string& command)
{
    std::size_t where=command.find("***VALUE***");
    if(where==std::string::npos)
    {
        std::string e="No key ***VALUE*** found in the command";
        std::cout<<e<<std::endl;
        throw e;
    }
    std::string com=command.substr(0,where);
    std::string value=command.substr(where+11);
    char type=findType(ReceiveInfos(com));
    netsnmp_pdu* pdu= snmp_pdu_create(SNMP_MSG_SET);
    netsnmp_pdu* response;
    netsnmp_variable_list *vars=nullptr;
    std::size_t name_length = MAX_OID_LEN;
    oid  name[MAX_OID_LEN];
    if (!snmp_parse_oid(com.c_str(),name, &name_length)) 
    {
        snmp_perror(com.c_str());
        throw -1;
    } 
    else if (snmp_add_var(pdu,name,name_length,type,value.c_str()))
    {
        snmp_perror(com.c_str());
        throw -1;
    }
    /*
    * do the request 
    */
    int status = snmp_synch_response(m_session, pdu, &response);
    if (status == STAT_SUCCESS) 
    {
        if (response->errstat == SNMP_ERR_NOERROR)
        {
           // for (vars = response->variables; vars;vars = vars->next_variable)print_variable(vars->name, vars->name_length, vars);
        } 
        else 
        {
            std::cout<<"Error in packet.\nReason: "<<snmp_errstring(response->errstat)<<"\n";
        }
    } 
    else if (status == STAT_TIMEOUT) 
    {
        std::cout<<"Timeout: No Response from "<<m_IP<<" \n";
    } 
    else 
    {                  
        snmp_sess_perror("snmpset",m_session);
    } 
    if (response) snmp_free_pdu(response);
    return ReceiveInfos(com);
}


char SNMPConnector::findType(Value value)
{
    if(value.getPersonalType()=="ASN_IPADDRESS") return 'a';
    else if (value.getPersonalType()=="ASN_INTEGER") return 'i';
    else if (value.getPersonalType()=="ASN_OPAQUE_DOUBLE") return 'D';
    else if (value.getPersonalType()=="ASN_OPAQUE_FLOAT") return 'F';
    else if (value.getPersonalType()=="ASN_OCTET_STR") return 's';
    else if (value.getPersonalType()=="ASN_BIT_STR") return 'b';
    else throw 3;
}

Value SNMPConnector::SendCommand(const std::string& command)
{   
    std::size_t where=command.find("***SEND***");
    if(where!=std::string::npos&&where==0)
    {
        std::string com=command;
        return SendInfos(com.erase(0,10));
    }
    else if(where=command.find("***RECEIVE***"),where!=std::string::npos&&where==0)
    {
        std::string com=command;
        return ReceiveInfos(com.erase(0,13));
    }
    else 
    {
        std::string e="No key ***SEND*** or ***RECEIVE*** found in the command";
        std::cout<<e<<std::endl;
        throw e;
    }
}
