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

//
// Port Audio implementation, heavily based on patest_record.c
// distributed with PortAudio, see http://portaudio.com
//

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <icf2/notify.hh>

#define SAVE_PLAY_TO_FILE 0
#define SAVE_REC_TO_FILE 0

#if SAVE_PLAY_TO_FILE
FILE *playFile= NULL;
#endif
#if SAVE_REC_TO_FILE
FILE *recFile= NULL;
#endif

#include "pasounddevice.h"

#define NUM_CHANNELS    (1)

/* Select sample format. */
#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif


#if USE_REC_CALLBACK
/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
int paRecCallback(const void *inputBuffer,
                  void *outputBuffer,
                  unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo* timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData
                 )
{
    PASoundDevice_t *parecsd= (PASoundDevice_t*)userData;

    assert (framesPerBuffer > 0 && "paRecCallback");

    if (inputBuffer == NULL)
    {
        NOTIFY("inputBuffer es NULL!!!\n");
    }

    parecsd->recRingBuffer.Enqueue((i16*)inputBuffer, framesPerBuffer);

    return paContinue;
}
#endif

#if USE_PLAY_CALLBACK
/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
int paPlayCallback(const void *inputBuffer,
                   void *outputBuffer,
                   unsigned long framesPerBuffer,
                   const PaStreamCallbackTimeInfo* timeInfo,
                   PaStreamCallbackFlags statusFlags,
                   void *userData
                  )
{
    PASoundDevice_t *paplaysd= (PASoundDevice_t*)userData;

    unsigned int ret= 0;

    assert (framesPerBuffer > 0 && "paPlayCallback");

    u8 data[2 * framesPerBuffer]; // 2 bytes per sample
    ret= paplaysd->playRingBuffer.Dequeue((i16*)data,
                                          framesPerBuffer,
                                          false
                                         );

    memcpy(outputBuffer, data, 2 * ret);

    // less samples than requested
    if ( ret < framesPerBuffer ) // silence
    {
        NOTIFY("Playing %d samples of silence\n", framesPerBuffer - ret);
        memset((char*)outputBuffer + 2 * ret, 0, 2 * (framesPerBuffer - ret));
    }

    return paContinue;
}
#endif

PASoundDevice_t::PASoundDevice_t(void)
: playStream(NULL),
  recStream(NULL)
#if USE_PLAY_CALLBACK
  , playRingBuffer(48000, 24000)  // 1 sec, seems excesive
#endif
#if USE_REC_CALLBACK
  , accum(0)
  , recRingBuffer(48000, 24000)   // 1 sec, seems excesive
#endif
{
    PaError err;

    //
    // PLAYER
    //
#if SAVE_PLAY_TO_FILE
    playFile= fopen ("/tmp/playkk.raw", "w");
#endif

    err = Pa_Initialize();
    if ( err != paNoError )
    {
        NOTIFY("PASoundDevice_t:: error, no audio player device\n");
        return;
    }

    // default output device
    playParameters.device= Pa_GetDefaultOutputDevice();

    if (playParameters.device == paNoDevice)
    {
        NOTIFY("PASoundDevice_t:: ERROR: No default output device.\n");
        Pa_Terminate();
        return;
    }

    playParameters.channelCount= 1;                     /* stereo output */
    playParameters.sampleFormat=  PA_SAMPLE_TYPE;
    playParameters.suggestedLatency=
        Pa_GetDeviceInfo (playParameters.device)->defaultLowOutputLatency;
    playParameters.hostApiSpecificStreamInfo= NULL;

    err= Pa_OpenStream(&playStream,
                       NULL, /* no input */
                       &playParameters,
                       SAMPLE_RATE,
                       paFramesPerBufferUnspecified,
                       paClipOff,      /* we won't output out of range samples so don't bother clipping them */
#if USE_PLAY_CALLBACK
                       paPlayCallback,
                       this // userData for playCallback
#else
                       NULL,
                       NULL
#endif
                      );

    if (err != paNoError)
    {
        NOTIFY("PASoundDevice_t:: error opening output stream\n");
        assert((playStream == NULL) && "PASoundDevice_t");
        return;
    }

    err = Pa_StartStream(playStream);
    if (err != paNoError)
    {
        NOTIFY("PASoundDevice_t:: error starting output stream\n");
        Pa_Terminate();
        playStream= NULL; // to avoid capturing and more Pa_Terminate...
    }

    //
    // RECORDER
    //

#if SAVE_REC_TO_FILE
    recFile= fopen ("/tmp/reckk.raw", "w");
#endif

    err = Pa_Initialize();
    if ( err != paNoError )
    {
        NOTIFY("PASoundDevice_t:: error, no audio player device\n");
        return;
    }

    // default input device
    recParameters.device= Pa_GetDefaultInputDevice();
    if (recParameters.device == paNoDevice)
    {
        NOTIFY("PASoundDevice_t:: Error: No default input device.\n");
        Pa_Terminate();
        return;
    }

    recParameters.channelCount= 1;                    /* stereo input */
    recParameters.sampleFormat= PA_SAMPLE_TYPE;
    recParameters.suggestedLatency=
        Pa_GetDeviceInfo(recParameters.device)->defaultLowInputLatency;
    recParameters.hostApiSpecificStreamInfo = NULL;

    /* Record some audio. -------------------------------------------- */
    err= Pa_OpenStream(&recStream,
                       &recParameters,
                       NULL,    /* no Output */
                       SAMPLE_RATE,
                       1024, //paFramesPerBufferUnspecified,
                       paClipOff,      /* we won't output out of range samples so don't bother clipping them */
#if USE_REC_CALLBACK
                       paRecCallback,
                       this // userData for playCallback
#else
                       NULL,
                       NULL
#endif
                      );

    if (err != paNoError)
    {
        NOTIFY("PASoundDevice_t:: error opening input stream\n");
        assert((recStream == NULL) && "PASoundDevice_t");
        return;
    }

    err= Pa_StartStream(recStream); // move to startCapture?
    if (err != paNoError)
    {
        NOTIFY("PASoundDevice_t:: error starting stream\n");
    }
}

