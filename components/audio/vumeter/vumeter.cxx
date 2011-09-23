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

//#define DEBUG


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <pthread.h>

#include "CtrlInterface.h"
#include "XVumeter.h"
#include "VumeterClient.h"

// #define DEBUG 1
#define MAX_VUMETERS 1024

class VumeterServer {
  pthread_mutex_t lock;
  XVumeter *vumeters[MAX_VUMETERS];
  struct timeval lastUpdate[MAX_VUMETERS];
 public:
  VumeterServer();
  ~VumeterServer();

  int AddVumeter(Display *dpy, Window w, int vuID);
  int DeleteVumeter(int id);

  int IOReady(Socket *s);
  void DecayAll();

  int UpdateVumeter(int id, int power);
  int SetSizeVumeter(int id, int w, int h);
  int GetInfo(char *data, int datasize);
};

VumeterServer::VumeterServer() {
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    vumeters[i] = NULL;
    gettimeofday(&(lastUpdate[i]), NULL);
  }
  pthread_mutex_init(&lock, NULL);
}

VumeterServer::~VumeterServer() {
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if (vumeters[i]) delete vumeters[i];
  }
}

// No se por que deben ser mayores que 0
int VumeterServer::AddVumeter(Display *dpy, Window w, int vuID) {
  int index;

  pthread_mutex_lock(&lock);
  // Comprobar que no esté ya creado
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if (vumeters[i]) {
      if (vumeters[i] -> GetID() == vuID) {
	pthread_mutex_unlock(&lock);
	return 1;
      }
    }
  }
  index = -1;
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if ( !vumeters[i] ) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    pthread_mutex_unlock(&lock);
    return -1;
  }
  vumeters[index] = XVumeter::Create(dpy, w, vuID);
  pthread_mutex_unlock(&lock);
  if (!vumeters[index]) return -1;
  else return 1;
}

int VumeterServer::DeleteVumeter(int id) {
  int valor = -1;

  pthread_mutex_lock(&lock);
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if (vumeters[i]) {
      if (vumeters[i] -> GetID() == id) {
	if ( vumeters[i] ) delete vumeters[i];
	vumeters[i] = NULL;
	valor = 1;
	break;
      }
    }
  }
  pthread_mutex_unlock(&lock);
  return valor;
}

int VumeterServer::UpdateVumeter(int id, int power) {
  int valor = -1;

  pthread_mutex_lock(&lock);
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if (vumeters[i]) {
      if (vumeters[i] -> GetID() == id) {
	gettimeofday(&(lastUpdate[i]), NULL);
	vumeters[i] -> SetPower(power);
	valor = 1;
	break;
      }
    }
  }
  pthread_mutex_unlock(&lock);
  return valor;
}

int VumeterServer::SetSizeVumeter(int id, int w, int h) {
  int valor = -1;

  pthread_mutex_lock(&lock);
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if (vumeters[i]) {
      if (vumeters[i] -> GetID() == id) {
	gettimeofday(&(lastUpdate[i]), NULL);
	vumeters[i] -> SetSize(w, h);
	valor = 1;
	break;
      }
    }
  }
  pthread_mutex_unlock(&lock);
  return valor;
}

int VumeterServer::GetInfo(char *data, int datasize) {
  struct timeval tv;
  float elapsed;
  char cadena[64*1024] = "";

  gettimeofday(&tv, NULL);
  pthread_mutex_lock(&lock);
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if (vumeters[i]) {
      char aux[1024] = "";
      elapsed = ((tv.tv_sec*1000000+tv.tv_usec)-(lastUpdate[i].tv_sec*1000000+lastUpdate[i].tv_usec))/1000;
      //      printf("[Vumetro %d actualizado hace %02.2fs]\n", i, elapsed/1000.0);
      sprintf(aux, "Vumetro %d actualizado hace %02.2fs\n", vumeters[i] -> GetID(), elapsed/1000.0);
      strcat(cadena, aux);
    }
  }
  pthread_mutex_unlock(&lock);
  strncpy(data, cadena, datasize);
  return 1;
}


