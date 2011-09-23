/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: configParam.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/notify.hh>

#include "configParam.hh"

#ifdef __BUILD_FOR_LINUX
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#endif

#include <icf2/sockIO.hh>

//
// Irouter Parameters
//
#define GLOBAL_SCOPE 0
int
get_ipv6_address(char *ifname,char* ip6[])
{
    /*
    char devname[128];
    sockaddr_in6 sockaddr;
    memset(&sockaddr,0,sizeof(sockaddr_in6));
    char addr6[128];
    memset(addr6,0,128);
    char addr6p[8][5];
    int plen, scope, dad_status, if_idx,i;
    FILE *f;

    i = 0;
    if ((f = fopen("/proc/net/if_inet6","r")) != NULL)
    {
        while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
                addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                addr6p[4], addr6p[5], addr6p[6], addr6p[7],
                &if_idx, &plen, &scope, &dad_status, devname) != EOF)
        {
//            ip6[i] = new char[128];
            if (!strcmp(devname, ifname) && scope == GLOBAL_SCOPE)
            {
                sprintf(addr6, "%s:%s:%s:%s:%s:%s:%s:%s",
                        addr6p[0], addr6p[1], addr6p[2], addr6p[3],
                        addr6p[4], addr6p[5], addr6p[6], addr6p[7]
                       );
                inet_pton(AF_INET6,addr6,&sockaddr.sin6_addr);
                inet_ntop(AF_INET6,(unsigned char *)&sockaddr.sin6_addr,ip6[i],128);
                i++;
            }
        }
    }
    ip6[i] = new char[1];
    ip6[i][0] = 0;
    fclose(f);
    */

    return 0;
}

irouterParam_t *
irouterParam_t::theInstance= NULL;

irouterParam_t *
irouterParam_t::getInstance(void)
{
    if ( ! theInstance)
    {
         theInstance = new irouterParam_t;
    }

    return theInstance;
}

irouterParam_t::irouterParam_t(void)
{
    family = AF_INET; //default family

    baseDataPort= 53020;
    lastNetPort = baseDataPort;

    gatewayMode = false;

    multicastMode = false;
    mcastBw  = 1536000;
    mcastTTL = 32;

    linkMgr= false;
    flowServerTgt= NULL;
    downStreamBw = 1536000;
    upStreamBw   = 1536000;

    //
    // SSM
    //
    mcastServerTgt = NULL;
    mcastLeader = false;
    stringArray_ref multicastSources;
    SSMFlag = false;


    flowList = new ql_t<string>;

    //! Default configuration
    configFlow("audio",         audioId);
//    configFlow("rtcpaudio",     rtcpaudioId);
    configFlow("vumeter",       vumeterId);
//    configFlow("rtcpvumeter",   rtcpvumeterId);
    configFlow("video",         videoId);
//    configFlow("rtcpVideo",     rtcpVideoId);
    configFlow("slidesFtp",     sldFtpId);
//    configFlow("rtcpslidesFtp", rtcpsldFtpId);
    configFlow("pointer",       pointerId);
//    configFlow("rtcppointer",   rtcppointerId);
    configFlow("shDisplay",     shDispId);
//    configFlow("rtcpShDisplay", rtcpShDispId);

    // Define clients, they are not activated...
    defineClient("audio"        , "51002", "51001");
//    defineClient("rtcpaudio"    , "51004", "51003");
    defineClient("shDisplay"    , "51006", "51005");
//    defineClient("rtcpShDisplay", "51008", "51007");
    defineClient("pointer"      , "51010", "51009");
//    defineClient("rtcppointer"  , "51012", "51011");
    defineClient("slidesFtp"    , "51014", "51013");
//    defineClient("rtcpslidesFtp", "51016", "51015");
    defineClient("video"        , "51018", "51017");
//    defineClient("rtcpVideo"    , "51020", "51019");
    defineClient("vumeter"      , "51022", "51021");
//    defineClient("rtcpvumeter"  , "51024", "51023");

    audio_n = audio_k = 0;
    video_n = video_k = 0;
    shdisp_n= shdisp_k= 0;
}

