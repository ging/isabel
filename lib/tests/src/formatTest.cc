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
// $Id: formatTest.cc 12519 2008-02-01 12:32:28Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <vCodecs/codecs.h>

#include <vUtils/imgTranslator.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/icfTime.hh>
#ifdef WIN32
#include <icf2/sched.hh>
#endif

#include <gwr/displayTask.h>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "paint.h"

//
// program options
//
#define GRABBER_OPTION     "-grabber"
#define SOURCE_FMT_OPTION  "-sourceFmt"
#define TEST_FMT_OPTION    "-testFmt"
#define MAX_FRAMES_OPTION  "-maxFrames"

unsigned  defWidth;
unsigned  defHeight;

char *grabberName;
u32   sourceFmt;
u32   paintFmt;
u32   testFmt;

#ifdef WIN32
// DO NOT DELETE
// keep as an example of a Window Application (no CONSOLE)
int WINAPI
WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    setNotifyFile("C:\\notify.log");

    int argc;
    LPWSTR *argList= CommandLineToArgvW(GetCommandLineW(), &argc);
    LPSTR *argv = new LPSTR [argc];
    for (int iIdx = 0; iIdx < argc; ++iIdx)
    {
        size_t qStrLen = wcslen(argList[iIdx]), qConverted = 0;
        argv[iIdx] = new CHAR [qStrLen+1];
        wcstombs_s(&qConverted, argv[iIdx], qStrLen+1, argList[iIdx], qStrLen+1);
    }

    for (int j= 0; j < argc; j++)
        NOTIFY("Parametro %d= %s\n", j, argv[j]);
