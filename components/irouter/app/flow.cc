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
// $Id: flow.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/dictionary.hh>

#include "flow.hh"

dictionary_t<int,int> * fecById = new dictionary_t<int,int>;

const char *
getFlowStr(flowId_e flow)
{
    const char *flowStr= NULL;

    switch (flow)
    {
    case audioId:
         flowStr = "audio";
         break;
    case vumeterId:
         flowStr = "vumetr";
         break;
    case videoId:
         flowStr = "video";
         break;
    case sldFtpId:
         flowStr = "sldFtp";
         break;
    case pointerId:
         flowStr = "pointr";
         break;
    case shDispId:
         flowStr = "shDisp";
         break;
    case rtcpVideoId:
         flowStr = "rtcpVd";
         break;
    case rtcpShDispId:
         flowStr = "rtcpSD";
         break;

    default:
         NOTIFY("getFlowStr:: Unknown media %s\n", flow);
         flowStr=" unknown";
    }

    return flowStr;
}

flow_t::flow_t(const char *nam, flowId_e id)
: flowName(strdup(nam)),
  flowId(id)
{
    assert(flowName);
}


flow_t::~flow_t(void)
{
    if (flowName)
    {
        free(flowName);
        flowName= NULL;  // not really needed, helps debug
    }
}


//
// flowBinder_t stuff
//
u32 flowBinder_t::idCounter= USER_DEFINED_FLOW_ID;  /*!< just a start value */

dictionary_t <flowId_e, flow_t *> flowBinder_t::byId(MAX_FLOW);
dictionary_t <string,   flow_t *> flowBinder_t::byName(MAX_FLOW);


flowBinder_t::flowBinder_t(void)
{
    ;
}


flowBinder_t::~flowBinder_t(void)
{
   debugMsg(dbg_App_Paranoic, "~flowBinder_t", "Destroying flowBinder_t\n");
};


ql_t<flowId_e> *
flowBinder_t::getFlowList(void) const
{
    return  byId.getKeys();
}


flow_t *
flowBinder_t::newFlow(const char *nam, flowId_e id)
{
    flow_t *theFlow= byName.lookUp(nam);

    if (theFlow)
    {  // exists!!
        assert(theFlow->flowId == id);
        return theFlow;
    }

    assert((u32)id <= USER_DEFINED_FLOW_ID);


    //
    // do the work
    //
    theFlow= new flow_t(nam, id);

    byId.insert(id, theFlow);
    byName.insert(nam, theFlow);

    return theFlow;
}


bool
flowBinder_t::deleteFlow(const char *nam)
{
    flow_t *theFlow= byName.lookUp(nam);

    if ( ! theFlow)
    {
        return false;
    }

    assert(strcmp(nam, theFlow->flowName) == 0);

    byId.remove(theFlow->flowId);
    byName.remove(theFlow->flowName);

    delete theFlow;

    return true;
}


bool
flowBinder_t::deleteFlow(flowId_e id)
{
    flow_t *theFlow= byId.lookUp(id);

    if ( ! theFlow)
    {
        return false;
    }

    assert(id == theFlow->flowId);

    byId.remove(theFlow->flowId);
    byName.remove(theFlow->flowName);

    delete theFlow;

    return true;
}


flow_t *
flowBinder_t::lookUp(const char *nam)
{
    return byName.lookUp(nam);
}


flow_t *
flowBinder_t::lookUp(flowId_e flowId)
{
    return byId.lookUp(flowId);
}

