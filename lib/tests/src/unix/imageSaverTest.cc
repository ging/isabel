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
// $Id: imageSaverTest.cc 10847 2007-09-19 11:47:04Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include <Isabel_SDK/builtins/rawImages.hh>
#include <Isabel_SDK/builtins/clutImages.hh>
#include <Isabel_SDK/displayTask.hh>


//
// program class
//
#define SOURCE_FMT_OPTION "-sourceFmt"
#define TEST_FMT_OPTION   "-testFmt"

unsigned   defWidth;
unsigned   defHeight;

formatId_t sourceFmt;
formatId_t testFmt;

void
paint(codecPipeline_ref cpr, displayTask_t::window_t *win, image_ref img)
{
    if(cpr.isValid()) {
        image_ref cimg=
            new image_t(img->getTimestamp(), img->isComplete());

        cimg->setImplementation(img->getImplementation());
        cimg= cpr->putImage(cimg);

        if(!IMAGE_OK(cimg)) {
            NOTIFY("dispatchMsg: could not paint image\n");
            return;
        }

        img= cimg;
    }

    imageImplementation2_t *imp= img->getImplementation();

    if(img->getFormat()== FMT_RAW_RGB24) {
        unsigned char *o= (unsigned char*)imp->getBuf();

        win->putRGB24(o,
                      img->getWidth(), img->getHeight(),
                      -1, -1
                     );

    } else {
        NOTIFY("No puedo pintar el formato 0x%x (s),\n",
               img->getFormat(),
               getFormatNameById(img->getFormat())
              );
    }
}