irouterParam_t::~irouterParam_t(void)
{
    delete flowList;

    if (flowServerTgt)
    {
        free(flowServerTgt);
    }

    //
    // MulticastSSM
    //
    if (mcastServerTgt)
    {
        free(mcastServerTgt);
    }
}

bool
irouterParam_t::configFlow(const char *fName, flowId_e flowId)
{
    int netPort;
    flowSpec_t *spec = flowSpecByName.lookUp(fName);

    if ( ! spec)
    {
        string flowName = string(fName);
        flowList->insert(flowName);
        netPort = lastNetPort + 1;
        lastNetPort += 2;
    }
    else
    {
        netPort= spec->netPort;
        delete spec;            //! Deleting old value
    }

    flowSpec_t *flowSpec = new flowSpec_t(netPort, flowId);
    NOTIFY("CONFIG FLOW fName=%s netPort=%d\n", fName, netPort);
    flowSpecByName.replace(fName, flowSpec);

    return true;
}



const ql_t<string> *
irouterParam_t::getFlowList(void) const
{
    return flowList;
}



const int
irouterParam_t::getNetPort(const char *fName)
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(fName);

    if ( ! flowSpec)
    {
        return -1;
    }

    return flowSpec->netPort;
}


bool
irouterParam_t::setBaseDataPort(int bdPort)
{
     baseDataPort= bdPort;
     lastNetPort = baseDataPort;

     // Actualizando los puertos de los flujos de datos
     for (ql_t<string>::iterator_t i = flowList->begin();
          i != flowList->end();
          i++
         )
     {
          string flowName = i;
          const char *flowStr = flowName.c_str();
          flowSpec_t *flowSpec = flowSpecByName.lookUp(flowStr);
          assert(flowSpec && "puff... las listas no son consistentes");
          flowSpec->netPort = lastNetPort + 1;
          lastNetPort += 2;
     }
     return true;
}


const int
irouterParam_t::getCtrlPort(void) const
{
     return baseDataPort;
}


const int
irouterParam_t::getBaseDataPort(void) const
{
     return baseDataPort;
}

//
// SSM Funciones para tratamiento Multicast SSM
//
void
irouterParam_t::connectMcastServer(char *mcastServerIP)
{
     mcastServerTgt=strdup(mcastServerIP);

}

const char *
irouterParam_t::getMcastServerTgt(void) const
{
    return mcastServerTgt;
}

char *
irouterParam_t::getmyip(void)
{
    char dest[128];
    memset(dest,0,128);
    int sfd;

    if (0 > (sfd = socket(AF_INET,SOCK_DGRAM,0)))
    {
       perror("socket()");
       exit(1);
    }

    /*
    struct ifconf conf;
    char buf[1024*5];
    conf.ifc_len = 1024*5;
    conf.ifc_buf = buf;

    ifreq * ifr;
    if (0 == ioctl(sfd,SIOCGIFCONF, &conf))
    {
        ifr = conf.ifc_req;
        for (unsigned int i = 0; i < conf.ifc_len/sizeof(ifreq); i++)
        {
            if (strcmp(ifr->ifr_name,"lo") != 0)
            {
                sockaddr *  sin= (sockaddr *)&ifr->ifr_broadaddr;
                char ip[128];
                char * ip2[5];
                for (int k = 0; k < 5; k++)
                {
                    ip2[k] = new char[128];
                }
                switch (getFamily())
                {
                case AF_INET6:
                    {
                    get_ipv6_address(ifr->ifr_name,ip2);
                    strcpy(dest,ip2[0]);
                    for (int k = 0; k < 5; k++)
                    {
                        delete ip2[k];
                    }
                    close(sfd);
                    return dest;
                    }
                case AF_INET:
                    {
                    sockaddr_in  * ip4 = (sockaddr_in*)sin;
                    inet_ntop(AF_INET,
                              (unsigned char *)&ip4->sin_addr.s_addr,
                              ip,
                              128
                             );
                    close(sfd);
                    return ip;
                    }
                default:
                    NOTIFY("unknown family [%d]\n",sin->sa_family);
                }
            }
            ifr++;
        }
    }
    else
    {
       NOTIFY("ioctl failed\n");
    }
    */

    return NULL;
}

