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
// $Id: systemRegistry.cc 20749 2010-07-02 12:57:23Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include <icf2/lockSupport.hh>
#include <icf2/notify.hh>

#include <Isabel_SDK/systemRegistry.hh>


//
// source factories book-keeping
//
sourceFactoryInfo_t::sourceFactoryInfo_t(sourceDescriptor_ref d, sourceFactory_ref f)
: descriptor(d), factory(f)
{
}

template class                           smartReference_t<sourceFactoryInfo_t>;
template class                  vector_t<smartReference_t<sourceFactoryInfo_t> >;
template class smartReference_t<vector_t<smartReference_t<sourceFactoryInfo_t> > >;


static class sourceFactoryManager_t: public virtual rwLockedItem_t
{
private:
    sourceFactoryInfoArray_t factoryArray;

public:
    bool
    registerSourceFactory(sourceDescriptor_ref d, sourceFactory_ref f)
    {
        locker_t lock= wrLock();

        factoryArray.add(new sourceFactoryInfo_t(d,f));

        return true;
    }

    bool
    releaseSourceFactory(const char *srcName)
    {
        locker_t lock= wrLock();

        for (int i= 0; i < factoryArray.size(); i++)
        {
            sourceDescriptor_ref srcDesc=
                factoryArray.elementAt(i)->getDescriptor();

            if (strcmp(srcName, srcDesc->getID()) == 0)
            {
                sourceFactoryInfo_ref sfi= factoryArray.remove(i);
                sfi= NULL;
                i--;
            }
        }

        return true;
    }

    sourceFactoryInfoArray_ref
    getSourceFactoryInfoArray(void)
    {
        locker_t lock= rdLock();

        sourceFactoryInfoArray_ref retVal=
            new sourceFactoryInfoArray_t(factoryArray);

        return retVal;
    };
} sourceFactoryManager;

template class smartReference_t<sourceFactory_t>;
template class smartReference_t<sourceDescriptor_t>;
template class smartReference_t<source_t>;


bool
registerSourceFactory(sourceDescriptor_ref d, sourceFactory_ref f)
{
    return sourceFactoryManager.registerSourceFactory(d, f);
}

bool
releaseSourceFactory(const char *srcName)
{
    return sourceFactoryManager.releaseSourceFactory(srcName);
}

sourceFactoryInfoArray_ref
getSourceFactoryInfoArray(void)
{
    return sourceFactoryManager.getSourceFactoryInfoArray();
}

