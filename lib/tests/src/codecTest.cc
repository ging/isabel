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
// $Id: codecTest.cc 10863 2007-09-19 17:14:18Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <vUtils/imgTranslator.h>
#include <vUtils/coderWrapper.h>

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
#define TEST_CODEC_OPTION  "-lineCodec"
#define MAX_FRAMES_OPTION  "-maxFrames"
#define BANDWIDTH_OPTION   "-bandwidth"

unsigned  defWidth;
unsigned  defHeight;

char *grabberName;
u32   sourceFmt;
u32   paintFmt;
u32   decodedFmt;
u32   testCodecFmt;

int
main(int argc, char **argv)
{

    grabberName= NULL;

    char *sourceFmtName= NULL;
    char *testCodecName= NULL;
    int   maxFrames    = 0x7fffffff;
    int   BW           = 512000;

    char originalName[1024];
    char processedName[1024];

    defWidth = 320;
    defHeight= 240;


    //
    // Initialize Grabbers
    //
    registerSyntheticGrabber();
    registerFileReader();
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
    for (int i= 1; i < argc; i++)
    {
        if (argv[i][0]== '-')
        {
            if (strcmp(argv[i], GRABBER_OPTION)== 0)
            {
                i++;
                if (i < argc)
                    grabberName= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           GRABBER_OPTION
                          );

            }
            else if (strcmp(argv[i], SOURCE_FMT_OPTION)== 0)
            {
                i++;
                if (i < argc)
                    sourceFmtName= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           SOURCE_FMT_OPTION
                          );

            }
            else if (strcmp(argv[i], TEST_CODEC_OPTION)== 0)
            {
                i++;
                if (i < argc)
                    testCodecName= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           TEST_CODEC_OPTION
                          );

            }
            else if (strcmp(argv[i], MAX_FRAMES_OPTION)== 0)
            {
                i++;
                if (i < argc)
                    maxFrames= atoi(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           MAX_FRAMES_OPTION
                          );

            }
            else if (strcmp(argv[i], BANDWIDTH_OPTION)== 0)
            {
                i++;
                if (i < argc)
                    BW= atoi(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           BANDWIDTH_OPTION
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
           testCodecName
          );

    // AVAILABLE CODECS
    int numCoders= vGetNumRegisteredCoders();
    u32 *Fmts= new u32[numCoders];

    int numFmts= vGetCodecFmtList(Fmts, numCoders);

    if (numFmts < 0)
    {
        delete []Fmts;
        NOTIFY("ERROR: number of codecs below zero\n");
        exit(1);
    }

    if (numFmts == 0)
    {
        delete []Fmts;
        NOTIFY("ERROR: no codecs registered!\n");
        exit(1);
    }

    NOTIFY("AVAILABLE CODECS: ");
    for (int i= 0; i < numFmts; i++)
    {
        u32 fmt= Fmts[i];

        NOTIFY_ND("%s ", vGetFormatNameById(fmt));
    }
    NOTIFY_ND("\n");

    delete []Fmts;

    // CHECK FORMATS
    if (sourceFmtName == NULL)
    {
        NOTIFY("Must specify source format\n");
        NOTIFY("Bailing out\n");
        exit(1);
    }
    if (testCodecName == NULL)
    {
        NOTIFY("Must specify test format\n");
        NOTIFY("Bailing out\n");
        exit(1);
    }

    try
    {
        sourceFmt= vGetFormatIdByName(sourceFmtName);
    }
    catch (const char *e)
    {
        NOTIFY("Unknown format \"%s\"\n", sourceFmtName);
        exit(1);
    }
    try
    {
        testCodecFmt= vGetFormatIdByName(testCodecName);
    }
    catch (const char *e)
    {
        NOTIFY("Unknown line format \"%s\"\n", testCodecName);
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
    NOTIFY("Created grabber \"%s\", returning format %s==0x%02x\n",
           s->getDescriptor()->getID(),
           vGetFormatNameById(sourceFmt),
           sourceFmt
          );

    unsigned actualW, actualH;
    s->getGrabSize(&actualW, &actualH);
    NOTIFY("Actual Source Grab Size %dx%d\n", actualW, actualH);

    // CODER
    vCoderArgs_t    coderArgs;
    coderWrapper_t *coder= NULL;

    coderArgs.width     = actualW;
    coderArgs.height    = actualH;
    coderArgs.bitRate   = BW;
    coderArgs.frameRate = 25;
    coderArgs.maxInter  = 2*25;
    coderArgs.quality   = 80;
    coderArgs.format    = sourceFmt;

    try
    {
        coder= new coderWrapper_t(sourceFmt, testCodecFmt, &coderArgs);

        NOTIFY("Built coderWrapper %s -> %s\n",
               vGetFormatNameById(sourceFmt),
               vGetFormatNameById(testCodecFmt)
              );

        // CODEC PARAMETERS
        NOTIFY("\tCodec Parameters: %dx%d, %d bps, %2.2f fps, Q= %d\n",
               coderArgs.width,
               coderArgs.height,
               coderArgs.bitRate,
               coderArgs.frameRate,
               coderArgs.quality
              );
    }
    catch (const char *error)
    {
        NOTIFY("Encoder not created due to an error: %s\n", error);
        exit(1);
    }

    // DECODER
    vDecoder_t *decoder= NULL;
    vDecoderArgs_t decoderArgs;

    decoderArgs.width = actualW;
    decoderArgs.height= actualH;
    decoderArgs.format= paintFmt;

    decoder = vGetDecoderByFmt(testCodecFmt, &decoderArgs);
    if (decoder != NULL)
    {
        NOTIFY("Found decoder for %s\n", vGetFormatNameById(testCodecFmt));
    }
    else
    {
        NOTIFY("Found no decoder for %s, bailing out\n",
               vGetFormatNameById(testCodecFmt)
              );
        exit(1);
    }

    decodedFmt= decoderArgs.format; // decoder may change it
    NOTIFY("Built coderWrapper %s -> %s\n",
           vGetFormatNameById(sourceFmt),
           vGetFormatNameById(testCodecFmt)
          );

    // TRANSLATORS
    imgTranslator_t *transSrc2RGB = NULL;
    imgTranslator_t *transOut2RGB = NULL;

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
        catch (const char *e)
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

    // decodedFmt --> paintFmt
    NOTIFY("Translator from decodedFmt (%s) to paintFmt (%s): ",
           vGetFormatNameById(decodedFmt),
           vGetFormatNameById(paintFmt)
          );
    if (decodedFmt != paintFmt)
    {
        try
        {
            transOut2RGB = new imgTranslator_t(decodedFmt, paintFmt);
        }
        catch (const char *e)
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
    dpy= createDisplayTask("GDI",NULL);
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
            vGetFormatNameById(testCodecFmt),
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
    int lineimagesize = 1024*768*4;
    u8* lineimage = new u8[lineimagesize];

    int outBuffLen= 2048*1536*4;
    u8 *outBuff= new u8[outBuffLen];

    image_t *srcImg, *dstImg;
    double numBytes= 0;

    for (int nFrames= 0; nFrames < maxFrames; nFrames++)
    {
        srcImg= s->getImage();

        // pinta la fuente
        paint(transSrc2RGB, winO, srcImg);

        // codifica
        int length = 0;
        u8 *src= srcImg->getBuff();
        length = coder->encode(lineimage, lineimagesize, src, width, height);

        if (length <= 0)
        {
            NOTIFY("frame %d with length=%d\n", nFrames, length);
            if (nFrames > 100)
            {
                NOTIFY("Bailing out");
                abort();
            }
            continue;
        }

        numBytes += length;

        // descodifica
        int res= vDecode(decoder,
                         lineimage,
                         length,
                         outBuff,
                         outBuffLen
                        );

        if (res < 0)
        {
            NOTIFY("bad decoded image!!, skipping\n");
            continue;
        }

        vGetParams(decoder, &decoderArgs);

        if (res > outBuffLen)
        {
            NOTIFY("decode image too big %dx%d!!, bailing out\n",
                   decoderArgs.width,
                   decoderArgs.height
                  );
            abort();
        }

        dstImg= new image_t(outBuff,
                            res,
                            decoderArgs.format,
                            decoderArgs.width,
                            decoderArgs.height,
                            srcImg->getTimestamp()
                           );

        if ( dstImg == NULL)
        {
            NOTIFY("ERROR: non valid image after decoding!!!\n");
            delete srcImg;
            continue;
        }

        // pinta el resultado
        paint(transOut2RGB, winP, dstImg);

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
                      "Ins fps=%2.2f (%ld msecs) ",
                      frameRate0,
                      elapsed0,
                      frameRate1,
                      elapsed1
                     );
            NOTIFY_ND("Avg BW=%2.2f kbps\r", numBytes * 8 / elapsed0);

            unsigned bit;
            for (bit= 0; bit< 32; bit++)
                if ((1<< bit)> frameRate1)
                    break;
            timeMask= (1<<bit)-1;

            previousTime= currTime;
            previousFrames= nFrames;
        }
    }
    // delete buffers
    delete []lineimage;
    delete []outBuff;

    // delete objects
    if (coder)
        delete coder;
    if (decoder)
        vDeleteDecoder(decoder);
    if (transSrc2RGB)
        delete transSrc2RGB;
    if (transOut2RGB)
        delete transOut2RGB;

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
#ifdef __BUILD_FOR_LINUX
    releaseV4LuGrabber();
#endif
    NOTIFY("Grabbers Released\n");

    //
    // Release vCodecs
    //
    vCodecRelease();
    NOTIFY("vCodecs Released\n");
}