void VumeterServer::DecayAll() {

  pthread_mutex_lock(&lock);
  for ( int i = 0 ; i < MAX_VUMETERS ; i++ ) {
    if (vumeters[i]) {
      vumeters[i] -> Decay();
    }
  }
  pthread_mutex_unlock(&lock);
}

int VumeterServer::IOReady(Socket *s) {
  unsigned char buffer[1024*64];
  VumeterPacket vp;
  int rcount;

  rcount = s->Read(buffer, 1024*64);
  if ( rcount < sizeof(vp)) {
    perror("IOReady::\n");
    if (!rcount) return 1;
    return -1;
  }

  RTPPacket *rtp;	      
  rtp = RTPPacketNew(buffer, rcount);
  if(!rtp) return -1;
	
  memcpy(&vp, rtp -> data, rtp -> len);

  RTPPacketDelete(rtp);
  vp.index = ntohl(vp.index);
  //  vp.power = ntohl(vp.power);
  return UpdateVumeter(vp.index, vp.power);
}

VumeterServer vs;  
int wantedToQuit = 0;
Display	*dpy= NULL;


char *funcVumeterNew(int nargs, char *args[]) {
   int N_ARGS = 1+2;
   Window w;
   int vuID;
   
   if (nargs < N_ARGS ) return strdup("ERROR : Faltan argumentos\n");
#ifdef DEBUG 
   for ( int i = 0 ; i < nargs ; i++ ) printf("%s ", args[i]);   printf("\n");
#endif
   sscanf(args[1], "0x%x", &w);
   vuID = atol(args[2]);
   if ( (vs.AddVumeter(dpy, w, vuID)) < 0 ) {
      return strdup("ERROR\n");
   }
   return strdup("OK\n");
}

char *funcVumeterDel(int nargs, char *args[]) {
   int N_ARGS = 1+1;
   int index;
   
   if (nargs < N_ARGS ) return strdup("ERROR : Faltan argumentos\n");
#ifdef DEBUG 
   for ( int i = 0 ; i < nargs ; i++ ) printf("%s ", args[i]);   printf("\n");
#endif
   index = (int)strtol(args[1], NULL, 10);
   if ( vs.DeleteVumeter(index) < 0 ) {
      return strdup("ERROR\n");
   }
   return strdup("OK\n");
}

char *funcVumeterSetPower(int nargs, char *args[]) {
   int N_ARGS = 1+2;
   int index, power;
   
   if (nargs < N_ARGS ) return strdup("ERROR : Faltan argumentos\n");
#ifdef DEBUG 
   for ( int i = 0 ; i < nargs ; i++ ) printf("%s ", args[i]);   printf("\n");
#endif
   index = atoi(args[1]);
   power = atoi(args[2]);
   if ( vs.UpdateVumeter(index, power) < 0 ) {
      return strdup("ERROR\n");
   }
   return strdup("OK\n");
}

char *funcVumeterSetSize(int nargs, char *args[]) {
   int N_ARGS = 1+2;
   int index, w, h;
   
   if (nargs < N_ARGS ) return strdup("ERROR : Faltan argumentos\n");
#ifdef DEBUG 
   for ( int i = 0 ; i < nargs ; i++ ) printf("%s ", args[i]);   printf("\n");
#endif
   index = atoi(args[1]);
   w = atoi(args[2]);
   h = atoi(args[3]);
   if ( vs.SetSizeVumeter(index, w,h) < 0 ) {
      return strdup("ERROR\n");
   }
   return strdup("OK\n");
}

char *funcVumeterNop(int nargs, char *args[]) {
   return strdup("OK\n");
}

char *funcVumeterQuit(int nargs, char *args[]) {
  wantedToQuit = 1;
  return strdup("OK\n");
}



