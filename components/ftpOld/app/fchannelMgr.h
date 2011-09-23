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
// $Id: fchannelMgr.h 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#ifndef	__FCHANNELMGR_H__
#define	__FCHANNELMGR_H__

#include "ctrlProtocol.h"
#include "fileTrans.h"


////////////////////////////////////////////
//                                        //
//	Class name : fchannelMgr_t         //
//                                        //
////////////////////////////////////////////

static const int APP_MAX_CHANNELS=1024;

//
// fchannelMgr_t -- file channel manager
//
class fchannelMgr_t
{
public:
    
    // fchannelMgr_t constructor
    fchannelMgr_t ();
    
    // fchannelMgr_t destructor
    virtual  ~fchannelMgr_t();
    
    
    /* Delete a channel
     *
     * u32 =  channel number
     */
    bool delete_channel (u32);
    
    
    /* Get channel for a channel number
     * 
     * u32 = channel number
     */
    fileTrans_t   * getChannel (u32);
    

    /* Insert channel
     * 
     * fileTrans_t = channel object
     */
    bool insertChan (fileTrans_t *);

private:
    
    int  numChannels;  //number of channels

    fileTrans_t * channelList [APP_MAX_CHANNELS]; // list of channels
};




#endif




