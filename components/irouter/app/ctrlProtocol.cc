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
// $Id: ctrlProtocol.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include <icf2/general.h>
#include <icf2/stdTask.hh>

#include "transApp.hh"
#include "linkProtocol.hh"
#include "ctrlProtocol.hh"
#include "flow.hh"
#include "link.hh"
#include "videoTrans.hh"

#define MY_INTER_VRFY_ARGC(b,x,msg)      \
    if(argc!= (x)) {                     \
        sprintf(b, "ERROR= %s", msg);    \
        return b;                        \
    }

#define MY_INTER_VRFY_ARGC_range(b,x,y, msg)          \
    if((argc< (x)) || (argc> (y))) {                  \
        sprintf(b, "ERROR= %s", msg);                 \
        return b;                                     \
    }

binding_t<webirouter_InterfaceDef> webirouter_methodBinding[]=
{
    { "ir7_query_id",        &webirouter_InterfaceDef::ir7_query_id },
    { "ir7_help",            &webirouter_InterfaceDef::ir7_help },
    { "help",                &webirouter_InterfaceDef::ir7_help },

    //
    // link management stuff
    //
    { "ir7_define_gateway", &webirouter_InterfaceDef::ir7_define_gateway },
    { "ir7_define_link",    &webirouter_InterfaceDef::ir7_define_link },
    { "ir7_delete_link",    &webirouter_InterfaceDef::ir7_delete_link },

#ifdef __FULL_IR7_INTERFACE
    { "ir7_define_flow",   &webirouter_InterfaceDef::ir7_define_flow },
    { "ir7_delete_flow",   &webirouter_InterfaceDef::ir7_delete_flow },
    { "ir7_define_target",  &webirouter_InterfaceDef::ir7_define_target },

    { "ir7_define_client",  &webirouter_InterfaceDef::ir7_define_client },

    { "def_flow",  &webirouter_InterfaceDef::xxx_define_flow },
#endif


    //
    // meta/santi
    //
    { "ir7_x_set_bw"    ,  &webirouter_InterfaceDef::ir7_x_set_bw     },
    { "ir7_x_add_target",  &webirouter_InterfaceDef::ir7_x_add_target },
    { "ir7_x_del_target",  &webirouter_InterfaceDef::ir7_x_del_target },

    { "ir7_version",       &webirouter_InterfaceDef::ir7_version },

    //
    // Protecting flows
    //
    { "ir7_get_links",       &webirouter_InterfaceDef::ir7_get_links        },
    { "ir7_get_links_fec",   &webirouter_InterfaceDef::ir7_get_links_fec    },
    { "ir7_get_links_stats", &webirouter_InterfaceDef::ir7_get_links_stats  },

    { "ir7_protect_flow",    &webirouter_InterfaceDef::ir7_protect_flow     },
    { "ir7_unprotect_flow",  &webirouter_InterfaceDef::ir7_unprotect_flow   },

    //
    // Mixing audio flow
    //
    { "ir7_mix_audio",       &webirouter_InterfaceDef::ir7_mix_audio    },
    { "ir7_no_mix_audio",    &webirouter_InterfaceDef::ir7_no_mix_audio },

    //
    // generating statistics
    //
    { "ir7_do_stats",      &webirouter_InterfaceDef::ir7_do_stats },

    //
    // video transcoder
    //

    { "ir7_video_transcode",  &webirouter_InterfaceDef::ir7_video_transcode },

    //
    // save measures on/off
    //
    { "ir7_start_measures",  &webirouter_InterfaceDef::ir7_start_measures },
    { "ir7_stop_measures",  &webirouter_InterfaceDef::ir7_stop_measures },

    //
    // common stuff
    //
    { "ir7_nop",  &webirouter_InterfaceDef::ir7_nop   },
    { "ir7_bye",  &webirouter_InterfaceDef::ir7_bye   },
    { "ir7_quit", &webirouter_InterfaceDef::ir7_quit  },
    { NULL, NULL }
};


#define APP (static_cast<transApp_t *>(this->get_owner()))

char const *
webirouter_Interface::ir7_help(int argc, char **)
{
    static char retVal[8192];

    char *pointer=retVal;
    int i=0;
    printf("Control Commands Help:\n");
    while (webirouter_methodBinding[i].name)
    {
        strcpy(pointer, webirouter_methodBinding[i].name);
        pointer += strlen(webirouter_methodBinding[i].name);
        strcpy(pointer, "\n");
        pointer+=strlen("\n");
        i++;
    }
    return retVal;
}



char const *
webirouter_Interface::ir7_version(int argc, char **)
{
    return "Irouter&ICF2\n";
}

char const *
webirouter_Interface::ir7_do_stats(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_do_stats(1|0)\n");

    int doStats= atoi(argv[1]);

    theStatsGatherer->doStats(static_cast<bool>(doStats));

    return "OK\n";
}


