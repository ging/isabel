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
#ifndef	__CtrlInterface_H__
#define	__CtrlInterface_H__

#include "Socket.h"
// pthreads
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <unistd.h>


#define MAX_COMMANDS	256

typedef char *(*Function)(int nargs, char *args[]);

typedef struct {
   char *directive;
   Function function;
   char *help;
} Directive;

typedef struct {
   Socket *socket;
   int port;
   Directive directives[MAX_COMMANDS];
   int nDirectives;
   int started;
   int listen;
   pthread_t netThread;
  int exitDirective;
} ControlInterface;

ControlInterface *ControlInterfaceNew(int port);
int ControlInterfaceInit(ControlInterface *ci);
int ControlInterfaceListen(ControlInterface *ci);
int ControlInterfaceDelete(ControlInterface *ci);
int ControlInterfaceRegisterDirective(ControlInterface *ci, const char *directive, Function function, const char *help);
int ControlInterfaceRegisterDirectiveSet(ControlInterface *ci, int nDirectives, Directive *d);
int ControlInterfaceStart(ControlInterface *ci);
int ControlInterfaceStop(ControlInterface *ci);
int ControlInterfaceDeal(ControlInterface *ci, Socket *newSocket);
int ControlInterfaceSetExitDirective(ControlInterface *ci, int directive);

#endif

