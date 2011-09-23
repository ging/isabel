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
// $Id: RTPFec.hh 6204 2005-02-11 14:44:40Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#ifndef __RTPFec_hh__
#define __RTPFec_hh__

#include <icf2/notify.hh>
#include <icf2/dictionary.hh>

#include "RTPPacket.hh"

#include "flow.hh"

#define NO_FEC         0

#define MAX_BUFF       24

#define VIDEO_RTP_TSU   1.0/90000.0
#define SHDISP_RTP_TSU  1.0/90000.0
#define POINTER_RTP_TSU 1.0/90000.0
#define DEFAULT_RTP_TSU 1.0/90000.0
#define AUDIO_RTP_TSU   1.0/8000.0

const u8 __fecPTBase=45;
const u8 minfecPT=__fecPTBase+1;
const u8 maxfecPT=__fecPTBase+24;

struct protectionBlock_t
{
    u8  version:2; //No se tiene en cuenta. Lo necesito para alinear a byte
    u8  padding:1;
    u8  extension:1;
    u8  cc:4;
    u8  marker:1;
    u8  payloadtype:7;
    u16 lengthsSum;
    u32 timestamp;
    
};

struct RTPFecHeader_t
{
    u16 SNBase;
    u16 lengthRecv;
    u32 secondWord; //E(1 bit), PT recovery(7 bits), mask(24 bits)
    u32 TSRecovery;
};


class RTPContainer_t : public item_t
{

private:
	
    RTPPacket_t*  frame[MAX_BUFF+1];
    int           nData;
	
public:

	RTPContainer_t(void);
	virtual ~RTPContainer_t(void);
	bool insert(RTPPacket_t *);
	void free(void);
	void free(int);
	RTPPacket_t * getData(void);
        int checkLost(ql_t<int>,int &);
	unsigned long getTimestamp(void);
	bool hasInfo(void);
      
};


class RTPFec_t: public item_t, public collectible_t
{
private:
    
    RTPContainer_t      __buffer; //to receive pkts
    ql_t<RTPPacket_t*>  __packetsList; //to send pkts

protected:

    flowId_e            __flowId;
    u32                 __ssrc;
    u16                 __seqNumber;
    u16                 __lastRcpSQ;
    double              __timestampUnit;
    u8                  __pt;
    u8                  __lastPT;
    u8                 *__matrix;
    int                 __n;
    int                 __k;
    u32                 __lastTS;
    bool                __working;

public:

    RTPFec_t(flowId_e flowId,u32 ssrc, u8 pt,int k);

    virtual ~RTPFec_t(void);

    u8   getPT(void) { return __pt; }
    u16  getLastSQ(void){ return __lastRcpSQ;}
    void setK(u8);

    RTPPacket_t * sendPacket(RTPPacket_t * pkt, icfString_ref addr);
    RTPPacket_t * recvPacket(RTPPacket_t * pkt, icfString_ref addr);
    
private:

    void resetTX(void);
    void resetRX(void);

    virtual RTPPacket_t * fecPktToSend();
    virtual RTPPacket_t * fecPktToRecv(RTPPacket_t * pkt);

    friend class smartReference_t<RTPFec_t>;
};

typedef smartReference_t<RTPFec_t> RTPFec_ref;



//Class RTPFecHolder_t: It stores a RTPFec_ref objects for each RTP Flow

class RTPFecHolder_t: public virtual item_t, public collectible_t
{

    flowId_e __flowId;
    dictionary_t<u32, RTPFec_ref> __sendChannelsDict;
    dictionary_t<u32, RTPFec_ref> __recvChannelsDict;
    u8 __fecPT;
    u8 __k;

public:

    RTPFecHolder_t(flowId_e flowId);
    ~RTPFecHolder_t(void);
    RTPPacket_t * sendPacket(RTPPacket_t * pkt, icfString_ref addr);
    RTPPacket_t * recvPacket(RTPPacket_t * pkt, icfString_ref addr, bool &fec);

    void setFecPT(char *fecType, int n, int k);
    u8   getPT(void);

    RTPFec_ref newRTPFec(u32 ssrc, u8 k = 0);
    void delRTPFec(u32 ssrc);
    bool getLastSQ(u32 ssrc,u16& SQ);


private:
    
    friend class smartReference_t<RTPFecHolder_t>;
};

typedef smartReference_t<RTPFecHolder_t> RTPFecHolder_ref;



#endif
