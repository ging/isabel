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
// $Id: stats.cc 20791 2010-07-08 14:25:47Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#if  defined(__BUILD_FOR_LINUX)
#include <netinet/in.h>
#elif defined(WIN32)
// winsock2 includes ntohs, ntohl, etc. included in icf2/general.h
#else
#error "Please, include or implement ntohs, ntohl for your O.S."
#endif

#include <icf2/general.h>
#include <icf2/icfTime.hh>

#include "stats.hh"

#define MAX_STRING_LEN 15

measures_t::measures_t(void)
{
    m.compId   = NULL;
    m.codecName= NULL;
    m.acronym  = NULL;
}

measures_t::~measures_t(void)
{
    if(m.compId != NULL) {
        free(m.compId);
    }
    if(m.codecName != NULL) {
        free(m.codecName);
    }
    if(m.acronym != NULL) {
        free(m.acronym);
    }
}

stats_t::stats_t(int channelId, const char *compId)
: chId(channelId)
{
    reset();
    this->compId= strdup(compId);
    desiredFR   = 0;
    desiredBW   = 0;
    codecName   = NULL;
    quality     = 0;
    acronym     = NULL;
    imageWidth  = 0;
    imageHeight = 0;
}

stats_t::~stats_t(void)
{
    free(compId);
    if(codecName != NULL) {
        free(codecName);
        codecName= NULL; // paranoid
    }
    if(acronym != NULL) {
        free(acronym);
        acronym= NULL; // paranoid
    }
}

void
stats_t::reset(void)
{
    gettimeofday(&startTime, NULL);
    codecBytes     = 0;
    sentFrames     = 0;
    sentBytes      = 0;
    recvBytes      = 0;
    ensembledFrames= 0;
    paintFrames    = 0;
}

void
stats_t::setDesiredFR(double dfr)
{
    desiredFR= dfr;
}

void
stats_t::setDesiredBW(double dbw)
{
    desiredBW= dbw;
}

void
stats_t::accountCodecBytes(u32 numBytes)
{
    codecBytes += numBytes;
}

void
stats_t::accountSentFrame(void)
{
    sentFrames++;
}

void
stats_t::accountSentBytes(u32 numBytes)
{
    sentBytes += numBytes;
}

void
stats_t::accountRecvBytes(u32 numBytes)
{
    recvBytes += numBytes;
}

void
stats_t::accountEnsembledFrame(void)
{
    ensembledFrames++;
}

void
stats_t::accountPaintFrame(void)
{
    paintFrames++;
}

void
stats_t::setCodecInUse(const char *theCodecName)
{
    if(codecName != NULL) {
        free(codecName);
        codecName= NULL;
    }

    if(    (theCodecName == NULL)
        || (strlen(theCodecName) == 0)) {
        return;
    }

    int len= strlen(theCodecName);
    len = (len > MAX_STRING_LEN) ? MAX_STRING_LEN : len;

    codecName= (char*)malloc(len+1);
    strncpy(codecName, theCodecName, len);
    codecName[len]= '\0';
}

void
stats_t::setQuality(u8 q)
{
    quality= q;
}

void
stats_t::setAcronym(const char *theAcronym)
{
    if(acronym != NULL) {
        free(acronym);
        acronym= NULL;
    }

    if(    (theAcronym == NULL)
        || (strlen(theAcronym) == 0)) {
        return;
    }

    int len= strlen(theAcronym);
    len = (len > MAX_STRING_LEN) ? MAX_STRING_LEN : len;

    acronym= (char*)malloc(len+1);
    strncpy(acronym, theAcronym, len);
    acronym[len]= '\0';
}

void
stats_t::setImageSize(u32 w, u32 h)
{
    imageWidth = w;
    imageHeight= h;
}