int
main(int argc, char **argv)
{
    char *sourceFmtName= NULL;
    char *testFmtName  = NULL;

    defWidth     = 320;
    defHeight    = 240;

    //
    // Initialize Grabbers
    //
    registerSyntheticGrabber();
    registerV4LuGrabber();
    NOTIFY("Grabbers Initialised\n");

    //
    // parse program options
    //
    for(int i= 1; i< argc; i++) {
        if(argv[i][0]== '-') {
            if(strcmp(argv[i], SOURCE_FMT_OPTION)== 0) {
                i++;
                if(i< argc)
                    sourceFmtName= strdup(argv[i]);
                else
                    NOTIFY("---Option `%s' requires a parameter\n",
                           SOURCE_FMT_OPTION
                          );

            } else if(strcmp(argv[i], TEST_FMT_OPTION)== 0) {
                i++;
                if(i< argc)
                    testFmtName= strdup(argv[i]);
                else
                    NOTIFY("---Option `%s' requires a parameter\n",
                           TEST_FMT_OPTION
                          );

            } else {
                NOTIFY("---Ignoring unknown option: `%s'\n", argv[i]);
            }
        }
    }

    NOTIFY("options parsed %s %s\n", sourceFmtName, testFmtName);

    if (sourceFmtName == NULL) {
        NOTIFY("Must specify source format\n");
        NOTIFY("Bailing out\n");
        exit(1);
    }
    if (testFmtName == NULL) {
        NOTIFY("Must specify test format\n");
        NOTIFY("Bailing out\n");
        exit(1);
    }

    sourceFmt= getFormatIdByName(sourceFmtName);
    testFmt  = getFormatIdByName(testFmtName);

    // CREATE THE SOURCE
    source_ref s;
    sourceFactoryInfoList_ref sfl= getSourceFactoryInfoList();

    sourceFactoryInfoList_t::iterator_t i;

    for(i= sfl->begin(); i != sfl->end(); i++) {
        sourceFactoryInfo_ref sfi= static_cast<sourceFactoryInfo_ref>(i);
        sourceFactory_ref     sf = sfi->getFactory();
        sourceDescriptor_ref  sd = sfi->getDescriptor();

        s= sf->createSource(sd);

        if(s.isValid()) {
            if(s->setGrabFormat(sourceFmt)) {
                break;
            }
        }
    }
    if(!s.isValid()) {
        NOTIFY("Grabber not found, bailing out\n");
        exit(-1);
    }

    s->setGrabSize(defWidth, defHeight);

    if(!s->setGrabFormat(sourceFmt)) {
        NOTIFY("---Could not set source format %s==0x%02x\n",
               getFormatNameById(sourceFmt),
               sourceFmt
              );
    }

    formatId_t sourceFmt= s->getGrabFormat();
    NOTIFY("+++Created grabber returning %s==0x%02x\n",
           getFormatNameById(sourceFmt),
           sourceFmt
          );

    // CODECS PIPELINES
    codecPipeline_ref           cP, cpr0;

    // PIPELINE: SOURCE FMT --> PAINT FMT
    cP= buildCodecPipeline("Source-->Paint", 0, sourceFmt, FMT_RAW_RGB24);

    // PIPELINE: SOURCE FMT --> TEST FMT
    cpr0= buildCodecPipeline("Source-->Test", 1, sourceFmt, testFmt);

    // IMAGE SAVER
    imageSaver_ref imgSaver;

    imageSaverFactoryInfoList_ref isfil= getImageSaverFactoryInfoList();
    for (imageSaverFactoryInfoList_t::iterator_t i= isfil->begin();
         i != isfil->end();
         i++) {
            imageSaverFactoryInfo_ref isi=
                static_cast<imageSaverFactoryInfo_ref>(i);
            imageSaverDescriptor_ref isd= isi->getDescriptor();

            NOTIFY("CANDIDATE: Image Saver %s [%s] from %s\n",
                   isd->getName(),
                   isd->getDescription(),
                   getFormatNameById(isd->getInputFormat())
                  );

            imageSaverFactory_ref isf = isi->getFactory();
            imgSaver= isf->createImageSaver();
    }

    if (imgSaver.isValid()) {
        imageSaverDescriptor_ref isd= imgSaver->getDescriptor();
        NOTIFY("Image Saver %s [%s] from %s loaded\n",
               isd->getName(),
               isd->getDescription(),
               getFormatNameById(isd->getInputFormat())
              );
    } else {
        NOTIFY("Image Saver NOT LOADED\n");
        NOTIFY("Bailing out\n");
        exit(-1);
    }

    // PINTOR
    displayTask_ref dpy= new displayTask_t;

    int vClass;
    int vDepth;
    dpy->getDefaultVisualCombination((unsigned*)&vDepth, &vClass);

    displayTask_t::window_t *winO=
        new displayTask_t::window_t(
            dpy,
            "original",  // title
            NULL,        // no geometry
            NULL,        // no parent
            vDepth, vClass,
            defWidth, defHeight
        );

    winO->map();

    sleep(1);

    if(!cpr0.isValid()) {
        NOTIFY("funcionando sin codecs... te acordates de cargarlos??\n");
    }

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
    image_ref img= s->getImage();
    int width = img->getWidth();
    int height= img->getHeight();

    winO->resize(width, height);


    // INIT TEST
    //
    // get some images, code, and save them up
    //
    for(unsigned nFrames= 0; nFrames< 0x7fffffff; nFrames++) {
        image_ref img;

        img= s->getImage();

        // pinta la fuente
        paint(cP, winO, img);

        if(cpr0.isValid()) {
            image_ref cimg=
                new image_t(img->getTimestamp(), img->isComplete());

            cimg->setImplementation(img->getImplementation());
            cimg= cpr0->putImage(cimg);

            if(!IMAGE_OK(cimg)) {
                NOTIFY("dispatchMsg: could not encode image\n");
                continue;
            }

            img= cimg;

            // salva la imagen a fichero
#if 0
            char nnn[256];

            sprintf(nnn, "/tmp/kk%03d.jpg", nFrames);

            if(imgSaver.isValid()) {
                char nnn[256];

                sprintf(nnn, "/tmp/kk%03d.test", nFrames);
                imgSaver->save(img, nnn, false);
                NOTIFY("saved [%s]\n", nnn);
            }
#endif
        }

        if((nFrames&timeMask)== 0) {
            struct timeval currTime;
            gettimeofday(&currTime, NULL);

            long elapsed0= (currTime.tv_sec -startTime.tv_sec )*1000+
                          (currTime.tv_usec-startTime.tv_usec)/1000;
            long elapsed1= (currTime.tv_sec -previousTime.tv_sec )*1000+
                          (currTime.tv_usec-previousTime.tv_usec)/1000;

            float frameRate0= float(nFrames)/elapsed0*1000;
            float frameRate1= float(nFrames-previousFrames)/elapsed1*1000;
            NOTIFY("Avg frame rate=%2.2f (%ld msecs) ||\t"
                   "Ins frame rate=%2.2f (%ld msecs)\r",
                   frameRate0,
                   elapsed0,
                   frameRate1,
                   elapsed1
                  );

            unsigned bit;
            for(bit= 0; bit< 32; bit++)
                if((1<< bit)> frameRate1)
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
    releaseV4LuGrabber();
    NOTIFY("Grabbers Released\n");
}


