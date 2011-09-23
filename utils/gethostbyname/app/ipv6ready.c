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
/* ----------------------------------------------------------------------
 *
 * $Id: ipv6ready.c 20206 2010-04-08 10:55:00Z gabriel $
 *
 * (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
 * Dec 31, 1999 Transfered to Agora Systems S.A.
 * (C) Copyright 2000-2006. Agora Systems S.A.
 *
   ------------------------------------------------------------------- */

#include <sys/socket.h>    
#include <sys/types.h>    
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    
    if (socket(PF_INET6,SOCK_DGRAM,0) == -1) {
	exit(1);
    } else {
	exit(0);
    }
}

