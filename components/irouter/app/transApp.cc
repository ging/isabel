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
////////////////////////////////////////////////////////////////////////
//
// $Id: transApp.cc 22241 2011-04-08 12:30:49Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/dictionary.hh>
#include <icf2/sockIO.hh>
#include <icf2/strOps.hh>

#include "ctrlProtocol.hh"
#include "linkProtocol.hh"
#include "transApp.hh"
#include "configFile.hh"
#include "webirouterConfig.hh"
#include "flow.hh"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

//
// chapuza impresentable
//
link_t *defaultMulticastLink= NULL;
link_t *defaultLocalLink= NULL;

//
//! irouter usage
//
/*
    Command line irouter usage
*/
void irouter_usage();



//
//! irouter version
//
/*
    Return irouter version and exit program.
*/
void irouter_version();


//
// mcast groups
//
//dictionary<string, char *> groupDictionary(256);

// -----------------------------------------------------------------
// Opciones de arranque
// Opcion -ASMcast  : Comunicacion ASM
// Opcion -SSMcast  : Comunicacion SSM
// Opcion -flowServer : Me conecto a FS
//
//
// -----------------------------------------------------------------
// Opciones de ISABEL
// FS:           Yo soy la maquina FS siempre y atiendo peticiones de
//               conexion de otras maquinas
//               Si yo no soy el FS : Se arranca con -flowServer para
//                                    conectarme al FS.
// MCAST:        Envia a la nube multicast
//
// -----------------------------------------------------------------



//
// Irouter statistics
//


bool
transApp_t::statsInfo_t::setGlobalStats(char *grName)
{
    if (groupName)     free(groupName);
    groupName = strdup(grName);

    if ((!globalStats) && (!localStats))
    {
         globalStats = true;
         return true;
    }
    globalStats=true;
    return false;
}

bool
transApp_t::statsInfo_t::setLocalStats(void)
{
    if ((!globalStats) && (!localStats))
    {
       localStats=true;
       return true;
    }
    localStats=true;
    return false;
}


bool
transApp_t::statsInfo_t::removeLocalStats(void)
{
    localStats=false;
    if (!globalStats)
       return true;
    return false;
}

bool
transApp_t::statsInfo_t::removeGlobalStats(void)
{
    globalStats=false;
    if (groupName) {
        free(groupName);
        groupName=NULL;
    }
    if (!localStats)
       return true;
    return false;
}

bool
transApp_t::statsInfo_t::getLocalStats(void)
{
    return localStats;
}


bool
transApp_t::statsInfo_t::getGlobalStats(void)
{
    return globalStats;
}



//
// Irouter scheduler
//

int
transApp_t::getBwMultiplier(char *trailer)
{
    int bandWidthMultiplier=1;

    switch (*trailer)
    {
    case 'M':
    case 'm':
        bandWidthMultiplier= 1000000;
        *trailer= 0;

        break;

    case 'K':
    case 'k':
        bandWidthMultiplier= 1000;
        *trailer= 0;

        break;

    default:
        if( ! isdigit(*trailer))
        {
            NOTIFY("transApp_t::getBwMultiplier: "
                   "Wrong bandwidth specification"
                  );
            abort();
        }
    }

    return bandWidthMultiplier;
}


