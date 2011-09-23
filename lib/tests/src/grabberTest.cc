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
// $Id: grabberTest.cc 11674 2007-10-03 14:48:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>

#include <vCodecs/codecs.h>

#include <vUtils/imgTranslator.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#ifdef WIN32
#include <icf2/sched.hh>
#endif

#include <gwr/displayTask.h>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/systemRegistry.hh>


#include "paint.h"

#define movChar  "|/.\\"
//static char const         movChar[]= {'|', '/', '-', '\\' };

//
// program options
//
#define GRABBER_OPTION     "-grabber"
#define SOURCE_FMT_OPTION  "-sourceFmt"
#define GRAB_SIZE_OPTION   "-grabSize"
#define SGRAY_OPTION       "-sg"
#define SCOLOR_OPTION      "-sc"
#define PCOLOR_OPTION      "-p"
#define TCOLOR_OPTION      "-c"
#define ZOOM_OPTION        "-zoom"
#define NOISE_OPTION       "-noise"
#define RGAMMA_OPTION      "-rGamma"
#define GGAMMA_OPTION      "-gGamma"
#define BGAMMA_OPTION      "-bGamma"
#define SAVE_OPTION        "-save"
#define INVERT_OPTION      "-invert"
#define FANCY_OPTION       "-fancy"
#define FAKESOURCE_OPTION  "-fakeSource"
#define NOPAINT_OPTION     "-noPaint"
#define ROOT_OPTION        "-root"
#define MAX_FRAMES_OPTION  "-maxFrames"
#define INPORT_OPTION      "-inputPort"

char  *grabberName;
u32    sourceFmt;
u32    paintFmt;
char  *grabSize;
char  *inPort;
int    vClass;
int    vDepth;
float  zoomFactor;
int    channelNoise;
float  rGamma;
float  gGamma;
float  bGamma;
char  *saveImages;
bool   invertBytes;
bool   fancyMode;
bool   fakeSource;
bool   noPaint;
bool   rootMode;
int    maxFrames;

