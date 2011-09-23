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
// $Id: fragmenter.cc 10824 2007-09-17 10:23:06Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <icf2/general.h>
#ifdef __BUILD_FOR_LINUX
#include <netinet/in.h>
#endif

#include <vCodecs/codecs.h>

#include <icf2/notify.hh>

#include <rtp/RTPPayloads.hh>
#include <rtp/fragmenter.hh>

#include "jpegHeaders.hh"
#include "JPEGimp.h"
#include "H263imp.h"
#include "H264imp.h"
#include "MPEG1imp.h"

fragmenter_t::fragmenter_t(void)
: buffer(NULL),
  numBytes(-1),
  frameData(NULL),
  frameCount(0)
{
}

fragmenter_t::~fragmenter_t(void)
{
    if (frameData)
    {
        free(frameData);
    }
    frameData= NULL; // paranoid
    frameCount= 0;   // paranoid
}

void
fragmenter_t::setMPEG1Frame(u8 *pBuffer, long BufferLen)
{
    frameData = malloc(sizeof(mpeg1FragData_t));

    mpeg1FragData_t *mpeg1Data= static_cast<mpeg1FragData_t*>(frameData);

    frameCount = 0;

    // we need it for creating the RTP/MPEG1 headers
    for (int i = 0; i < BufferLen; i++)
    {
        if ((pBuffer[i]   == 0x00) &&
            (pBuffer[i+1] == 0x00) &&
            (pBuffer[i+2] == 0x01) &&
            (pBuffer[i+3] == 0x00)
           )
        {
            u16 aux  = pBuffer[i+5]; aux &= 0x00c0; aux >>= 6;
            u16 aux2 = pBuffer[i+4]; aux2 <<= 2; aux2 &= 0x03fc;
            mpeg1Data->temporal_reference = aux | aux2;

            u8 aux3 = pBuffer[i+5]; aux3 &= 0x38; aux3 >>= 3;
            mpeg1Data->picture_type = aux3;

            if (mpeg1Data->picture_type == 2)
            {
                u8 aux4 = pBuffer[i+7]; aux4 <<= 1; aux4 &= 0x06;
                u8 aux5 = pBuffer[i+8]; aux5 &= 0x80; aux5 >>= 7;
                mpeg1Data->FFC = aux4 | aux5;
            }
            else
            {
                mpeg1Data->FFC = 0;
            }
        }
    }
}

void
fragmenter_t::setMPEG2Frame(u8 *pBuffer, long BufferLen)
{
    setMPEG1Frame(pBuffer, BufferLen);
}

void
fragmenter_t::setH263Frame(u8 *pBuffer, long BufferLen)
{
    frameData = malloc(sizeof(h263FragData_t));

    h263FragData_t *h263Data = static_cast<h263FragData_t *>(frameData);
    h263Data->mode= 0;
}

void
fragmenter_t::setH264Frame(u8 *pBuffer, long BufferLen)
{
#if 0
    // Here we set get the NAL units

    frameData = malloc(sizeof(h264FragData_t));
    h264FragData_t *h264Data = static_cast<h264FragData_t*>(frameData);
    H264GetNALByFrame(&h264Data->header,
                      pBuffer,
                      BufferLen,
                      payload,
                      h264Data->len
                     );
#endif
}