transApp_t::transApp_t(int &argc, argv_t &argv)
: application_t(argc, argv),
  statsPort("51000"),
  centralSwitch(this)
{
    dictionary_t<string, string> mcastGroupDict;

    // irouter initial config information
    irouterParam = irouterParam_t::getInstance();

    linkBinder = new linkBinder_t(this);
    targetMgr  = new targetMgr_t(this);

    bool  echoMode  = false;

    bool  activateAllClients = false;

    bool  daemonize= false;

    serverSock= NULL;

    int flowId=10001;

    // this link represent the local client, if exists
    linkClass_t linkClass("noWay");
    defaultLocalLink = new link_t(this, linkClass, false, 0);

#if 0
    short defPT= PayloadTypeForCodec("none-16KHz");
    if (defPT == -1)
    {
        NOTIFY("irouter_t:: audio codec \"none-16KHz\" required\n");
        NOTIFY("irouter_t:: bailing out\n");
        exit (-1);
    }
#endif

    audioMixer2 = new AudioMixer2_t(NONE_16_FORMAT);
    insertTask(static_cast<AudioMixer2_t*>(audioMixer2));

    bool doStatsF= true;

    const char *GWaddr= "::ffff:127.0.0.1";

    enum myOptions {
        optHelp,

        optGateway,     // Gateway

        optASMcast,     // ASM
        optSSMcast,     // SSM

        optUpStreamBW,  // up stream bandwidth (send)
        optDownStreamBW,// down stream bandwidth (receive)
        optMcastBW,     // up stream mcast bandwidth (send)

        optClient,      // flow to local client
        optAllClients,  // all clients

        optAudioMixer,  // audio mixer

        optTtl,         // Ttl for multicast
        optMcastGroup,  // groups for multicast

        optShell,       // control
        optCport,       // control

        optDaemonize,   // if set, the program forks and go daemon

        optLinkMgr,     // linkMgr
        optFlowServer,  // flow server

        optDoStats,     // statistics

        optBaseDataPort,

        optEcho,

        optVersion,

        optConfigFile,

        optStatsPort,
        optStatsHost
    };

    optionDefList_t opt;
    appParamList_t *parList;

    opt
        << new optionDef_t("h", optHelp)

        << new optionDef_t("@GW",      optGateway)
        << new optionDef_t("@gateway", optGateway)

        << new optionDef_t("ASMcast", optASMcast)
        << new optionDef_t("@SSMcast", optSSMcast)

        << new optionDef_t("echo", optEcho)
        << new optionDef_t("@upStreamBw", optUpStreamBW)
        << new optionDef_t("@upStreamBandWidth", optUpStreamBW)
        << new optionDef_t("@downStreamBw", optDownStreamBW)
        << new optionDef_t("@downStreamBandWidth", optDownStreamBW)
        << new optionDef_t("@mcastBw", optMcastBW)

        << new optionDef_t("@client", optClient)
        << new optionDef_t("@Client", optClient)
        << new optionDef_t("allclients", optAllClients)
        << new optionDef_t("allClients", optAllClients)

        << new optionDef_t("@audioMixer", optAudioMixer)

        << new optionDef_t("shell", optShell)
        << new optionDef_t("@cport", optCport)

        << new optionDef_t("daemonize", optDaemonize)

        << new optionDef_t("@ttl", optTtl)
        << new optionDef_t("@mcastGroup", optMcastGroup)

        << new optionDef_t("@linkMgr",    optLinkMgr)
        << new optionDef_t("@flowServer", optFlowServer)

        << new optionDef_t("@bdport", optBaseDataPort)

        << new optionDef_t("doStats", optDoStats)

        << new optionDef_t("version", optVersion)
        << new optionDef_t("v",       optVersion)

        << new optionDef_t("@configFile", optConfigFile)
        << new optionDef_t("@configfile", optConfigFile)
        << new optionDef_t("@cFile",      optConfigFile)
        << new optionDef_t("@cfile",      optConfigFile)

        << new optionDef_t("@statsPort", optStatsPort);


    parList= getOpt(opt, argc, argv);


    //
    // now parse options
    //
    for( ; parList->len(); parList->behead())
    {
        switch(parList->head()->parId)
        {
        case optHelp:
            irouter_usage();
            exit(0);
            break;

        case optGateway: // Gateway

            irouterParam->setGatewayMode(true);
            GWaddr= parList->head()->parValue;

            break;

        case optASMcast: // ASM

            irouterParam->setMulticastMode(true);
            NOTIFY("ASMcast\n");

            break;

        case optSSMcast:

            if (strstr(parList->head()->parValue,":") != 0)
            {
                irouterParam->setFamily(AF_INET6);
            }
            irouterParam->setMulticastMode(true);
            irouterParam->setSSM(true);
            irouterParam->connectMcastServer(parList->head()->parValue);//Ip de mcast
            irouterParam->setMcastLeader(parList->head()->parValue); //Padre o hijo
            NOTIFY("SSMcast %s :: i'm %s, family = %d\n",
                   parList->head()->parValue,
                   irouterParam->getMcastLeader()?"FATHER":"SON",
                   irouterParam->getFamily()
                  );
            break;

        case optEcho:  // solo para pruebas, cuidadin...
            echoMode= true;
            break;

        case optUpStreamBW:
            {
                char *parameter= parList->head()->parValue;
                char *trailer  = parameter + (strlen(parameter) - 1);

                int bandWidthMultiplier= getBwMultiplier(trailer);

                int upStreamBandWidth= atoi(parameter)*bandWidthMultiplier;

                irouterParam->setUpStreamBw(upStreamBandWidth);
            } break;

        case optDownStreamBW:
            {
                char *parameter = parList->head()->parValue;
                char *trailer   = parameter + (strlen(parameter) - 1);

                int bandWidthMultiplier= getBwMultiplier(trailer);

                int downStreamBandWidth= atoi(parameter)*bandWidthMultiplier;

                irouterParam->setDownStreamBw(downStreamBandWidth);
            } break;

        case optMcastBW:
            {
                char *parameter= parList->head()->parValue;
                char *trailer  = parameter + (strlen(parameter) - 1);

                int  bandWidthMultiplier= getBwMultiplier(trailer);

                int mcastBandWidth= atoi(parameter)*bandWidthMultiplier;

                irouterParam->setMulticastBw(mcastBandWidth);
            } break;

        case optClient:
            irouterParam->activateClient(parList->head()->parValue);
            break;

        case optAllClients:
            activateAllClients =true;
            break;

        case optAudioMixer:
            {
                char *codecName= parList->head()->parValue;
                unsigned int codecFmt= aGetFormatIdByName(codecName);
                if (codecFmt < 0)
                {
                    NOTIFY("No such audio codec name [%s]\n", codecName);
                    NOTIFY("bailing out\n");
                    abort();
                }
                setAudioMixerMode(codecFmt);
                break;
            }
        case optShell:
            {
                io_ref io = new shellIO_t(0, 1);
                this->insertTask(new webirouter_Interface(io));
            } break;

        case optCport:
            {
                inetAddr_t addr("127.0.0.1", parList->head()->parValue);
                serverSock = new streamSocket_t(addr);
                if (serverSock->listen(addr) < 0)
                {
                    NOTIFY("Problems when calling listen() for control socket");
                    abort();
                }

                *this<< new tcpServer_t<webirouter_Interface>(*serverSock);

            } break;

        case optDaemonize:
            daemonize= true;
            break;

        case optBaseDataPort:
            {
                // puerto base para el irouter
                // el primer puerto para el control interno del
                // irouter, los siguientes para el transporte de
                // flujos multimedia

                irouterParam->setBaseDataPort(
                    atoi(parList->head()->parValue));
            } break;

        case optTtl:
            irouterParam->setMulticastTTL(atoi(parList->head()->parValue));
            break;

        case optMcastGroup:
            {
                char b1[2048];
                char b2[2048];

                int n= sscanf(parList->head()->parValue,
                              "%[a-zA-Z]=%s", b1, b2);

                if(n< 2)
                {
                    NOTIFY("transApp_t::bad option syntax: "
                           "use -mcastGroup flowName=group\n"
                          );
                    abort();
                }

                if (!irouterParam->configFlowMcastGroup(b1, b2))
                {
                    NOTIFY("Flow type has not been "
                           "previously registered... \n"
                           "Please, use configFile to define and to "
                           "configure this flow type\n"
                          );
                    abort();
                }

            } break;

        case optLinkMgr:
            irouterParam->setLinkMgr(true);
            break;

        case optFlowServer:
            irouterParam->setFlowserverMode(true);
            irouterParam->connectFlowServer(parList->head()->parValue);
            break;

        case optDoStats:
            doStatsF= true;
            break;

        case optVersion:
            irouter_version();
            exit(0);

        case optStatsPort:
            statsPort= strdup(parList->head()->parValue);
            break;

        case optConfigFile:
            {
            /*
                char *fname=parList->head()->parValue;
                confIrouter_Interface_t interface(this, fname);
                if (!interface.parseFile())
                {
                     NOTIFY("Problems when loading irouter "
                            "config file\n",
                           );
                     abort();
                }
            */

            } break;

        default:
            NOTIFY("Invalid command line option, irouter usage error\n");
            irouter_usage();

            exit(-1);
        }
    }

    if (serverSock)
    {
        if (daemonize)
        {
            goDaemon();
        }
    }

    //
    //! Define flows
    //

    const ql_t<string> *fList = irouterParam->getFlowList();
    ql_t<string>::iterator_t i;
    for (i = fList->begin(); i != fList->end(); i++)
    {
         string flowName = i;
         const char *flowStr= flowName.c_str();
         flowId = irouterParam->getFlowIdByName(flowStr);
         define_flow(flowStr, flowId);

         //flowId+=10000;
    }


    //
    // Init fec id dict
    //
    int fecId   = 45;

    for (i = fList->begin(); i != fList->end(); i++)
    {
        string flowName = i;
        const char *flowStr=flowName.c_str();
        flowId = irouterParam->getFlowIdByName(flowStr);

        fecById->insert(flowId,fecId);
        fecId++;
    }
    //
    //! Active clients
    //

    if (activateAllClients)
    {
        const ql_t<string> *flowList = irouterParam->getFlowList();
        for (i = flowList->begin(); i != flowList->end(); i++)
        {
            string client = i;
            const char *flowStr = client.c_str();
            irouterParam->activateClient(flowStr);
        }
    }



    // para no depender del arranque de Santiago

    if(getenv("IROUTER_DEBUG"))
    {
        char fname[1024];
#ifdef __BUILD_FOR_LINUX
        sprintf(fname, "/usr/tmp/irouter:pid=%d", getpid());
#endif
#ifdef WIN32
        sprintf(fname, "irouter_pid_%d", GetCurrentProcessId());
#endif
        item_t::debugLevel= 666;
        item_t::setDebugLevel(item_t::debugLevel);

        if(item_t::debugFile>0)
        {
           fclose(item_t::debugFile);
        };

        item_t::debugFile = fopen(fname, "w");
    }


    //
    // connect flowServer:
    //     theHarbinger implements irouter connection protocol.
    //     Sends to the FlowServer connection packets peridically
    //     and informs the FlowServer the reception bw
    //

    //
    // SSM Consultamos la existencia del McastServerTgt (se ha activado con la opcion anterior)
    // A continuacion inicializamos el mcastHarbinger solo si NO somos el padre multicast.

    const char *mcastServerTgt=irouterParam->getMcastServerTgt();
    bool mcastLeader=irouterParam->getMcastLeader();
    if(mcastServerTgt && !mcastLeader)
    {
        theMcastHarbinger=
            new mcastLinkHarbinger_t(this,
                                     mcastServerTgt,
                                     irouterParam->getMulticastBw(),
                                     echoMode,
                                     0,
                                     0
                                    );
        *this<< theMcastHarbinger;
    }

    const char *flowServerTgt=irouterParam->getFlowServerTgt();
    if (flowServerTgt)
    {
        theHarbinger= new linkHarbinger_t(this,
                                          flowServerTgt,
                                          irouterParam->getDownStreamBw(),
                                          echoMode,
                                          0,
                                          0
                                         );
        *this<< theHarbinger;
    }

    //
    // linkControl_t task, server task to wait for flowserver
    // connection requests, and anwers... must be always up
    //
    *this<< new linkControl_t(this, irouterParam->getLinkMgr());


    //
    // targetMgr
    //
    *this << static_cast<simpleTask_t *>(targetMgr);

    //
    // link for multicast
    //
    if (irouterParam->getMulticastMode())
    {
        linkClass_t linkClass("inputOutput");
        defaultMulticastLink= define_link("mcastDefault",
                                          linkClass,
                                          false,
                                          irouterParam->getMulticastBw()
                                         );
    }

    // define local clients

    const ql_t<string> *flowList = irouterParam->getFlowList();
    for (i = flowList->begin(); i != flowList->end(); i++)
    {
        string flowName = i;
        const char *flowStr = flowName.c_str();

        if (irouterParam->createLocalClient(flowStr))
        {
            define_client(flowStr,
                          irouterParam->getIrouterFlowPort(flowStr),
                          irouterParam->getComponentFlowPort(flowStr)
                         );
        }
    }

    if (irouterParam->getMulticastMode())
    {
        const ql_t<string> *flowList = irouterParam->getFlowList();
        for (ql_t<string>::iterator_t i = flowList->begin();
             i != flowList->end();
             i++
            )
        {
            string flowName = i;
            const char *flowStr = flowName.c_str();
            flow_t *flow  = flowBinder.lookUp(flowStr);
            assert(flow && "Que estoy configurando????");
            add_target("mcastDefault",
                       flow->flowId,
                       irouterParam->getMcastGroup(flowStr),
                       irouterParam->getNetPort(flowStr),
                       0, 0,
                       irouterParam->getMulticastTTL()
                      );
        }
    }

    if(irouterParam->getGatewayMode())
    {
        NOTIFY("Gateway ON, peer %s!\n", GWaddr);

        link_t *l= define_gateway(GWaddr);
        if (l == NULL)
        {
            NOTIFY("transApp_t::transApp_t: unable to create gateway link "
                   "for host %s, bailing out\n",
                   GWaddr
                  );
            abort();
        }
    }

    NOTIFY("transApp_t::transApp_t: irouterStatsGatherer statsPort=%s\n",
           statsPort
          );

    theStatsGatherer = new irouterStatsGatherer_t(statsPort);
    irouterStatsGatherer_t *isgt= theStatsGatherer;

    (*this) << isgt;

    theStatsGatherer->doStats(doStatsF);

    //debugMsg((debugMask_e)0, "transApp_t", "Muriendo...\n");
    //*this << new dieHard_t(120);
}


