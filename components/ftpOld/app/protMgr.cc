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
// $Id: protMgr.cc 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include "protMgr.h"

#include "fileTrans.h"
#include "cftpApp.h"
#include "fchannelMgr.h"
#include <icf2/sockIO.hh>


#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>


extern    cftpApp_t * theApp;


//------------------------------------------------------------------

protState::protState() {

    packets = NULL;

/*
    for (int i=0 ; i<MAX_NUM_PKT ; i++) {
	packets[i] = NULL;
    }
*/
}

protState::~protState() {

    if (packets)
	delete [] packets;
    
/*
    for (int i=0 ; i<MAX_NUM_PKT ; i++) {
	if (packets[i])
	    delete packets[i];
    }
*/
}

//------------------------------------------------------------------

protS_Send::protS_Send (char *fname, u32 chanId, char * f) {

    alr      = NULL;

    debugMsg(dbg_App_Normal, "protS_Send", 
             "sending fname=%s chid=%d path=%s\n", 
             fname, chanId, f);
    channelId = chanId;  
    read_file (fname, chanId, f);
}

protS_Send::~protS_Send () 
{ 
    debugMsg(dbg_App_Normal, "~protS_Send", "Deleting task\n");

    if (alr.isValid()) 
        del_alarm (alr);

}

bool
protS_Send::read_file (char * fullname, u32 chanId, char * fname) {

    struct stat    sbuf;

    int file;
    int len;
    int fsize;

    // opening the file
 
    if (access(fullname, R_OK)){
        NOTIFY("read_file:: file does not exist\n");
        return false;
    }
  
    if ((file = open (fullname, O_RDONLY) ) < 0){
        /* mensaje de error */
        NOTIFY("read_file:: could not open file [%s] errno=[%s]\n",
	       fullname,strerror(errno));
        return false;
    }
  
    if (stat ( fullname , &sbuf) == -1) {
        NOTIFY("read_file:: could not stat file\n");
        close (file);
        return false;
    }
  
    // reading the file
    // reserva 256 bytes de sobra para meter en el primer
    // paquete el nombre del fichero

    fsize =  sbuf.st_size + 256;

    npackets  = fsize / DATA_P_L;
    if ((npackets * DATA_P_L) < fsize)
        npackets ++;

    packets = new appData_t[npackets];
    if (!packets) {
	NOTIFY("read_file: Could not allocate memory\n");
	NOTIFY("read_file: Bailing out\n");
	exit(1);
    }

/*
    if (npackets > MAX_NUM_PKT) {
        NOTIFY("read_file:: Could not send this big file [%d] "
               "maximum file size=%d!\n", fsize, MAX_NUM_PKT*DATA_P_L);
        close(file);
        return false;
    }
*/

    debugMsg(dbg_App_Normal, "read_file", 
             "File=%s npackets=%d\n", fullname, npackets);

    for(int i= 0; i < npackets; i++) {

/*
	packets[i] = new appData_t;
	if (!packets[i]) {
	    NOTIFY("read_file:: Could not allocate memory\n");
	    NOTIFY("read_file: Bailing out\n");
	    exit(1);
	}
*/

        packets[i].dataChannel  = chanId;
        packets[i].frameNumber  = i;
        packets[i].dataSize     = fsize;
        if (i == 0) {
            sprintf ((char *) packets[i].dataData, "%s", fname);
            len= read ( file, 
                       (char * ) (& packets[i].dataData[256]) , 
                        DATA_P_L - 256) ;
        } else 
            len= read (file, packets[i].dataData , DATA_P_L) ;

        debugMsg(dbg_App_Verbose, "read_file",
                 "fsize=%d, pkt=%d , lenPkt=%d\n", fsize, i, len);

        if  (len < 0) {
            NOTIFY("read_file:: error reading from file=%s\n", fullname);
	    close (file);
            return false;
        }
    }
    close (file);
    return true;
}




void
protS_Send::start() {
    // Preparado para enviar el primer paquete
    index    = 0;
    nextStart = 0;

    if (npackets < 1) {
        NOTIFY("protS_Send::start: zero packets to send\n");
	return;
    }

    __sendingPkt(); // Envia el primer paquete
}
    
