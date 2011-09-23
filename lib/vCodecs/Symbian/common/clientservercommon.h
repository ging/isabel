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
/*
* ============================================================================
*  Name     : ClientServerCommon.h
*  Part of  : SIPHONE
*  Created  : 14.09.2005 by Fernando Escribano
*  Description:
*     Codec client and server commons resources
*  Version  :
*  Copyright: 
* ============================================================================
*/



#ifndef __CLIENTSERVERCOMMON_H__
#define __CLIENTSERVERCOMMON_H__

// INCLUDE FILES
#include <e32base.h>

#define XDIM 128
#define YDIM 96
#define BPP  3


// CONSTANTS
_LIT( KCodecServerName,"CodecServer" ); // Server name
_LIT( KCodecServerSemaphoreName, "CodecServerSemaphore" );

// The server version. A version must be specified when
// creating a session with the server.
const TUint KCodecServMajorVersionNumber=0;
const TUint KCodecServMinorVersionNumber=1;
const TUint KCodecServBuildVersionNumber=1;

// DATA TYPES
// Opcodes used in message passing between client and server
enum TCodecServRqst
    {
    ECodecServEncode,
	ECodecServDecode
    };

#endif // __CLIENTSERVERCOMMON_H__

// End of File
