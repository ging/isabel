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
// $Id: lockedImage.hh 20770 2010-07-07 13:19:44Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __locked_image_hh__
#define __locked_image_hh__

#include <icf2/notify.hh>
#include <icf2/lockSupport.hh>
#include <icf2/smartReference.hh>

#include <vUtils/image.h>

class lockedImage_t: public virtual collectible_t, public virtual lockedItem_t
{
private:
    image_t *theImg;

public:
    lockedImage_t(void)
    : theImg(NULL)
    {}

    virtual ~lockedImage_t(void)
    {
        if (theImg != NULL)
        {
            delete theImg;
        }
    }

    void putImage(image_t *img)
    {
        locker_t rlock= lock();

        if (theImg != NULL)
        {
            delete theImg;
        }
        theImg= img;
    }

    image_t *getImage(void)
    {
        locker_t rlock= lock();

        image_t *retVal= theImg;
        theImg= NULL;

        return retVal;
    }

    friend class smartReference_t<lockedImage_t>;
};

typedef smartReference_t<lockedImage_t> lockedImage_ref;

#endif
