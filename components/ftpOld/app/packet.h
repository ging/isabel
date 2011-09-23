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
// $Id: packet.h 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#ifndef	__PACKET_H__
#define	__PACKET_H__

#include <icf2/general.h>
#include <string.h>

const int DATA_P_L = 850;

// Tamanio del paquete de datos =
//     3*4 + DATA_P_L(800) = 862 octetos
struct appData_t
{
   u32	dataChannel;           // numero de canal
   u32	frameNumber;           // indice del frame dentro del fichero
   u32	dataSize;              // tamaño del fichero
   u8	dataData[DATA_P_L];    // datos
};

#endif
