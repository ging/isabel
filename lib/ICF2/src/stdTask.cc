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
// $Id: stdTask.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <ctype.h>

#include <icf2/stdTask.hh>
#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/parseCmdLine.hh>
#include <icf2/notify.hh>

unsigned tcpServerCount = 0;

application_t::application_t(int nTasks): sched_t(nTasks)
{
    __appInit();
}

application_t::application_t(int &argc, argv_t &argv,int nTasks): sched_t(nTasks)
{
    enum myOptions {optDebugMask,
                    optDebugLevel,
                    optDebugFile, 
                    optNotifyFile,
                    optHelp
                   };
    
    optionDefList_t optList;
    appParamList_t *parList;

    __appInit();

    optList
        << new optionDef_t("@debugLevel", optDebugLevel, "Sets debug level (if debug enabled)")
        << new optionDef_t("@debugMask", optDebugMask, "Sets debug mask (if debug enabled)")
        << new optionDef_t("@debugFile", optDebugFile, "File to dump debug info")
        << new optionDef_t("@notify", optNotifyFile, "File to dump log info")
        << new optionDef_t("help", optHelp, "Help!!! When I need somebody...");

    parList= getOpt(optList, argc, argv);

    appParamList_t::iterator_t i;

    for(i= parList->begin(); i != parList->end(); i++) {
        appParam_t *param= static_cast<appParam_t*>(i);

        if(param->parId== optHelp) {

            optionDefList_t::iterator_t j;

            for(j= optList.begin(); j != optList.end(); j++) {
                optionDef_t *op= static_cast<optionDef_t*>(j);
                if(op->optDesc)
                    fprintf(stderr, "    %-10s %-5s\t-- %s\n",
                            op->optName,
                            op->optHasArg?"value":"",
                            op->optDesc
                            );
                else
                    fprintf(stderr, "    %-10s %-5s\t",
                            op->optName,
                            op->optHasArg?"":"value"
                            );
            }
            exit(0);
        }
    }

    for(; parList->len(); parList->behead()) {
        switch(parList->head()->parId) {
        case optDebugLevel: 
            item_t::debugLevel=(atoi(parList->head()->parValue)); // deprecated
            item_t::setDebugLevel(item_t::debugLevel);
            debugMsg(dbg_App_Normal, "application_t", 
                     "debugMask set to %d\n", item_t::debugMask);
            break;

        case optDebugMask: {
            item_t::debugMask = dbg_No_Msg;
            char *s =parList->head()->parValue;
            char *aux=s;
            size_t len = strlen(s);
            for (unsigned i= 0; i < len; i++) {
                 if (isspace(s[i]) || s[i]=='|') {
                     aux++;
                 } else {
                    int len_w=0;
                    char p[32];
                    memset(p, 0, 32);
                    while ( ! isspace(s[i]) && s[i] != '|' && i < len) {
                        i++;
                        len_w++;
                    }
                    strncpy(p, aux, len_w);
                    item_t::debugMask= item_t::debugMask | item_t::strToMask(p);
                    aux=aux+len_w+1;
                 }
            }
            debugMsg(dbg_App_Normal, "application_t", 
                     "debugMask set to %d\n", item_t::debugMask);
        } break;

        case optDebugFile:
            if(item_t::debugFile && (item_t::debugFile != stderr))
                fclose(item_t::debugFile);
            item_t::debugFile= fopen(parList->head()->parValue, "w+");
            break;

        case optNotifyFile: 
            setNotifyFile(parList->head()->parValue);
            break;
 
 
        default:
            // aqui se deberia lanzar una excepcion
            fprintf(stderr, "application_t::application_t:: "
                    "bogus parList received from application_t::getOpt()\n");
            abort();
            break;
        }
    }

    char b2[1000];
    memset(b2,0,1000);
    for (int j = 0; j< argc;j++)
    {
      strcat(b2,argv[j]);
      if (j<argc-1)
          strcat(b2," ");
    }
    NOTIFY("%s\n",b2);
}

