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
// $Id: cmdParser.cc,v 1.0 2005/04/12 10:49:01 sgonzalez Exp $
//
/////////////////////////////////////////////////////////////////////////


#include "cmdParser.hh"

cmdParser_t::cmdParser_t()
{
}

cmdParser_t::~cmdParser_t()
{

}
const char* 
cmdParser_t::nop(char* buffer)
{
    sprintf(buffer,"nop()\n");
    return buffer;
}

const char* 
cmdParser_t::create_session(char* buffer, u16 sessionID)
{
    strcat(buffer,"create_session(");
    char session[16];
    //itoa(sessionID, &session[0], 10);
    sprintf(session,"%d",sessionID);
    strcat(buffer, session);
    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::remove_session(char* buffer, u16 sessionID)
{
    strcat(buffer,"remove_session(");
    char session[16];
    //itoa(sessionID, &session[0], 10);
    sprintf(session,"%d",sessionID);
    strcat(buffer, session);
    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::new_participant(char* buffer,
                             u16 sessionID,
                             const char* IPaddr,
                             const ql_t<string>* flowList,
                             bool localFlow
                            )
{
    strcat(buffer,"new_participant(");
    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");
    strcat(buffer,IPaddr);
    
    for (ql_t<string>::iterator_t i = flowList->begin();
         i != flowList->end();
         i++
        )
    {
        string flowName = i;
        const char *flowStr = flowName.c_str();
        
        strcat(buffer,",");
        strcat(buffer, flowStr);
    }
    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::configure_participant(char* buffer, u16 sessionID, u16 partID, bool FEC, u32 BW)
{
    strcat(buffer,"configure_participant(");
    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::remove_participant(char* buffer, u16 sessionID, u16 partID)
{
    strcat(buffer,"remove_participant(");
    
    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");
    
    char part[16];
//    itoa(partID, &part[0], 10);
        sprintf(part, "%d",partID);
    strcat(buffer, part);

    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::get_participants(char* buffer, u16 sessionID)
{    
    strcat(buffer,"get_participants(");
    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::bind_rtp(char* buffer, u16 sessionID, u16 IDPart, u8 PT, u16 localRTPPort, u16 remoteRTPPort, u16 localRTCPPort)
{
    strcat(buffer,"bind_rtp(");

    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");

    char part[16];
//    itoa(IDPart, &part[0], 10);
    sprintf(part,"%d",IDPart);
    strcat(buffer, part);
    strcat(buffer,",");

    char PTbuff[16];
    //itoa(PT, &PTbuff[0], 10);
    sprintf(PTbuff,"%d",PT);
    strcat(buffer, PTbuff);
    strcat(buffer,",");

    char port[16];
    //itoa(localRTPPort, &port[0], 10);
    sprintf(port,"%d",localRTPPort);
    strcat(buffer, port);
    strcat(buffer,",");

    //itoa(remoteRTPPort, &port[0], 10);
    sprintf(port,"%d",remoteRTPPort);
    strcat(buffer, port);
    strcat(buffer,",");

    //itoa(localRTCPPort, &port[0], 10);
    sprintf(port,"%d",localRTCPPort);
    strcat(buffer, port);
    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::unbind_rtp(char* buffer, u16 sessionID, u16 partIDorig, u16 txPort)
{
    strcat(buffer,"unbind_rtp(");
    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::receive(char* buffer,u16 sessionID,u16 ID_partRX,u16 ID_partTX,u8 PT)
{
    strcat(buffer,"receive(");
    
    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");

    char part[16];
    //itoa(ID_partRX, &part[0], 10);
    sprintf(part, "%d",ID_partRX);
    strcat(buffer, part);
    strcat(buffer,",");

    //itoa(ID_partTX, &part[0], 10);
    sprintf(part, "%d",ID_partTX);
    strcat(buffer, part);
    strcat(buffer,",");

    //itoa(PT, &part[0], 10);
    sprintf(part, "%d", PT);
    strcat(buffer, part);

    strcat(buffer, ")\n");
    return buffer;
}

const char* 
cmdParser_t::receive_video_mode(char* buffer, u16 sessionID, u16 partID)
{
    strcat(buffer,"receive_video_mode(");
    
    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");
    
    char part[16];
//    itoa(partID, &part[0], 10);
        sprintf(part, "%d",partID);
    strcat(buffer, part);

    strcat(buffer, ", SWITCH_MODE)\n");
    
    return buffer;
}


const char* 
cmdParser_t::receive_video(char* buffer,u16 sessionID,u16 ID_partRX,u16 ID_partTX)
{
    strcat(buffer,"receive_video(");
    
    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");

    char part[16];
    //itoa(ID_partRX, &part[0], 10);
    sprintf(part, "%d",ID_partRX);
    strcat(buffer, part);
    strcat(buffer,",");

    //itoa(ID_partTX, &part[0], 10);
    sprintf(part, "%d",ID_partTX);
    strcat(buffer, part);

//PRUEBA    strcat(buffer,",96");

    strcat(buffer, ")\n");
    return buffer;
}



const char* 
cmdParser_t::receive_audio_mode(char* buffer, u16 sessionID, u16 partID)
{
    strcat(buffer,"receive_audio_mode(");
    
    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");
    
    char part[16];
//    itoa(partID, &part[0], 10);
        sprintf(part, "%d",partID);
    strcat(buffer, part);

    strcat(buffer, ", SWITCH_MODE)\n");
    return buffer;
}


const char* 
cmdParser_t::receive_audio(char* buffer,u16 sessionID,u16 ID_partRX,u16 ID_partTX)
{
    strcat(buffer,"receive_audio(");
    
    char session[16];
//    itoa(sessionID, &session[0], 10);
        sprintf(session, "%d",sessionID);
    strcat(buffer, session);
    strcat(buffer,",");

    char part[16];
    //itoa(ID_partRX, &part[0], 10);
    sprintf(part, "%d",ID_partRX);
    strcat(buffer, part);
    strcat(buffer,",");

    //itoa(ID_partTX, &part[0], 10);
    sprintf(part, "%d",ID_partTX);
    strcat(buffer, part);

    strcat(buffer, ")\n");
    return buffer;
}