//
// define_flow
//     Define a flow and a downsStreamInfo for this flow
//

void
transApp_t::define_flow(const char *flowName, const u32 flowId)
{
    debugMsg( dbg_App_Verbose,
              "define_flow",
              "flowName=%s flowId=%u",
              flowName,
              flowId
            );

    if(!flowBinder.newFlow(flowName, (flowId_e)flowId))
    {
        NOTIFY("transApp_t::define_flow: error: cannot register flow %d\n",
               flowId
              );
        return;
    }

    downstreamRoutingInfo_t *dri= new downstreamRoutingInfo_t();
    centralSwitch.insertDri(flowId, dri);
}


//
// define a gateway
// accepts flows from "peerHost", but flows
// are NOT sent to it
// (thus, an inputOnly link)
//
link_t*
transApp_t::define_gateway(const char *peerHost)
{
    link_t *l= linkBinder->newLink(peerHost,
                                   link_t::inputOnly,
                                   false, // echo
                                   0,     // bandwidth
                                   NORMAL
                                  );

    add_target_for_all_flows(peerHost, peerHost, 0, 0);

    return l;
}


//
// define_link
//     Define a new link
//
link_t *
transApp_t::define_link(const char *linkName,
                        linkClass_t &linkClass,
                        const bool echoBool,
                        const int bw
                       )
{
    debugMsg(dbg_App_Normal,
             "define_link",
             "linkName=%s bw=%d",
             linkName,
             bw
            );

    return linkBinder->newLink(linkName, linkClass, echoBool, bw);
}