void
irouterParam_t::setMcastLeader(char *addr)
{
    int sfd;

    if (0 > (sfd = socket(AF_INET,SOCK_DGRAM,0)))
    {
        perror("socket()");
        exit(1);
    }

/*
    struct ifconf conf;
    char buf[1024*5];
    conf.ifc_len = 1024*5;
    conf.ifc_buf = buf;

    ifreq * ifr;
    if (0 == ioctl(sfd,SIOCGIFCONF, &conf))
    {
        ifr = conf.ifc_req;
        for (unsigned int i = 0;i<conf.ifc_len/sizeof(ifreq);i++)
        {
            sockaddr *  sin= (sockaddr *)&ifr->ifr_broadaddr;
            char ip[128];
            char * ip2[5];
            for (int k = 0;k<5;k++)
            {
                ip2[k] = new char[128];
            }
            switch (sin->sa_family)
            {
            case AF_INET6:
                {
                    sockaddr_in6 * ip6 = (sockaddr_in6*)sin;
                    inet_ntop(AF_INET6,(unsigned char *)&ip6->sin6_addr,ip,128);
                    break;
                }
            case AF_INET:
                {
                    sockaddr_in  * ip4 = (sockaddr_in*)sin;
                    inet_ntop(AF_INET,
                              (unsigned char *)&ip4->sin_addr.s_addr,
                              ip,
                              128
                             );
                    break;
                }
            default:
                NOTIFY("unknown family [%d]\n",sin->sa_family);
            }
            get_ipv6_address(ifr->ifr_name,ip2);
            NOTIFY("len = %d :: [%s] ::  %s \n",
                   conf.ifc_len/sizeof(ifreq),
                   ifr->ifr_name,
                   ip
                  );
            int n = 0;
            if (getFamily() == AF_INET)
            {
NOTIFY("Comparing IPv4 %s with %s\n", addr, ip);
                if (strcmp(addr, ip) == 0)
                {
NOTIFY("Leader detected!\n");
                    mcastLeader= true;
                    break;
                }
            }
            else
            {
                while(ip2[n][0])
                {
NOTIFY ("Comparing IPv6 %s with %s\n", addr, ip2[n]);
                    if (strcmp(addr, ip2[n]) == 0)
                    {
NOTIFY("Leader detected!\n");
                        mcastLeader= true;
                        break;
                    }
                    n++;
                }
            }
            ifr++;
            for (int k = 0;k<5;k++)
            {
                delete ip2[k];
            }
        }
    }
    close(sfd);
    */
}

bool
irouterParam_t::getMcastLeader(void)
{
    return mcastLeader;
}

stringArray_ref
irouterParam_t::getSources(void)
{
    return multicastSources;
}

void
irouterParam_t::setMulticastSources(stringArray_ref sources)

{
    multicastSources= sources;
}

void
irouterParam_t::setSSM(bool state)
{
    SSMFlag = state;
    return;
}

bool
irouterParam_t::getSSM(void)
{
    return SSMFlag;
}


//
// GATEWAY PARAMETERS
//

void
irouterParam_t::setGatewayMode(bool mode)
{
    gatewayMode= mode;
}

bool
irouterParam_t::getGatewayMode(void) const
{
    return gatewayMode;
}

//
// MULTICAST PARAMETERS
//

u8
irouterParam_t::getMulticastTTL(void) const
{
    return mcastTTL;
}


bool
irouterParam_t::setMulticastTTL(u8 mcTTL)
{
     mcastTTL= mcTTL;
     return true;
}


const char *
irouterParam_t::getMcastGroup(const char *fName)
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(fName);

    if ( ! flowSpec)
    {
        return NULL;
    }

    return flowSpec->mcastGroup;
}