void
fragmenter_t::setJPEGFrame(u8 *pBuffer, long BufferLen)
{
    frameData= malloc(sizeof(jpegFragData_t));

    jpegFragData_t *jpegData = static_cast<jpegFragData_t*>(frameData);

    // skip the jpeg file header (I think)
    frameCount = 0;
    for (int i= 0; i < numBytes; i++)
    {
        if (   (buffer[i]   == 0xff)
            && (buffer[i+1] == 0xda)
           )
        {
            frameCount = i;
            break;
        }
    }
    frameCount += 14;
    jpegData->type = 0;
    jpegData->typeSpecific = 0;
    memset(jpegData->lqt, 0, 64);
    memset(jpegData->cqt, 0, 64);

    u8 *aux = NULL;

    //-------------------------------------
    // look up dri value
    //-------------------------------------
    jpegData->dri = 0;
    for (int i = 0; i < numBytes; i++)
    {
        if (   (buffer[i]   == 0xff)
            && (buffer[i+1] == 0xdd)
           )
        {
            short driSize = (short)buffer[i+3]; // 2 bytes syze
            for (int k = 0; k < driSize - 2; k++)
            {
                jpegData->dri = jpegData->dri << 8 | buffer[i+4+k];
                jpegData->type= 64;
            }
            break;
        }
    }
    for (int i = 0; i < numBytes; i++)
    {
        if (   (buffer[i]   == 0xff)
            && (buffer[i+1] == 0xdb)
           )
        {
            aux = buffer + i;
            break;
        }
    }
    memcpy (jpegData->lqt, aux+5,  64);
    memcpy (jpegData->cqt, aux+74, 64);

    //-------------------------------------
    // get Q RTP param from quantization tables
    //-------------------------------------
    double Q = (jpegData->lqt[0]*100.0 - 50.0)/jpeg_luma_quantizer[0];
    for (int n = 1; n < 64; n++)
    {
        Q = (Q + (jpegData->lqt[n]*100.0 - 50.0)/jpeg_luma_quantizer[n])/2;
    }
    jpegData->quality = (int)Q;

    buffer += frameCount;   // skip the begining
    numBytes -= frameCount; // less bytes to send
    frameCount= 0;
    frameData= jpegData;
}

u8
fragmenter_t::setFrame(u8   *pBuffer,
                       long  BufferLen,
                       u32   fcc,
                       int   Width,
                       int   Height
                      )
{
    if (frameData)
    {
        free(frameData);
        frameData= NULL; // required, not every format need it
    }

    // common stuff for all video payloads
    this->buffer    = pBuffer;   // borrowed, do not free
    this->numBytes  = BufferLen;
    this->frameCount= 0;
    this->fcc       = fcc;
    this->Width     = Width;
    this->Height    = Height;

    // specific stuff for certain video payloads
    switch (fcc)
    {
    case CELB_FORMAT:
        return CELB_PT;
    case MJPEG_FORMAT:
        setJPEGFrame(pBuffer, BufferLen);
        return MJPEG_PT;
    case H264_FORMAT:
        setH264Frame(pBuffer, BufferLen);
        return H264_PT;
    case XVID_FORMAT:
        return XVID_PT;
    case MPEG4_FORMAT:
        return MPEG4_PT;
    case MPEG1_FORMAT:
        setMPEG1Frame(pBuffer, BufferLen);
        return MPEG1_PT;
    case MPEG2_FORMAT:
        setMPEG2Frame(pBuffer, BufferLen);
        return MPEG2_PT;
    case H263_FORMAT:
        setH263Frame(pBuffer, BufferLen);
        return H263_PT;
    case H263P_FORMAT:
        setH263Frame(pBuffer, BufferLen);
        return H263P_PT;
    default:
        NOTIFY("Fragmenter_t::setFrame: Unknown format = %d\n", fcc);
        abort();
    }
}

int
fragmenter_t::getMPEG4Fragment(u8 *fragment, int &size)
{
    if ((frameCount + size) < numBytes)
    {
        memcpy(fragment, buffer + frameCount, size);
        frameCount += size;
    }
    else
    {
        memcpy(fragment, buffer + frameCount, numBytes - frameCount);
        size = numBytes - frameCount;
        frameCount = numBytes;
    }

    // return remaining of frame
    return numBytes - frameCount;
}