//
// delete_link
//     Delete a link
//

void
transApp_t::delete_link(const char *linkName)
{
    debugMsg(dbg_App_Verbose,
             "delete_link",
             "linkName=%s",
             linkName
            );

    link_t  *l= linkBinder->lookUp(linkName);
    if ( ! l)
    {
        NOTIFY("transApp_t::delete_link:: error, undefined link=[%s]\n",
               linkName);
        return;
    }

    ql_t<flowId_e> *flowList = flowBinder.getFlowList();
    for (ql_t<flowId_e>::iterator_t i = flowList->begin();
                                    i != flowList->end();
                                    i++)
    {
        flowId_e flowId = static_cast<flowId_e>(i);
        centralSwitch.removeLinkFromDri(flowId, l);
    }

    linkBinder->deleteLink(linkName);
    delete flowList;
}

//
// define_client
//     Define a new client for a flowName, if no uriFlow,
//     creates a new one
//

void
transApp_t::define_client(const char *flowName,
                          const char *lPort,
                          const char *rPort
                         )
{
    debugMsg(dbg_App_Verbose,
             "define_client",
             "flowName=%s lPort=%s rPort=%s",
             flowName,
             lPort,
             rPort
            );

    flow_t *m= flowBinder.lookUp(flowName);

    if(!m) {
        NOTIFY("transApp_t::define_client:: unknown flow=[%s]\n", flowName);
        return ;
    }

    clientModule_t *newClient= clientBinder.newClient(m, lPort, rPort);

    upstreamRoutingInfo_t *uriFlow = centralSwitch.getUri(m->flowId);

    if (!uriFlow) {
        uriFlow    = new upstreamRoutingInfo_t;
        centralSwitch.insertUri(m->flowId, uriFlow);
    }
    uriFlow->clients.insert(newClient);

    insertTask(newClient);
}