int
main(int argc, char **argv)
{

    grabberName= NULL;

    const char *sourceFmtName= NULL;

    grabSize= NULL;

    vClass      = -1;
    vDepth      = -1;
    zoomFactor  = 1.0;
    channelNoise= 0;
    rGamma      = 1.0;
    gGamma      = 1.0;
    bGamma      = 1.0;
    saveImages  = NULL;
    invertBytes = false;
    fancyMode   = false;
    fakeSource  = false;
    noPaint     = false;
    rootMode    = false;
    maxFrames   = 0x7fffffff;
    inPort      = NULL;

    //
    // Initialize Grabbers
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
    // parse module options
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
            else if (strcmp(argv[i], GRAB_SIZE_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    grabSize= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           GRAB_SIZE_OPTION
                          );

            }
            else if (strcmp(argv[i], SGRAY_OPTION)== 0)
            {
                vClass= CLASS_STATICGRAY;

            }
            else if (strcmp(argv[i], SCOLOR_OPTION)== 0)
            {
                vClass= CLASS_STATICCOLOR;

            }
            else if (strcmp(argv[i], PCOLOR_OPTION)== 0)
            {
                vClass= CLASS_PSEUDOCOLOR;

            }
            else if (strcmp(argv[i], TCOLOR_OPTION)== 0)
            {
                vClass= CLASS_TRUECOLOR;

            }
            else if (isdigit(argv[i][1]))
            {
                vDepth= atoi(argv[i]+1);

            }
            else if (strcmp(argv[i], ZOOM_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    zoomFactor= atof(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           ZOOM_OPTION
                          );

            }
            else if (strcmp(argv[i], NOISE_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    channelNoise= atoi(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           NOISE_OPTION
                          );

            }
            else if (strcmp(argv[i], RGAMMA_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    rGamma= atof(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           RGAMMA_OPTION
                          );
            }
            else if (strcmp(argv[i], GGAMMA_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    gGamma= atof(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           GGAMMA_OPTION
                          );
            }
            else if (strcmp(argv[i], BGAMMA_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    bGamma= atof(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           BGAMMA_OPTION
                          );
            }
            else if (strcmp(argv[i], SAVE_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    saveImages= strdup(argv[i]);
                else
                    NOTIFY("Option `%s' requires a parameter\n",
                           SAVE_OPTION
                          );
            }
            else if (strcmp(argv[i], INVERT_OPTION)== 0)
            {
                invertBytes= true;
            }
            else if (strcmp(argv[i], FANCY_OPTION)== 0)
            {
                fancyMode= true;
            }
            else if (strcmp(argv[i], FAKESOURCE_OPTION)== 0)
            {
                fakeSource= true;
            }
            else if (strcmp(argv[i], NOPAINT_OPTION)== 0)
            {
                noPaint= true;
            }
            else if (strcmp(argv[i], ROOT_OPTION)== 0)
            {
                rootMode= true;
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
            else if (strcmp(argv[i], INPORT_OPTION)== 0)
            {
                i++;
                if (i< argc)
                    inPort = strdup(argv[i]);
                else
                     NOTIFY("Option `%s' requires a parameter\n",
                           INPORT_OPTION
                          );
            }
            else
            {
                NOTIFY("Ignoring unknown option: `%s'\n", argv[i]);
            }
        }
    }

    // CHECK FORMATS
    if (sourceFmtName == NULL)
    {
        NOTIFY("Must specify source format\n");
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
    paintFmt = RGB24_FORMAT;

    // CREATE THE SOURCE
    source_ref s;
    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();

    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        sourceDescriptor_ref  sd = sfi->getDescriptor();

        NOTIFY("Found source factory \n\t\"%s\"\n", sd->getID());
    }

    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        sourceFactory_ref     sf = sfi->getFactory();
        sourceDescriptor_ref  sd = sfi->getDescriptor();

        NOTIFY("Trying source \"%s\"\n", sd->getID());

        if (grabberName == NULL)
        {
            s= sf->createSource(sd, inPort);
        }
        else if (strcmp(grabberName, sd->getID()) == 0)
        {
            s= sf->createSource(sd, inPort);
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

    if (grabSize != NULL)
    {
        int w, h;

        if (sscanf(grabSize, "%dx%d", &w, &h) == 2)
        {
            if ((w <= 0) || (h <= 0))
            {
                NOTIFY("Bad values for grab size (%d, %d)\n", w, h);
                abort();
            }
        }
        else
        {
            NOTIFY("Invalid specification for grab size (\"%s\")\n",
                   grabSize
                  );
            abort();
        }

        s->setGrabSize(w, h);
        NOTIFY("Using %dx%d grab size\n", w, h);
    }
    else
    {
        NOTIFY("Using default grab size\n");
    }

    if ( ! s->setGrabFormat(sourceFmt))
    {
        NOTIFY("Could not set source format %s==0x%02x\n",
               vGetFormatNameById(sourceFmt),
               sourceFmt
              );
        exit(-1);
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

    // translator
    imgTranslator_t *trans = NULL;

    if ( ! noPaint )
    {
        // PIPELINE: SOURCE FMT --> PAINT FMT
        NOTIFY("Translator from sourceFmt (%s) to paintFmt (%s): ",
               vGetFormatNameById(sourceFmt),
               vGetFormatNameById(paintFmt)
              );
        if (sourceFmt != paintFmt)
        {
            try
            {
                trans = new imgTranslator_t(sourceFmt, paintFmt);
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

    //
    // verify depths
    //
    displayTask_t::window_t *win;
    if (rootMode)
    {
        win= dpy->rootWindow();
    }
    else
    {
        if ((vClass< 0) && (vDepth< 0))
        {
            dpy->getDefaultVisualCombination((unsigned*)&vDepth, &vClass);
        }
        else if (vClass< 0)
        {
            vClass= 3;
        }
        else if (vDepth< 0)
        {
            vDepth= 8;
        }
        else if (!dpy->supportedVisualCombination(vDepth, vClass))
        {
            NOTIFY("invalid visual spec\n");
            exit(1);
        }

        win = dpy->createWindow(
            "source grabber test",  // title
            NULL,         // no geometry
            NULL,         // no parent
            vDepth, vClass,
            320, 240
        );

        win->map();
    }

    int lastWidth= -1;
    int lastHeight= -1;

    u8 *fakeRaster= NULL;
    int fakeWidth;
    int fakeHeight;
    u32 fakeNumBytes;
    if (fakeSource)
    {
        fakeWidth= 320;
        fakeHeight= 240;
        fakeNumBytes= fakeWidth * fakeHeight * 3;
        fakeRaster= new u8[fakeNumBytes];
        for (int i= 0; i < fakeHeight; i++)
            for (int j= 0; j < fakeWidth; j++)
                fakeRaster[3*(i*fakeWidth+j)+0]=u8(255.0*(i+0)/fakeHeight),
                fakeRaster[3*(i*fakeWidth+j)+1]=u8(255.0*(0+j)/fakeWidth),
                fakeRaster[3*(i*fakeWidth+j)+2]=u8(255.0*(i+j)/(fakeWidth+fakeHeight))
                ;
    }

    // TIMING
    struct timeval startTime;
    gettimeofday(&startTime, NULL);

    int counterForFancy= 0;

    //win->setGamma(rGamma, gGamma, bGamma);

    // INIT TEST
    //
    // get some images, and paint them up
    //
    for (int nFrames= 0; nFrames< maxFrames; nFrames++)
    {
        image_t *img;

        if (fakeSource)
        {
            img= new image_t(fakeRaster,
                             fakeNumBytes,
                             RGB24_FORMAT,
                             fakeWidth,
                             fakeHeight,
                             0
                            );
        }
        else
        {
            img= s->getImage();
        }

        if ( img == NULL )
        {
            //NOTIFY("Image not ok\n");
            nFrames--;
            continue;
        }

        struct timeval currTime;
        gettimeofday(&currTime, NULL);

        long elapsed= (currTime.tv_sec -startTime.tv_sec )*1000+
                      (currTime.tv_usec-startTime.tv_usec)/1000;

        NOTIFY_ND("%cframe rate=%2.2f\r",
                  movChar[nFrames%4],
                  float(nFrames)/elapsed*1000
                 );

        if (noPaint) continue;

        int currWidth= img->getWidth();
        int currHeight= img->getHeight();
        if ((lastWidth!= currWidth) || (lastHeight!= currHeight))
        {
            if (!rootMode)
            {
                if (zoomFactor> 1.0)
                {
                    win->resize((u32)(img->getWidth()*zoomFactor),
                                (u32)(img->getHeight()*zoomFactor)
                               );
                }
                else
                {
                    win->resize(img->getWidth(), img->getHeight());
                }

                lastWidth = currWidth;
                lastHeight= currHeight;
            }
        }

        if (fancyMode)
        {
            zoomFactor=1.0 + 0.5*sin(float(counterForFancy)/41);
            rGamma    =1.0 + 1.0*sin(float(counterForFancy)/33);
            gGamma    =1.0 + 1.0*sin(float(counterForFancy)/35);
            bGamma    =1.0 + 1.0*sin(float(counterForFancy)/37);

            //win->setGamma(rGamma, gGamma, bGamma);

            u8 *o= img->getBuff();

            unsigned nBytes= img->getWidth()*img->getHeight()*3;

            char *b= (char*)malloc(nBytes*4);

            memcpy(b+0*nBytes, o, nBytes);
            memcpy(b+1*nBytes, o, nBytes);
            memcpy(b+2*nBytes, o, nBytes);
            memcpy(b+3*nBytes, o, nBytes);

            int xPos= int(img->getWidth()*(10*sin(float(counterForFancy)/83)));
            int yPos= int(img->getHeight()*(10*sin(float(counterForFancy)/99)));

            xPos%= img->getWidth();
            yPos%= img->getHeight();

            xPos+= img->getWidth();
            yPos+= img->getHeight();

            xPos%= img->getWidth();
            yPos%= img->getHeight();

            unsigned startPos= yPos*img->getWidth()+xPos;

            memcpy(o, b+3*startPos, nBytes);

            free(b);

            counterForFancy++;
        }

        paint(trans, win, img);

        if (saveImages)
        {
            u8 *o= img->getBuff();

            if (invertBytes)
            {
                unsigned l= img->getWidth()*img->getHeight();
                for (unsigned i= 0; i< l; i++)
                {
                    u8 aux= o[3*i+0];

                    o[3*i+0]= o[3*i+2];
                    o[3*i+2]= aux;
                }
            }

            char nnn[256];
            char head[256];

            sprintf(nnn, saveImages, nFrames);

            int h= open(nnn, O_WRONLY|O_CREAT|O_TRUNC, 0666);
            sprintf(head,
                    "P6\n%d\n%d\n255\n",
                    img->getWidth(),
                    img->getHeight()
                   );

            write(h, head, strlen(head));
            write(h, o, img->getWidth()*img->getHeight()*3);
            close(h);
        }

        delete img;
    }
    if (fakeSource)
        delete []fakeRaster;

    // delete objects
    if (trans)
        delete trans;

    //
    // Release Windows and Display Task
    //
    dpy->destroyWindow(win);

    win= NULL;

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
}

