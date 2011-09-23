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

#include <icf2/notify.hh>

#include "winsounddevice.h"
#include "Dxerr.h"


// -----------------------------------------------------------------------------
// WinSoundDevice_t::WinSoundDevice_t
// Constructor de la clase. Inicializa los bufferes
// -----------------------------------------------------------------------------
//
WinSoundDevice_t::WinSoundDevice_t(void *renderDevice, void *captureDevice)
{
    CoInitializeEx(NULL, 0);
    if (/*SoundDeviceInit(renderDevice, captureDevice, true)*/false)
    {
        NOTIFY("Dispositivo Inicializado con AEC\n");
    }
    else if (SoundDeviceInit(renderDevice, captureDevice, false))
    {
        NOTIFY("Dispositivo Inicializado sin AEC\n");
    }
    else
    {
        NOTIFY("Error al inicializar el dispositivo\n");
        exit(0);
    }
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::SoundDeviceInit
// Inicializa los bufferes
// -----------------------------------------------------------------------------
//
bool
WinSoundDevice_t::SoundDeviceInit(void *renderDevice,
                             void *captureDevice,
                             bool aec
                            )
{

    // En windows por ahora no permito seleccion de dispositivo
    renderDevice = NULL;
    captureDevice = NULL;

    pDSBuffer = NULL;
    pDSCaptureBuffer = NULL;
    pDSFullDuplex = NULL;

    HRESULT hr;
    HWND hWnd = GetDesktopWindow();

    WAVEFORMATEX wfx= {WAVE_FORMAT_PCM, 1, SAMPLE_RATE, SAMPLE_RATE*BPS , BPS, 8*BPS, 0};
    // wFormatTag, nChannels, nSamplesPerSec, mAvgBytesPerSec,
    // nBlockAlign, wBitsPerSample, cbSize

    // Descripcion del buffer de captura
    DSCBUFFERDESC dscbd;
    DSCEFFECTDESC effects;

    effects.dwSize            = sizeof(DSCEFFECTDESC);
    effects.dwFlags           = DSCFX_LOCSOFTWARE;
    effects.guidDSCFXClass    = GUID_DSCFX_CLASS_AEC;
    effects.guidDSCFXInstance = GUID_DSCFX_MS_AEC;
    effects.dwReserved1       = 0;
    effects.dwReserved2       = 0;

    dscbd.dwSize = sizeof(DSCBUFFERDESC);
    dscbd.dwFlags = aec?DSCBCAPS_CTRLFX:0;
    dscbd.dwBufferBytes = REC_BUF_SIZE;
    dscbd.dwReserved = 0;
    dscbd.lpwfxFormat = &wfx;
    dscbd.dwFXCount = aec?1:0;
    dscbd.lpDSCFXDesc = aec?&effects:NULL;

    // Descripcion del buffer de escritura
    DSBUFFERDESC dsbdesc;
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags =   DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME
                      | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
    dsbdesc.dwBufferBytes = PLAY_BUF_SIZE;
    dsbdesc.lpwfxFormat = &wfx;

    // Creo los el interfaz con el dispositivo y los buferes
    hr = DirectSoundFullDuplexCreate8((LPGUID)captureDevice,
                                      (LPGUID)renderDevice,
                                      &dscbd,
                                      &dsbdesc,
                                      hWnd,
                                      DSSCL_PRIORITY,
                                      &pDSFullDuplex,
                                      &pDSCaptureBuffer,
                                      &pDSBuffer,
                                      NULL
                                     );
    if (FAILED(hr))
    {
        NOTIFY("%s\n", DXGetErrorDescription(hr));
        NOTIFY("Error al crear dispositivo\n");
        return false;
    }

    if (aec)
    {
        // Enable echo canceller
        IDirectSoundCaptureFXAec8 * echo = NULL;
        pDSCaptureBuffer->GetObjectInPath(GUID_DSCFX_CLASS_AEC,
                                          0,
                                          IID_IDirectSoundCaptureFXAec8,
                                          (void **)&echo
                                         );
        if(FAILED(hr))
        {
            NOTIFY("Error al obtener el cancelador de echo\n");
        }

        else if (echo)
        {
            NOTIFY("Configurando cancelador de echo\n");

            DSCFXAec params;
            echo->GetAllParameters(&params);
            params.fEnable = true;
            params.fNoiseFill = true;
            hr = echo->SetAllParameters(&params);
            if(FAILED(hr))
            {
                NOTIFY("Error al configurar el cancelador de echo\n");
            }

            echo->Release();
        }
    }
    return true;
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::~WinSoundDevice_t
// Destructor de la clase. Libera los recursos de directsound
// -----------------------------------------------------------------------------
//
WinSoundDevice_t::~WinSoundDevice_t(void)
{
    if (pDSBuffer)
        pDSBuffer->Release();

    if (pDSCaptureBuffer)
        pDSCaptureBuffer->Release();

    if (pDSFullDuplex)
        pDSFullDuplex->Release();

    CoUninitialize();
}


// -----------------------------------------------------------------------------
// WinSoundDevice_t::StartCapture
//
// -----------------------------------------------------------------------------
//
void WinSoundDevice_t::startCapture(void)
{
    HRESULT hr = pDSCaptureBuffer->Start(DSCBSTART_LOOPING);
    if (FAILED(hr))
        NOTIFY("Error al hacer record\n");
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::StopCapture
//
// -----------------------------------------------------------------------------
//
void WinSoundDevice_t::stopCapture(void)
{
    HRESULT hr = pDSCaptureBuffer->Stop();
    if (FAILED(hr))
        NOTIFY("Error al hacer stop\n");
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::StartPlaying
//
// -----------------------------------------------------------------------------
//
void WinSoundDevice_t::startPlaying(void)
{
    HRESULT hr = pDSBuffer->Play(0,0,DSBPLAY_LOOPING);
    if (FAILED(hr))
        NOTIFY("Error al hacer play\n");
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::StopPlaying
//
// -----------------------------------------------------------------------------
//
void WinSoundDevice_t::stopPlaying(void)
{
    HRESULT hr = pDSBuffer->Stop();
    if (FAILED(hr))
        NOTIFY("Error al hacer stop\n");
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::resetDevices
//
// -----------------------------------------------------------------------------
//
void
WinSoundDevice_t::resetDevices(void)
{
    // NOT IMPLEMENTED
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::read
// Lee del buffer de captura todo lo que hay disponible sin pasarse
// del tamaño del buffer de salida
// -----------------------------------------------------------------------------
//
int
WinSoundDevice_t::read(unsigned char *pBuffer, int length)
{
    static DWORD offset = 0;

    HRESULT hr;
    VOID* pbCaptureData  = NULL;
    DWORD dwCaptureLength;
    VOID* pbCaptureData2 = NULL;
    DWORD dwCaptureLength2;

    DWORD dwReadPos;
    LONG lLockSize;

    if (FAILED (hr = pDSCaptureBuffer->GetCurrentPosition(NULL, &dwReadPos)))
        return -1;

    // Lock everything between the private cursor
    // and the read cursor, allowing for wraparound.
    lLockSize = dwReadPos - offset;

    if( lLockSize < 0 ) lLockSize +=  REC_BUF_SIZE;

    // Como mucho leo el tamaño del buffer
    if (lLockSize > length)
        lLockSize = length;


    if( lLockSize == 0 ) return 0;


    if (FAILED(hr = pDSCaptureBuffer->Lock(
        offset, lLockSize,
        &pbCaptureData, &dwCaptureLength,
        &pbCaptureData2, &dwCaptureLength2, 0)))
        return -1;

    // Write the data. This is done in two steps
    // to account for wraparound.
    CopyMemory(pBuffer, pbCaptureData, dwCaptureLength);

    if (NULL != pbCaptureData2)
    {
        CopyMemory(pBuffer+dwCaptureLength, pbCaptureData2, dwCaptureLength2);
    }

    // Unlock the capture buffer.
    pDSCaptureBuffer->Unlock(pbCaptureData,
                             dwCaptureLength,
                             pbCaptureData2,
                             dwCaptureLength2
                            );

    // Move the capture offset forward.
    offset += lLockSize;
    offset %= REC_BUF_SIZE;

    return lLockSize;
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::write
//
// -----------------------------------------------------------------------------
//
bool
WinSoundDevice_t::write(const unsigned char *pBuffer, int length)
{
    static DWORD offset = 0;

    LPVOID  lpvPtr1;
    DWORD   dwBytes1;
    LPVOID  lpvPtr2;
    DWORD   dwBytes2;
    HRESULT hr;

    DWORD play, write;

    pDSBuffer->GetCurrentPosition(&play, &write);

    // Si estamos fuera de sincronismo resincronizamos
    if ( (play < offset) && (offset < write) )   // He escrito poco y el puntero de reproduccion me esta cogiendo
    {
        //NOTIFY("SD Desincronizado\n");
        offset = (write + 1000)%PLAY_BUF_SIZE;
    }

    DWORD temp = (offset+length)%PLAY_BUF_SIZE;
    if ( (play < temp) && (temp < write) )   // He escrito demasiado y estoy cogiendo al puntero de reproduccion o me ha pasado de largo
    {
        //NOTIFY("SD Desincronizado 2\n");
        offset = (write + 1000)%PLAY_BUF_SIZE;
    }

    // Obtain memory address of write block. This will be in two parts
    // if the block wraps around.
    hr = pDSBuffer->Lock(offset,
                         length,
                         &lpvPtr1,
                         &dwBytes1,
                         &lpvPtr2,
                         &dwBytes2,
                         0
                        );

    // If the buffer was lost, restore and retry lock.
    if (DSERR_BUFFERLOST == hr)
    {
        pDSBuffer->Restore();
        hr = pDSBuffer->Lock(offset,
                             length,
                             &lpvPtr1,
                             &dwBytes1,
                             &lpvPtr2,
                             &dwBytes2,
                             0
                            );
    }

    if (SUCCEEDED(hr))
    {
        // Write to pointers.
        CopyMemory(lpvPtr1, pBuffer, dwBytes1);
        if (NULL != lpvPtr2)
        {
            CopyMemory(lpvPtr2, pBuffer+dwBytes1, dwBytes2);
        }

        // Release the data back to DirectSound.
        hr = pDSBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2,dwBytes2);

        if (SUCCEEDED(hr))
        {
            // Success
            offset += length;
            offset %= PLAY_BUF_SIZE;
            return TRUE;
        }
    }

    NOTIFY("error al reproducir\n");
    // Lock, Unlock, or Restore failed
    return FALSE;
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::setPlayDevice
//
// -----------------------------------------------------------------------------
//
bool
WinSoundDevice_t::setPlayDevice(const char *device)
{
    // En windows por ahora no permito seleccion de dispositivo
    NOTIFY("setPlayDevice\n");
    return TRUE;
}

// -----------------------------------------------------------------------------
// WinSoundDevice_t::setCaptureDevice
//
// -----------------------------------------------------------------------------
//
bool
WinSoundDevice_t::setCaptureDevice(const char *device)
{
    // En windows por ahora no permito seleccion de dispositivo
    NOTIFY("setCaptureDevice\n");
    return TRUE;
}