PASoundDevice_t::~PASoundDevice_t(void)
{
#if SAVE_PLAY_TO_FILE
    fclose(playFile);
    playFile= NULL;
#endif

    if (playStream)
    {
        // paired, not neccessarily nested, with Pa_Initialize,
        // only if Pa_Initialize was succesful
        Pa_Terminate();
    }

#if SAVE_REC_TO_FILE
    fclose(recFile);
    recFile= NULL;
#endif

    if (recStream)
    {
        // paired, not neccessarily nested, with PaInitialize,
        // only if PaInitialize was succesful
        Pa_Terminate();
    }
}

void
PASoundDevice_t::startCapture(void)
{
}


void
PASoundDevice_t::stopCapture(void)
{
}

void
PASoundDevice_t::startPlaying(void)
{
}

void
PASoundDevice_t::stopPlaying(void)
{
}

void
PASoundDevice_t::resetDevices(void)
{
    PaError err;

    NOTIFY("RESETTING\n");

    if ( ! recStream )
    {
        NOTIFY("CANNOT RESET DEVICES\n");
    }


    err= Pa_StopStream(recStream);
    if ( err != paNoError )
    {
        NOTIFY("PASoundDevice_t::resetDevices: error stopping stream\n");
        return;
    }

    err= Pa_StartStream(recStream); // move to startCapture?
    if (err != paNoError)
    {
        NOTIFY("PASoundDevice_t::resetDevices: error starting stream\n");
    }
}

// -----------------------------------------------------------------------------
// PASoundDevice_t::read
// Lee del buffer de captura todo lo que hay disponible sin pasarse
// del tamaño del buffer de salida
// -----------------------------------------------------------------------------
int
PASoundDevice_t::read(unsigned char *pBuffer, int length)
{
#if USE_REC_CALLBACK
    int ret= recRingBuffer.Dequeue((i16*)pBuffer, length / 2, false) * 2;

    if (ret <= 0)
    {
        NOTIFY("PARecSoundDevice_t::read: no samples available\n");
        return 0;
    }

#if SAVE_REC_TO_FILE
    fwrite(pBuffer, 1, ret, recFile);
#endif

#if 0
    // non blocking call, we need to "simulate" time going by...
    accum += ret;

    while (accum > SAMPLES_IN_100_mS)
    {
#if 0
        struct timespec req;

        req.tv_sec=0;
        req.tv_nsec=100000000L;
        nanosleep(&req, NULL);

#else
        usleep(100000); // sleep 100 milliseconds
#endif
        accum -= SAMPLES_IN_100_mS;
    }
#endif

    return ret;
#else
    PaError err;

    if (recStream == NULL) return 0;

    err= Pa_ReadStream(recStream, pBuffer, length / 2);

    if (err != paNoError)
    {
        NOTIFY("PASoundDevice_t::read: error\n");
        return 0;
    }

#if SAVE_REC_TO_FILE
    fwrite(pBuffer, 1, length, recFile);
#endif

    return length;
#endif
}

bool
PASoundDevice_t::write(const unsigned char *pBuffer, int length)
{
#if SAVE_PLAY_TO_FILE
    fwrite(pBuffer, 1, length, playFile);
#endif

#if USE_PLAY_CALLBACK
    return playRingBuffer.Enqueue((i16*)pBuffer, length / 2) * 2;
#else
    PaError err;

    if (playStream == NULL) return false;

    err= Pa_WriteStream(playStream, pBuffer, length / 2);

    if (err != paNoError)
    {
        NOTIFY("PASoundDevice_t::write: error\n");
        return false;
    }

    return true;
#endif
}

bool
PASoundDevice_t::setPlayDevice(const char *device)
{
    return true;
}


// -----------------------------------------------------------------------------
// PASoundDevice_t::setCaptureDevice
// 
// -----------------------------------------------------------------------------
bool
PASoundDevice_t::setCaptureDevice(const char *device)
{
    return true;
}

