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
// $Id: configParam.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __config_param_hh__
#define __config_param_hh__

#include <string>

using namespace std;

#include <icf2/general.h>
#include <icf2/vector.hh>
#include <icf2/dictionary.hh>

#include "flow.hh"

//
// SSM Lista para guardar las fuentes multicast
//
typedef vector_t<char*> stringArray_t;
typedef smartReference_t<stringArray_t> stringArray_ref;


typedef enum
{
    noProtection = 0,
    parity
} protection_e;

class irouterParam_t
{
private:
    class flowSpec_t
    {
    public:
        int netPort;
        flowId_e flowId;
        char *mcastGroup;
        bool createLocalClient;
        char *irouterPort;
        char *componentPort;

        flowSpec_t(int nPort, flowId_e f)
        :netPort(nPort),
         flowId(f),
         mcastGroup(NULL),
         createLocalClient(false),
         irouterPort(NULL),
         componentPort(NULL)
        {}

        virtual ~flowSpec_t(void)
        {
            if (mcastGroup)
                free(mcastGroup);
            if (irouterPort)
                free(irouterPort);
            if (componentPort)
                free(componentPort);
        }
    };

private:
    static irouterParam_t *theInstance;

protected:
    irouterParam_t(void);

public:
    static irouterParam_t *getInstance(void);
    virtual ~irouterParam_t(void);

    const int getNetPort(const char *flowName);

    bool setBaseDataPort(int baseDataPort);
    const int getBaseDataPort(void) const;

    const int getCtrlPort(void) const;

    const ql_t<string> *getFlowList(void) const;


    //
    // link for gateway
    //
    void setGatewayMode(bool mode);
    bool getGatewayMode(void) const;

    //
    //! Multicast configuration
    //
    bool configFlowMcastGroup(const char *flowName, const char *mcastGr);

    const char *getMcastGroup (const char *flowName);

    void setMulticastMode(bool mcastMode);
    bool getMulticastMode(void) const;
    bool setMulticastTTL(u8 mcTTL);
    u8   getMulticastTTL(void) const;
    void setMulticastBw(int mcastBw);
    int  getMulticastBw(void) const;

    //
    // Multicast SSM
    //
    void connectMcastServer(char *mcastServerIP);
    const char *getMcastServerTgt(void) const;
    char *getmyip(void);
    void setMcastLeader(char *addr);
    bool getMcastLeader(void);
    void setSSM(bool);
    bool getSSM(void);
    void setMulticastSources(stringArray_ref sources);
    stringArray_ref getSources(void);


    //
    //! Connection to other flowservers
    //
    void setLinkMgr(bool lMgr);
    bool getLinkMgr(void) const;

    void connectFlowServer(char *flowServerTgt);
    const char *getFlowServerTgt(void) const;

    void setDownStreamBw(int downStreamBw);
    void setUpStreamBw(int upStreamBw);

    int getDownStreamBw(void) const;
    int getUpStreamBw(void) const;

    const char *getIrouterFlowPort(const char *flowName);
    const char *getComponentFlowPort(const char *flowName);

    bool createLocalClient(const char *flowName) const;
    bool defineClient(const char *flowName,
                      const char *iPort,
                      const char *cPort
                     );
    bool activateClient(const char *flowName);

    void setAudioParity(int new_n, int new_k)
    {
        audio_n= new_n; audio_k= new_k;
    }
    void setVideoParity(int new_n, int new_k)
    {
        video_n= new_n; video_k= new_k;
    }
    void setShDispParity(int new_n, int new_k)
    {
        shdisp_n= new_n; shdisp_k= new_k;
    }
    void getAudioParity(int &ret_n, int &ret_k)
    {
        ret_n= audio_n; ret_k= audio_k;
    }
    void getVideoParity(int &ret_n, int &ret_k)
    {
        ret_n= video_n; ret_k= video_k;
    }
    void getShDispParity(int &ret_n, int &ret_k)
    {
        ret_n= shdisp_n; ret_k= shdisp_k;
    }

    void setFlowserverMode(bool fsMode);
    bool getFlowserverMode(void) const;

    int  getFamily(void) { return family; }
    void setFamily(int i) { family = i; }
    int  getFlowIdByName(const char * name)
    {
        return flowSpecByName.lookUp(name)->flowId;
    }

private:

    int baseDataPort;
    int lastNetPort;

    bool gatewayMode;

    bool multicastMode;
    u8   mcastTTL;
    int  mcastBw;

    bool  flowserverMode;
    bool  linkMgr;
    char *flowServerTgt;
    int   downStreamBw;
    int   upStreamBw;

    int audio_n, audio_k;   // protect ALL audio flows coming out from this irt
    int video_n, video_k;   // protect ALL video flows coming out from this irt
    int shdisp_n, shdisp_k; // protect ALL shdisp flows coming out from this irt

    ql_t<string> *flowList;
    dictionary_t<string, flowSpec_t *> flowSpecByName;

    //
    // Multicast SSM
    //
    char *mcastServerTgt;
    bool mcastLeader;
    stringArray_ref multicastSources;
    bool SSMFlag;
    int  family;

    bool configFlow(const char *flowName, flowId_e flowId);

    friend class confIrouter_Interface_t;
};

#endif