int
fragmenter_t::getMPEG1Fragment(u8 *fragment, int &size)
{
    mpeg1FragData_t *mpeg1Data = static_cast<mpeg1FragData_t *>(frameData);

    MPEG1Header_t header1;
    header1.word1 = mpeg1Data->temporal_reference;
    header1.word1 &= 0x03ff;
    header1.word1 = htons (header1.word1);

    header1.AN = 0;
    header1.N  = 0;
    header1.P  = mpeg1Data->picture_type;

    header1.FBV = 0;
    header1.BFC = 0;
    header1.FFV = 0;
    header1.FFC = mpeg1Data->FFC;

    int dataSize = 0;

    if (frameCount == 0)   // first Packet
    {
        if ( (buffer[0] == 0x00) &&
             (buffer[1] == 0x00) &&
             (buffer[2] == 0x01) &&
             (buffer[3] == 0xb3)
           )
        {
            header1.S = 1;
        }
        else
        {
            header1.S = 0;
        }
        header1.B = 1;
        // All in one packet
        if (numBytes < (size - sizeof (MPEG1Header_t)))
        {
            dataSize = numBytes;
            header1.E = 1;
        } else {
            // All in various packets
            dataSize = size - sizeof (MPEG1Header_t);
            if (   (buffer[dataSize]   == 0x00)
                && (buffer[dataSize+1] == 0x00)
                && (buffer[dataSize+2] == 0x01)
                && (buffer[dataSize+3] >= 0x01)
                && (buffer[dataSize+3] <= 0xaf)
               )
            {
                header1.E = 1;
            }
        }
    } else {
        // second and later packets
        header1.S = 0;
        if (   (buffer[frameCount]   == 0x00)
            && (buffer[frameCount+1] == 0x00)
            && (buffer[frameCount+2] == 0x01)
            && (buffer[frameCount+3] >= 0x01)
            && (buffer[frameCount+3] <= 0xaf)
           )
        {
            header1.B = 1;
            // last Packet
            if ( (numBytes - frameCount) < (size - sizeof (MPEG1Header_t)) )
            {
                dataSize = numBytes - frameCount;
                header1.E = 1;

                // not last
            }
            else
            {
                dataSize = size - sizeof (MPEG1Header_t);
                if (   (buffer[frameCount+dataSize]   == 0x00)
                    && (buffer[frameCount+dataSize+1] == 0x00)
                    && (buffer[frameCount+dataSize+2] == 0x01)
                    && (buffer[frameCount+dataSize+3] >= 0x01)
                    && (buffer[frameCount+dataSize+3] <= 0xaf)
                   )
                {
                    header1.E = 1;
                }
            }
        }
        else
        {
            header1.B = 0;
            bool newSlice = false;
            int i;
            for (i = frameCount; i < numBytes; i++)
            {
                if (   (buffer[i]   == 0x00)
                    && (buffer[i+1] == 0x00)
                    && (buffer[i+2] == 0x01)
                    && (buffer[i+3] >= 0x01)
                    && (buffer[i+3] <= 0xaf)
                   )
                {
                    newSlice = true;
                    break;
                }
            }
            if (newSlice)
            {
                // last packet
                if ( (i - frameCount) < (size - sizeof (MPEG1Header_t)) )
                {
                    dataSize = i - frameCount;
                    header1.E = 1;
                    // not last
                }
                else
                {
                    dataSize = size - sizeof (MPEG1Header_t);
                    header1.E = 0;
                }
            }
            else
            {
                if ( (numBytes - frameCount) < (size - sizeof (MPEG1Header_t)) )
                {
                    // last packet
                    dataSize = numBytes - frameCount;
                    header1.E = 1;
                }
                else
                {
                    // not last
                    dataSize = size - sizeof (MPEG1Header_t);
                    header1.E = 0;
                }
            }
        }
    }

    memcpy (fragment, &header1, sizeof(MPEG1Header_t));
    memcpy (fragment + sizeof(MPEG1Header_t), buffer + frameCount, dataSize);

    if ((frameCount + dataSize) < numBytes)
    {
        frameCount += dataSize;
    }
    else
    {
        size = (numBytes - frameCount) + sizeof(MPEG1Header_t);
        frameCount = numBytes;
    }

    // return remaining of frame
    return numBytes - frameCount;
}

int
fragmenter_t::getMPEG2Fragment(u8 *fragment, int &size)
{
    return getMPEG1Fragment(fragment, size);
}

