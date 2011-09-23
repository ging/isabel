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
// $Id: mcuSender.cc,v 1.0 2005/04/12 10:49:01 sgonzalez Exp $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include "mcuSender.hh"

#include <aCodecs/codecs.h>
#include <vCodecs/codecs.h>

mcuSender_t::mcuSender_t(irouterParam_t *irouterParam):simpleTask_t(NOP_PERIOD)
{
    Param = irouterParam;    

    inetAddr_t inet1("127.0.0.1", NULL, serviceAddr_t::STREAM);
    sock = new streamSocket_t(inet1, socketIO_t::CLIENT_SOCK);

    launchMCU();

    audioPT= new int[128];
    videoPT= new int[128];

    audioNum = GetCodecPTList(audioPT,128);

    int totalCoders= vGetNumRegisteredCoders();
    u32 *videoPT= new u32[totalCoders];
    videoNum = vGetCodecFmtList(videoPT, totalCoders);

    const char* myFlows[] = { "video",
        "audio",
        "vumeter",
        "pointer",           
        "slidesFtp"
    };

    for (int i =0; i<5; i++){   
        ports[i]=Param->getNetPort(myFlows[i]);
        localports[i] = atoi(Param->getIrouterFlowPort(myFlows[i])) ;
    }
}

mcuSender_t::~mcuSender_t()
{
    if (sock) delete sock;
    if (audioPT) delete audioPT;
    if (videoPT) delete audioPT;
}

void 
mcuSender_t::launchMCU(void)
{
    char cmd[1024];
    NOTIFY("Arrancando MCU\n");
#ifdef __BUILD_FOR_LINUX
    sprintf(cmd,"/usr/local/isabel/bin/isabel_mcu -cport %d",CPORT);
#elif WIN32
    sprintf(cmd,"mcu.exe -cport %d",CPORT);
#endif
    if (Param->getNotifyFile() != NULL)
    {
        sprintf(cmd,"%s -notify %s",cmd,Param->getNotifyFile());
    }

#ifdef __BUILD_FOR_LINUX
    sprintf(cmd,"%s &",cmd);
    system(cmd);
    sleep (1);
#elif WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );



    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        cmd,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
        ) 
    {
        printf( "CreateProcess failed (%d)\n", GetLastError() );        
        exit(-1);
    }
#endif 
    char port[8];
    sprintf(port, "%d",CPORT);    
    inetAddr_t inet2("127.0.0.1",port, serviceAddr_t::STREAM);
    while (sock->connect(inet2)!=0)
    {
        NOTIFY("mcuSender_t::mcuSender_t: trying to connect to MCU...\n");
#ifdef __BUILD_FOR_LINUX
        sleep(1); 
#endif
#ifdef __BUILD_FOR_WINDOWS
        Sleep(1000);
#endif
    }
}

void 
mcuSender_t::heartBeat(void)
{
    nop_t nop;
    sendCommand(&nop);
}

int 
mcuSender_t::sendCommand(mcuCommand_t* command)
{
    if (command->getType() != mcuCommand_t::nop)
    {
        NOTIFY("Command : %s", command->getCommandStr());
    }
    while(sock->write(command->getCommandStr(),(size_t)strlen(command->getCommandStr())) < 0)
    {
        NOTIFY("MCU is dead\n");
        exit(-1);
    }
    
    char temp[1024];
    memset(temp,0,1024);
    if(sock->read(temp, 1024)<0)
    {
        NOTIFY("MCU is dead\n");
        exit(-1);
    }
    if (command->getType() !=  mcuCommand_t::nop)
    {
        NOTIFY("Response = %s", temp);
    }
    command->setResponse(&temp[0]);

    if (command->getType() == mcuCommand_t::new_participant)
    {
        newParticipant_t* tmp = static_cast<newParticipant_t*>(command);
        definedClients.insert(tmp->getIP(), atoi(&temp[4])); 
        printf("Insertado cliente con ip : %s , nombre %d \n", tmp->getIP(), atoi(&temp[4]));
    }
    else if (command->getType() == mcuCommand_t::remove_participant){
        removeParticipant_t *tmp = static_cast<removeParticipant_t*>(command);
        const char* ip = tmp->getIP();
        definedClients.remove(ip);
    }
    return 0;    
}

ql_t<u16> *mcuSender_t::getIDList(){
    return definedClients.getValues();
}

u16  mcuSender_t::getUserIDByIP(const char* IP){
    return definedClients.lookUp(IP);
}

