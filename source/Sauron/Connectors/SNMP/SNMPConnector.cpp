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
#define WIN32_LEAN_AND_MEAN
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <vector>
#include <bitset>
#include "Kernel.h"

#ifdef _WIN32
#  define EXPORTIT __declspec( dllexport )
#else
#  define EXPORTIT
#endif

extern "C" EXPORTIT void register_pugg_plugin(pugg::Kernel* kernel)
{
	kernel->add_driver(new SNMPConnectorDriver());
}

SNMPConnector::SNMPConnector(const SNMPConnector& other):SNMPConnector()
{
    m_params=other.m_params;
}

SNMPConnector::SNMPConnector()
{
    m_name="SNMPConnector";
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

void SNMPConnector::setIP()
{
    if(m_params.hasParam("IP"))
    {
        m_IP=m_params["IP"].CString();
    }
    else
    {
        std::cout<<"You must provide the IP !\n";
        throw -1;
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


void SNMPConnector::initialize()
{
    setTimeout();
    setRetries();
    setVersion();
    setReadCommunity();
    setMIBFilename();
    setIP();
	init_snmp("");
    setMIBPath();
	init_mib();  
	if (!read_module(m_MIBFilename.c_str())) 
    {   
        std::cout<<"Unable to load SNMP MIB file "<<m_MIBFilename<<" !\n";
		throw false;
	}
	SOCK_STARTUP;
}

bool SNMPConnector::isConnected()
{
    if(m_session==nullptr) return false;
    else return true;
}

void SNMPConnector::release()
{

}

void SNMPConnector::disconnect()
{
    if(isConnected())
    {    
        snmp_close(m_session);
        m_session=nullptr;
        SOCK_CLEANUP;
        std::cout<<"SNMP session for host "<<m_IP<<" closed \n";
    }
}


void SNMPConnector::connect()
{
    if(!isConnected())
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
}

SNMPConnector& SNMPConnector::operator=(const SNMPConnector& other)
{
    m_name=other.m_name;
    m_params=other.m_params;
    m_session=new snmp_session(*other.m_session);
    return *this;
}

SNMPConnector& SNMPConnector::operator()(const SNMPConnector& other)
{
    m_name=other.m_name;
    m_params=other.m_params;
    m_session=new snmp_session(*other.m_session);
    return *this;
}

Value SNMPConnector::receiveInfos(std::vector<Value> command)
{
    const char* com=command[1].CString();
    netsnmp_pdu* pdu= snmp_pdu_create(SNMP_MSG_GET);
    netsnmp_pdu* response=nullptr;
    netsnmp_variable_list *vars=nullptr;
    std::size_t name_length = MAX_OID_LEN;
    oid name[MAX_OID_LEN];
    if (!snmp_parse_oid(com,name, &name_length)) 
    {
        snmp_perror(com);
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
                        bool imstring=true;
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
                                    static std::bitset<1> dumb;
                                    dumb[0]=p[o];
                                    p[o]=p[7-o];
                                    p[7-o]=dumb[0];
                                }
                                toto|=(p.to_ulong()<<8*(i));
                            }
                            retour=toto;
                        }
                        else
                        {
                            for ( int i = 0 ; i < ret.length(); i++)
                            {
                                if(!iscntrl(ret[i])&&!isprint(ret[i]))
                                {
                                    imstring=false;
                                    break;
                                }
                            }
                            if(imstring==true) retour=str;
                            else
                            {
                                for(unsigned int i=0;i!=vars->val_len;++i)
                                {
                                    std::bitset<8> p=(vars->val.bitstring)[i];
                                    for(std::size_t o=0;o!=4;++o)
                                    {
                                        static std::bitset<1> dumb;
                                        dumb[0]=p[o];
                                        p[o]=p[7-o];
                                        p[7-o]=dumb[0];
                                    }
                                    toto|=(p.to_ulong()<<8*(i));
                                }
                                retour=toto;
                            }
                            
                        }
                    } 
                    else if(vars->type == ASN_BIT_STR)
                    {
                        std::string ret{reinterpret_cast<char*>(vars->val.string)};
                        ret.erase(vars->val_len);
                        retour.setPersonalType("ASN_BIT_STR");
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

Value SNMPConnector::sendInfos(std::vector<Value> command)
{
    std::string com=command[1].String();
    std::string value=command[2].String();
    char type=findType(receiveInfos(command));
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
    return receiveInfos(command);
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

Value SNMPConnector::buildCommand(const std::vector<Value>& command)
{   
    if(command[0].String()=="SEND")
    {
        return sendInfos(command);
    }
    else if(command[0].String()=="RECEIVE")
    {
        return receiveInfos(command);
    }
    else 
    {
        std::string e="No key SEND or RECEIVE found in the command";
        std::cout<<e<<std::endl;
        throw e;
    }
}