#else
int
main(int argc, char **argv)
{
#endif

    grabberName= NULL;

    char *sourceFmtName= NULL;
    char *testFmtName  = NULL;
    int   maxFrames    = 0x7fffffff;

    char originalName[1024];
    char processedName[1024];

    defWidth     = 320;
    defHeight    = 240;

    //
    // Initialize Isabel SDK
    //
    registerSyntheticGrabber();
    registerFileReader();
#ifdef __BUILD_FOR_WINXP
    registerDSGrabber();
#endif
#ifdef __BUILD_FOR_LINUX
    registerV4LuGrabber();
#endif
    NOTIFY("Grabbers Initialised\n");

    //
    // Initialize vCodecs
    //
    vCodecInit();
    NOTIFY("vCodecs Initialised\n");

    //
    // parse program options
    //
    for (int i= 1; i< argc; i++)
    {
        if (argv[i][0]== '-')
        {
            if (strcmp(argv[i], GRABBER_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    grabberName= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           GRABBER_OPTION
                          );

            }
            else if (strcmp(argv[i], SOURCE_FMT_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    sourceFmtName= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           SOURCE_FMT_OPTION
                          );

            }
            else if (strcmp(argv[i], TEST_FMT_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    testFmtName= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           TEST_FMT_OPTION
                          );

            }
            else if (strcmp(argv[i], MAX_FRAMES_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    maxFrames= atoi(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           MAX_FRAMES_OPTION
                          );

            }
            else
            {
                NOTIFY("Unknown option: `%s'\n", argv[i]);
                NOTIFY("Bailing out\n");
                exit(1);
            }
        }
    }

    NOTIFY("options parsed %s %s\n",
           sourceFmtName,
           testFmtName
          );

    // CHECK FORMATS
    if (sourceFmtName == NULL)
    {
        NOTIFY("Must specify source format\n");
        NOTIFY("Bailing out\n");
        exit(1);
    }
    if (testFmtName == NULL)
    {
        NOTIFY("Must specify test format\n");
        NOTIFY("Bailing out\n");
        exit(1);
    }

    try
    {
        sourceFmt= vGetFormatIdByName(sourceFmtName);
    }
    catch (char const *e)
    {
        NOTIFY("Unknown format \"%s\"\n", sourceFmtName);
        exit(1);
    }
    try
    {
        testFmt= vGetFormatIdByName(testFmtName);
    }
    catch (char const *e)
    {
        NOTIFY("Unknown format \"%s\"\n", testFmtName);
        exit(1);
    }
    paintFmt = RGB24_FORMAT;

    // CREATE THE SOURCE
    source_ref s;
    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();

    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        sourceFactory_ref     sf = sfi->getFactory();
        sourceDescriptor_ref  sd = sfi->getDescriptor();

        if ( grabberName == NULL )
        {
            s= sf->createSource(sd);
        }
        else if (strcmp(grabberName, sd->getID()) == 0)
        {
            s= sf->createSource(sd);
        }

        if (s.isValid())
        {
            break;
        }
    }
    if ( ! s.isValid())
    {
        NOTIFY("Grabber not found, bailing out\n");
        exit(-1);
    }

    s->setGrabSize(defWidth, defHeight);

    if ( ! s->setGrabFormat(sourceFmt))
    {
        NOTIFY("Could not set source format %s==0x%02x\n",
               vGetFormatNameById(sourceFmt),
               sourceFmt
              );
        exit(1);
    }

    u32 sourceFmt= s->getGrabFormat();
    NOTIFY("Created grabber \"%s\", returning %s==0x%02x\n",
           s->getDescriptor()->getID(),
           vGetFormatNameById(sourceFmt),
           sourceFmt
          );

    unsigned actualW, actualH;
    s->getGrabSize(&actualW, &actualH);
    NOTIFY("Actual Source Grab Size %dx%d\n", actualW, actualH);

    // TRANSLATORS
    imgTranslator_t *transSrc2RGB = NULL;
    imgTranslator_t *transSrc2Tst = NULL;
    imgTranslator_t *transTst2RGB = NULL;

    // sourceFmt --> paintFmt
    NOTIFY("Translator from sourceFmt (%s) to paintFmt (%s): ",
           vGetFormatNameById(sourceFmt),
           vGetFormatNameById(paintFmt)
          );
    if (sourceFmt != paintFmt)
    {
        try
        {
            transSrc2RGB = new imgTranslator_t(sourceFmt, paintFmt);
        }
        catch (char const *e)
        {
            NOTIFY_ND("Cannot build, error= %s\n", e);
            exit(1);
        }
        NOTIFY_ND("Built !\n");
    }
    else
    {
        NOTIFY_ND("Not needed !\n");
    }

    // sourceFmt --> testFmt
    NOTIFY("Translator from sourceFmt (%s) to testFmt (%s): ",
           vGetFormatNameById(sourceFmt),
           vGetFormatNameById(testFmt)
          );
    if (sourceFmt != testFmt)
    {
        try
        {
            transSrc2Tst = new imgTranslator_t(sourceFmt, testFmt);
        }
        catch (char const *e)
        {
            NOTIFY_ND("Cannot build, error= %s\n", e);
            exit(1);
        }
        NOTIFY_ND("Built !\n");
    }
    else
    {
        NOTIFY_ND("Not needed !\n");
    }

    // testFmt --> paintFmt
    NOTIFY("Translator from testFmt (%s) to paintFmt (%s): ",
           vGetFormatNameById(testFmt),
           vGetFormatNameById(paintFmt)
          );
    if (testFmt != paintFmt)
    {
        try
        {
            transTst2RGB = new imgTranslator_t(testFmt, paintFmt);
        }
        catch (char const *e)
        {
            NOTIFY_ND("Cannot build, error= %s\n", e);
            exit(1);
        }
        NOTIFY_ND("Built !\n");
    }
    else
    {
        NOTIFY_ND("Not needed !\n");
    }

    // PINTOR
    displayTask_ref dpy= NULL;
#ifdef WIN32
    dpy= createDisplayTask("GDI",(char*)&hinstance);
    sched_t app(128);
    app << (task_ref)((abstractTask_t *)dpy);
#else
    dpy= createDisplayTask("X11", NULL);
#endif

    int vClass;
    int vDepth;
    dpy->getDefaultVisualCombination((unsigned*)&vDepth, &vClass);

    // NOMBRES DE LAS VENTANAS
    memset(originalName, 0, 1024);
    memset(processedName, 0, 1024);

    sprintf(originalName,
            "source (from %s) -> %s",
            vGetFormatNameById(sourceFmt),
            vGetFormatNameById(paintFmt)
           );
    sprintf(processedName,
            "processed %s -> %s",
            vGetFormatNameById(testFmt),
            vGetFormatNameById(paintFmt)
           );

    // VENTANAS
    displayTask_t::window_t *winO = dpy->createWindow(
            originalName, // title
            "+0+0",       // geometry
            NULL,         // no parent
            vDepth, vClass,
            defWidth, defHeight
        );

    displayTask_t::window_t *winP = dpy->createWindow(
            processedName, // title
            "+400+0",      // geometry
            NULL,          // no parent
            vDepth, vClass,
            defWidth, defHeight
        );

    winO->map();
    winP->map();

    // TIMING
    struct timeval startTime;
    struct timeval previousTime;
    unsigned       previousFrames= 0;

    gettimeofday(&startTime, NULL);
    gettimeofday(&previousTime, NULL);
    unsigned timeMask= 0;

    //
    // set windows size, same as image size
    //
    image_t *img= s->getImage();
    int width = img->getWidth();
    int height= img->getHeight();
    delete img;

    winO->resize(width, height);
    winP->resize(width, height);
    NOTIFY("Image Size %dx%d\n", width, height);

    // INIT TEST
    //
    // get some images, code, and decode them up
    //
    image_t *srcImg, *dstImg;
    for (int nFrames= 0; nFrames < maxFrames; nFrames++)
    {
        srcImg= s->getImage();

        // pinta la fuente
        paint(transSrc2RGB, winO, srcImg);

        // traduce el espacio de colores
        if (transSrc2Tst != NULL)
        {
            int dstWidth= -1, dstHeight= -1;
            int srcWidth = srcImg->getWidth();
            int srcHeight= srcImg->getHeight();
            u8 *src= srcImg->getBuff();

            int transSize = vFrameSize(srcWidth, srcHeight, testFmt);
            u8 *transBuff= new u8[transSize];
            if ( ! transBuff )
            {
                NOTIFY("paint: run out of memory, bailing out\n");
                exit(-1);
            }

            transSrc2Tst->translate(transBuff,
                                    dstWidth,
                                    dstHeight,
                                    src,
                                    srcWidth,
                                    srcHeight
                                   );

            dstImg= new image_t(transBuff,
                                transSize,
                                testFmt,
                                dstWidth,
                                dstHeight,
                                srcImg->getTimestamp()
                               );

            delete []transBuff;
        }
        else
        {
            dstImg = new image_t(srcImg->getBuff(),
                                 srcImg->getNumBytes(),
                                 srcImg->getFormat(),
                                 srcImg->getWidth(),
                                 srcImg->getHeight(),
                                 srcImg->getTimestamp()
                                );
        }

        // pinta el resultado
        paint(transTst2RGB, winP, dstImg);

        delete srcImg;
        delete dstImg;

        if ((nFrames&timeMask)== 0)
        {
            struct timeval currTime;
            gettimeofday(&currTime, NULL);

            long elapsed0= (currTime.tv_sec -startTime.tv_sec )*1000 +
                           (currTime.tv_usec-startTime.tv_usec)/1000;
            long elapsed1= (currTime.tv_sec -previousTime.tv_sec )*1000 +
                           (currTime.tv_usec-previousTime.tv_usec)/1000;

            float frameRate0= float(nFrames)/elapsed0*1000;
            float frameRate1= float(nFrames-previousFrames)/elapsed1*1000;
            NOTIFY_ND("Avg fps=%2.2f (%ld msecs) || "
                      "Ins fps=%2.2f (%ld msecs)\r",
                      frameRate0,
                      elapsed0,
                      frameRate1,
                      elapsed1
                     );

            unsigned bit;
            for (bit= 0; bit< 32; bit++)
                if ((1<< bit)> frameRate1)
                    break;
            timeMask= (1<<bit)-1;

            previousTime= currTime;
            previousFrames= nFrames;
        }
    }

    //
    // Release Windows and Display Task
    //
    dpy->destroyWindow(winO);
    dpy->destroyWindow(winP);

    winO= NULL;
    winP= NULL;

    dpy= NULL;

    //
    // Release Grabbers
    //
    releaseSyntheticGrabber();
    releaseFileReader();
#ifdef __BUILD_FOR_WINXP
    releaseDSGrabber();
#endif
#ifdef __BUILD_FOR_LINUX
    releaseV4LuGrabber();
#endif
    NOTIFY("Grabbers Released\n");

    //
    // Release vCodecs
    //
    vCodecRelease();
    NOTIFY("vCodecs Released\n");

#ifdef WIN32
    // delete argv
    for (int j = 0; j < argc; ++j)
    {
        delete [] argv[j];
    }

    delete [] argv;
    LocalFree(argList);
#endif
}

