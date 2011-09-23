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
// $Id: systemRegistry.hh 20757 2010-07-05 10:01:13Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __isabel_sdk__system_registry_hh__
#define __isabel_sdk__system_registry_hh__

#include <icf2/general.h>
#include <icf2/smartReference.hh>
#include <icf2/vector.hh>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>
#include <Isabel_SDK/sourceFactory.hh>

//
// source factory management
//
class sourceFactoryInfo_t: public virtual collectible_t
{
private:
    sourceDescriptor_ref descriptor;
    sourceFactory_ref    factory;

    sourceFactoryInfo_t(sourceDescriptor_ref, sourceFactory_ref);

public:
    sourceDescriptor_ref getDescriptor(void) const { return descriptor; }
    sourceFactory_ref    getFactory   (void) const { return factory   ; }

    friend class sourceFactoryManager_t;
    friend class smartReference_t<sourceFactoryInfo_t>;
};

typedef smartReference_t<         sourceFactoryInfo_t  >   sourceFactoryInfo_ref;
typedef                  vector_t<sourceFactoryInfo_ref>   sourceFactoryInfoArray_t;
typedef smartReference_t<vector_t<sourceFactoryInfo_ref> > sourceFactoryInfoArray_ref;


extern bool registerSourceFactory(sourceDescriptor_ref, sourceFactory_ref);
extern bool releaseSourceFactory(const char *srcName);
extern sourceFactoryInfoArray_ref getSourceFactoryInfoArray(void);

#endif
