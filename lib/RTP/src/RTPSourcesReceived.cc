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
// $Id: RTPSourcesReceived.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPSourcesReceived.hh>

RTPFlowsReceived_t::RTPFlowsReceived_t (void)
{
    debugMsg(dbg_App_Verbose, "RTPFlowsReceived_t", "Creating RTPFlowsReceived_t");
    flowDictionary = new dictionary_t<u32, RTPSource_t*>;
}

RTPFlowsReceived_t::~RTPFlowsReceived_t (void)
{
    debugMsg(dbg_App_Verbose,
             "~RTPFlowsReceived_t",
             "Deleting RTPFlowsReceived_t"
            );        

    ql_t<u32> *sourceList = flowDictionary -> getKeys();

    for (ql_t<u32>::iterator_t i = sourceList -> begin();
         i != sourceList -> end();
         i++
        )
    {
        u32 ssrc = static_cast<u32>(i);
        RTPSource_t *source = flowDictionary -> lookUp(ssrc);
        delete source;
    }

    delete sourceList;
    delete flowDictionary;
}

void 
RTPFlowsReceived_t::createFlow (u32 id, double tsUnit)
{
    RTPSource_t *source = flowDictionary -> lookUp (id); 

    if (source != NULL)
    {
        debugMsg(dbg_App_Normal, "createFlow", "Flow already created");
        return;
    }

    source = new RTPSource_t (id, tsUnit);
    flowDictionary -> insert (id, source);
}

void 
RTPFlowsReceived_t::deleteFlow (u32 id)
{
    RTPSource_t *source = flowDictionary -> lookUp (id);
    if (source == NULL)
    {
        debugMsg(dbg_App_Normal, "deleteFlow", "Flow doesn't exist");
        return;
    }
    
    delete source;
    flowDictionary -> remove (id);
}

bool 
RTPFlowsReceived_t::processRTPPacket (RTPPacket_t *packet, double tsUnit)
{
    u32 ssrc = packet -> getSSRC();
    u16 seqnum = packet -> getSequenceNumber();
    u32 timestamp = packet -> getTimestamp ();
    ql_t <u32> *csrcList = packet -> getCSRCList ();
    RTPSource_t *source = flowDictionary -> lookUp (ssrc);
    if (source == NULL)
    {
        if (!getenv("DINAMIC"))
        {
            debugMsg(dbg_App_Normal, "processRTPPacket", "Flow doesn't exist");
            return false;
        
        }else{
            
            RTPSource_t *source = new RTPSource_t (ssrc, tsUnit);
            flowDictionary -> insert (ssrc, source);
            int status = source -> processPacket (seqnum, timestamp, csrcList);
            if (!status)
                return true;
        }
    
    }else{
        
        int status = source -> processPacket (seqnum, timestamp, csrcList);
        if (!status)
        {
            debugMsg(dbg_App_Verbose, "processRTPPacket", "RTP Packet processed correctly");
            return true;
        }
    }
    
    debugMsg(dbg_App_Normal, "processRTPPacket", "RTP Packet processed incorrectly");
    return false;
}

void 
RTPFlowsReceived_t::processSRInfo (u32 ssrc, SenderInfo_t *si, double tsUnit)
{
    RTPSource_t *source = flowDictionary -> lookUp (ssrc);
    if (source == NULL)
    {
        if ( ! getenv("DYNAMIC"))
        {
            debugMsg(dbg_App_Normal,
                     "processSenderInfo",
                     "Source doesn't exist"
                    );
            return;
        
        }
        else
        {
            RTPSource_t *source = new RTPSource_t (ssrc, tsUnit);
            flowDictionary -> insert (ssrc, source);
            source -> processSenderInfo (si);
        }
    }
    else
    {
        source -> processSenderInfo (si); 
    } 
}

void 
RTPFlowsReceived_t::processSDESInfo (u32 ssrc, ql_t<SDESItem_t*> *list)
{
    ql_t<u32> *sourceList = flowDictionary -> getKeys();
    for (ql_t<u32>::iterator_t i = sourceList -> begin (); 
         i != sourceList -> end (); 
         i++)
         {
             u32 ssrc2 = static_cast<u32> (i);
             RTPSource_t *source = flowDictionary -> lookUp (ssrc2); 
             if (ssrc == ssrc2)
             { 
                 source -> processSDES (list);
                 return;                
             
             }else{
                 ql_t<u32> *csrcList = source -> getCSRCList();
                 if (csrcList != NULL)
                 {
                     for (ql_t<u32>::iterator_t i=csrcList -> begin(); 
                          i!= csrcList -> end(); 
                          i++)
                          {
                              u32 aux = static_cast<u32> (i);
                              if (aux == ssrc)
                              { 
                                  source -> processSDES (ssrc, list);
                                  return;
                              }
                          }
                 }
                 delete csrcList;
             }
             source = NULL;
         } 
    delete sourceList;
}

