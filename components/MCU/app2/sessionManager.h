/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: sessionManager.h 7506 2005-10-20 13:29:33Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_SESSION_MANAGER_H_
#define _MCU_SESSION_MANAGER_H_

#include <icf2/stdTask.hh>

#include "general.h"
#include "flowProcessor.h"
#include "output.h"
#include "demux.h"
#include "RTCPRecv.h"
#include "statGatherer.h"

class sessionManager_t: public application_t
{
public:

    class participantInfo_t
    {
    public:
        u16 ID;
        videoInfo_t videoInfo;
        audioInfo_t audioInfo;
        sockaddr_storage    IP;
        ql_t<u16>   portList;

        participantInfo_t(u16,
                          sockaddr_storage const&,
                          ql_t<u16> portList
                         );
        ~participantInfo_t(void);
    };

    class sessionInfo_t
    {
    public:

        u16 ID;
        ql_t<flowProcessor_t *> flowProcessorList;
        vector_t<participantInfo_t *> participantInfoArray;

        sessionInfo_t(u16);
        ~sessionInfo_t(void);
    };

private:

    vector_t<sessionInfo_t *> sessionArray;

    // Receivers
    demux_t *demux;
    RTCPDemux_t *RTCPDemux;

public:

    bool noSL;
    
    vector_t<target_t *> targetArray;

    // RTCP Sender and Stats Gatherer
    statGatherer_t *statGatherer;

    sessionManager_t(int &argc, argv_t &argv);
    virtual ~sessionManager_t(void);

    // session Commands
    HRESULT createSession        (u16 ID_session);
    HRESULT removeSession        (u16 ID_session);

    // participants
    HRESULT newParticipant       (u16 ID_session,
                                  sockaddr_storage const &ip,
                                  ql_t<u16> portList,
                                  u16 *ID
                                 );

    HRESULT removeParticipant    (u16 ID_session,
                                  u16 ID_part
                                 );

    HRESULT configureParticipant (u16 ID_session,
                                  u16 ID_part,
                                  bool alive,
                                  u8  PT,
                                  u8  FEC,
                                  u32 BW
                                 );

    HRESULT getParticipants      (u16 ID_session,
                                  char **participants
                                 );

    HRESULT bindRtp              (u16 ID_session,
                                  u16 ID_part,
                                  u8  PT,
                                  u16 remoteRTPport,
                                  u16 localRTPport,
                                  u16 localRTCPport
                                 );

    HRESULT unbindRtp            (u16 ID_session,
                                  u16 ID_part,
                                  u8  PT
                                 );

    HRESULT bindRtcp             (u16 ID_session,
                                  u16 ID_part,
                                  u16 localRTPport,
                                  u16 remoteRTCPport,
                                  u16 localRTCPport
                                 );

    HRESULT unbindRtcp           (u16 ID_session,
                                  u16 ID_part,
                                  u16 localRTPport,
                                  u16 remoteRTCPport
                                 );

    // codec
    HRESULT getCodecs            (char *codecs);
    HRESULT getAudioCodecs       (char *codecs);
    HRESULT getVideoCodecs       (char *codecs);

    // flows
    HRESULT receive               (u16 ID_session,
                                   u16 ID_part_rx,
                                   u16 ID_part_tx,
                                   u8  PT,
                                   u32 SSRC = 0
                                  );

    HRESULT discard               (u16 ID_session,
                                   u16 ID_part_rx,
                                   u16 ID_part_tx,
                                   u8  PT
                                  );

    HRESULT receiveVideoMode    (u16 ID_session,
                                 u16 ID_part,
                                 videoMode_e mode = VIDEO_SWITCH_MODE,
                                 int  PT = -1,
                                 u32  BW = 0,
                                 u8   FR = 0,
                                 u8   Q  = 0,
                                 u16  Width = 0,
                                 u16  Height = 0,
                                 u32  SSRC = 0,
                                 gridMode_e gridMode = GRID_AUTO
                                );

    HRESULT receiveVideo         (u16 ID_session,
                                  u16 ID_part_rx,
                                  u16 ID_part_tx,
                                  int  PT = -1,
                                  u32  BW = 0,
                                  u8   FR = 0,
                                  u8   Q  = 0,
                                  u16  Width = 0,
                                  u16  Height = 0,
                                  u32  SSRC = 0
                                 );

    HRESULT discardVideo         (u16 ID_session,
                                  u16 ID_part_rx,
                                  u16 ID_part_tx
                                 );

    HRESULT receiveAudioMode    (u16 ID_session,
                                 u16 ID_part,
                                 audioMode_e mode,
                                 int PT= -1,
                                 int SL= -100
                                );

    HRESULT receiveAudio         (u16 ID_session,
                                  u16 ID_part_rx,
                                  u16 ID_part_tx,
                                  u32 SSRC= 0
                                 );

    HRESULT discardAudio         (u16 ID_session,
                                  u16 ID_part_rx,
                                  u16 ID_part_tx
                                 );

    // DTE methods
    HRESULT getAudioLosses       (u16 ID_session,
                                  u16 ID_part,
                                  double *losses
                                 );

    HRESULT getVideoLosses       (u16 ID_session,
                                  u16 ID_part,
                                  double *losses
                                 );

    HRESULT getLosses            (u16 ID_session,
                                  u16 ID_part,
                                  double *losses
                                 );

    HRESULT configAudioPart      (u16 ID_session,
                                  u16 ID_part,
                                  u8  oldPT,
                                  u8  newPT
                                 );

    HRESULT configVideoCBRPart   (u16 ID_session,
                                  u16 ID_part,
                                  u8  oldPT,
                                  u8  newPT,
                                  u32 BW
                                 );

    HRESULT configVideoVBRPart   (u16 ID_session,
                                  u16 ID_part,
                                  u8  oldPT,
                                  u8  newPT,
                                  double FR,
                                  u8  Q,
                                  u16 Width,
                                  u16 Height
                                 );

    // public info methods

    HRESULT getSession(u16 ID_session, sessionInfo_t **session = NULL);

    HRESULT getParticipantByID(sessionInfo_t *session,
                               u16 ID_part,
                               participantInfo_t **participant = NULL
                              );

    HRESULT getParticipantByIPPort(sessionInfo_t *session,
                                   sockaddr_storage const &IP,
                                   ql_t<u16> portList,
                                   participantInfo_t **participant = NULL
                                  );


    HRESULT getSessionByIPPort(sockaddr_storage const& IP, u16 port, int *ID);

    virtual const char *className(void) const { return "sessionManager_t"; }
};

extern sessionManager_t *APP;

#endif