int
fragmenter_t::getH263Fragment(u8 *fragment, int &size)
{
    h263FragData_t *h263Data = static_cast<h263FragData_t *>(frameData);

    if (h263Data->mode == 0)
    {
        H263_AHeader_t header;
        header.F = 0;
        header.P = 0;

        header.sbit = 0;
        header.ebit = 0;

        if ((Width == 128) && (Height == 96))
            header.src = SQCIF;
        else if ((Width == 176) && (Height == 144))
            header.src = QCIF;
        else if ((Width == 352) && (Height == 288))
            header.src = CIF;
        else if ((Width == 704) && (Height == 576))
            header.src = CIF4;
        else if ((Width == 1408) && (Height == 1152))
            header.src = CIF16;
        else
            header.src = OTRO;

        header.I   = 0;
        header.U   = 0;
        header.S   = 0;
        header.A   = 0;
        header.R1  = 0;
        header.R   = 0;
        header.dbq = 0;
        header.trb = 0;
        header.tr  = 0;

        memcpy(fragment, &header, sizeof(H263_AHeader_t));
        memcpy(fragment + sizeof(H263_AHeader_t),
               buffer + frameCount,
               size - sizeof(H263_AHeader_t)
              );
        int dataSize = size - sizeof(H263_AHeader_t);
        if ((frameCount + dataSize) < numBytes)
        {
            frameCount += dataSize;
        }
        else
        {
            size = (numBytes - frameCount) + sizeof(H263_AHeader_t);
            frameCount = numBytes;
        }

        // return remaining of frame
        return numBytes - frameCount;

        // end of if (mode == 0)
    }

    NOTIFY("fragmenter_t::getH263Fragment: mode not valid\n");

    return -1;
}

int
fragmenter_t::getH264Fragment(u8 *fragment, int &size)
{
    H264Header_t h264Header;

    h264Header.width  = Width / 8;
    h264Header.height = Height / 8;

    memcpy (fragment, &h264Header, sizeof (H264Header_t));

    unsigned int sizeToGet = 0;
    if (frameCount + size < numBytes)
    {
        sizeToGet= size;
    }
    else
    {
        sizeToGet= numBytes - frameCount;
    }

    memcpy (fragment + sizeof(H264Header_t), buffer + frameCount, sizeToGet);
    frameCount += sizeToGet;

    size= sizeToGet + sizeof(H264Header_t);

    // return remaining of frame
    return numBytes - frameCount;
}

int
fragmenter_t::getCellBFragment(u8 *fragment, int &size)
{
    return -1;
}

