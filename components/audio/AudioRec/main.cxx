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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <pthread.h>

#include "CtrlInterface.h"

char *history[] = {
  "4/09/03-001\n\tVersión Inicial.\n"
};

void info(int soloUno);

int wantedToQuit = 0;
int ofile = 0;
char outfilename[1024] = "";
int started = 0;

char *funcar_start(int nargs, char *args[]) {
   int N_ARGS = 1+1;
   
   if (nargs < N_ARGS ) return strdup("ERROR : Faltan argumentos\n");
#ifdef DEBUG 
   for ( int i = 0 ; i < nargs ; i++ ) printf("%s ", args[i]);   printf("\n");
#endif
   if(ofile) {
     close(ofile);
   }
   strcpy(outfilename, args[1]);
   if ( (ofile = open(args[1],O_WRONLY|O_APPEND, 0666)) < 0 ) {
     printf("name = %s\n", outfilename);
     ofile = 0;
     return strdup("ERROR. Abriendo el fichero.\n");
   }
   started = 1;
   return strdup("OK\n");
}

char *funcar_restart(int nargs, char *args[]) {
  started = 1;
  return strdup("OK\n");
}

char *funcar_stop(int nargs, char *args[]) {
  started = 0;
  return strdup("OK\n");
}

char *funcar_nop(int nargs, char *args[]) {
  return strdup("OK\n");
}

char *funcar_quit(int nargs, char *args[]) {
  wantedToQuit = 1;
  return strdup("OK\n");
}



Directive comandos[] = {
  {"ar_start", funcar_start, "ar_start(fichero_donde_escribes). Comienza a copiar el contenido del fichero de entrada en este fichero."},
  {"ar_restart", funcar_restart, "ar_restart(). No abre el fichero, sigue escribiendo en el mismo que ya había abierto."},
  {"ar_stop", funcar_stop, "No copia datos al fichero de salida pero sigue leyendo del de entrada."},
  {"ar_nop", funcar_nop, "Devuelve OK."},
  {"ar_quit", funcar_quit, "Termina el programa y cierra la conexión telnet."}	
};

#define NCOMANDOS (sizeof(comandos)/sizeof(Directive))

void
usage(void)
{
  fprintf(stderr, "\tAudioRec -c controlPort -i inputfile\n\n");
}

void handler(int signal) {
  //   printf("Recibida señal %d\n", signal);
  //   printf("Terminando\n");
   wantedToQuit = 1;
}


int main(int argc, char *argv[]) {
  
  char *displayName=":0.0";
  int ctrlSocket;
  unsigned short int audiorecControlPort = 29669;
  ControlInterface *ci;
  int ifile = 0;
  char infilename[1024];
  struct timeval tv0;
  
  /*
   * parse args
   *
   */
  if (argc == 1) {
    usage();
    exit(1);
  }
  if ( (argc==2) && ( (!strcmp(argv[1],"-V")) || (!strcmp(argv[1],"--version")) ) ) {
    info(1);
  }
  if ( (argc==2) && ( (!strcmp(argv[1],"-H")) || (!strcmp(argv[1],"--history")) ) ) {
    info(0);
  }
  {
    int	c;
    extern char	*optarg;
    extern int	optind;
    while ((c = getopt(argc, argv, "c:i:o:")) != -1) {
      switch (c) {
      case 'c': {
	audiorecControlPort= atoi(optarg);
	break;	
      }
      case 'i': {
	strcpy(infilename, optarg);
	break;	
      }
      }
    }
  }
  if ( (ifile = open(infilename, O_RDONLY, 0)) < 0 ) {
    printf("Error abriendo <%s>\n", infilename);
    ifile = 0;
  }

  ci = ControlInterfaceNew(audiorecControlPort);
  if (!ci) {
    printf("Error Creando Interfaz de control\n");
    return -1;
  }
  if (( audiorecControlPort = ControlInterfaceInit(ci)) < 0 ) {
    printf("Error\n");
    return -1;
  }
  for ( int i = 0 ; i < NCOMANDOS ; i++ ) {
    if ( ControlInterfaceRegisterDirective(ci, comandos[i].directive, comandos[i].function, comandos[i].help) < 0 ) {
      printf("Error2\n");
      return -1;
    }
  }
  
  printf("%d\n", audiorecControlPort); fflush(stdout);
  
  ControlInterfaceListen(ci);
  /*
  if ( ControlInterfaceStart(ci) < 0 ) {
    printf("Error3\n");
    return -1;
  }
  */
  ctrlSocket = accept(ci -> socket, NULL, 0);
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
    int elapsed;
    struct timeval tv1;

    
    FD_ZERO(&rdFd);
    FD_SET(ifile, &rdFd);
    FD_SET(ctrlSocket, &rdFd);
    /*
    if ( fstat(ctrlSocket, &s) != 0 ) {
      wantedToQuit = 1;
      printf("fstat !=0\n");
      break;
    }
    */
    tv.tv_sec= 0;
    tv.tv_usec= 100000;
    if(select(ctrlSocket+1, &rdFd, NULL, NULL, &tv) > 0) {
      
      if(FD_ISSET(ctrlSocket, &rdFd)) {
	ControlInterfaceDeal(ci, ctrlSocket);
      }
      
      if(FD_ISSET(ifile, &rdFd)) {
	int rcount;
	unsigned char rdata[1024];
	
	rcount = read(ifile, rdata, 1024);
	if ( rcount < 0) {
	  perror("IOReady::\n");
	  if (!rcount) return 1;
	  return -1;
	}
	printf("Leidos %d\n", rcount);
	if (started) {
	  if (ofile) {
	    rcount = write(ofile, rdata, rcount);
	    printf("Escritos %d\n", rcount);
	  }
	}
      }
      
    }
    gettimeofday(&tv1, NULL);
    elapsed = ((tv1.tv_sec*1000000+tv1.tv_usec)-(tv0.tv_sec*1000000+tv0.tv_usec))/1000;
    
    if (elapsed > 500) {
      memcpy(&tv0, &tv1, sizeof(tv0));
      printf("500ms\n");
    }
  }
  if (ofile) close(ofile);
  close(ifile);
  /*
  ControlInterfaceStop(ci);
  */
  ControlInterfaceDelete(ci);
}

void info(int soloUno) {
	static char* id="$Id: main.cxx 20206 2010-04-08 10:55:00Z gabriel $";
	int i = 0;

	printf("%s\n", id);
	if (soloUno) {
	  printf("%s\n", history[0]);
	} else {
	  while (history[i]) {
	    printf("%s\n", history[i]);
	    i++;
	  }
	}
	exit(0);
}