application_t::application_t(int &argc, argv_t &argv): sched_t(128)
{
    enum myOptions {optDebugMask, optDebugLevel, optDebugFile, 
                    optNotifyFile, optHelp};
    
    optionDefList_t optList;
    appParamList_t *parList;

    __appInit();

    optList
        << new optionDef_t("@debugLevel", optDebugLevel, "Sets debug level (if debug enabled)")
        << new optionDef_t("@debugMask", optDebugMask, "Sets debug mask (if debug enabled)")
        << new optionDef_t("@debugFile", optDebugFile, "File to dump debug info")
        << new optionDef_t("@notify", optNotifyFile, "File to dump log info")
        << new optionDef_t("help", optHelp, "Help!!! When I need somebody...");

    parList= getOpt(optList, argc, argv);

    appParamList_t::iterator_t i;

    for(i= parList->begin(); i != parList->end(); i++) {
        appParam_t *param= static_cast<appParam_t*>(i);

        if(param->parId== optHelp) {

            optionDefList_t::iterator_t j;

            for(j= optList.begin(); j != optList.end(); j++) {
                optionDef_t *op= static_cast<optionDef_t*>(j);
                if(op->optDesc)
                    fprintf(stderr, "    %-10s %-5s\t-- %s\n",
                            op->optName,
                            op->optHasArg?"value":"",
                            op->optDesc
                            );
                else
                    fprintf(stderr, "    %-10s %-5s\t",
                            op->optName,
                            op->optHasArg?"":"value"
                            );
            }
            exit(0);
        }
    }

    for(; parList->len(); parList->behead()) {
        switch(parList->head()->parId) {
        case optDebugLevel: 
            item_t::debugLevel=(atoi(parList->head()->parValue)); // deprecated
            item_t::setDebugLevel(item_t::debugLevel);
            debugMsg(dbg_App_Normal, "application_t", 
                     "debugMask set to %d\n", item_t::debugMask);
            break;

        case optDebugMask: {
            item_t::debugMask = dbg_No_Msg;
            char *s =parList->head()->parValue;
            char *aux=s;
            size_t len = strlen(s);
            for (unsigned i= 0; i < len; i++) {
                 if (isspace(s[i]) || s[i] == '|') {
                     aux++;
                 } else {
                    int len_w=0;
                    char p[32];
                    memset(p, 0, 32);
                    while ( ! isspace(s[i]) && s[i] != '|' && i < len) {
                        i++;
                        len_w++;
                    }
                    strncpy(p, aux, len_w);
                    item_t::debugMask= item_t::debugMask | item_t::strToMask(p);
                    aux=aux+len_w+1;
                 }
            }
            debugMsg(dbg_App_Normal, "application_t", 
                     "debugMask set to %d\n", item_t::debugMask);
        } break;

        case optDebugFile:
            if(item_t::debugFile && (item_t::debugFile != stderr))
                fclose(item_t::debugFile);
            item_t::debugFile= fopen(parList->head()->parValue, "w+");
            break;

        case optNotifyFile: 
            setNotifyFile(parList->head()->parValue);
            break;
 
 
        default:
            // aqui se deberia lanzar una excepcion
            fprintf(stderr, "application_t::application_t:: "
                    "bogus parList received from application_t::getOpt()\n");
            abort();
            break;
        }
    }

    char b2[1000];
    memset(b2,0,1000);
    for (int j = 0; j< argc;j++)
    {
      strcat(b2,argv[j]);
      if (j<argc-1)
          strcat(b2," ");
    }
    NOTIFY("%s\n",b2);

}

void
application_t::__appInit(void)
{
//    item_t::debugMask= 0;
//    item_t::debugFile= stderr;
}

appParamList_t *
application_t::getOpt(optionDefList_t &op, int &argc, argv_t &argv)
{
    return ::getOpt(op, argc, argv);
}

