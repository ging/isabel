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
// $Id: stats.hh 20791 2010-07-08 14:25:47Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __stats_hh__
#define __stats_hh__

#include <icf2/general.h>
#include <icf2/icfTime.hh>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>

// -------------------------------------------------------
// Snapshot of state
// -------------------------------------------------------

class measures_t: public collectible_t
{
public:
    struct m_t
    {
        u32   chId;
        char *compId;
        u32   desiredFR;
        u32   desiredBW;
        u32   codecBW;
        u32   sentFR;
        u32   sentBW;
        u32   recvBW;
        u32   ensembledFR;
        u32   paintFR;
        char *codecName;
        u8    quality;
        char *acronym;
        u32   imageWidth;
        u32   imageHeight;
    } m;

    measures_t(void);

    ~measures_t(void);

    friend class smartReference_t<measures_t>;
};

typedef smartReference_t<      measures_t  >   measures_ref;
typedef                   ql_t<measures_ref>   measuresList_t;
typedef smartReference_t< ql_t<measures_ref> > measuresList_ref;


// -------------------------------------------------------
// State object
// -------------------------------------------------------
class stats_t
{
private:
    struct timeval startTime;
    struct timeval currTime;

    int     chId;
    char   *compId;
    double  desiredFR;
    double  desiredBW;
    double  codecBytes;
    double  sentFrames;
    double  sentBytes;
    double  recvBytes;
    double  ensembledFrames;
    double  paintFrames;
    char   *codecName;
    u8      quality;
    char   *acronym;
    u32     imageWidth;
    u32     imageHeight;

public:
    stats_t(int channelId, const char *compId);

    ~stats_t(void);

    char *getCompo(void) const { return compId; }

    void reset(void);

    void setDesiredFR          (double dfr);
    void setDesiredBW          (double dbw);
    void accountCodecBytes     (u32 numBytes);
    void accountSentFrame      (void);
    void accountSentBytes      (u32 numBytes);
    void accountRecvBytes      (u32 numBytes);
    void accountEnsembledFrame (void);
    void accountPaintFrame     (void);
    void setCodecInUse         (const char *codecName);
    void setQuality            (u8 q);
    void setAcronym            (const char *acronym);
    void setImageSize          (u32 w, u32 h);

    virtual measures_ref getStatistics(void);
};


// -------------------------------------------------------
// SDU of snapshot
// -------------------------------------------------------

// FORMATO DE PAQUETE DE DATOS:  (type= 1: Data; 0: CTRL)
//
//  | type | channelId | compId |  Data |
//  |  u32 |    u32    | 3*char |       |
//  |<----------HEADER--------->|       !
//
class stats_data_packet_t
{
private:
    int  size;
    u8  *data;

public:
    stats_data_packet_t(measures_ref measures);
    ~stats_data_packet_t();

    inline u8  *serialize(void) { return data; }
    inline int   PDUSize(void)  { return size; }
};


// FORMATO DE PAQUETE DE CONTROL:  (type= 1: Data; 0: CTRL)
//
//  | type | channelId | compId |
//  |  u32 |    u32    | 3*char |
//  |<----------HEADER--------->|
//
class stats_ctrl_packet_t
{
private:
    int   size;
    u8   *data;

public:
    stats_ctrl_packet_t(const char *compId, u32 theChId);

    virtual ~stats_ctrl_packet_t(void);

    inline u8  *serialize(void) { return data; }
    inline int   PDUSize(void)  { return size; }
};


#endif
