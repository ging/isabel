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
// $Id: linkProtocol.hh 10643 2007-08-23 13:22:23Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __link_protocol_hh__
#define __link_protocol_hh__

#include <string>

using namespace std;

#include <icf2/general.h>
#include <icf2/stdTask.hh>

#include "transApp.hh"
#include "socketPool.hh"



//
// all irouter link protocol messages are sent over the
// following port
//
extern const char *irouterLinkProtocol;

//
// SSM Tipo definido para la lista global de fuentes multicast.
//
typedef ql_t<char* > charList_t;

//
// link protocol pdu types
//
enum msgID_e
{
    linkReq      =0x100,
    linkAns      =0x101,
    versionErr   =0x102,
    linkAudioM   =0x103,

//
// SSM
//
    mcastLinkReq =0x104,
    mcastLinkAns =0x105,


    sessionM  =0x200
};



//
// linkMsg message --
//
//   Each leaf send a linkMsg message every 'LM_TIMEOUT'
//   seconds. On receipt of linkMsg a network node will
//   start a traffic scheduler for the leaf assuming an
//   independent link for each leaf.
//
const unsigned LM_TIMEOUT= 10;
struct linkMsg_t
{
    msgID_e msgType;
    u16     iMajorVer;
    u16     iMinorVer;
    u32     linkBandWidth;          // link BW
    u32     linkEchoRequired;       // para devolver una copia por
                                    // el mismo link de origen
    //
    //SSM
    //
    char    multicastMsg[512];      // Para guardar las fuentes multicast

    u16     n;
    u16     k;
};


//
// sessionMsg message --
//
//   Leaves send session messages for every session beeing
//   serviced every SM_TIMEOUT. Session information is used
//   to optimize downstream traffic.
//
const unsigned SM_TIMEOUT= 10;
struct sessionMsg_t
{
    msgID_e msg;

    u32     nSessions;
    u32     sessionIDs[1];
};



class linkControl_t;  // forward declared


//
// aux class
//
class linkOnDemandTask_t: public simpleTask_t
{
public:
    transApp_t    *myApp;
    linkControl_t *manager;

    bool   stillActive;
    bool   type;

    char       *host;
    inetAddr_t  hostAddr;
    link_t     *link;

    unsigned long nominalBandWidth;

    linkOnDemandTask_t(transApp_t *app,
                       linkControl_t *mgr,
                       const inetAddr_t &remote,
                       long bw,
                       bool echoBool,
                       int new_n,
                       int new_k,
                       bool type=NORMAL
                      );

    linkOnDemandTask_t(linkOnDemandTask_t &other);

    virtual ~linkOnDemandTask_t(void);


    linkOnDemandTask_t &operator=(linkOnDemandTask_t &other);

    virtual void heartBeat(void);

    // SSM
//    void deleteMulticastSources(const char *buffer);


    void adjustBandWidth(u32);

    const char *className(void) const { return "linkOnDemandTask_t"; };

    friend class smartReference_t<linkOnDemandTask_t>;
};

typedef smartReference_t<linkOnDemandTask_t> linkOnDemandTask_ref;

//
// server task
//

class remotesByAddrDict_t: public dictionary_t<inetAddr_t, linkOnDemandTask_ref>{
private:
    virtual int _priv_compare_keys(inetAddr_t const key1,
                                   inetAddr_t const key2) const
    {
          return key1.equalIPAddr(key2);
    }

public:
     remotesByAddrDict_t(int n)
     : dictionary_t<inetAddr_t, linkOnDemandTask_ref>(n) {}
};

class linkControl_t: public simpleTask_t
{
private:
    int             remoteUniqueId;
    bool            acceptConnections;
    char            irouterLinkPort[16];
    dgramSocket_t     *internalSock;

    void processConnectionRequest(dgramSocket_t *iioo, inetAddr_t &addr, linkMsg_t &itsYou, int length);
    void sendErrorVersionPkt(dgramSocket_t *iioo, inetAddr_t &addr, int length);
    void processConnectionAnswer(linkMsg_t &itsYou, inetAddr_t &addr, int length);

    //
    //  SSM
    //
    /*
    char * leaderIpMsg();
    char * getMulticastSources();
    void refreshMulticastSources(char *buffer);
    void processMcastConnectionRequest(dgramSocket_t *iioo, inetAddr_t &addr, linkMsg_t &itsYou, int length);
    void processMcastConnectionAnswer(linkMsg_t &itsYou,inetAddr_t &addr,int length);
*/


public:
    transApp_t *myApp;
    dictionary_t<string, linkOnDemandTask_ref> remotesByAddress;
    char *flowServerTgt;

//
// SSM
/*
    bool  mcastLeader;
    char *mcastServerTgt;
*/
    linkControl_t(transApp_t *app, bool nacceptConnections);
    virtual ~linkControl_t(void);

    virtual void IOReady(io_ref &);

    virtual const char *className(void) const { return "linkControl_t"; };
};



//
// client task
//
struct linkHarbinger_t: public simpleTask_t
{
private:
    dgramSocket_t   *hSock;

    int              bandWidth;

    bool             echoMode;
    u32              remoteUniqueId;
    inetAddr_t      *remoteAddr;
    dgramSocket_t      *internalSock;

    int n, k;

public:
    transApp_t *myApp;

    linkHarbinger_t(transApp_t *app,
                    const char *remote,
                    int bw,
                    bool echo,
                    int new_n,
                    int new_k
                   );
    virtual ~linkHarbinger_t(void);

    virtual void heartBeat(void);

    virtual const char *className(void) const { return "linkHarbinger_t"; };

    virtual void adjustBandWidth(u32 bw) { bandWidth= bw; };
};
/*

struct mcastLinkHarbinger_t: public simpleTask_t
{
private:
    dgramSocket_t   *hSock;

    int              bandWidth;

    bool             echoMode;
    u32              remoteUniqueId;
    inetAddr_t      *remoteAddr;
    tgtSock_ref      internalSock;

    int n, k;

public:
    transApp_t *myApp;

    mcastLinkHarbinger_t(transApp_t *app,
                         const char *remote,
                         int bw,
                         bool echo,
                         int new_n,
                         int new_k
                        );
    virtual ~mcastLinkHarbinger_t(void);

    virtual void heartBeat(void);

    virtual const char *className(void) const { return "mcastLinkHarbinger_t"; };

    virtual void adjustBandWidth(u32 bw) { bandWidth= bw; };

    virtual char *getIpMsg();
};
*/
//
// global harbinger //chapuza santi & jsr & trobles
//
extern linkHarbinger_t *theHarbinger;
//
// SSM
//
//extern mcastLinkHarbinger_t *theMcastHarbinger;

#endif