void
transApp_t::add_target_for_all_flows(const char *linkName,
                                     const char *usrTgt,
                                     int n,
                                     int k
                                    )
{
    u8 mcastTTL = irouterParam->getMulticastTTL();
    debugMsg(dbg_App_Verbose,
             "add_target_for_all_flows",
             "linkName=%s usrTgt=%s mcastTTl=%u",
             linkName,
             usrTgt,
             mcastTTL
            );

    const ql_t<string> *flowList= irouterParam->getFlowList();

    if ( ! flowList)
    {
       NOTIFY("transApp_t::add_target_for_all_flows: "
              "problems getting flow List... empty!\n");
       NOTIFY("transApp_t::add_target_for_all_flows: Bailing out\n");
       exit(-1);
    }

    for (ql_t<string>::iterator_t i = flowList->begin();
         i != flowList->end();
         i++
        )
    {
         string flowName = i;

         if ( flowName.empty())
         {
              NOTIFY("transApp_t::add_target_for_all_flows: "
                     "problems with the list\n"
                    );
              NOTIFY("transApp_t::add_target_for_all_flows: bailing out\n");
              exit(-1);
         }

         const char *flowStr= flowName.c_str();

         flow_t *flow= flowBinder.lookUp(flowStr);
         int netPort = irouterParam->getNetPort(flowStr);
         debugMsg(dbg_App_Paranoic,
                  "add_target_for_all_flows",
                  "Adding flow=%s netPort=%d\n",
                  flowStr,
                  netPort
                 );

         // choose n,k for FEC
         int global_n= 0, global_k= 0;
         if (strcmp(flowStr, "audio") == 0)
         {
             irouterParam->getAudioParity(global_n, global_k);
         }
         if (strcmp(flowStr, "video") == 0)
         {
             irouterParam->getVideoParity(global_n, global_k);
         }
         if (strcmp(flowStr, "shDisplay") == 0)
         {
             irouterParam->getShDispParity(global_n, global_k);
         }
         n= global_n > n ? global_n : n;
         k= global_k > k ? global_k : k;

         add_target(linkName, flow->flowId, usrTgt, netPort, n, k, mcastTTL);
    }

    linkList.insert(string(linkName));
}