void 
RTPFlowsReceived_t::processBYEInfo (u32 ssrc)
{
    RTPSource_t *source = NULL;
    bool found = false;
    ql_t<u32> *sourceList = flowDictionary -> getKeys();
    
    for (ql_t<u32>::iterator_t i = sourceList -> begin (); 
         i != sourceList -> end (); 
         i++)
         {
             u32 ssrc2 = static_cast<u32> (i);
             source = flowDictionary -> lookUp (ssrc2);
             if (ssrc == ssrc2)
             {
                 found = true;
                 deleteFlow (ssrc);
             
             }else{
                 
                 ql_t<u32> *csrcList = source -> getCSRCList();
                 if (csrcList != NULL)
                 {
                     for (ql_t<u32>::iterator_t i=csrcList -> begin();
                          i!= csrcList -> end();
                          i++)
                          {
                              u32 aux = static_cast<unsigned long> (i);
                              if (aux == ssrc)
                              {
                                  source -> processBYE (ssrc);
                                  return;
                              }
                          }
                 }
                 
                 delete csrcList;
             }
         }
         
    delete sourceList;
}

const char * 
RTPFlowsReceived_t::getJitter (u32 ssrc)
{
    RTPSource_t *source = NULL;

    ql_t<u32> *sourceList = flowDictionary -> getKeys();
    
    for (ql_t<u32>::iterator_t i = sourceList -> begin ();
         i != sourceList -> end (); 
         i++
        )
    {
        u32 ssrc2 = static_cast<u32> (i);
        source = flowDictionary -> lookUp (ssrc2); 
        if (ssrc == ssrc2)
        {
            return  source -> getJitter ();
        }       
    }
         
    delete sourceList;

    return NULL;
}

const char * 
RTPFlowsReceived_t::getPacketsLost (u32 ssrc)
{
    RTPSource_t *source = NULL;
    ql_t<u32> *sourceList = flowDictionary -> getKeys();
    
    for (ql_t<u32>::iterator_t i = sourceList -> begin (); 
         i != sourceList -> end (); 
         i++
        )
    {
        u32 ssrc2 = static_cast<u32> (i);
        source = flowDictionary -> lookUp (ssrc2);
        if (ssrc == ssrc2)
        {
            return source -> getPacketsLost ();
        }
    }
         
    delete sourceList;

    return NULL;
}

const char *
RTPFlowsReceived_t::getSDESInfo (u32 ssrc)
{
    RTPSource_t *source = NULL;

    ql_t<u32> *sourceList = flowDictionary -> getKeys();
    
    for (ql_t<u32>::iterator_t i = sourceList -> begin ();
         i != sourceList -> end (); 
         i++
        )
    {
        u32 ssrc2 = static_cast<u32> (i);
        source = flowDictionary -> lookUp (ssrc2); 
        if (ssrc == ssrc2)
        {
            return source -> getSDESItems ();
        }
        else
        { 
            ql_t<u32> *csrcList = source -> getCSRCList(); 
            if (csrcList != NULL)
            {
                for (ql_t<u32>::iterator_t i = csrcList -> begin();
                     i!= csrcList -> end(); 
                     i++
                    )
                {
                    u32 aux = static_cast<u32> (i);
                    if (aux == ssrc)
                    { 
                        return source -> getSDESItem (ssrc, 2);
                    }
                }
            }
            delete csrcList;
        }
    }
         
    delete sourceList; 

    return NULL;
}

RTPTimeOutChecker_t::RTPTimeOutChecker_t (RTPSession_t *session,
                                          RTPFlowsReceived_t *flows
                                         )
:simpleTask_t(1000000)
{
    debugMsg(dbg_App_Verbose,
             "RTPTimeOutChecker_t",
             "Creating RTPTimeOutChecker_t"
            );

    rtpSession = session;
    flowsReceived = flows;
}

RTPTimeOutChecker_t::~RTPTimeOutChecker_t (void)
{
    debugMsg(dbg_App_Verbose,
             "~RTPTimeOutCheceker_t",
             "Deleting RTPTimeOutChecker_t"
            );
}

void 
RTPTimeOutChecker_t::heartBeat (void)
{
    dictionary_t<u32, RTPSource_t*> *flowDictionary =
        flowsReceived -> getFlowDictionary ();

    unsigned long currentTime = time (NULL);
    ql_t<u32> *sourceList = flowDictionary -> getKeys();
    ql_t<u32> *list = new ql_t<u32>;
    
    for (ql_t<u32>::iterator_t i = sourceList -> begin (); 
         i != sourceList -> end (); 
         i++
    )
    {
        u32 ssrc = static_cast<u32> (i);
        RTPSource_t *source = flowDictionary -> lookUp (ssrc);
        if (!(source -> getProving()))
        {
            signed long diff = currentTime - (source -> getLastPacketTime ());
            if (diff < 0) diff = 0;
            if (diff >= RTP_TIMEOUTSEC)
            {
                list -> insert (ssrc);
            }
        }
    }
    
    delete sourceList;
    if (list -> len() != 0)
    {
        rtpSession -> timeOut(list);
    }
    else
    {
        delete list;
    }
}

