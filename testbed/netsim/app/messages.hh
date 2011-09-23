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
// $Id: messages.hh 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __MESSAGES__
#define __MESSAGES__

#include <stdlib.h>
#include <icf2/smartReference.hh>
#include <icf2/icfMessages.hh>

const int MAX_PKT_SIZE=16384;

enum typeMsg_t {
    PKTMSG_ID = 21212
};

class pktBuff_t: public virtual collectible_t
{
     public:
         char *data;
         pktBuff_t() { data=NULL;}
         pktBuff_t(char *d) { data=d;}
         virtual ~pktBuff_t() { 
              if (data) 
                  free(data);
              data = NULL;
         }
         friend class smartReference_t<pktBuff_t>;
};

typedef smartReference_t<pktBuff_t> pktBuff_ref;

class pktMsg_t: public tlMsg_t {

    public:
        pktBuff_ref pktBuff;
        int pktLen;

        pktMsg_t():tlMsg_t(PKTMSG_ID) {
            pktBuff=NULL;
            pktLen=0;
        }
        pktMsg_t(pktBuff_ref &buff, int len): tlMsg_t(PKTMSG_ID){ 
             pktBuff = buff;
             pktLen = len;
        }
};

typedef smartReference_t<pktMsg_t> pktMsg_ref;

#endif