void
transApp_t::add_target(const char *linkName,
                       const flowId_e flowId,
                       const char *usrTgt,
                       const int lPort,
                       int n,
                       int k,
                       const i8 mcastTTL
                      )
{
    NOTIFY("transApp_t::add_target: "
           "linkName=%s flowId=%s usrTgt=%s lPort=%d "
           "protection=(%d, %d), mcastTTl=%u\n",
           linkName,
           getFlowStr(flowId),
           usrTgt,
           lPort,
           n, k,
           mcastTTL
          );

    link_t  *l= linkBinder->lookUp(linkName);
    if ( ! l)
    {
        NOTIFY("transApp_t::add_target: error undefined link=[%s]\n", linkName);
        return;
    }

    l->addTarget(flowId, usrTgt, lPort, n, k, mcastTTL);

    centralSwitch.addLinkToDri(flowId, l);
}


void
transApp_t::getLinks(char *retVal)
{
    char buf[4096];

    memset(retVal, 0, sizeof(retVal));
    strcat(retVal,"{");

    for (ql_t<string>::iterator_t i = linkList.begin();
         i != linkList.end();
         i++
        )
    {
        string aux= i;

        if (i == linkList.begin())
        {
            sprintf(buf, "%s", aux.c_str());
        }
        else
        {
            sprintf(buf, ",%s", aux.c_str());
        }
        strcat(retVal, buf);
    }
    strcat(retVal,"}\n");
}


void
transApp_t::getLinksFecParams(char *retVal)
{
    char buf[4096];

    int n, k;
    flowId_e flowId;

    retVal[0]= '\0';
    strcat(retVal,"{");

    ql_t<string> *linkNames= linkBinder->getKeys();
    ql_t<link_t*> *linkList= linkBinder->getValues();

    ql_t<string>::iterator_t itKey = linkNames->begin();
    ql_t<link_t*>::iterator_t itVal = linkList->begin();

    for (   ;
         itKey != linkNames->end() && itVal != linkList->end();
         itKey++, itVal++
        )
    {
        string theLinkName= itKey;
        link_t *theLink= static_cast<link_t*>(itVal);

        targetList_t *tgtList= theLink->getTargets();

        for (targetList_t::iterator_t itTgt= tgtList->begin();
             itTgt != tgtList->end();
             itTgt++
            )
        {
            target_t *theTgt= static_cast<target_t*>(itTgt);

            theTgt->getFecParams(flowId, n, k);

            sprintf(buf,
                    "{%s,%s,%d,%d,%s},",
                    theLinkName.c_str(),
                    k == 0 ? "None" : "Parity", // REVISAR
                    n,
                    k,
                    getFlowStr(flowId)
                   );
            strcat(retVal, buf);
        }
    }

    int len= strlen(retVal);
    if (len > 1)
        retVal[len-1]= '\0'; // removes last ','

    strcat(retVal,"}\n");
}


void
transApp_t::getLinksStats(char *retVal)
{
    theStatsGatherer->getLinksStats(retVal);
}