char const *
webirouter_Interface::ir7_query_id(int argc, char **)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_query_id()\n");

    sprintf(retVal, "0x%x\n", interID);
    return retVal;
}


char const *
webirouter_Interface::ir7_get_links(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_get_links()\n");

    APP->getLinks(retVal);

    return retVal;
}


char const *
webirouter_Interface::ir7_get_links_fec(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_get_links_fec()\n");

    APP->getLinksFecParams(retVal);

    return retVal;
}


char const *
webirouter_Interface::ir7_get_links_stats(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_get_links_stats()\n");

    APP->getLinksStats(retVal);

    return retVal;
}


char const *
webirouter_Interface::ir7_protect_flow(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal,6,
                       "ir7_protect_flow(link,flow,fec type|None,n,k)\n");

    if (strcmp(argv[3],"Parity"))
    {
        return "ERROR :: Protection type not supported, use Parity";
    }
    NOTIFY("protect: %s,%s,%s,%s,%s\n",argv[1],argv[2],argv[3],argv[4],argv[5]);

    return APP->protectFlow(argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
}


char const *
webirouter_Interface::ir7_unprotect_flow(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal,3, "ir7_unprotect_flow(link,flow)\n");
    NOTIFY("protect: %s,%s\n",argv[1],argv[2]);

    return APP->protectFlow(argv[1], argv[2], "None", 0, 0);
}

char const *
webirouter_Interface::ir7_mix_audio(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_mix_audio(mixerCodecName)\n");
NOTIFY("ir7_mix_audio(%s)\n", argv[1]);

    unsigned int codecFmt= aGetFormatIdByName(argv[1]);

    if (codecFmt < 0)
    {
        return "Error: such codec does not exist\n";
    }

    if ( ! APP->setAudioMixerMode(codecFmt))
    {
        return "Error: setting audio to mix\n";
    }

    return "OK\n";
}


char const *
webirouter_Interface::ir7_no_mix_audio(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_no_mix_audio()\n");
NOTIFY("ir7_no_mix_audio()\n");

    APP->unsetAudioMixerMode();

    return "OK\n";
}


char const *
webirouter_Interface::ir7_define_gateway(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_define_gateway(linkName)\n");

    char *linkName= strdup(argv[1]);

    link_t *l= APP->define_gateway(linkName);

    if( ! l)
    {
        return "ERROR: cannot register gateway definition\n";
    }

    free(linkName);

    return "OK\n";
}


char const *
webirouter_Interface::ir7_define_link(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_define_link(linkName)\n");

    char *linkName= strdup(argv[1]);

    link_t *l=
        APP->linkBinder->newLink(linkName, link_t::inputOutput, false, 0);

    if ( ! l)
    {
        return "ERROR: cannot register link definition\n";
    }

    APP->add_target_for_all_flows(linkName, linkName, 0, 0);

    free(linkName);

    return "OK\n";
}


char const *
webirouter_Interface::ir7_delete_link(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_delete_link(linkName)\n");

    char *linkName= strdup(argv[1]);

    link_t *l= APP->linkBinder->lookUp(linkName);
    if ( ! l)
    {
        return "ERROR: undefined link\n";
    }

    APP->delete_link(linkName);
    free (linkName);

    return "OK\n";
}


#ifdef __FULL_IR7_INTERFACE
char const *
webirouter_Interface::ir7_define_flow(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal,
                       4,
                       "ir7_define_flow(flowName, flowId, flowPrio)\n"
                      );

    char *flowName= strdup(argv[1]);
    u32   flowId  = atoi(argv[2]);
    u32   flowPrio= atoi(argv[3]);

    if ( ! APP->flowBinder.newMedia(flowName, flowId, flowPrio, 0))
    {
        return "ERROR: cannot register flow\n";
    }

    downstreamRoutingInfo_t *dri= new downstreamRoutingInfo_t(flowId, flowPrio);

    if ( ! APP->centralSwitch.insertDri(flowId, dri))
    {
        return "ERROR: cannot register dri for flow\n";
    }

    return "OK\n";
}

char const *
webirouter_Interface::ir7_delete_flow(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_delete_flow(flowName)\n");

    char *flowName= strdup(argv[1]);
    flow_t *flow= APP->flowBinder.lookUp(flowName);

    if ( ! flow)
    {
        return "ERROR: unknown flow\n";
    }

    u32 flowId= flow-> flowId;

    if ( ! APP->flowBinder.deleteMedia(flowName))
    {
        return "ERROR: cannot unregister flow\n";
    }

    if ( ! APP->centralSwitch.removeDri(flowId))
    {
        return "ERROR: cannot unregister dri for flow\n";
    }

    free (flowName);

    return "OK\n";
}

char const *
webirouter_Interface::ir7_define_target(int argc, char **argv)
{
    static char retVal[8192];

    // MY_INTER_VRFY_ARGC(retVal, 5);

    return "ERROR\n";
}