int
fragmenter_t::getJPEGFragment(u8 *fragment, int &size)
{
    // Rellenamos los campos de la cabecera JPEG. Se supone que dentro
    // del codec estan los siguientes parametros:
    // - Type: En el RFC se especifica que los cuatro types definidos
    //         actualmente son los de valores 0, 1, 64, 65
    // - Q:    Especifica si las tablas de cuantizacion se envian en el
    //         paquete cuyo fragment offset es 0 (solamente se envian en
    //         este por ser modo abreviado) o no se envian (en este caso
    //         se las tablas se obtienen a partir del parametro Q).
    // - typeSpecific: valores posibles 0, 1, 2, 3. Sirve para
    //         especificar las caracteristicas de la senal de video

    jpegFragData_t *jpegData = static_cast<jpegFragData_t *>(frameData);

    JPEGHeader_t jpegHeader;
    jpegHeader.typeSpecific = jpegData->typeSpecific;
    jpegHeader.offset       = htonl(frameCount) >> 8;
    jpegHeader.type         = jpegData->type;
    jpegHeader.Q            = jpegData->quality;
    jpegHeader.width        = Width / 8;
    jpegHeader.height       = Height / 8;

    memcpy (fragment, &jpegHeader, sizeof (JPEGHeader_t));

    bool restartPresent = false;
    if ( (jpegHeader.type >= 64) && (jpegHeader.type <= 127) )
    {
        restartPresent = true;
        RestartHeader_t restartHeader;
        restartHeader.restartInterval = jpegData->dri;
        restartHeader.F = 1;
        restartHeader.L = 1;
        restartHeader.restartCount = 1;//0x3FFF;
        memcpy (fragment + sizeof (JPEGHeader_t),
                &restartHeader,
                sizeof (RestartHeader_t)
               );
    }

    // Suponemos que el tamano de los coeficientes de cuantizacion es 1 byte.
    // Los types mencionados anteriormente solo utilizan dos tablas de
    // cuantizacion, una para la luminacia y otra para las crominacias.
    // Estas tablas las podemos obtener del objeto utilizado para la
    // compresion de la imagen.

    u8 *tables;
    QuantizationHeader_t quantizationHeader;
    if ( (jpegHeader.offset == 0) &&
         (jpegHeader.Q >= 128) && (jpegHeader.Q <= 255)
       )
    {
        quantizationHeader.mbz = 0;
        quantizationHeader.precision = 0; // Indica que las dos tablas tienen una precision de 1 byte
        quantizationHeader.length = (u32)128;
        if (restartPresent)
        {
            memcpy (fragment + sizeof(JPEGHeader_t) + sizeof(RestartHeader_t),
                    &quantizationHeader,
                    sizeof(QuantizationHeader_t)
                   );
            tables = fragment + sizeof(JPEGHeader_t)
                     + sizeof(RestartHeader_t) + sizeof(QuantizationHeader_t);
        }
        else
        {
            memcpy (fragment + sizeof(JPEGHeader_t),
                    &quantizationHeader,
                    sizeof(QuantizationHeader_t)
                   );
            tables = fragment + sizeof(JPEGHeader_t)
                     + sizeof(QuantizationHeader_t);
        }
        memcpy (tables, jpegData->lqt, 64);
        tables += 64 * sizeof(u8);
        memcpy (tables, jpegData->cqt, 64);
    }

    int headerLength = sizeof (JPEGHeader_t);
    if (restartPresent)
        headerLength += sizeof (RestartHeader_t);
    if ( (jpegHeader.offset == 0) &&
         (jpegHeader.Q >= 128) && (jpegHeader.Q <= 255)
       )
    {
        headerLength += sizeof (QuantizationHeader_t) + quantizationHeader.length;
    }

    unsigned int sizeToGet = 0;
    bool finish = false;

    for (int i = frameCount; i < numBytes; i++)
    {
        if (buffer[i] == 0xff)
        {
            if (   (buffer[i+1] >= 0xd0)
                && (buffer[i+1] <= 0xd7)
               )
            {
                if ( (i - frameCount) < (size - headerLength) )
                {
                    sizeToGet = i - frameCount;
                }
                else
                {
                    if (sizeToGet == 0)
                    {
                        sizeToGet = i - frameCount;
                        //NOTIFY ("fragmenter_t::getJPEGFragment: "
                        //        "fragment size (%d bytes) higher "
                        //        "than requested (%d bytes)\n",
                        //        sizeToGet+headerLength,
                        //        size
                        //       );
                    }
                    break;
                }
            }
            else if (buffer[i+1] == 0xd9)
            {
                finish = true;
                break;
            }
        }
    }
    if ((numBytes - frameCount) == 1)
    {
        finish = true;
    }

    if (finish)
    {
        if (sizeToGet != 0)
        {
            if ((numBytes - sizeToGet) <= ((size - headerLength) - sizeToGet))
            {
                sizeToGet += numBytes - sizeToGet;
            }
        }
        else
        {
            if ( (numBytes - frameCount) <= (size - headerLength) )
            {
                sizeToGet = numBytes - frameCount;
            }
            else
            {
                sizeToGet = size - headerLength;
            }
        }
    }

    memcpy (fragment + headerLength, buffer + frameCount, sizeToGet);
    frameCount += sizeToGet;

    size= sizeToGet + headerLength;

    // return remaining of frame
    return numBytes - frameCount;
}

int
fragmenter_t::getFragment(u8 *fragment, int &size)
{
    if (frameCount < numBytes)
    {
        switch (fcc)
        {
        case CELB_FORMAT:
            return getCellBFragment(fragment, size);
        case MJPEG_FORMAT:
            return getJPEGFragment(fragment, size);
        case H264_FORMAT:
            return getH264Fragment(fragment, size);
        case XVID_FORMAT:
        case MPEG4_FORMAT:
            return getMPEG4Fragment(fragment, size);
        case MPEG1_FORMAT:
            return getMPEG1Fragment(fragment, size);
        case MPEG2_FORMAT:
            return getMPEG2Fragment(fragment, size);
        case H263_FORMAT:
            return getH263Fragment(fragment, size);
        case H263P_FORMAT:
            return getH263Fragment(fragment, size);
        default:
            NOTIFY("fragmenter_t::getFragment: unknown format = %d\n", fcc);
            abort();
        }
    }
    return -1;
}

long
fragmenter_t::getOffset(void)
{
    return frameCount;
}