int mcuSender_t::newParticipant(const char* IP){
    int ret = -1;
    int * used_ports = NULL;

    if (strstr(IP,"127.0.0.1") > 0 || strcmp(IP,"localhost") == 0)
    { 
        NOTIFY("New participant localhost\n");
        used_ports = localports;
    }
    else
    {
        used_ports = ports;
    }

    int partID = -1;
    ql_t<string> flowList;
    string temp;
    char buffer [16];
    memset(&buffer,0,16);
    for (int i = 0; i<NPORTS; i++)
    {
        sprintf(buffer, "%i", used_ports[i]);
        string temp = string(buffer);
        flowList.insert(temp);
    }
    newParticipant_t newParticipant(0,IP,&flowList);
    sendCommand(&newParticipant);
    partID = definedClients.lookUp(IP);
    if (partID >= 0)
    {
        if (bindAll(partID)==0)
        {
            ret = 0;
        }
    }
    return ret;
}

int
mcuSender_t::bindAll(int participant)
{ // We assume the participants want to receive every flow
    
    if (participant == 0){ // participant 0 is always localhost
        NOTIFY("Bindind video for localhost \n");
        for (int i = 0; i<videoNum; i++){
            bindRTP_t bindRTP(0, participant,videoPT[i], localports[0]-1, localports[0], 0); 
            sendCommand(&bindRTP);
        }
        NOTIFY("Binding audio for participant %i \n", participant);
        for (int i = 0; i<audioNum; i++){
            bindRTP_t bindRTP(0, participant,audioPT[i], localports[1]-1, localports[1], 0); 
            sendCommand(&bindRTP);
        }
        NOTIFY("Binding vumeter for participant %i \n", participant);
        bindRTP_t bindRTPvu(0, participant,22, localports[2]-1, localports[2], 0); 
        sendCommand(&bindRTPvu);

        NOTIFY("Binding pointer for participant %i \n", participant);
        bindRTP_t bindRTPpo(0, participant,44, localports[3]-1, localports[3], 0); 
        sendCommand(&bindRTPpo);

        NOTIFY("Binding FTP for participant %i \n", participant);
        bindRTP_t bindRTPftp(0, participant,43, localports[4]-1, localports[4], 0); 
        sendCommand(&bindRTPftp);

    }else{
        NOTIFY("Binding video for participant %i ports %i \n", participant, ports[0]);
        for (int i = 0; i<videoNum; i++){
            bindRTP_t bindRTP(0, participant,videoPT[i], ports[0], ports[0], 0); 
            sendCommand(&bindRTP);
        }
        NOTIFY("Binding audio for participant %i \n", participant);
        for (int i = 0; i<audioNum; i++){
            bindRTP_t bindRTP(0, participant,audioPT[i], ports[1], ports[1], 0); 
            sendCommand(&bindRTP);
        }
        NOTIFY("Binding vumeter for participant %i ports %i \n", participant,ports[2]);
        bindRTP_t bindRTPvu(0, participant,22, ports[2], ports[2], 0); 
        sendCommand(&bindRTPvu);
        NOTIFY("Binding pointer for participant %i \n", participant);
        bindRTP_t bindRTPpo(0, participant,44, ports[3], ports[3], 0); 
        sendCommand(&bindRTPpo);
        NOTIFY("Binding FTP for participant %i \n", participant);
        bindRTP_t bindRTPftp(0, participant,43, ports[4], ports[4], 0); 
        sendCommand(&bindRTPftp);
    }

    receiveVideoMode_t receiveVideoMod(0,participant);
    sendCommand(&receiveVideoMod);
    receiveAudioMode_t receiveAudioMod(0,participant);   
    sendCommand(&receiveAudioMod);

    // RECEIVES

    ql_t<u16> *listaID = getIDList();
    for (ql_t<u16>::iterator_t i = listaID->begin(); i!=listaID->end(); i++){
        u16 p = static_cast<int>(i);
        if (p!= participant){

            receiveVideo_t receiveVideo (0,p,participant);        
            sendCommand(&receiveVideo);
            receiveVideo_t receiveVideorec (0,participant,p);
            sendCommand(&receiveVideorec);

            receiveAudio_t receiveAudio (0,p,participant);
            sendCommand(&receiveAudio);
            receiveAudio_t receiveAudiorec (0,participant,p);
            sendCommand(&receiveAudiorec);

            receive_t receiveVumeter(0,participant,p,22);
            sendCommand (&receiveVumeter);
            receive_t receiveVumeterrec(0,p,participant,22);
            sendCommand (&receiveVumeterrec);

            receive_t receivePointer(0,participant,p,44);
            sendCommand (&receivePointer);

            receive_t receivePointerrec(0,p,participant,44);
            sendCommand (&receivePointerrec);

            receive_t receiveftp (0,participant,p,43);
            sendCommand (&receiveftp);

            receive_t receiveftprec (0,p,participant,43);
            sendCommand (&receiveftprec);

        }
    }
    return 0;
}
