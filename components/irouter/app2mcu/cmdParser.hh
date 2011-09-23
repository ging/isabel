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
// $Id: cmdParser.hh,v 1.0 2005/04/12 10:49:01 sgonzalez Exp $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __cmd_parser_hh__
#define __cmd_parser_hh__

#include <string>

using namespace std;

#include <icf2/sockIO.hh>
#include <icf2/general.h>

#include "flow.hh"
#include "configParam.hh"

class cmdParser_t
{
public:
    
    cmdParser_t();
    ~cmdParser_t();

    const char* nop(char* buffer);
    const char* create_session(char* buffer, u16 sessionID);
    const char* remove_session(char* buffer, u16 sessionID);

    const char* new_participant(char* buffer, u16 sessionID, const char* flowServer, const ql_t<string>* flowList, bool localFlow=false);
    const char* configure_participant(char* buffer, u16 sessionID, u16 partID, bool FEC, u32 BW);
    const char* remove_participant(char* buffer, u16 sessionID, u16 partID);
    const char* get_participants(char* buffer, u16 sessionID);

    const char* bind_rtp(char* buffer, u16 sessionID, u16 IDPart, u8 PT, u16 localRTPPort, u16 remoteRTPPort, u16 localRTCPPort);
    const char* unbind_rtp(char* buffer, u16 sessionID, u16 partIDorig, u16 txPort);

    const char* receive(char* buffer,u16 sessionID,u16 ID_partRX,u16 ID_partTX,u8 PT);
    const char* receive_video_mode(char *buffer, u16 sessionID, u16 IDPart);

    const char* receive_video(char* buffer,u16 sessionID,u16 ID_partRX,u16 ID_partTX);

    const char* receive_audio_mode(char *buffer, u16 sessionID, u16 IDPart);

    const char* receive_audio(char* buffer,u16 sessionID,u16 ID_partRX,u16 ID_partTX);


};

#endif
