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
// $Id: mcuCommand.cc,v 1.0 2005/04/12 10:49:01 sgonzalez Exp $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include "mcuCommand.hh"


mcuCommand_t::mcuCommand_t(cmdType_t type)
{
    memset(buffer,0,1024);
    memset(cmdString,0,1024);
    memset(response,0,1024);
    cmdType = type;
}

mcuCommand_t::~mcuCommand_t()
{

}

mcuCommand_t::cmdType_t mcuCommand_t::getType()
{
    return cmdType;
}

int 
mcuCommand_t::setResponse(const char* string)
{
    strcpy(&response[0],string);
    return 0;
}

const char*
mcuCommand_t::getCommandStr()
{
    return this->cmdString;
}

nop_t::nop_t():mcuCommand_t(/*mcuCommand_t::cmdType_t::*/nop)
{
    strcpy(cmdString,cmdParser.nop(&buffer[0]));
}

nop_t::~nop_t()
{

}

createSession_t::createSession_t():mcuCommand_t(/*mcuCommand_t::cmdType_t::*/create_session)
{
    strcpy(cmdString,cmdParser.create_session(&buffer[0],0));
}

createSession_t::~createSession_t()
{

}

newParticipant_t::newParticipant_t(u16 session,
                                   const char* IP, 
                                   const ql_t<string>* ports
                                  )
: mcuCommand_t(/*mcuCommand_t::cmdType_t::*/new_participant)
{
    memset(&IPAddr,0,16);

    strcpy(IPAddr,IP);
    sessionID = session;
    bool localFlow = false;

    if (strcmp(IP,"localhost")==0 || strstr(IP,"127.0.0.1")>0)
    {
        localFlow = true;
    }

    strcpy(cmdString,cmdParser.new_participant(&buffer[0],session,IP,ports,localFlow));
}

newParticipant_t::~newParticipant_t()
{

}


const char* newParticipant_t::getIP()
{
    return IPAddr;
}

removeParticipant_t::removeParticipant_t(u16 session,u16 ID, const char* IP):mcuCommand_t(/*mcuCommand_t::cmdType_t::*/remove_participant)
{
   partID = ID;
   memset(&IPAddr,0,16);
   strcpy(IPAddr,IP);

   strcpy(cmdString,cmdParser.remove_participant(&buffer[0],session,ID));
}

removeParticipant_t::~removeParticipant_t()
{

}
u16  removeParticipant_t::getID(){
   return(partID);
}

const char* removeParticipant_t::getIP()
{
    return IPAddr;
}

bindRTP_t::bindRTP_t(u16 session, 
                     u16 ID, u8 PT, 
                     u16 remoteRTPport, 
                     u16 localRTPport, 
                     u16 localRTCPport):mcuCommand_t(/*mcuCommand_t::cmdType_t::*/bind_rtp)
{
    strcpy(cmdString,cmdParser.bind_rtp(&buffer[0],session,ID,PT,remoteRTPport,localRTPport,localRTCPport));
}

bindRTP_t::~bindRTP_t()
{

}

receive_t::receive_t(u16 session, 
                     u16 ID_partRX, 
                     u16 ID_partTX, 
                     u8 PT):mcuCommand_t(/*mcuCommand_t::cmdType_t::*/receive)
{
    strcpy(cmdString,cmdParser.receive(&buffer[0],session,ID_partRX,ID_partTX,PT));
}

receive_t::~receive_t()
{

}

receiveVideoMode_t::receiveVideoMode_t(u16 session, u16 ID):mcuCommand_t(receive_video_mode) // siempre switch mode       
{     
   strcpy(cmdString,cmdParser.receive_video_mode(&buffer[0],session,ID));
}
receiveVideoMode_t::~receiveVideoMode_t()
{

}


receiveVideo_t::receiveVideo_t(u16 session, 
                     u16 ID_partRX, 
                     u16 ID_partTX):mcuCommand_t(/*mcuCommand_t::cmdType_t::*/receive_video)
{
    strcpy(cmdString,cmdParser.receive_video(&buffer[0],session,ID_partRX,ID_partTX));
}

receiveVideo_t::~receiveVideo_t()
{

}

receiveAudioMode_t::receiveAudioMode_t(u16 session, u16 ID):mcuCommand_t(receive_audio_mode) // siempre switch mode       
{     
   strcpy(cmdString,cmdParser.receive_audio_mode(&buffer[0],session,ID));
}

receiveAudioMode_t::~receiveAudioMode_t()
{

}


receiveAudio_t::receiveAudio_t(u16 session, 
                               u16 ID_partRX, 
                               u16 ID_partTX
                              )
: mcuCommand_t(/*mcuCommand_t::cmdType_t::*/receive_audio)
{
    strcpy(cmdString,
           cmdParser.receive_audio(&buffer[0],
                                   session,
                                   ID_partRX,
                                   ID_partTX
                                  )
           );
}

receiveAudio_t::~receiveAudio_t()
{

}

