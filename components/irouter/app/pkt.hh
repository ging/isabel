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
// $Id: pkt.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __pkt_hh__
#define __pkt_hh__


#include <icf2/general.h>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>
#include <icf2/smartReference.hh>

#include "flow.hh"
#include "webirouterConfig.hh"


class link_t;   // externally declared


//
//! Align pointer to u32
//
inline
void *
ptrAlign(void *p)
{
    void *retVal= NULL;

    unsigned long aux= (unsigned long)p;

    aux+= 3;
    aux&= ~0x03;

    retVal= (void *)aux;

    return retVal;
};


//
//! pktTrailer_t -- packet trailer
//
/*!
    Trailer info for irouter's own use
*/

class pktTrailer_t
{
public:

    u32 MAGIK; /*!< magic word */

    pktTrailer_t(void):MAGIK(0xDECADA90){};
    u32  id;         /*!< trailer identifier */
    u32 size;        /*!< size               */
};



const u32 MAGIK_VISITED_HOST= 0x44444444;

//
//! trlVisitedHost_t -- trailer info for irouter's own use
//

class trlVisitedHost_t: public pktTrailer_t
{
public:
    u32 visitedHost;
};



//
//! pktBuf_t -- shared buffer for pkt_t
//

/*!
    Copyless buffer (see sharedPkt_t)
    Only contains data to be routed. Additional
    information is saved in sharedPkt_t.
*/

class pktBuf_t: public virtual item_t,
                public virtual collectible_t
{
public:
    u8    data[MAX_PKT_SIZE];  /*!< user flow data  (payload)  */
    i32  len;                  /*!< data length                */

    //! pktBuf_t constructor
    pktBuf_t(void);

    //! pktBuf_t copy constructor
    pktBuf_t(const pktBuf_t &other);

    //! pktBuf_t  destructor
    virtual ~pktBuf_t(void);

    u32           clientLen(void);

    //! Assign operator
    /*!
        \param other data to be copied
    */
    pktBuf_t &operator=(const pktBuf_t &other);

    //! For ICF2 debugging rites...
    /*! class name string (pktBuf_t) to be printed when using debug messages */
    char const *className(void) const { return "pktBuf_t";};

    friend class smartReference_t<pktBuf_t>;
};

typedef smartReference_t<pktBuf_t> pktBuf_ref;


//
//! pkt_t -- packet abstraction
//

/*!
    Represents a queued packet ready for delivery.
*/

//  a packet could be:
//  --------------------
//  |                  |
//  |       data       |
//  |                  |
//  --------------------
//  \________ _________/
//           V
//     sharedBuf->len
//
//  or ("trlr" stands for "trailer"
//
//                                  total trailer len
//                      ____________________/\_____________________
//                     |                                           |
//  ----------------------------------------------------------------
//  |                  |      |      |      |     | trlr   | total |
//  |       data       | trlr | trlr | trlr | ... | magik  | trlr  |
//  |                  |  id  | len  | data |     | number | len   |
//  ----------------------------------------------------------------
//                                   \__ ___/
//                                      V
//                                  trlr->len
//  \_______________________________ ______________________________/
//                                  V
//                            sharedBuf->len


// ----------
// MI VERSION
// ----------

/*!

<pre>
                         StartTrailer              EndTrailer
                         |                         |
                         |                         |
                         |                         |
        data len         |     all trailers len    |
      _____/\______      |____________/\___________|
     /             \     /                         \
    ------------------------------------------------------------------------
    |                | A |      |      |      |     |        | all   | data |
    |       data     | L | trlr | trlr | trlr | ... | MAGIK  | trlrs |      |
    |                | G | id   | len  | data |     |        | len   | len  |
    ------------------------------------------------------------------------
                    align \_________ ________/      \___________ ___________/
                   (max 3           V                           V
                    bytes)      trlr->len                   last 12 oct
    \_______________________________ ______________________________________/
                                    V
                              sharedBuf->len

</pre>

*/



class
sharedPkt_t: public virtual item_t,
             public virtual collectible_t
{
public:
    struct timeval time;
    flowId_e  flowId;    //!< flow type: audioId, videoId...
    int       target;    //!< an opaque identifier to be resolved within
                         //!  the context of an output netLink (see link_t)
    pktBuf_t *sharedBuf; //!< packetBuffer: copyless data

    link_t   *pktInLink; //!< Input link: link from which packet was read
    link_t   *pktOutLink;//!< Ouput link: link to write the packet

    //! sharedPkt_t constructor
    sharedPkt_t(void);

    //! sharedPkt_t copy constructor
    sharedPkt_t(const sharedPkt_t &other);

    //! sharedPkt_t destructor
    virtual ~sharedPkt_t(void);

    //! Assign operator
    sharedPkt_t &operator =(const sharedPkt_t &other);

    //! Cancel packet. Too much time inside the irouter... old packet
    void cancel(void)
    {
         delete sharedBuf;
         sharedBuf= NULL;
    }

    //! For ICF2 debugging rites...
    /*! class name string (sharedPkt_t) to be printed when using debug messages */
    char const *className(void) const { return "sharedPkt_t";}

    friend class smartReference_t<sharedPkt_t>;
};

typedef smartReference_t<sharedPkt_t> sharedPkt_ref;

#endif