const char *
transApp_t::protectFlow(const char *linkName,
                        const char *flowName,
                        const char *fecType,
                        int n,
                        int k
                       )
{
    ql_t<string> auxLinkList;
    ql_t<flow_t*> flowList;

    if ( strcmp(linkName, "ALL") == 0)
    {
        for (ql_t<string>::iterator_t i = linkList.begin();
             i != linkList.end();
             i++
            )
        {
            string aux= i;

            auxLinkList.insert(aux);
        }
    }
    else
    {
        auxLinkList.insert(string(linkName));
    }

    if ( strcmp(flowName, "ALL") == 0)
    {
        flowList.insert(flowBinder.lookUp("audio"))
                .insert(flowBinder.lookUp("video"))
                .insert(flowBinder.lookUp("shDisplay"));
    }
    else
    {
        flow_t *flow= flowBinder.lookUp(flowName);
        if ( ! flow)
        {
            return "Error: Bad flow\n";
        }
        else
        {
            flowList.insert(flow);
        }
    }

    for (ql_t<string>::iterator_t i= auxLinkList.begin();
         i != auxLinkList.end();
         i++
        )
    {
        string aux= i;
        link_t *link= linkBinder->lookUp(aux.c_str());
        if ( ! link)
        {
            return "Error: Bad link\n";
        }

        for (ql_t<flow_t*>::iterator_t j = flowList.begin();
             j != flowList.end();
             j++
            )
        {
            flow_t *flow= static_cast<flow_t*>(j);
            link->protectFlow(aux.c_str(),
                              flow->flowId,
                              fecType,
                              n,
                              k
                             );
        }
    }

    // cache protection info for future targets, only if "ALL" targets
    if (strcmp(linkName, "ALL") == 0)
    {
        if (strcmp(flowName, "ALL") == 0)
        {
            irouterParam->setAudioParity(n, k);
            irouterParam->setVideoParity(n, k);
            irouterParam->setShDispParity(n, k);
        }
        if (strcmp(flowName, "audio") == 0)
        {
            irouterParam->setAudioParity(n, k);
        }
        if (strcmp(flowName, "video") == 0)
        {
            irouterParam->setVideoParity(n, k);
        }
        if (strcmp(flowName, "shDisplay") == 0)
        {
            irouterParam->setShDispParity(n, k);
        }
    }

    return "Ok\n";
}

const char *
transApp_t::videoTranscode(u32 SSRC, char *dest, u32 BW, u32 lineFmt)
{
    ql_t<link_t *> *linkList = linkBinder->getValues();

    while (linkList->len())
    {
        link_t *link = linkList->head();
        if (link->videoTranscode(SSRC, dest, BW, lineFmt))
        {
            delete linkList;
            return "OK\n";
        }
        linkList->behead();
    }
    delete linkList;

    return "Error: target video not found\n";
}

int
transApp_t::joinSSM(char *source)
{
    defaultMulticastLink->joinSSM(source);

    return 0;
}

int
transApp_t::leaveSSM(char *source)
{
    defaultMulticastLink->leaveSSM(source);

    return 0;
}

bool
transApp_t::setAudioMixerMode(unsigned int mixerFmt)
{
    audioMixer2->setCoder(mixerFmt);
    audioMixer2->setActive(true);
    return true;
}


void
transApp_t::unsetAudioMixerMode(void)
{
    audioMixer2->setActive(false);
}

void
transApp_t::goDaemon(void)
{
#ifdef __BUILD_FOR_LINUX

    if ( ! serverSock)
    {
         NOTIFY("transApp_t::goDaemon:: problems opening control socket\n");
         return;
    }

    struct sockaddr_in sin;
    int len= sizeof(sin);

    getsockname(serverSock->sysHandle(),
                (struct sockaddr *)&sin,
                (unsigned int *)&len
               );

    printf("%d\n", ntohs(sin.sin_port)); fflush(stdout);

    if(fork())
    {
        exit(0);
    }
    else
    {
        freopen("/dev/tty", "a+", stdout);
        freopen("/dev/tty", "a+", stderr);
    }
#else
    NOTIFY("transApp_t::goDaemon: NOT IMPLEMENTED in your O.S., sorry\n");
    NOTIFY("transApp_t::goDaemon: running as a common application\n");
#endif
}

transApp_t::~transApp_t()
{
    debugMsg(dbg_App_Paranoic, "~transApp_t", "Destroying transApp_t");

    linkBinder= NULL;
    targetMgr= NULL;
    theStatsGatherer= NULL;

    assert(irouterParam);
    delete irouterParam;

    debugMsg(dbg_App_Paranoic, "~transApp_t", "Destroyed transApp_t");
}

void
transApp_t::startMeasures(char * filename)
{
    measures->startMeasures(filename);
}

void
transApp_t::stopMeasures(void)
{
    measures->stopMeasures();
}

