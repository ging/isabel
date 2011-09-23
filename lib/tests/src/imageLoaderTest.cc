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
// $Id: imageLoaderTest.cc 10846 2007-09-19 11:42:50Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <vUtils/imgTranslator.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#ifdef WIN32
#include <icf2/sched.hh>
#endif

#include <gwr/displayTask.h>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/imageLoader.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "paint.h"

int
minimun(int a, int b)
{
    return (a < b) ? a : b;
}

//
// program options
//
#define TEST_PATH_OPTION  "-testPath"
#define LOOP_OPTION       "-loop"
#define MAX_FRAMES_OPTION "-maxFrames"

int
main(int argc, char **argv)
{
    bool  forever = false;
    int   maxFrames = -1; // not set
    char *testPath = NULL;
    DIR  *dirp;

    //
    // parse program options
    //
    for (int i= 1; i< argc; i++)
    {
        if (argv[i][0]== '-')
        {
            if (strcmp(argv[i], TEST_PATH_OPTION) == 0)
            {
                i++;
                if (i< argc)
                    testPath= strdup(argv[i]);
                else
                    NOTIFY("---Option `%s' requires a parameter\n",
                           TEST_PATH_OPTION
                          );

            }
            else if (strcmp(argv[i], LOOP_OPTION) == 0)
            {
                forever= true;

            }
            else if (strcmp(argv[i], MAX_FRAMES_OPTION) == 0)
            {
                i++;
                if (i< argc)
                    maxFrames= atoi(argv[i]);
                else
                    NOTIFY("---Option `%s' requires a parameter\n",
                           MAX_FRAMES_OPTION
                          );

            }
            else
            {
                NOTIFY("---Ignoring unknown option: `%s'\n", argv[i]);
            }
        }
    }

    if (testPath == NULL)
    {
        NOTIFY("Must specify test directory path\n");
        NOTIFY("Bailing out\n");
        exit(1);
    }

    NOTIFY("options parsed %s=%s %s\n",
           TEST_PATH_OPTION,
           testPath,
           forever ? "-loop":""
          );

    // PINTOR
    displayTask_ref dpy= NULL;
#ifdef WIN32
    dpy= createDisplayTask("GDI",NULL);
    sched_t app(128);
    app << (task_ref)((abstractTask_t *)dpy);
#else
    dpy= createDisplayTask("X11", NULL);
#endif

    int vClass;
    int vDepth;
    dpy->getDefaultVisualCombination((unsigned*)&vDepth, &vClass);

    // VENTANAS
    displayTask_t::window_t *winO = dpy->createWindow(
            "Loading files...", // title
            NULL,               // no geometry
            NULL,               // no parent
            vDepth, vClass,
            640, 480
        );

    winO->map();

    // INIT TEST
    //
    // transverse "testPath", get some images, and paint them
    //
    imgTranslator_t *trans2RGB= NULL;
    image_t *img;
    struct stat statbuf;
    struct dirent *dp;
    char fullName[4096];
    int maxW, maxH;

    int numImages= 0;
    do
    {
        dirp= opendir(testPath);

        if (dirp == NULL)
        {
            NOTIFY("\"%s\" is not a directory path\n", testPath);
            exit(1);
        }

        for (dp= readdir(dirp); dp != NULL; dp= readdir(dirp))
        {
            if ( ! stat(dp->d_name, &statbuf))
            {
                NOTIFY("could not stat \"%s\", skipping\n", dp->d_name);
                continue;
            }

            if (S_ISREG(statbuf.st_mode) )
            {
                NOTIFY("\"%s\" is not a regular file, skipping\n", dp->d_name);
                continue;
            }

            sprintf(fullName, "%s/%s", testPath, dp->d_name);

            NOTIFY("LOAD \"%s\"\n", fullName);

            img= loadFile(fullName);

            if ( img == NULL )
            {
                NOTIFY("could not load \"%s\", skipping\n", fullName);
                continue;
            }

            if (img->getFormat() != RGB24_FORMAT)
            {
                try
                {
                    trans2RGB = new imgTranslator_t(img->getFormat(), RGB24_FORMAT);
                }
                catch (char const *e)
                {
                    NOTIFY("Cannot build, error= %s\n", e);
                    exit(1);
                }
                NOTIFY("Built translator for %s !\n", dp->d_name);
            }

            maxW= minimun(1024, img->getWidth());
            maxH= minimun(728,  img->getHeight());
            winO->resize(maxW, maxH);

            // pinta la imagen
            paint(trans2RGB, winO, img);

            delete img;
            img= NULL;

#ifdef WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            numImages++;

            if ((maxFrames > 0) && (numImages >= maxFrames))
            {
                // maxFrames is set (> 0) and we reached it,
                NOTIFY("%d images, maxFrames reached\n", numImages);
                if (forever)
                {
                    NOTIFY("-loop overriden by -maxFrames!!\n");
                }
                forever= false;
                break;
            }
        }
        closedir(dirp);
        if (numImages == 0)
        {
            NOTIFY("Cannot load any image, bailing out\n");
            NOTIFY("Please, be sure there are images under %s\n", testPath);
            exit(1);
        }
    } while (forever);

    // delete objects
    if (trans2RGB)
        delete trans2RGB;

    //
    // Release Windows and Display Task
    //
    dpy->destroyWindow(winO);

    winO= NULL;

    dpy= NULL;
}

