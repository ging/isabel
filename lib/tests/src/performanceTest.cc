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

vCoderArgs_t  coderArgs;
vCoder_t     *coder;

imgTranslator_t *translator;
unsigned char *transBuff;
int transSize;

long encodingTime, decodingTime, translatorTime;

int
encode(unsigned char *dst,
       int dstlen,
       unsigned char* src,
       int srcWidth,
       int srcHeight
      )
{
    int inSize;
    unsigned char *inBuff;

    if (translator == NULL)
    {
        // if not translation, input for coder is the src image

        inSize = vFrameSize(srcWidth, srcHeight, sourceFmt);
        inBuff= src;
    }
    else
    {
        // if translation, input for coder is the translated image
        
        int dstWidth=-1, dstHeight= -1;

        inSize = vFrameSize(srcWidth, srcHeight, translator->getOutputFormat());

        if (inSize > transSize) // reallocation if needed
        {
            delete []transBuff;
            transSize= inSize;
            transBuff= new unsigned char[transSize];
            if ( ! transBuff )
            {
                printf("coderWrapper_t::encode: run out of memory\n");
                printf("coderWrapper_t::encode: skipping\n");
                transSize = -1;
                return -1;
            }
        }
 
        translator->translate(transBuff,
                              dstWidth, dstHeight,
                              src,
                              srcWidth, srcHeight
                             );

        inSize= vFrameSize(dstWidth, dstHeight, translator->getOutputFormat());
        inBuff= transBuff;
    }

    struct timeval afterTime;
    struct timeval beforeTime;

    gettimeofday(&beforeTime, NULL);
    int n1 = vEncode(coder,
                     inBuff,
                     inSize,
                     dst,
                     dstlen
                    );
    gettimeofday(&afterTime, NULL);

    long encTime= (afterTime.tv_sec  - beforeTime.tv_sec )*1000000 +
                  (afterTime.tv_usec - beforeTime.tv_usec);

    encodingTime += encTime;

    if (n1 <= 0)
    {
        printf("coderWrapper_t::encode: vEncode error\n");
        printf("coderWrapper_t::encode: skipping\n");
        return -2;
    }

    if (dstlen < n1)
    {
        return 0;
    }

    return n1;
}

int
main(int argc, char **argv)
{

    grabberName= NULL;

    char *sourceFmtName= NULL;
    char *testCodecName= NULL;
    int   maxFrames    = 0x7fffffff;
    int   BW           = 512000;

    defWidth = 320;
    defHeight= 240;

    encodingTime= decodingTime= translatorTime= 0;

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
    coder= NULL;

    coderArgs.width     = actualW;
    coderArgs.height    = actualH;
    coderArgs.bitRate   = BW;
    coderArgs.frameRate = 25;
    coderArgs.maxInter  = 2*25;
    coderArgs.quality   = 80;
    coderArgs.format    = sourceFmt;

    // TRANSLATOR, if needed
    translator= NULL;

    transBuff= NULL;
    int transSize= 0;

    try
    {
        printf("BUILDING CODER FOR %s -> %s\n",
               vGetFormatNameById(sourceFmt),
               vGetFormatNameById(testCodecFmt)
              );

        coder = vGetCoderByFmt(testCodecFmt, &coderArgs);
        if (coder == NULL)
        {
            printf("CANNOT BUILT THE CODER!!\n");
            printf("coderWrapper_t:: codec not found for %s!!\n",
                   vGetFormatNameById(testCodecFmt)
                  );
            throw "No coder found";
        }

        if (coderArgs.format != sourceFmt)
        {
            try
            {
                translator = new imgTranslator_t(sourceFmt, coderArgs.format);
            }
            catch (const char *e)
            {
                printf("coderWrapper_t:: cannot build translator %s -> %s"
                       ", error= %s\n",
                       vGetFormatNameById(sourceFmt),
                       vGetFormatNameById(coderArgs.format),
                       e
                      );
                exit(1);
            }
            printf("coderWrapper_t:: built translator %s -> %s\n",
                   vGetFormatNameById(sourceFmt),
                   vGetFormatNameById(coderArgs.format)
                  );
            transSize= 1024 * 768 * 3;
            transBuff= new unsigned char[transSize];
        }
        else
        {
            printf("coderWrapper_t:: no translator needed\n");
        }

        printf("CODER BUILT CORRECTLY\n");

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

    // TIMING
    struct timeval startTime;
    struct timeval previousTime;
    unsigned       previousFrames= 0;

    gettimeofday(&startTime, NULL);
    gettimeofday(&previousTime, NULL);
    unsigned timeMask= 0;

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

        // codifica
        int length = 0;
        u8 *src= srcImg->getBuff();
        length = encode(lineimage, lineimagesize, src, actualW, actualH);

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

        struct timeval afterTime;
        struct timeval beforeTime;

        gettimeofday(&beforeTime, NULL);
        // descodifica
        int res= vDecode(decoder,
                         lineimage,
                         length,
                         outBuff,
                         outBuffLen
                        );
        gettimeofday(&afterTime, NULL);

        long decTime= (afterTime.tv_sec  - beforeTime.tv_sec )*1000000 +
                      (afterTime.tv_usec - beforeTime.tv_usec);

        decodingTime += decTime;


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
            NOTIFY_ND("Avg fps=%2.2f (%ld msecs) | "
                      "Ins fps=%2.2f (%ld msecs) | ",
                      frameRate0,
                      elapsed0,
                      frameRate1,
                      elapsed1
                     );
            NOTIFY_ND("Avg Enc=%2.2f usecs/frame (%ld msecs) | ",
                      nFrames > 0 ? float(encodingTime) / nFrames : 0,
                      encodingTime / 1000
                     );
            NOTIFY_ND("Avg Dec=%2.2f usecs/frame (%ld msecs) | ",
                      nFrames > 0 ? float(decodingTime) / nFrames : 0,
                      decodingTime / 1000
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

