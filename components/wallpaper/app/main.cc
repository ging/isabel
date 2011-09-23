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
// $Id:$
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <vUtils/imgTranslator.h>

#include <gwr/displayTask.h>

#include <Isabel_SDK/imageLoader.hh>

void
help(const char *progname)
{
    NOTIFY("usage: %s [-display dpyname] [-notify notifyFile] <filename>\n",
           progname
          );
}

image_t*
loadImage2RGB24(const char *filename)
{
    imgTranslator_t *imgTranslator= NULL;

    image_t *img= loadFile(filename);

    if (img == NULL)
    {
        NOTIFY("loadImage2RGB24: invalid image in \"%s\"\n", filename);
        return NULL;
    }

    u32 imgFmt= img->getFormat();

    if (imgFmt != RGB24_FORMAT)
    {
        // colorspace translation needed
        try
        {
            imgTranslator =
                new imgTranslator_t(img->getFormat(), RGB24_FORMAT);
        }
        catch (char const *e)
        {
            NOTIFY("loadImage2RGB24: "
                   "cannot build imgTranslator_t, error= %s\n",
                   e
                  );
            exit(1);
        }
    }

    if (imgTranslator != NULL)
    {
        int dstWidth= -1, dstHeight= -1;
        int srdWidth = img->getWidth();
        int srcHeight= img->getHeight();
        u8 *src= img->getBuff();

        int transSize = vFrameSize(srdWidth, srcHeight, RGB24_FORMAT);
        u8 *transBuff= new u8[transSize];
        if ( ! transBuff )
        {
            NOTIFY("paint: run out of memory, bailing out\n");
            exit(-1);
        }
 
        imgTranslator->translate(transBuff,
                                 dstWidth, dstHeight,
                                 src,
                                 srdWidth, srcHeight
                                );

        image_t *newImg= new image_t(transBuff,
                                     transSize,
                                     RGB24_FORMAT,
                                     dstWidth,
                                     dstHeight,
                                     img->getTimestamp()
                                    );

        delete [] transBuff;
        delete img;

        img= newImg;
    }

    return img;
}

#define DISPLAY_OPTION "-display"
#define NOTIFY_OPTION "-notify"

int
main(int argc, char *argv[])
{
    const char *progname= argv[0];
    const char *filename= NULL;

    for (int i= 1; i < argc; i++)
    {
        if (argv[i][0]== '-')
        {
            if (strcmp(argv[i], DISPLAY_OPTION)== 0)
            {
                i++;
                if (i < argc)
                {
                    char *b= (char *)malloc(2048);
                    sprintf(b, "DISPLAY=%s", argv[i]);

                    putenv(b);
                }
                else
                {
                    NOTIFY("Option `%s' requires a parameter\n",
                           DISPLAY_OPTION
                          );
                }
            }
            else if (strcmp(argv[i], NOTIFY_OPTION)== 0)
            {
                i++;
                if (i < argc)
                {
                    setNotifyFile(argv[i]);
                }
                else
                {
                    NOTIFY("Option `%s' requires a parameter\n",
                           NOTIFY_OPTION
                          );
                }
            }
            else
            {
                NOTIFY("Unknown option: `%s'\n", argv[i]);
                help(progname);
                exit(1);
            }
        }
        else
        {
            if (filename != NULL)
            {
                help(progname);
                exit(1);
            }
            filename= argv[i];
        }
    }

    if (filename == NULL)
    {
        help(progname);
        exit(1);
    }

    displayTask_ref dpy= createDisplayTask("X11", NULL);

    window_ref rootWin= dpy->rootWindow();

    image_t *img= loadImage2RGB24(filename);

    if (img == NULL)
    {
        NOTIFY("%s ERROR: could not load %s\n", progname, filename);
        exit(1);
    }

    rootWin->putRGB24inBG(img->getBuff(),
                          img->getWidth(), img->getHeight(),
                          -1, -1
                         );

    delete img;

    return 0; // keeps compiler happy
}

