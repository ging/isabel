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
// $Id: output.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __output_hh__
#define __output_hh__

#include <assert.h>
#include <string.h>


#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/ql.hh>
#include <icf2/sockIO.hh>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>
#include <icf2/dictionary.hh>
#include <icf2/smartReference.hh>

#include <rtp/RTPPacket.hh>

#include "webirouterConfig.hh"
#include "pkt.hh"
#include "flow.hh"

#include "RTPFecHolder.hh"
#include "videoTrans.hh"

class link_t;  // externally declared

//
// tgtSock_t -- a target socket
//

//
// Target sockets are managed by socketPool_t (see below)
// A tgtSock_t is just a dgramSocket_t with a reference
// count and identifier (to be used by socketPool_t)
//
class tgtSock_t: public dgramSocket_t
{
public:
    int id;

    flowId_e flowId;

    long sentBytes;     // internal statistics
    long recvBytes;     // internal statistics

    tgtSock_t(int _id, inetAddr_t &a);
    virtual ~tgtSock_t(void);

    virtual const char *className(void) const { return "tgtSock_t"; };

    friend class smartReference_t<tgtSock_t>;
};

typedef smartReference_t<tgtSock_t> tgtSock_ref;



//
// socketPool_t -- the actual packet sender
//

//
// The socketPool_t implements the handling of sockets.
// It allows the sharing of a given socket between different
// targets.
//
// A socket must be shared because the same local port
// can be used to connect to different remote targets.
// If this happens demultiplexing on receipt is required.
//
// The demux logic is as follows:
//
//     + if pkt.dstAddr == mcast lookup target by mcast addr
//     + else lookup target by pkt.srcAddr
//
//     + if no target found... discard
//     + else dispatch packet
//
// The demultiplexing logic is not handled here but at
// targetMgr_t (see below)
//
class
socketPool_t: public virtual item_t
{
private:
    unsigned MAX_SOCKETS;
    dictionary_t<int, tgtSock_ref> byId;
    dictionary_t<int, tgtSock_ref> byLocalPort;

    int sockCount;

public:
    socketPool_t(void);
    virtual ~socketPool_t(void);

    virtual bool lookUpSock(const int port) const ;

    virtual tgtSock_ref getSock(const char *lAddr, const int port);

    virtual const char *className(void) const { return "socketPool_t"; };
};




//
// target_t -- a given network target
//

//
// target_t objects are handled by the targetMgr_t (see below)
//
// A target is characterized by a:
//
//     + tgtSock
//     + tgtAddr
//
class target_t: public virtual item_t
{
private:
    link_t *link;

    socketPool_t   *sp;

    dictionary_t<videoTranscodeInfo_t,videoTranscoder_t *> transcoderDict;

    char pad[256];

    target_t(socketPool_t *_sp,
             int id,
             link_t *l,
             flowId_e flowId,
             const char *tgt,
             const int port,
             int n,
             int k,
             i8 mcastTTL=0,
             bool SSMFlag=false
            );

    virtual ~target_t(void);

    videoTranscoder_t *checkVideoTranscoder(RTPPacket_t *rtp);

    int   getPort(void);
    void  setPort(int port);

public:
    int     tgtId;
    inetAddr_t     *tgtAddr;
    tgtSock_ref     tgtSock;
    RTPFecHolder_t *RTPFecHolder;

    void deliver(sharedPkt_t *pkt);

    void protectFlow(const char *fecType, int n, int k);
    void getFecParams(flowId_e &flowId, int &n, int &k);

    void videoTranscode(u32 SSRC, u32 BW, u32 lineFmt);

    // SSM
    int joinSSM(char *source);
    int leaveSSM(char *source);

    virtual const char *className(void) const { return "target_t"; };

    friend class targetMgr_t;
};


//
// targetMgr_t -- the target manager
//

//
// targetMgr_t handles a target_t list (creation/deletion of targets)
// and performs demultiplexing on packet receipt.
//
// The demux logic is as follows:
//
//     + if pkt.dstAddr == mcast lookup target by mcast addr
//     + else lookup target by pkt.srcAddr
//
//     + if no target found... discard
//     + else dispatch packet
//

class transApp_t;
class targetMgr_t: public simpleTask_t
{
private:
    transApp_t *myApp;

    int tgtCount;

protected:
    virtual void IOReady(io_ref &io);

public:
    socketPool_t *sockPool;

    targetMgr_t(transApp_t *app);

    virtual ~targetMgr_t(void);

    virtual target_t *newTarget(link_t *l,
                                flowId_e flowId,
                                const char *addr,
                                const int port,
                                int n,
                                int k,
                                i8 mcastTTL=0
                               );

    virtual void  delTarget(target_t *t);

    virtual const char *className(void) const { return "targetMgr_t"; };

    friend class smartReference_t<targetMgr_t>;
};

typedef smartReference_t<targetMgr_t> targetMgr_ref;


#endif

