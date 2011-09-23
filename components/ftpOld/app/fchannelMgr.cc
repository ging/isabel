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
// $Id: fchannelMgr.cc 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/item.hh>
#include <icf2/stdTask.hh>

#include "fchannelMgr.h"
#include "cftpRTPSession.h"


////////////////////////////////////
//   the channel mgr 
////////////////////////////////////


fchannelMgr_t::fchannelMgr_t () {
    for(int i= 0; i< APP_MAX_CHANNELS ; i++) {
        channelList[i]= NULL;
    }
    numChannels = 0;
}


fchannelMgr_t::~fchannelMgr_t() {
    for(int j= 0; j< numChannels; j++) {
        if (channelList[j])
            delete channelList[j];
    }
}



fileTrans_t * 
fchannelMgr_t::getChannel (u32 id) {
    for(int i= 0; i< numChannels; i++) {
        if (!channelList[i]) {
            NOTIFY("getChannel:: channel list corrupted \n");
            return NULL;
        }
	
        if (channelList[i]->id == id) {
            return channelList[i];
        }
    }
    
    return  0;
}


bool
fchannelMgr_t::insertChan (fileTrans_t *chan) {
    if (getChannel(chan->id) != 0) {
        NOTIFY("insertChan:: channel %d already inserted\n", chan->id);
        return false;
    }
    
    if (numChannels == APP_MAX_CHANNELS) {
        NOTIFY("insertChan:: cannot insert more channels [%d]\n", chan->id);
        return false;
    }
    
    channelList[numChannels] = chan;
    numChannels ++;
    
    return true;
}

bool
fchannelMgr_t::delete_channel (u32 cid) {
    for(int i= 0; i< numChannels; i++) {
        if(channelList[i]->id  == cid){
	    // borra el canal
	    delete channelList[i]; 
	    channelList[i] = NULL;
	    
	    // dec numero de canales
	    numChannels--;
	    
	    // recoloca para que los canales ocupados
	    // esten siempre al principio
	    channelList[i]= channelList[numChannels];
	    
	    channelList[numChannels]= NULL;
            rtpSession->deleteSender(cid, "channel deleted");
	    return true;
        }
    }
    
    return false;
}

