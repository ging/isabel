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
// $Id: jitterTask.hh 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __JITTER__TASK__
#define __JITTER__TASK__

#include "netTask.hh"

class jitterTask_t: public netTask_t {
    private:

       class pktNode_t: public virtual collectible_t {
           public:
               pktBuff_ref pBuff;
               int         pLen;

               pktNode_t(pktBuff_ref &pktBuff, int pktLen)
               : pBuff(pktBuff), pLen(pktLen) { }
               
           friend class smartReference_t<pktNode_t>;
       };

       typedef smartReference_t<pktNode_t> pktNode_ref;

       ql_t<pktNode_ref> pktNodeList;

       static netsimInit_t netsimInit;

       int __minBurst;
       int __maxBurst;
       int __prob;
       int __burstSize;

       void __dispatchPktNode();

    public:
        jitterTask_t(int minBurst, int maxBurst, int prob)
        : __minBurst(minBurst), __maxBurst(maxBurst), __prob(prob),
          __burstSize(0) 
        {
            debugMsg(dbg_App_Normal, "jitterTask_t", "Burst=[%d, %d]\n",
                     __minBurst, __maxBurst);
        }

        virtual void recvPkt(pktBuff_ref &pktBuff, int pktLen); 

        virtual const char *className() const {return "jitterTask_t";}

};

#endif
