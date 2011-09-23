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
// $Id: mcuCommand.hh,v 1.0 2005/04/12 10:49:01 sgonzalez Exp $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __mcu_command_hh__
#define __mcu_command_hh__

#include <string>

using namespace std;

#include <icf2/general.h>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>

#include "cmdParser.hh"

class mcuCommand_t
{
public:
    enum cmdType_t
    {
        nop,
        create_session            ,
        new_participant            ,
        configure_participant    ,
        bind_rtp                ,
        receive                    ,
        unbind_rtp                ,
        remove_participant        ,
        remove_session ,
        receive_video_mode,
        receive_video,
        receive_audio_mode,
        receive_audio
    };

    int setResponse(const char*);
    const char* getCommandStr();

    mcuCommand_t(cmdType_t);
    ~mcuCommand_t();

    cmdType_t getType();

protected:
    
    char            buffer[1024];
    cmdType_t        cmdType;
    char            cmdString[1024];
    char            response[1024];
    u16                sessionID;
    cmdParser_t        cmdParser;


};

class nop_t:public mcuCommand_t
{
public:
    nop_t();
    ~nop_t();

};

class createSession_t:public mcuCommand_t
{
public:
    createSession_t();
    ~createSession_t();

};

class newParticipant_t:public mcuCommand_t
{
public:
    newParticipant_t(u16 session, const char* IP, const ql_t<string>* ports);
    ~newParticipant_t();

    const char* getIP();

private:
    char                    IPAddr[16];
};

class removeParticipant_t:public mcuCommand_t
{
public:
   removeParticipant_t(u16 session, u16 ID, const char* IP);
   const char *getIP();
   ~removeParticipant_t();

   u16 getID();
private:
   u16 partID;
   char IPAddr[16];
};

class bindRTP_t:public mcuCommand_t
{
public:
    bindRTP_t(u16 session, u16 ID, u8 PT, u16 remoteRTPport, u16 localRTPport, u16 localRTCPport);
    ~bindRTP_t();

};

class receive_t:public mcuCommand_t
{
public:
    receive_t(u16 session, u16 ID_partRX, u16 ID_partTX, u8 PT);
    ~receive_t();

};


class receiveVideoMode_t :public mcuCommand_t
{
public : 
   receiveVideoMode_t(u16 session, u16 ID); // siempre switch mode
   ~receiveVideoMode_t();
};

class receiveVideo_t : public mcuCommand_t
{
public:
   receiveVideo_t (u16 session, u16 ID_partRX, u16 ID_partTX);
   ~receiveVideo_t();
};


class receiveAudioMode_t :public mcuCommand_t
{
public : 
   receiveAudioMode_t(u16 session, u16 ID); // siempre switch mode
   ~receiveAudioMode_t();
};

class receiveAudio_t : public mcuCommand_t
{
public:
   receiveAudio_t (u16 session, u16 ID_partRX, u16 ID_partTX);
   ~receiveAudio_t();
};
#endif