Directive comandos[] = {
  {"vumeter_quit", funcVumeterQuit, "Termina el programa y cierra la conexión telnet."},	
  {"vumeter_new", funcVumeterNew, "vumeter_new(winid, vumid). Crea un nuevo vumetro gráfico en la ventana winid y lo denomina vumid. Devuelve OK."},
  {"vumeter_setsize", funcVumeterSetSize, "vumeter_setsize(vumid, w,h). Cambia el tamaño."},
  {"vumeter_del", funcVumeterDel, "vumeter_del(vumid). Borra el vumetro vumid."},
  {"vumeter_nop", funcVumeterNop, "Devuelve Ok. Para comprobar que el demonio está vivo."},	
  {"vumeter_setpower", funcVumeterSetPower, "vumeter_setpower(vumid, power). Activa el vumetro vumid al valor power."}
};

#define NCOMANDOS (sizeof(comandos)/sizeof(Directive))

char *funcInfo(int nargs, char *args[]) {
  char cadena[1024] ="";

  vs.GetInfo(cadena, 1024);
  return strdup(cadena);
}

Directive comandos2[] = {
  {"vumeter_info", funcInfo, "Muestra información util para depuración."}	
};

#define NCOMANDOS2 (sizeof(comandos2)/sizeof(Directive))


void
usage(void)
{
  fprintf(stderr, "vumeter [-c controlPort] [-d dataPort] display\n");
}

void handler(int signal) {
  //   printf("Recibida señal %d\n", signal);
  //   printf("Terminando\n");
   wantedToQuit = 1;
}


