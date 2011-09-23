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
// $Id: imageLoader.cc 10576 2007-07-12 16:42:47Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <Isabel_SDK/imageLoader.hh>

#include "bmp/bmpImpl.hh"
#ifndef WIN32
#include "gif/gifImpl.hh"
#include "png/pngImpl.hh"
#include "tiff/tiffImpl.hh"
#endif
#include "pnm/pnmImpl.hh"
#include "jpeg/jpegImpl.hh"

typedef struct
{
    const char *name;
    image_t *(*loadFile)(FILE *);
} imageLoader_t;

imageLoader_t imageLoader[] =
{
    { "BMP",  bmpLoadFile  },
#ifndef WIN32
    { "GIF",  gifLoadFile  },
    { "PNG",  pngLoadFile  },
    { "TIFF", tiffLoadFile },
#endif
    { "PNM",  pnmLoadFile  },
    { "JPEG", jpegLoadFile },
    { "",     NULL }
};


image_t*
loadFile (const char *const filename)
{
    FILE *f;

    if ( ! filename || *filename == 0)
    {
        NOTIFY ("loadFile:: invalid filename [empty]\n");
        return NULL;
    }

#ifdef WIN32
    const char *openFlags= "rb"; // binary mode to avoid stopping in EOF
#else
    const char *openFlags= "r";  // plain read mode
#endif

    if ((f= fopen (filename, openFlags)) == NULL)
    {
        NOTIFY("loadFile:: cannot open file [%s]\n", filename);
        return NULL;
    }

    image_t *imgRef= NULL;

    NOTIFY("loadFile: trying");
    for ( int i= 0; imageLoader[i].loadFile != NULL; i++)
    {
        // every imageLoader gets the file rewinded

        fseek (f, 0L, SEEK_SET);

        NOTIFY_ND(" %s...", imageLoader[i].name);
        imgRef= imageLoader[i].loadFile (f);

        if (imgRef != NULL)    // I've got it!
        {
            break;
        }
    }

    NOTIFY_ND(" %s!\n", imgRef != NULL ? "SUCCEED" : "FAILED");

    fclose (f);

    return imgRef;
}