void
irouter_usage()
{
    fprintf(stdout, "isabel_irouter help::\n\n");
    fprintf(stdout,
            "~> isabel_irouter [-cport <controlPort>]\n"
            "\t[-configFile <irouterfile.cfg>]\n"
            "\t[-linkMgr]\n"
            "\t[-flowServer <hostname>]\n"
            "\t[-GW | -gateway host]\n"
            "\t[-ASMcast | -SSMcast]\n"
            "\t[-mcastGroup (audio     =<mcastGr> | \n"
            "\t              vumeter   =<mcastGr> | \n"
            "\t              video     =<mcastGr> | \n"
            "\t              slidesFtp =<mcastGr> | \n"
            "\t              pointer   =<mcastGr> | \n"
            "\t              controlWp =<mcastGr> | \n"
            "\t              shDisplay =<mcastGr> | \n"
            "\t              halloween =<mcastGr> | \n"
            "\t              rtpVideo  =<mcastGr> | \n"
            "\t              rtcpVideo =<mcastGr>)] \n"
            "\t[-ttl <mcastTTLValue>]\n"
            "\t[-mcastBw <bandwidth(bps)>]\n"
            "\t[-upStreamBw <bandwidth(bps)>]\n"
            "\t[-downStreamBw <bandwidth(bps)>]\n"
            "\t[-allClients | -allclients]\n"
            "\t[-client (audio     | \n"
            "\t          vumeter   | \n"
            "\t          video     | \n"
            "\t          slidesFtp | \n"
            "\t          pointer   | \n"
            "\t          controlWp | \n"
            "\t          shDisplay | \n"
            "\t          halloween | \n"
            "\t          rtcpVideo )]\n"

            "\t[-bdport <basePort>]\n"
            "\t[-doStats]\n"
            "\t[-levaPort <levaPortNumber>]\n"
            "\t[-levaHost <levaHostname>]\n"
            "\t[-shell]\n"
            "\t[-echo]\n"
            "\t[-noCheckLoops]\n"
            "\t[ -h]\n"
            "\n"
           );
    fprintf(stdout,
            " OPTIONS\n"
            "-cport: irouter control port, TCP server to configure irouter\n"
                     "\tdaemon.\n"

            "-configFile: irouter configuration file... take care using\n"
                "\tconfig file and other command line options at the same time.\n"
                "\tThe order is important.\n"

            "-linkMgr: allows connections from other FS.\n"
            "-flowServer: connect to another FS to receive flows.\n"

            "-gateway: accept flows from <peerHost> as a non-irouter source.\n"

            "-ASMcast: send to ASM (Any Source Multicast).\n"
            "-SSMcast: send to SSM (Specific Source Multicast).\n"
            "-mcastGroup: multicast groups configuration. Default values:\n"
               "\taudio         = 239.255.6.1\n"
               "\tvumeter       = 239.255.6.2\n"
               "\tvideo         = 239.255.6.3\n"
               "\tslidesFtp     = 239.255.6.4\n"
               "\tpointer       = 239.255.6.5\n"
               "\tcontrolWp     = 239.255.6.6\n"
               "\tshDisplay     = 239.255.6.7\n"
               "\thalloween     = 239.255.6.8\n"
               "\trtcpVideo     = 239.255.6.9\n"
               "\trtcpShDisplay = 239.255.6.10\n"

            "-ttl: TTL for multicast datagrams. Default value=32.\n"

            "-mcastBw: bandwidth limit to traffic shaping function when \n"
               "\tsending to multicast.\n"

            "-upStreamBw: bandwidth limit to traffic function when \n"
               "\tsending to FS root. Default value 1.5 Mbps.\n"

            "-downStreamBw: bandwidth limit to traffic shaping function\n"
               "\twhen sending from FS root to this FS. Default value \n"
               "\t1.5 Mbps.\n"

            "-allClients: there are local components sending/receiving flows\n"
               "\tAll Components= audio, vumeter, video, slidesFtp, pointer,\n"
               "\tcontrolWp, shDisplay, halloween, rtpVideo y rtcpVideo.\n"

            "-client: define local components explicitly.\n"

            "-bdport: first port to be used by irouter.\n"

            "-doStats: send statistics to ISABEL SNMP Agent.\n"
            "-levaHost: host where a leva network exists.\n"
            "-levaPort: port where a leva network is waiting connections.\n"
            "-shell: control interface using stdout.\n"

            "-noCheckLoops: does not check irouter ids. Could be loops at \n"
                 "\t irouter network.\n"

            "-echo: it is used with -flowServer option to receive the own \n"
                 "\traffic. Traffic feedback. FOR TESTING. It is used with \n"
                 "\t-noCheckLoops to allow component server receiving this \n"
                 "\ttraffic.\n"
            "\n");

}

void
irouter_version()
{
     printf("isabel_irouter version [%d.%d]\n",
            IROUTER_MAJOR_VERSION, IROUTER_MINOR_VERSION);
}


