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
// $Id: webirouterConfig.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __webirouter_config_hh__
#define __webirouter_config_hh__

#include <icf2/general.h>

const unsigned MAX_PKT_MEMORY_CHUNK= 16384;
const unsigned MAX_PKT_SIZE        = (MAX_PKT_MEMORY_CHUNK - 4096);
//const unsigned MAX_PKT_SIZE        = 2048;

const unsigned MAX_FLOW= 256;   // number of supported flow
const u32      USER_DEFINED_FLOW_ID= 0xffffff;


const unsigned MAX_CLIENTS= 512; // number of supported clients


const unsigned MAX_DRIS= 512;   // to be documented
const unsigned MAX_URIS= 512;




//
// link management
//
#define IROUTER_LINK_PROTOCOL        "23332"     // Puerto para conectar FSs
extern u32 localNodeIdentifier;             // defined @ main.C


// irouter version
const u16 IROUTER_MAJOR_VERSION=7;
const u16 IROUTER_MINOR_VERSION=1;

//
// chapuza impresentable
//
class link_t;
extern link_t *defaultMulticastLink;
extern link_t *defaultLocalLink;


#endif