bool
irouterParam_t::configFlowMcastGroup(const char *fName,
                                     const char *mcastGr)
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(fName);

    if ( ! flowSpec)
    {
        return false;
    }

    if (flowSpec->mcastGroup)
    {
        free(flowSpec->mcastGroup);
    }

    flowSpec->mcastGroup=strdup(mcastGr);

    NOTIFY("CONFIG MCAST FLOW fName=%s mcastGroup=%d\n",
           fName,
           flowSpec->mcastGroup
          );

    return true;
}

void
irouterParam_t::setMulticastMode(bool mcastMode)
{
    multicastMode = mcastMode;
}


bool
irouterParam_t::getMulticastMode(void) const
{
    return multicastMode;
}


void
irouterParam_t::setMulticastBw(int mBw)
{
    mcastBw = mBw;
}

int
irouterParam_t::getMulticastBw(void) const
{
    return mcastBw;
}



//
// CONNECTION FROM/TO OTHER FLOWSERVERS
//

void
irouterParam_t::setLinkMgr(bool lMgr)
{
    linkMgr= lMgr;
}

bool
irouterParam_t::getLinkMgr(void) const
{
    return linkMgr;
}

void
irouterParam_t::connectFlowServer(char *flowServerIP)
{
    if (flowServerTgt)
    {
        free(flowServerTgt);
    }

    flowServerTgt= strdup(flowServerIP);
}

const char *
irouterParam_t::getFlowServerTgt(void) const
{
    return flowServerTgt;
}


int
irouterParam_t::getDownStreamBw(void) const
{
    return downStreamBw;
}

int
irouterParam_t::getUpStreamBw(void) const
{
    return upStreamBw;
}

void
irouterParam_t::setDownStreamBw(int dStreamBw)
{
    downStreamBw = dStreamBw;
}

void
irouterParam_t::setUpStreamBw(int uStreamBw)
{
    upStreamBw = uStreamBw;
}


bool
irouterParam_t::defineClient(const char *flowName,
                             const char *iPort,
                             const char *cPort
                            )
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(flowName);

    if ( ! flowSpec)
    {
        NOTIFY("defineClient:: The flow=%s does not exit...\n", flowName);
        return false;
    }

    flowSpec->createLocalClient= false;
    if (flowSpec->irouterPort)
    {
        free(flowSpec->irouterPort);
    }
    flowSpec->irouterPort= strdup(iPort);

    if (flowSpec->componentPort)
    {
        free(flowSpec->componentPort);
    }
    flowSpec->componentPort= strdup(cPort);

    return true;
}


bool
irouterParam_t::activateClient(const char *flowName)
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(flowName);

    if ( ! flowSpec)
    {
        NOTIFY("activateClient:: The flow=%s does not exit...\n", flowName);
        return false;
    }

    if ( ! flowSpec->irouterPort || ! flowSpec->componentPort)
    {
        NOTIFY("activateClient:: Not configured client ports...\n");
        return false;
    }

    flowSpec->createLocalClient= true;

    return true;
}

const char *
irouterParam_t::getIrouterFlowPort(const char *flowName)
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(flowName);

    if ( ! flowSpec)
    {
        NOTIFY("irouterParam_t::getIrouterFlowPort: "
               "flow=%s does not exit...\n",
               flowName
              );
        return NULL;
    }

    return flowSpec->irouterPort;
}

const char *
irouterParam_t::getComponentFlowPort(const char *flowName)
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(flowName);

    if ( ! flowSpec)
    {
        NOTIFY("irouterParam_t::getComponentFlowPort: "
               "flow=%s does not exit...\n",
               flowName
              );
        return NULL;
    }

    return flowSpec->componentPort;
}


bool
irouterParam_t::createLocalClient(const char *flowName) const
{
    flowSpec_t *flowSpec = flowSpecByName.lookUp(flowName);

    if ( ! flowSpec)
    {
        NOTIFY("irouterParam_t::createLocalClient: "
               "flow=%s does not exit...\n",
               flowName
              );
        return false;
    }

    return flowSpec->createLocalClient;
}


void
irouterParam_t::setFlowserverMode(bool fsMode)
{
     flowserverMode= fsMode;
}

bool
irouterParam_t::getFlowserverMode(void) const
{
    return flowserverMode;
}

