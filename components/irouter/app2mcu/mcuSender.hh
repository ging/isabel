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
// $Id: mcuSender.hh,v 1.0 2005/04/12 10:49:01 sgonzalez Exp $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __mcu_sender_hh__
#define __mcu_sender_hh__

#include <string>

using namespace std;

#include <icf2/dictionary.hh>
#include <icf2/sockIO.hh>
#include <icf2/notify.hh>
#include <icf2/task.hh>

#include "configParam.hh"
#include "mcuCommand.hh"

class mcuSender_t: public simpleTask_t
{
public:
    static const int NOP_PERIOD = 1000000; //1 sec.
    static const int CPORT  = 55555;
    static const int NPORTS = 5;

    mcuSender_t(irouterParam_t *irouterParam);
    ~mcuSender_t();

    int sendCommand(mcuCommand_t* command);
    ql_t<u16> *getIDList();

    u16 getUserIDByIP(const char* IP);
    int newParticipant(const char* IP);

private:

    void heartBeat(void);

    dictionary_t<string, u16> definedClients;
    streamSocket_t *sock;

    irouterParam_t *Param;
    int ports[NPORTS];
    int localports[NPORTS];    

    int *audioPT;
    int *videoPT;
    int audioNum;
    int videoNum;

    int bindAll(int participant);
    void launchMCU(void);
};

#endif