char const *
webirouter_Interface::ir7_define_client(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC_range(retVal, 3, 4,
                             "ir7_define_client"
                             "(flowName, localPort [, remotePort])\n"
                            );

    char *mName= argv[1];
    char *lPort= argv[2];
    char *rPort= NULL;

    if (argc == 4)
    {
        rPort= argv[3];
    }

    flow_t *m= APP->flowBinder.lookUp(mName);

    if ( ! m)
    {
        return "ERROR: unknown flow\n";
    }

    u32 pt= m->flowId;

    clientModule_t *newClient= APP->clientBinder.newClient(pt, lPort, rPort);

    APP->insertTask(newClient);

    return "OK\n";
}

char const *
webirouter_Interface::ir7_link_client(int argc, char **argv)
{
    static char retVal[8192];

    // MY_INTER_VRFY_ARGC(retVal, 5);

    return "ERROR\n";
}


char const *
webirouter_Interface::xxx_define_flow(int argc, char **)
{
    static char retVal[8192];

    // MY_INTER_VRFY_ARGC(retVal, 1);

    return "ERROR\n";
}
#endif


//
// meta/santi
//
char const *
webirouter_Interface::ir7_x_set_bw(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 3, "ir7_x_set_bw(linkName, bw)\n");

    link_t *l= APP->linkBinder->lookUp(argv[1]);

    if ( ! l)
    {
        return "ERROR: no such link\n";
    }

    //l->adjustBandWidth(atoi(argv[2]));
    if (theHarbinger)
    {
        theHarbinger->adjustBandWidth(atoi(argv[2]));
    }

    return "OK\n";
}

struct ctl_ctl_t
{
    link_t *l;

    int target[MAX_FLOW];
};

char const *
webirouter_Interface::ir7_x_add_target(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 3, "ir7_x_add_target(linkName, dstTarget)\n");

    link_t *link= APP->linkBinder->lookUp(argv[1]);

    if ( ! link)
    {
        return "ERROR: no such link\n";
    }

    // for unicast with traffic regulation, link MUST exist,
    // it is used by all targets

    ctl_ctl_t *ctl_ctl= new ctl_ctl_t;

    ctl_ctl->l= link;

    const ql_t<string> *flowList = APP->irouterParam->getFlowList();

    int j= 0;
    for (ql_t<string>::iterator_t i = flowList->begin();
         i != flowList->end();
         i++
        )
    {
        /*string flowName = i;
        const char *flowStr = flowName.c_str();
        flow_t *flow= APP->flowBinder.lookUp(flowStr);
        ctl_ctl->target[j]=
            link->addTarget(flow->flowId,
                            argv[2],
                            APP->irouterParam->getNetPort(flowStr),
                            0
                           );
        */
        j++;
    }

    sprintf(retVal, "%ld\n", (long)ctl_ctl);
    return retVal;
}

char const *
webirouter_Interface::ir7_x_del_target(int argc, char **argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_x_del_target(idTgt)\n");

    ctl_ctl_t *ctl_ctl= (ctl_ctl_t *) atoi(argv[1]);

    link_t *link= ctl_ctl->l;

    if ( ! link)
    {
        return "ERROR: no such link\n";
    }

    const ql_t<string> *flowList = APP->irouterParam->getFlowList();

    for (unsigned int i = 0; i != flowList->len() - 1; i++)
    {
       link->delTarget(ctl_ctl->target[i]);
    }
    delete ctl_ctl;

    return "OK\n";
}


//
// video transcoder
//

char const *
webirouter_Interface::ir7_video_transcode (int argc, char ** argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal,5, "ir7_video_transcode(SSRC,dest,BW,lineFmt)\n");

    u32 lineFmt;

    try
    {
        lineFmt= vGetFormatIdByName(argv[4]);
    }
    catch (const char *e)
    {
        return "ERROR: unknown video format\n";
    }

    return APP->videoTranscode(atoi(argv[1]), argv[2], atoi(argv[3]), lineFmt);
}

//
// save measures on/off
//

char const *
webirouter_Interface::ir7_start_measures(int argc, char ** argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 2, "ir7_start_measures()\n");

    APP->startMeasures(argv[1]);

    return "OK\n";
}

char const *
webirouter_Interface::ir7_stop_measures(int argc, char ** argv)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_stop_measures()\n");

    APP->stopMeasures();

    return "OK\n";
}

//
// old stuff
//
char const *
webirouter_Interface::ir7_nop(int argc, char **)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_nop()\n");

    return "OK\n";
}


char const *
webirouter_Interface::ir7_bye(int argc, char **)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_bye()\n");

//    get_owner()->removeTask(this);
    return (char*)-1;
}


char const *
webirouter_Interface::ir7_quit(int argc, char **)
{
    static char retVal[8192];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_quit()\n");

    get_owner()->shutdown();
    return (char*)-1;
}