int main(int argc, char *argv[]) {
  
  char *displayName=":0.0";
  Socket *ctrlSocket;
  Socket *dataSocket;
  int XSocket;
  unsigned short int vumeterDataPort = 18190;
  unsigned short int vumeterControlPort = 29669;
  ControlInterface *ci;
  ControlInterface *ci2;
  struct timeval tv0;
 
  /*
   * parse args
   *
   */
  {
    int	c;
    extern char	*optarg;
    extern int	optind;
    while ((c = getopt(argc, argv, "c:d:")) != -1) {
      switch (c) {
      case 'c':
	vumeterControlPort= atoi(optarg);
	break;
	
      case 'd':
	vumeterDataPort= atoi(optarg);
	break;
	
      case '?':
	printf("Ignoring bad input from command line\n");
	exit(1);
	break;
      }
    }
    argc-= optind;
    argv+= optind;
    if(argc!= 1) {
      usage();
      exit(1);
    }
    displayName= argv[0];
  }
    
  dpy = XOpenDisplay(displayName);

  if(!dpy) {
    perror(" NO DPY ");
    exit(1);
  }
      
  XSocket= XConnectionNumber(dpy);
  {
    Address *localAddr = new Address(PF);
    if ( localAddr->Resolve(NULL, vumeterDataPort) < 0 ) {
      perror("error");
      return -1;
    }
    dataSocket = new Socket(localAddr->Getprotocol(), SOCK_DGRAM);
    if ( dataSocket -> Bind(localAddr) < 0 ) {
      perror("bind 2\n");
      return -1;
    }
    delete localAddr;
  }  
  
  {
    ci2 = ControlInterfaceNew(6502);
    if (!ci2) {
      printf("Error Creando Interfaz de control 2\n");
      return -1;
    }
    if (( ControlInterfaceInit(ci2)) < 0 ) {
      printf("Error 2\n");
      return -1;
    }
    for ( int i = 0 ; i < NCOMANDOS2 ; i++ ) {
      if ( ControlInterfaceRegisterDirective(ci2, comandos2[i].directive, comandos2[i].function, comandos2[i].help) < 0 ) {
	printf("Error2 2\n");
	return -1;
      }
    }
  }  

  ci = ControlInterfaceNew(vumeterControlPort);
  if (!ci) {
    printf("Error Creando Interfaz de control\n");
    return -1;
  }
  if (( vumeterControlPort = ControlInterfaceInit(ci)) < 0 ) {
    printf("Error\n");
    return -1;
  }
  for ( int i = 0 ; i < NCOMANDOS ; i++ ) {
    if ( ControlInterfaceRegisterDirective(ci, comandos[i].directive, comandos[i].function, comandos[i].help) < 0 ) {
      printf("Error2\n");
      return -1;
    }
  }
  ControlInterfaceSetExitDirective(ci, 0);
  printf("%d\n", vumeterControlPort); fflush(stdout);
  
#if 0
  if(fork())
    exit(0);
  else {
    if (freopen("/dev/tty", "a+", stdout) == NULL) {
      if (freopen("$HOME/.isabel/logs/vumeter.log", "a+", stdout) == NULL) {
	if (freopen("/dev/null", "a+", stdout) == NULL) {
	  system("touch /tmp/kkvumeter");
	  exit(1);
	}
      }
    }
    if (freopen("/dev/tty", "a+", stderr) == NULL) {
      if (freopen("$HOME/.isabel/logs/vumeterlog", "a+", stderr) == NULL) {
	if (freopen("/dev/null", "a+", stderr) == NULL) {
	  system("touch /tmp/kkvumeter2");
	  exit(1);
	}
      }
    }
  }
#endif
  ControlInterfaceListen(ci2);
  ControlInterfaceStart(ci2);

  ControlInterfaceListen(ci);
  /*
  if ( ControlInterfaceStart(ci) < 0 ) {
    printf("Error3\n");
    return -1;
  }
  */
  ctrlSocket = ci -> socket->Accept(NULL);
  /*
  fcntl(ctrlSocket, O_NONBLOCK);
  */
  signal(SIGPIPE, handler);
  signal(SIGINT, handler);
  signal(SIGQUIT, handler);

  gettimeofday(&tv0, NULL);

  while(!wantedToQuit) {
    struct timeval	tv;
    fd_set	rdFd;		
    XEvent	event;
    int elapsed;
    struct timeval tv1;

    
    FD_ZERO(&rdFd);
    FD_SET(XSocket, &rdFd);
    FD_SET(dataSocket->GetFD(), &rdFd);
    FD_SET(ctrlSocket->GetFD(), &rdFd);
    /*
    if ( fstat(ctrlSocket, &s) != 0 ) {
      wantedToQuit = 1;
      printf("fstat !=0\n");
      break;
    }
    */
    tv.tv_sec= 0;
    tv.tv_usec= 100000;
    if(select(ctrlSocket->GetFD()+1, &rdFd, NULL, NULL, &tv) > 0) {
      
      if(FD_ISSET(XSocket, &rdFd)) {
	XNextEvent(dpy, &event);
      }
      
      if(FD_ISSET(ctrlSocket->GetFD(), &rdFd)) {
	if (ControlInterfaceDeal(ci, ctrlSocket) < 0 ) wantedToQuit = 1;
      }

      if(FD_ISSET(dataSocket->GetFD(), &rdFd)) {
	vs.IOReady(dataSocket);
      }
    } else {
      //      vs.DecayAll();
      /*
      // Comprobar que no han cerrado las conexiones
      rcount = recv(ctrlSocket, dummy, 0, MSG_PEEK);
      if (rcount <= 0) {
	perror("Lo que estoy depurando");
      } else {
	printf("rcount = %d\n", rcount);
      }
      */
    }
    gettimeofday(&tv1, NULL);
    elapsed = ((tv1.tv_sec*1000000+tv1.tv_usec)-(tv0.tv_sec*1000000+tv0.tv_usec))/1000;
    
    if (elapsed > 500) {
      vs.DecayAll();
      memcpy(&tv0, &tv1, sizeof(tv0));
    }
  }
  
  dataSocket->Close();
  /*
  ControlInterfaceStop(ci);
  */
  ControlInterfaceDelete(ci);
}