void
protS_Send::__sendingPkt() {

    // traffic shaping: start time
    struct timeval startTime;
    gettimeofday (&startTime, NULL);
    unsigned long startusec = startTime.tv_sec * 1000000 + startTime.tv_usec;

    // traffic shaping: error due to doing some tasks
    unsigned long errorusec = 0;
    if (nextStart != 0) {
	errorusec = startusec - nextStart;
    }

    // envio original de los paquetes
    appData_t *  pkt = packets+index;
    pkt->dataChannel = htonl(pkt->dataChannel);
    pkt->frameNumber = htonl(pkt->frameNumber);
    pkt->dataSize    = htonl(pkt->dataSize);

    u32 timestamp = (u32)(startusec / 1000000.0 / FTP_RTP_TSU);
    rtpSession->sendData(channelId,
                         (u8*)pkt,
                         sizeof(appData_t),
                         FTP_PT,
                         false,
                         (u16)pkt->frameNumber,
                         timestamp
                        );
    // Si el PC es lento y no puede seguir el ritmo de envio, quitar el comentario. SPG + Eduardo
    // usleep (10);

    pkt->dataChannel = ntohl(pkt->dataChannel);
    pkt->frameNumber = ntohl(pkt->frameNumber);
    pkt->dataSize    = ntohl(pkt->dataSize);

    index ++;

    // traffic shaping: time after sending packet
    struct timeval nowTime;
    gettimeofday (&nowTime, NULL);
    unsigned long nowusec = nowTime.tv_sec * 1000000 + nowTime.tv_usec;
    
    if (index == npackets) {
	index = 0;  // empezar otra vez
    }

    // traffic shaping: wait for time to send next packet
    nextStart = startusec + timeout;
    unsigned long sleeptime = 1;
    if (nextStart > nowusec+errorusec) {
	sleeptime = nextStart-nowusec-errorusec;
    }
    alr =  set_alarm(sleeptime, 0);

}

void        
protS_Send::timeOut(const alarm_ref &al) 
{
    __sendingPkt();    // envio infinito de paquetes
}


void 
protS_Send::setTimeOut(int tout) 
{ 
   timeout=tout; 
}


bool 
protS_Send::receive (appData_t  *p) {
    // Un emisor no deberia recibir data...
    delete p;
    return false;
} 



//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

protS_Recv::protS_Recv(char *f, u32 chanId)
{ 
    channelId = chanId;
    fname = strdup (f);
    done = NULL;
}

protS_Recv::~protS_Recv () 
{ 
    debugMsg(dbg_App_Normal, "~protS_Recv", "Deleting task\n");
    free (fname);
    if (!done) delete [] done; 
}


bool
protS_Recv::write_file (char * fullname) {
    int fd;
    char name [512];

    dir_check (fullname);
    if ((fd = open (fullname, O_WRONLY|O_CREAT, 0666)) == -1) {
        NOTIFY("write_file:: Could not open file [%s] "
               "to write errno=[%s]\n", name, strerror(errno));
        return false;
    }

    // Escribiendo el primer paquete
    int sizeb= DATA_P_L;
    if (npackets==1) 
         sizeb =  packets[0].dataSize % DATA_P_L;

    int n= write ( fd, 
                    (char * ) (& packets[0].dataData [256]), 
                    sizeb -256); 
    if (n != sizeb -256) {
          NOTIFY("write_file:: error when writting the first pkt\n");
          close(fd);
          return false;
    }
    
    // Escribiendo el resto de paquetes
    for (int i = 1; i < npackets; i++) {
        sizeb = DATA_P_L;
        if (i == npackets -1)
            sizeb =  packets[i].dataSize % DATA_P_L;

        if (write (fd, packets[i].dataData, sizeb) != sizeb) {
            NOTIFY("write_file:: error when writting the pkt\n");
            close(fd);
            return false;
        }
    }

    close (fd);
    return true;

}


void
protS_Recv::dir_check (char * file)  {
    if (file[0] == '\0')
        return;

    for (int i = strlen(file); i > 0; i --) {
        if (file[i] == '/') {
            file[i] = '\0';
            dir_check (file);
            mkdir (file, 0755);
            file[i] = '/';
            return;
        }
    }
}


void
protS_Recv::start() 
{
    index   = 0;
    npackets = 0;
}


bool 
protS_Recv::receive (appData_t *p)
{
    // asegurarme que es para mi
    debugMsg(dbg_App_Paranoic, "receive",
             "llega el frame=%d \n", p->frameNumber);
 
    // si es el primero calculo el numero de paquetes
    // que ocupa el fichero que estoy recibiendo
    if (index ==0) {
        int fsize = p->dataSize;
        npackets  = fsize / DATA_P_L;
        if ((npackets * DATA_P_L) < fsize)
            npackets ++;

	packets = new appData_t[npackets];
	if (!packets) {
	    NOTIFY("read_file:: Could not allocate memory\n");
	    NOTIFY("read_file:: Bailing out\n");
	    exit(1);
	}

	done = new bool[npackets];
	if (!done) {
	    NOTIFY("read_file:: Could not allocate memory\n");
	    NOTIFY("read_file:: Bailing out\n");
	    exit(1);
	}
	for (int i=0 ; i<npackets ; i++) {
	    done[i] = false;  
	}



    } 
  
    // almaceno el frame
    int i = p->frameNumber;

    if (!done[i]) {
	done[i] = true;  
        index ++;
        packets[i] = *p;
    }
    delete p;
 
    // si es el ultimo escribo el fichero
    if (index == npackets)
        write_file (fname);

    return true;
}


void 
protS_Recv::setTimeOut(int tout) 
{ 
}
