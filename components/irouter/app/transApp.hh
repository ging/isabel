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
// $Id: transApp.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __trans_app_hh__
#define __trans_app_hh__

#include <string>

using namespace std;

#include <icf2/general.h>
#include <icf2/stdTask.hh>
#include <icf2/dictionary.hh>

#include "clientModule.hh"
#include "flow.hh"
#include "output.hh"
#include "configParam.hh"
#include "AudioMixer2.hh"
#include "centralSwitch.hh"
#include "irouterGatherer.hh"
#include "measures.hh"

//
//! transApp_t -- application scheduler
//

class transApp_t: public application_t
{
public:

    irouterParam_t *irouterParam;

    //
    //! dieHard_t -- task to kill application
    //
    /*!
        Take care with this task... kills the irouter...
        I _ONLY_ use this function when debugging with insure/purify
        to get information of memory leaks...
    */
    class dieHard_t: public simpleTask_t
    {
     public:
         //! dieHard_t constructor
         /*
             \param n number of seconds to allow irouter up...
         */
         dieHard_t(int n): simpleTask_t(1000000*n) {};

         //! Callback to be executed after n seconds
         void heartBeat(void)
         {
             NOTIFY("MURIENDO DESCARADAMENTE!!!\n");
             get_owner()->shutdown();
         };
    };

private:
    const char *statsPort; /*!< statsPort waiting irouter stats data*/

    ql_t<string> linkList;

    //! statsInfo_t -- statistics gatherer
    class statsInfo_t: public rwLockedItem_t
    {
    private:
        bool localStats;
        bool globalStats;
        char *groupName;

        bool setGlobalStats(char *grName);
        bool setLocalStats(void);
        bool removeLocalStats(void);
        bool removeGlobalStats(void);

        bool getLocalStats(void);
        bool getGlobalStats(void);

    public:
        statsInfo_t(void)
        : localStats(false), globalStats(false), groupName(NULL)
        {}

        virtual ~statsInfo_t(void)
        {
            if (groupName)
            {
                free(groupName);
            }
        }

        friend class transApp_t;
    };

    statsInfo_t statsInfo;

    int getBwMultiplier(char *trailer);

public:

    streamSocket_t *serverSock;

    dictionary_t<flowId_e, char *> groupDictionary;

    //
    // instance vars
    //
    targetMgr_ref    targetMgr;  // Para que todo se destruya adecuadamente
    linkBinder_ref   linkBinder; // Para que todo se destruya adecuadamente

    flowBinder_t  flowBinder;
    clientBinder_t clientBinder;
    AudioMixer2_ref audioMixer2;
    centralSwitch_t centralSwitch;

    //! transApp_t constructor
    transApp_t(int &argc, argv_t &argv);

    //! transApp_t constructor
    virtual ~transApp_t(void);

    //! Create a child process. Execute unattached to terminal.
    void goDaemon(void);

    bool getLocalStats (void) { return statsInfo.getLocalStats();  }
    bool getGlobalStats(void) { return statsInfo.getGlobalStats(); }

    void define_flow(const char *flowName, const u32 flowId);

    link_t *define_gateway(const char *linkName);
    link_t *define_link(const char *linkName,
                        linkClass_t &linkClass,
                        const bool echoBool,
                        const int bw
                       );

    void delete_link(const char *linkName);

    void define_client(const char *flowName,
                       const char *lPort,
                       const char *rPort
                      );

    bool define_mcast_flow_group(const char *flowName, char *classDAddress);

    void add_target_for_all_flows(const char *linkName,
                                  const char *usrTgt,
                                  int n,
                                  int k
                                 );

    void add_target(const char *linkName,
                    const flowId_e flowId,
                    const char *usrTgt,
                    const int lPort,
                    int n,
                    int k,
                    const i8 mcastTTL= 0
                   );

    void getLinks(char *retVal);
    void getLinksFecParams(char *retVal);
    void getLinksStats(char *retVal);

    const char *protectFlow(const char *linkName,
                            const char *flowName,
                            const char *fecType,
                            int n= 0,
                            int k= 0
                           );
    const char *videoTranscode(u32 SSRC, char *dest, u32 BW, u32 lineFmt);

    int joinSSM(char *source);
    int leaveSSM(char *source);

    bool setAudioMixerMode(unsigned int mixerFmt);
    void unsetAudioMixerMode(void);

    void startMeasures(char *filename);
    void stopMeasures(void);

    char const *className(void) const { return "transApp_t"; }
};

#endif

