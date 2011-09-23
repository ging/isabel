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
// $Id: configParam.hh 10645 2007-08-24 10:51:20Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __irouter__param__hh__
#define __irouter__param__hh__

#include <string>

using namespace std;

#include <icf2/general.h>
#include <icf2/dictionary.hh>

#include "flow.hh"

//
// SSM Lista para guardar las fuentes multicast
//
typedef ql_t<char* > charList_t;


typedef enum
{
    noProtectionId = 0,
    parityId
} protection_e;

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
class irouterParam_t
{
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
    // notify file
    //
    void setNotifyFile(const char * file);
    const char * getNotifyFile(void);

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
    void setMulticastSources(charList_t sources);
    charList_t getSources(void);


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

    void setParity(int new_n, int new_k) {n= new_n; k= new_k;}
    void getParity (int &ret_n, int &ret_k)
    {
        ret_n= n; ret_k= k;
    }

    void setFlowserverMode(bool fsMode);
    bool getFlowserverMode(void) const;

    int  getFamily(void) { return family; }
    void setFamily(int i) { family = i;}
    int  getFlowIdByName(const char * name)
    {
        return flowSpecByName.lookUp(name)->flowId;
    }
    
    flowSpec_t *  getFlowPortByName(const char* name) { return flowSpecByName.lookUp(name);}

private:

	char *notifyFile;
    int baseDataPort;
    int lastNetPort;

    bool gatewayMode;

    bool multicastMode;
    u8   mcastTTL;
    int  mcastBw;

    bool flowserverMode;
    bool linkMgr;
    char *flowServerTgt;
    int  downStreamBw;
    int  upStreamBw;

    int n, k;

    ql_t<string> *flowList;
    dictionary_t<string, flowSpec_t *> flowSpecByName;

    //
    // Multicast SSM
    //
    char *mcastServerTgt;
    bool mcastLeader;
    charList_t multicastSources;
    bool SSMFlag;
    int  family;

    bool configFlow(const char *flowName, flowId_e flowId);

    friend class confIrouter_Interface_t;
};

#endif
