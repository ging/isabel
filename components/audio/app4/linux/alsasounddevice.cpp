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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <icf2/notify.hh>


#include "alsasounddevice.h"



// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::AlsaSoundDevice_t
// Constructor de la clase. Inicializa los dispositivos
// -----------------------------------------------------------------------------
//
AlsaSoundDevice_t::AlsaSoundDevice_t(const char *renderDevice, const char *captureDevice)
{
    char render[32];

    if (strcmp(renderDevice, "100"))
    {
        sprintf(render, "plughw:%s,0", renderDevice);
    }
    else
    {
        sprintf(render, "plug:dmix");
    }

    char capture[32];
    sprintf(capture, "plughw:%s,0", captureDevice);

    if ( ! initRenderDevice(render)) renderHandle = NULL;
    if ( ! initCaptureDevice(capture)) captureHandle = NULL;
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::~AlsaSoundDevice_t
// Destructor de la clase. Libera los recursos de directsound
// -----------------------------------------------------------------------------
//
AlsaSoundDevice_t::~AlsaSoundDevice_t(void)
{
    if (captureHandle)
    {
        snd_pcm_close (captureHandle);
        captureHandle = NULL;
    }

    if (renderHandle)
    {
        snd_pcm_close (renderHandle);
        renderHandle = NULL;
    }

}


// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::xrunRecovery
// 
// -----------------------------------------------------------------------------
//
int
AlsaSoundDevice_t::xrunRecovery(snd_pcm_t *handle, int err)
{
    if (err == -EPIPE)      /* under-run */
    {
        err = snd_pcm_prepare(handle);
        if (err < 0)
        {
            NOTIFY("Can't recovery from underrun, prepare failed: %s\n",
                   snd_strerror(err)
                  );
        }
        else
        {
            NOTIFY("Recovered\n");
            return 0;
        }

    }
    else if (err == -ESTRPIPE)
    {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN)
            sleep(1);       /* wait until the suspend flag is released */

        if (err < 0)
        {
            err = snd_pcm_prepare(handle);
            if (err < 0)
            {
                NOTIFY("Can't recovery from suspend, prepare failed: %s\n",
                       snd_strerror(err)
                      );
            }
        }
        else
        {
            NOTIFY("Recovered 2\n");
            return 0;
        }
    }
    return err;
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::initCaptureDevice
// 
// -----------------------------------------------------------------------------
//
bool
AlsaSoundDevice_t::initCaptureDevice(const char *device)
{
    int err = 0;

    snd_pcm_hw_params_t* hwParams;

    if (device == NULL)
    {
        device = "plughw:0,0";
    }

    NOTIFY("Initializing Audio Capturer: %s\n", device);

    err= snd_pcm_open(&captureHandle,
                      device,
                      SND_PCM_STREAM_CAPTURE,
                      SND_PCM_NONBLOCK
                     );

    if (err < 0)
    {
        NOTIFY("cannot open audio device %s (%s)\n", 
               device,
               snd_strerror(err)
              );
        return false;
    }


    if ((err = snd_pcm_hw_params_malloc (&hwParams)) < 0)
    {
        NOTIFY("cannot allocate hardware parameter structure (%s)\n",
               snd_strerror(err)
              );
        return false;
    }

    if ((err = snd_pcm_hw_params_any (captureHandle, hwParams)) < 0)
    {
        NOTIFY("cannot initialize hardware parameter structure (%s)\n",
               snd_strerror(err)
              );
        return false;
    }

    if ((err = snd_pcm_hw_params_set_access (captureHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        NOTIFY("cannot set access type (%s)\n", snd_strerror(err));
        return false;
    }

    if ((err = snd_pcm_hw_params_set_format (captureHandle, hwParams, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        NOTIFY("cannot set sample format (%s)\n", snd_strerror(err));
        return false;
    }

    unsigned int rate = SAMPLE_RATE;
    if ((err = snd_pcm_hw_params_set_rate_near (captureHandle, hwParams, &rate, 0)) < 0)
    {
        NOTIFY("cannot set sample rate (%s)\n", snd_strerror(err));
        return false;
    }

    NOTIFY("Selected Rate: %d\n", rate);

    if ((err = snd_pcm_hw_params_set_channels (captureHandle, hwParams, 1)) < 0)
    {
        NOTIFY("cannot set channel count (%s)\n", snd_strerror(err));
        return false;
    }

    snd_pcm_uframes_t  size = REC_BUF_SIZE;
    if (( err = snd_pcm_hw_params_set_buffer_size_near(captureHandle, hwParams,  &size)) < 0)
    {
        NOTIFY("cannot set buffer size %s\n", snd_strerror(err));
        return false;
    }
    NOTIFY("Selected Buffer Size: %lu\n", size);


    if ((err = snd_pcm_hw_params (captureHandle, hwParams)) < 0)
    {
        NOTIFY( "cannot set parameters (%s)\n", snd_strerror(err));
        return false;
    }

    snd_pcm_hw_params_free (hwParams);

    /*      
    if ((err = snd_pcm_prepare (captureHandle)) < 0) {
    NOTIFY( "cannot prepare audio interface for use (%s)\n",
    snd_strerror (err));
    return false;
    }
    */

    snd_pcm_state_t state= snd_pcm_state(captureHandle);

    NOTIFY("Audio Capturer Ready, State= %d\n", state);

    return true;
}



// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::initRenderDevice
// 
// -----------------------------------------------------------------------------
//
bool
AlsaSoundDevice_t::initRenderDevice(const char *device)
{
    int err = 0;

    // Configuracion de los parametros HW
    snd_pcm_hw_params_t *hwParams;

    if (device == NULL)
    {
        device = "plughw:0,0";
    }

    NOTIFY("Initializing play device: %s\n", device);

    if ((err = snd_pcm_open (&renderHandle, device, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0)
    {
        NOTIFY("cannot open audio device %s (%s)\n", 
               device,
               snd_strerror (err)
              );
        return false;
    }

    if ((err = snd_pcm_hw_params_malloc (&hwParams)) < 0)
    {
        NOTIFY("cannot allocate hardware parameter structure (%s)\n",
               snd_strerror (err)
              );
        return false;
    }

    if ((err = snd_pcm_hw_params_any (renderHandle, hwParams)) < 0)
    {
        NOTIFY("cannot initialize hardware parameter structure (%s)\n",
               snd_strerror (err)
              );
        return false;
    }

    if ((err = snd_pcm_hw_params_set_access (renderHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        NOTIFY("cannot set access type (%s)\n", snd_strerror(err));
        return false;
    }

    if ((err = snd_pcm_hw_params_set_format (renderHandle, hwParams, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        NOTIFY("cannot set sample format (%s)\n", snd_strerror(err));
        return false;
    }

    unsigned int rate = SAMPLE_RATE;    
    if ((err = snd_pcm_hw_params_set_rate_near (renderHandle, hwParams, &rate, 0)) < 0)
    {
        NOTIFY("cannot set sample rate (%s)\n", snd_strerror(err));
        return false;
    }

    NOTIFY("Selected Rate: %d\n", rate);

    if ((err = snd_pcm_hw_params_set_channels (renderHandle, hwParams, 1)) < 0)
    {
        NOTIFY("cannot set channel count (%s)\n", snd_strerror(err));
        return false;
    }

    snd_pcm_uframes_t size= PLAY_BUF_SIZE;

    if (( err = snd_pcm_hw_params_set_buffer_size_near(renderHandle, hwParams,  &size)) < 0)
    {
        NOTIFY("cannot set buffer size %s\n", snd_strerror(err));
        return false;
    }
    NOTIFY("Selected Buffer Size: %lu\n", size);

    if ((err = snd_pcm_hw_params (renderHandle, hwParams)) < 0)
    {
        NOTIFY("cannot set hw parameters (%s)\n", snd_strerror(err));
        return false;
    }

    // Configuracion de los parametros SW
    snd_pcm_sw_params_t *swParams;

    if ((err = snd_pcm_sw_params_malloc (&swParams)) < 0)
    {
        NOTIFY("cannot allocate software parameter structure (%s)\n",
               snd_strerror (err)
              );
        return false;
    }

    if ((err = snd_pcm_sw_params_current (renderHandle, swParams)) < 0)
    {
        NOTIFY("cannot initialize software parameter structure (%s)\n",
               snd_strerror (err)
              );
        return false;
    }

    unsigned int threshold= SAMPLE_RATE / 8; //125 ms

    if (threshold > size)
    {
        NOTIFY("Buffer smaller than 125 ms\n");
        threshold = 5000;
    }    

    if ((err = snd_pcm_sw_params_set_start_threshold (renderHandle, swParams, threshold)) < 0)
    {
        NOTIFY("cannot set threshold count (%s)\n", snd_strerror(err));
        return false;
    }

    if ((err = snd_pcm_sw_params (renderHandle, swParams)) < 0)
    {
        NOTIFY("cannot set sw parameters (%s)\n", snd_strerror(err));
        return false;
    }

    snd_pcm_sw_params_free (swParams);
    snd_pcm_hw_params_free (hwParams);

    /*  
    if ((err = snd_pcm_prepare (renderHandle)) < 0)
    {
        NOTIFY( "cannot prepare audio interface for use (%s)\n",
        snd_strerror (err));
        return false;
    }
    */

    snd_pcm_state_t state= snd_pcm_state(renderHandle);

    NOTIFY("Player Ready, State= %d\n", state);

    return true;
}


// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::startCapture
// 
// -----------------------------------------------------------------------------
//
void
AlsaSoundDevice_t::startCapture(void)
{
    if (captureHandle == NULL) return;

    int err = 0;

    if (( err = snd_pcm_prepare (captureHandle)) < 0)
    {
        NOTIFY("cannot prepare audio interface for use (%s)\n",
               snd_strerror(err)
              );
        exit(1);
    }

    if ((err = snd_pcm_start(captureHandle)) < 0)
    {
        NOTIFY("cannot start audio capture interface (%s)\n",
               snd_strerror(err)
              );
        exit(1);
    }

    snd_pcm_state_t state= snd_pcm_state(captureHandle);

    NOTIFY("Audio Capturing, State= = %d\n", state);
}


// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::stopCapture
//
// -----------------------------------------------------------------------------
//
void
AlsaSoundDevice_t::stopCapture(void)
{
    if (captureHandle == NULL) return;

    int err= 0;

    if ((err = snd_pcm_drop(captureHandle)) < 0)
    {
        NOTIFY("error stopping audio capture device (%s)\n",
               snd_strerror(err)
              );
        exit(1);
    }
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::startPlaying
// 
// -----------------------------------------------------------------------------
void
AlsaSoundDevice_t::startPlaying(void)
{
    /*
    snd_pcm_state_t state = snd_pcm_state(renderHandle);

    if (state != SND_PCM_STATE_RUNNING)
    {
        int err = 0;
        if ((err = snd_pcm_start(renderHandle)) < 0)
        {
            NOTIFY("cannot start audio play interface (%s)\n",
                   snd_strerror(err)
                  );
            exit(1);
        }
    }
    */
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::stopPlaying
// 
// -----------------------------------------------------------------------------
void
AlsaSoundDevice_t::stopPlaying(void)
{
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::resetDevices
// 
// -----------------------------------------------------------------------------
void
AlsaSoundDevice_t::resetDevices(void)
{
    // not implemented
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::read
// Lee del buffer de captura todo lo que hay disponible sin pasarse
// del tamaño del buffer de salida
// -----------------------------------------------------------------------------
//
int
AlsaSoundDevice_t::read(unsigned char *pBuffer, int length)
{  
    static int fails = 0;

    if (captureHandle == NULL) return 0;

    int readed= snd_pcm_readi(captureHandle, pBuffer, length/BPS);

/*
    struct timeval now;
    gettimeofday(&now, NULL);
    NOTIFY(" [%d.%d] readed %d bytes(o muestras)\n",now.tv_sec, now.tv_usec, readed);
*/
    if (readed < 0)
    {
        //NOTIFY( "read error (%s)(%d)\n", snd_strerror (readed), readed);
        // Si es temporalmente no disponible paso de el a no ser que sean muchas
        // veces seguidas
        // if (readed == -EAGAIN)
        // {
        //     if (++fails == 50)
        //     {
        //         NOTIFY("Too many temporarily unavalaible errors\n");
        //         exit(1);
        //     } 
        // }
        // Si es un XRUN trato de iniciar el dispositivo otra vez
        if (readed == -EPIPE)
        {
            startCapture();
        }
        return 0;
    }
    fails = 0;

    return readed * BPS;
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::write
// 
// -----------------------------------------------------------------------------
//
bool
AlsaSoundDevice_t::write(const unsigned char *pBuffer, int length)
{
    static int fails = 0;

    if (renderHandle == NULL) return false;

    int total = 0;
    while (total < length)
    {
        snd_pcm_sframes_t written=
            snd_pcm_writei(renderHandle, pBuffer+total, (length-total) / BPS);

        if (written < 0)
        {
            NOTIFY("write error (%s)(%d)\n", snd_strerror (written), written);
            if (written == -EAGAIN)
            {
                //if (++fails == 50)
                //{
                //    NOTIFY("Too many temporarily unavalaible errors\n");
                //    exit(1);
                //} 
                return false;
            }
            int err = 0;
            if ((err = xrunRecovery(renderHandle, (int)written)) < 0)
            {
                NOTIFY("cannot recover audio interface for use (%s)\n",
                       snd_strerror (err)
                      );
                exit(1);
            }
            return false;
        }
        total += (written * BPS);
    }

    /* 
    if (written != length/2)
    {
        NOTIFY("escritos distintos w=%d l=%d\n", written, length);
    }
    */
    fails = 0;
    return true;
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::setPlayDevice
// 
// -----------------------------------------------------------------------------
//
bool
AlsaSoundDevice_t::setPlayDevice(const char *device)
{
    if (renderHandle)
    {
        snd_pcm_close (renderHandle);
        renderHandle = NULL;
    }

    char render[32];
    if (strcmp(device, "100"))
    {
        sprintf(render, "plughw:%s,0", device);
    }
    else
    {
        sprintf(render, "plug:dmix");
    }
    
    initRenderDevice(render);

    return true;
}

// -----------------------------------------------------------------------------
// AlsaSoundDevice_t::setCaptureDevice
// 
// -----------------------------------------------------------------------------
//
bool
AlsaSoundDevice_t::setCaptureDevice(const char *device)
{
    if (captureHandle)
    {
        stopCapture();
        snd_pcm_close (captureHandle);
        captureHandle = NULL;
    }

    char capture[32];
    sprintf(capture, "plughw:%s,0", device);
    
    if ( ! initCaptureDevice(capture))
    {
        return false;
    }

    startCapture();

    return true;
}

