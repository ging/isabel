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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#ifndef __RTP_fec_hh__
#define __RTP_fec_hh__

#include <icf2/vector.hh>

#include <rtp/RTPPacket.hh>
#include <rtp/RTPPayloads.hh>

const int NO_FEC= 0;
const int MAX_BUFF= 24;

struct protectionBlock_t
{
    u32 version:2;
    u32 padding:1;
    u32 extension:1;
    u32 cc:4;
    u32 marker:1;
    u32 payloadtype:7;
    u32 lengthsSum:16;
    u32 timestamp;
};

struct RTPFecHeader_t
{
    u16 SNBase;
    u16 lengthRecv;
    u32 secondWord; // E(1 bit), PT recovery(7 bits), mask(24 bits)
    u32 TSRecovery;
};


class RTPContainer_t
{
private:

    RTPPacket_t *frame[MAX_BUFF];
    u16          nData;

public:

    RTPContainer_t(void);
    ~RTPContainer_t(void);

    bool insert(RTPPacket_t *pkt);
    void free(void);
    void free(int SQ);
    RTPPacket_t *getData(void);
    int  checkLost(vector_t<int> *SQArray, int &SQ);
    u32  getTimestamp(void);
};


class RTPFecReceiver_t
{
private:

    u8 fecPT;

    RTPContainer_t __buffer;

    bool active;
    void resetRX(void);

    RTPPacket_t *fecPktToRecv(RTPPacket_t *pkt);

public:

    RTPFecReceiver_t(u8 nfecPT);
    ~RTPFecReceiver_t(void);

    RTPPacket_t *recvPacket(RTPPacket_t *pkt);
};

class RTPFecSender_t
{
private:

    u8           fecPT;  // RTP payload type used for FEC
    RTPPacket_t *fecPkt;
    u16          k;
    u32          mask;

    vector_t<u16> SQArray;
    u16       maxLength;

    void resetTX(void);
    RTPPacket_t *fecPktToSend(RTPPacket_t *pkt);

public:

    RTPFecSender_t(u8 nfecPT);
    ~RTPFecSender_t(void);

    void setK(u8 k);

    RTPPacket_t *sendPacket(RTPPacket_t *pkt);
};

#endif