measures_ref
stats_t::getStatistics(void)
{
    measures_ref result= new measures_t;

    gettimeofday(&currTime, NULL);
    double elapsed= (currTime.tv_sec -startTime.tv_sec )+
                    (currTime.tv_usec-startTime.tv_usec)/1000000.0;

    result->m.chId       = (u32)(chId);
    result->m.compId     = strdup(compId);
    result->m.desiredFR  = (u32)(100*desiredFR);
    result->m.desiredBW  = (u32)(100*desiredBW);
    result->m.codecBW    = (u32)(100*8*codecBytes/elapsed);
    result->m.sentFR     = (u32)(100*sentFrames/elapsed);
    result->m.sentBW     = (u32)(100*8*sentBytes/elapsed);
    result->m.recvBW     = (u32)(100*8*recvBytes/elapsed);
    result->m.ensembledFR= (u32)(100*ensembledFrames/elapsed);
    result->m.paintFR    = (u32)(100*paintFrames/elapsed);
    result->m.codecName  = (codecName == NULL) ? strdup("") : strdup(codecName);
    result->m.quality    = (u32)(quality);
    result->m.acronym    = (acronym  == NULL) ? strdup("") : strdup(acronym);
    result->m.imageWidth = (u32)imageWidth;
    result->m.imageHeight= (u32)imageHeight;

    return result;
}

// FORMATO DE PAQUETE DE DATOS:  (type= 1: Data; 0: CTRL)
//
//  | type | compId | channelId | Data |
//  |  u8  | 3*char |    u32    |      |
//  |<----------HEADER--------->|      |
//
stats_data_packet_t::stats_data_packet_t(measures_ref measures)
{
    u8  X8;
    u32 X32;

    size =   11*sizeof(u32)                  // 11 u32 fields
           +  3*sizeof(char)                 // 1 component ID field
           +  2*sizeof(u8)                   // 2 u8 field
           + 1 + strlen(measures->m.codecName)  // codec name + len
           + 1 + strlen(measures->m.acronym);   // acronym name + len

    data= (u8*)malloc(size);
    u8 *aux= data;

    // Header
    X8= 1;     // type=1 : DATA PDU
    memcpy(aux, &X8, 1); aux += 1;

    memcpy(aux, measures->m.compId, 3); aux += 3;

    X32= htonl(measures->m.chId);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);

    // Data
    X32= htonl(measures->m.desiredFR);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.desiredBW);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.codecBW);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.sentFR);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.sentBW);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.recvBW);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.ensembledFR);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.paintFR);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);

    X8= strlen(measures->m.codecName);
    memcpy(aux, &X8, 1);                     aux += 1;
    memcpy(aux, measures->m.codecName, X8);  aux += X8;

    X8= measures->m.quality;
    memcpy(aux, &X8, 1);                     aux += 1;

    X8= strlen(measures->m.acronym);
    memcpy(aux, &X8, 1);                     aux += 1;
    memcpy(aux, measures->m.acronym, X8);    aux += X8;

    X32= htonl(measures->m.imageWidth);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);
    X32= htonl(measures->m.imageHeight);
    memcpy(aux, &X32, sizeof(u32));          aux += sizeof(u32);

    assert(((aux - data) == size) && "stats_data_packet_t length erroneous!");
}

stats_data_packet_t::~stats_data_packet_t(void)
{
    free(data);
}

// FORMATO DE PAQUETE DE CONTROL:  (type= 1: Data; 0: CTRL)
//
//  | type | compId | channelId |
//  |  u8  | 3*char |    u32    |
//  |<----------HEADER--------->|
//
stats_ctrl_packet_t::stats_ctrl_packet_t(const char *compId, u32 theChId)
{
    u8   X8;
    u32  X32;
    u8  *aux;

    size= 1 + 4 + 3;
    data= (u8*)malloc(size);

    aux= data;

    X8= 0; //type=0 CONTROL PDU
    memcpy(aux, &X8, 1); aux += 1;

    memcpy(aux, compId, 3); aux += 3;

    X32= htonl(theChId);
    memcpy(aux, &X32, sizeof(u32)); aux += sizeof(u32);

    assert(((aux - data) == size) && "stats_ctrl_packet_t length erroneous!");
}

stats_ctrl_packet_t::~stats_ctrl_packet_t(void)
{
    free(data);
}

