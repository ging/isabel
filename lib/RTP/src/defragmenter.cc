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
// $Id: defragmenter.cc 10815 2007-09-17 09:08:31Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>

#include <icf2/general.h>

#ifdef __BUILD_FOR_LINUX
#include <arpa/inet.h>
#endif

#include <icf2/notify.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>
#include <rtp/defragmenter.hh>

#include "CellBimp.h"
#include "jpegHeaders.hh"
#include "JPEGimp.h"
#include "H263imp.h"
#include "H264imp.h"
#include "MPEG1imp.h"

#define PACKET_BUFFER_SIZE 500

class RTPBuffer_t
{
private:

    RTPPacket_t *frame[PACKET_BUFFER_SIZE];
    int numPkts;

    u32  timestamp;
    u8   PT;

    bool getMPEG4Image(u8 *outBuf, int *lastW, int *lastH);
    bool getMPEG1Image(u8 *outBuf, int *lastW, int *lastH);
    bool getMPEG2Image(u8 *outBuf, int *lastW, int *lastH);
    bool getH263Image (u8 *outBuf, int *lastW, int *lastH);
    bool getH264Image (u8 *outBuf, int *lastW, int *lastH);
    bool getJPEGImage (u8 *outBuf, int *lastW, int *lastH);
    bool getCellBImage(u8 *outBuf, int *lastW, int *lastH);

public:

    RTPBuffer_t(RTPPacket_t *pkt);
    virtual ~RTPBuffer_t(void);

    bool insert(RTPPacket_t *pkt);

    u32  getTimestamp(void);
    int  getImage(u8 *outBuf,
                  int outBufLen,
                  bool *gotFrame,
                  u32 *fmt,
                  int *lastW,
                  int *lastH
                 );
};

defragmenter_t::defragmenter_t(int initFrameBufferSize)
: frameBufferSize(initFrameBufferSize),
  lastW(0),
  lastH(0)
{
}

defragmenter_t::~defragmenter_t(void)
{
    while (frameBuffer.len())
    {
        delete frameBuffer.head();
        frameBuffer.behead();
    }
}

int
defragmenter_t::process(u8 *outBuf,
                        int outBufLen,
                        bool *gotFrame,
                        u32 *fmt,
                        int *width,
                        int *height,
                        RTPPacket_t *pkt
                       )
{
    //NOTIFY("process PT= %d, TS=%d SN=%d\n",
    //       pkt->getPayloadType(),
    //       pkt->getTimestamp(),
    //       pkt->getSequenceNumber()
    //      );

    RTPBuffer_t *RTPBuffer = NULL;

    int retVal= -8;
    *gotFrame= false;

    for (ql_t<RTPBuffer_t*>::iterator_t i= frameBuffer.begin();
        i != frameBuffer.end();
        i++
        )
    {
        RTPBuffer_t *aux= static_cast<RTPBuffer_t*>(i);

        if (aux->getTimestamp() == pkt->getTimestamp())
        {
            RTPBuffer= aux;
            break;
        }
    }

    // if we detect a new Frame
    if (RTPBuffer == NULL)
    {
        // not found, created a new one with the packet and insert it
        RTPBuffer = new RTPBuffer_t(pkt);
        frameBuffer.insert(RTPBuffer);
    }
    else
    {
        // found!, just insert the packet
        bool ins= RTPBuffer->insert(pkt);

        if ( ! ins ) // far away packet?
        {
            NOTIFY("defragmenter_t::process: "
                   "SSRC= %d, discarding packet SQ= %d\n",
                   pkt->getSSRC(),
                   pkt->getSequenceNumber()
                  );
            return -8; // could not process packet
        }
    }

    // check if any image can be returned

    if (frameBuffer.len() > frameBufferSize)
    {
        retVal= frameBuffer.head()->getImage(outBuf,
                                             outBufLen,
                                             gotFrame,
                                             fmt,
                                             &lastW,
                                             &lastH
                                            );
        delete frameBuffer.head();
        frameBuffer.behead();
    }

    *width= lastW;
    *height= lastH;

    return retVal;
}

RTPBuffer_t::RTPBuffer_t(RTPPacket_t *pkt)
{
    assert (pkt != NULL);

    PT       = pkt->getPayloadType();
    timestamp= pkt->getTimestamp();

    for (int i = 0; i < PACKET_BUFFER_SIZE; i++)
    {
        frame[i] = NULL;
    }

    frame[0] = new RTPPacket_t(*pkt);

    numPkts= 1;
}

RTPBuffer_t::~RTPBuffer_t(void)
{
    for (int i = 0; i < PACKET_BUFFER_SIZE; i++)
    {
        if (frame[i])
        {
            delete frame[i];
            frame[i]= NULL;
        }
    }
}


bool
RTPBuffer_t::insert(RTPPacket_t *pkt)
{
    assert (pkt != NULL);

    assert (PT == pkt->getPayloadType() && timestamp == pkt->getTimestamp());

    assert (frame[0] != NULL);

    int diff = (int)pkt->getSequenceNumber()-(int)frame[0]->getSequenceNumber();

    // sequence cicle test
    if (abs(diff) > RTP_MAX_SEQ_DIFF)
    {
        if (diff > 0)
            diff-= RTP_MAX_SEQ_NUM;
        else
            diff+= RTP_MAX_SEQ_NUM;
    }

    // duplication test
    for (int j = 0; j < PACKET_BUFFER_SIZE; j++)
    {
        if (frame[j] != NULL)
        {
            if (frame[j]->getSequenceNumber() == pkt->getSequenceNumber())
            {
                return true;
            }
        }
    }

    // check if there is room for a new packet
    if (numPkts >= PACKET_BUFFER_SIZE)
    {
        NOTIFY("RTPBuffer_t::insert: "
            "[%d] buffer full, last pkt->SQ= %d, frame[0]->SQ=%d\n",
            frame[0]->getSSRC(),
            pkt->getSequenceNumber(),
            frame[0]->getSequenceNumber()
            );
        return false;
    }

    // check if diff is a correct diff
    if (diff > PACKET_BUFFER_SIZE || diff < -10)
    {
        NOTIFY("RTPBuffer_t::insert: "
            "[%d] diff %d is too big, pkt->SQ= %d, frame[0]->SQ=%d\n",
            frame[0]->getSSRC(),
            diff,
            pkt->getSequenceNumber(),
            frame[0]->getSequenceNumber()
            );
        return false;
    }

    // if we receive a delayed pkt, move to the right
    // and insert this one
    if (diff < 0)
    {
        for (int j= PACKET_BUFFER_SIZE + diff; j >= 0; j--)
        {
            frame[j-diff] = frame[j];
        }
        frame[0] = new RTPPacket_t(*pkt);
        numPkts++;
        return true;
    }

    // Save the packet
    frame[diff] = new RTPPacket_t(*pkt);
    numPkts++;

    return true;
}

u32
RTPBuffer_t::getTimestamp(void)
{
    return timestamp;
}

bool
RTPBuffer_t::getMPEG4Image(u8 *outBuf, int *lastW, int *lastH)
{
    u8 *data= (u8*)frame[0]->getData();

    // RFC3016 VOL header
    if (data[0] == 0x00 &&  // START_SEQ = 0x000001B0
        data[1] == 0x00 &&
        data[2] == 0x01 &&
        data[3] == 0xB0 &&
        data[15]== 0x00 &&  // VIDEO_OBJECT_LAYER_START_CODE= 0x00000120
        data[16]== 0x00 &&
        data[17]== 0x01 &&
        data[18]== 0x20)    // We've received a new I-Frame
    {
        *lastW = (*(data + 25) & 0xFF) << 5 |
                 (*(data + 26) & 0xF8) >> 3;
        *lastH = (*(data + 26) & 0x03) << 11|
                 (*(data + 27) & 0xFF) << 3 |
                 (*(data + 28) & 0xE0) >> 5;

    }
    else
    {

        // Some codecs use these VOL distribution, different than RFC3016
        // (what were they thinking in when buildin' these codecs???!!!)

        if (data[0] == 0x00 &&  // START_SEQ = 0x000001B0
            data[1] == 0x00 &&
            data[2] == 0x01 &&
            data[3] == 0xB0 &&
            data[14]== 0x00 &&  // VIDEO_OBJECT_LAYER_START_CODE= 0x00000120
            data[15]== 0x00 &&  // changes position :(
            data[16]== 0x01 &&
            data[17]== 0x20)    // We've received a new I-Frame
        {
            *lastW = (*(data + 23) & 0x03) << 11 |
                     (*(data + 24) & 0xFF) << 3  |
                     (*(data + 25) & 0xE0  >> 5); //changes position !!
            *lastH = (*(data + 25) & 0x0F) << 9  |
                     (*(data + 26) & 0xFF) << 1  |
                     (*(data + 27) & 0x80) >> 7;  //changes position !!

        }
    }

    if ( ! (*lastW * *lastH))   // we haven't got size, nor previous
    {
        return false;
    }

    int dataLen = 0;
    for (int j= 0; j < PACKET_BUFFER_SIZE; j++)
    {
        if (frame[j] != NULL)
        {
            memcpy(outBuf + dataLen,
                   frame[j]->getData(),
                   frame[j]->getDataLength()
                  );

            dataLen += frame[j]->getDataLength();
            if (frame[j]->getMark())
            {
                break;
            }
        }
    }
    return true;
}

bool
RTPBuffer_t::getMPEG1Image(u8 *outBuf, int *lastW, int *lastH)
{
    int payloadHeaderLength = sizeof (MPEG1Header_t);

    MPEG1Header_t *header = (MPEG1Header_t*)frame[0]->getData();

    u8 *mpegData = (u8*)header + sizeof (MPEG1Header_t);
    if (header -> S == 1)
    {
        u16 aux  = mpegData[5]; aux &= 0x00f0; aux >>= 4;
        u16 aux2 = mpegData[4]; aux2 <<= 4; aux2 &= 0x0ff0;
        *lastW = aux | aux2;

        aux = mpegData[6]; aux &= 0x00ff;
        aux2 = mpegData[5]; aux2 <<= 8; aux2 &= 0x0f00;
        *lastH = aux | aux2;
    }

    if ( ! (*lastW * *lastH))   // we haven't got size, nor previous
    {
        return false;
    }

    int dataLen = 0;
    for (int j= 0; j < PACKET_BUFFER_SIZE; j++)
    {
        if (frame[j] != NULL)
        {
            memcpy(outBuf + dataLen,
                   frame[j]->getData()+payloadHeaderLength,
                   frame[j]->getDataLength()-payloadHeaderLength
                  );

            dataLen += frame[j]->getDataLength()-payloadHeaderLength;
            if (frame[j]->getMark())
            {
                break;
            }
        }
    }
    return true;
}

bool
RTPBuffer_t::getMPEG2Image(u8 *outBuf, int *lastW, int *lastH)
{
    // FIXME: code MP2T fragmenter/defragmenter
    return getMPEG1Image(outBuf, lastW, lastH);
}

bool
RTPBuffer_t::getH263Image(u8 *outBuf, int *lastW, int *lastH)
{
    int payloadHeaderLength= 0;
    int mode;

    Mode_Header_t *modeHeader = (Mode_Header_t*)frame[0]->getData();

    if ((modeHeader->F == 0) && (modeHeader->P == 0))
    {
        payloadHeaderLength = sizeof(H263_AHeader_t);
        mode = 0;
    }
    else if ((modeHeader->F == 0) && (modeHeader->P == 1))
    {
        payloadHeaderLength = sizeof(H263_BHeader_t);
        mode = 1;
    }
    else
    {
        NOTIFY("bit F == 1, H263 Mode C not supported (yet)!\n");
        payloadHeaderLength = 0;
        return false;
    }

    if (mode == 0 || mode == 1)
    {
        H263_AHeader_t *header = (H263_AHeader_t*)modeHeader;
        switch (header -> src)
        {
        case 1:
            *lastW = 128;
            *lastH = 96;
            break;
        case 2:
            *lastW = 176;
            *lastH = 144;
            break;
        case 3:
            *lastW = 352;
            *lastH = 288;
            break;
        case 4:
            *lastW = 704;
            *lastH = 576;
            break;
        case 5:
            *lastW = 1408;
            *lastH = 1152;
            break;
        case 7:
            *lastW = 1408;
            *lastH = 1152;
            break;
        default:
            *lastW = 1408;
            *lastH = 1152;
        }
    }
    else
    {
        NOTIFY("IMPOSSIBLE: mode unknown, cannot calculate frame size\n");
        return false;
    }

    if ( ! (*lastW * *lastH) )   // we haven't got size, nor previous
    {
        return false;
    }

    int dataLen = 0;
    for (int j= 0; j < PACKET_BUFFER_SIZE; j++)
    {
        if (frame[j] != NULL)
        {
            Mode_Header_t *modeHeader = (Mode_Header_t*)frame[j]->getData();
            u8 *pData = frame[j]->getData() + payloadHeaderLength;
            int size = frame[j]->getDataLength()-payloadHeaderLength;

            if (modeHeader -> sbit)
            {
                int mask = 0xff;
                switch (modeHeader -> sbit)
                {
                case 1: mask = 0x7f; break;
                case 2: mask = 0x3f; break;
                case 3: mask = 0x1f; break;
                case 4: mask = 0x0f; break;
                case 5: mask = 0x07; break;
                case 6: mask = 0x03; break;
                case 7: mask = 0x01; break;
                }
                outBuf[dataLen-1] |= (pData[0] & mask);
                pData++;
                size--;
            }

            memcpy(outBuf + dataLen, pData, size);
            dataLen += size;

            if (modeHeader -> ebit != 0)
            {
                int mask = 0xff;
                switch (modeHeader -> ebit)
                {
                case 1: mask = 0xfe; break;
                case 2: mask = 0xfc; break;
                case 3: mask = 0xf8; break;
                case 4: mask = 0xf0; break;
                case 5: mask = 0xe0; break;
                case 6: mask = 0xc0; break;
                case 7: mask = 0x80; break;
                }
                outBuf[dataLen-1] &= mask;
            }
            if (frame[j]->getMark())
            {
                break;
            }
        }
    }

    return true;
}

bool
RTPBuffer_t::getH264Image(u8 *outBuf, int *lastW, int *lastH)
{
    int headerLength= sizeof(H264Header_t);

    H264Header_t *header = (H264Header_t*)frame[0]->getData();
    *lastW = header->width  << 3;
    *lastH = header->height << 3;

    int dataLen = 0;
    for (int j= 0; j < PACKET_BUFFER_SIZE; j++)
    {
        if (frame[j] != NULL)
        {
            memcpy(outBuf + dataLen,
                   frame[j]->getData() + headerLength,
                   frame[j]->getDataLength() - headerLength
                  );

            dataLen += (frame[j]->getDataLength() - headerLength);
            if (frame[j]->getMark())
            {
                break;
            }
        }
    }
    return true;
}

bool
RTPBuffer_t::getCellBImage(u8 *outBuf, int *lastW, int *lastH)
{
    unsigned int xpos, ypos;

    CellBHeader_t *header = (CellBHeader_t *)frame[0]->getData();

    xpos = ntohs(header->Xpos);
    ypos = ntohs(header->Ypos);
    *lastW = ntohs(header->w);
    *lastH = ntohs(header->h);

    // TODO

    return false;
}

bool
RTPBuffer_t::getJPEGImage(u8 *outBuf, int *lastW, int *lastH)
{
    JPEGHeader_t *jpegHeader = (JPEGHeader_t *)frame[0]->getData();

    *lastW = jpegHeader->width << 3;
    *lastH = jpegHeader->height << 3;

    jpegHeader->offset = ntohl(jpegHeader->offset) << 8;

    if (jpegHeader->offset > 0)
    {
        NOTIFY("RTPBuffer_t::getJPEGImage:"
               "jpegHeader->offset = %d, throwing frame\n",
               jpegHeader->offset
              );
        return false;
    }

    u8 lqt[64];
    u8 cqt[64];
    u16 dri= 0;

    u8 *inBuff= frame[0]->getData();

    bool quantizationPresent = false;
    bool restartPresent = false;

    RestartHeader_t *restartHeader = NULL;
    QuantizationHeader_t *quantizationHeader = NULL;

    if ( (jpegHeader -> type >= 64) && (jpegHeader -> type <= 127) )
    {
        restartPresent = true;
        restartHeader = (RestartHeader_t *)(inBuff + sizeof (JPEGHeader_t));
        dri = restartHeader -> restartInterval;
    }

    if ( jpegHeader -> Q >= 128 )
    {
        quantizationPresent = true;
        u8 *aux = NULL;
        if (restartPresent)
        {
            quantizationHeader = (QuantizationHeader_t *)(inBuff + sizeof (JPEGHeader_t) + sizeof (RestartHeader_t));
            aux = inBuff + sizeof (JPEGHeader_t) + sizeof (RestartHeader_t) + sizeof (QuantizationHeader_t);
        }
        else
        {
            quantizationHeader = (QuantizationHeader_t *)(inBuff + sizeof (JPEGHeader_t));
            aux = inBuff + sizeof (JPEGHeader_t) + sizeof (QuantizationHeader_t);
        }
        u8 precision;

        // Es necesario saber a priori el numero de tablas que
        // vamos a necesitar. Por ejemplo para los tipos 0, 1,
        // 64, 65 necesitamos dos tablas, que seran las que vendran
        // dentro del paquete con fragment offset 0.

        precision = quantizationHeader -> precision;
        precision &= 0x01;
        if (precision == 0)
        {
            memcpy (lqt, aux, 64);
            aux += 64;
        }
        if (precision == 1)
        {
            u16 *aux2 = (u16 *)aux;
            for (int i = 0; i < 64; i++)
            {
                lqt[i] = (u8)(*aux2);
                aux2++;
            }
            aux += 128;
        }
        precision = quantizationHeader -> precision;
        precision >>= 1;
        precision &= 0x01;
        if (precision == 0)
        {
            memcpy (cqt, aux, 64);
            aux += 64;
        }
        if (precision == 1)
        {
            u16 *aux2 = (u16 *)aux; //Esto hay que mirarlo
            for (int i = 0; i < 64; i++)
            {
                cqt[i] = (u8)(*aux2);
                aux2++;
            }
            aux += 128;
        }
    }
    else
    {
        MakeTables (jpegHeader->Q, lqt, cqt);
    }

    int headerLength = sizeof (JPEGHeader_t);
    if (restartPresent)
    {
        headerLength += sizeof (RestartHeader_t);
    }
    if (quantizationPresent)
    {
        headerLength +=
            sizeof (QuantizationHeader_t) + quantizationHeader->length;
    }

    int imageHeader = 0;

    imageHeader = MakeHeaders(outBuf,
                              jpegHeader->type,
                              jpegHeader->width,
                              jpegHeader->height,
                              lqt,
                              cqt,
                              dri
                             );

    int dataLength = frame[0]->getDataLength() - headerLength;
    memcpy(outBuf + imageHeader,
           frame[0]->getData() + headerLength,
           dataLength
          );
    for (int j= 1; j < PACKET_BUFFER_SIZE; j++)
    {
        if (frame[j] != NULL) // skip packet
        {
            inBuff= frame[j]->getData();
            jpegHeader= (JPEGHeader_t*)inBuff;
            jpegHeader -> offset = ntohl (jpegHeader -> offset<<8);
            inBuff += sizeof(JPEGHeader_t);
            headerLength= sizeof(JPEGHeader_t);

            //fprintf(stderr, "frame=%d, jpegHeadertype=%d\n", j, jpegHeader->type);
            if ( (jpegHeader -> type >= 64) && (jpegHeader -> type <= 127) )
            {
                restartHeader = (RestartHeader_t *)(inBuff);
                dri = restartHeader -> restartInterval;
                inBuff += sizeof(RestartHeader_t);
                headerLength += sizeof(RestartHeader_t);
            }

            if ( jpegHeader -> Q >= 128 )   // quantization present
            {
                quantizationHeader = (QuantizationHeader_t *)(inBuff);
                inBuff += sizeof(QuantizationHeader_t) + quantizationHeader->length;
                headerLength += sizeof(QuantizationHeader_t) + quantizationHeader->length;
            }

            memcpy(outBuf + imageHeader + jpegHeader->offset,
                   inBuff,
                   frame[j]->getDataLength() - headerLength
                  );
            if (frame[j]->getMark())
                break;
        }
    }
    return true;
}

int
RTPBuffer_t::getImage(u8   *outBuf,
                      int   outBufLen,
                      bool *gotFrame,
                      u32  *fmt,
                      int  *lastW,
                      int  *lastH
                     )
{
    bool hr = false;

    memset(outBuf, 0, outBufLen);
    *gotFrame= false;
    *fmt= 0;

#if 1
    //---------------------------
    // this code checks if frame is complete
    //---------------------------
    int SQ= frame[0]->getSequenceNumber(); // for warning if lost SQ
    u32 SSRC= frame[0]->getSSRC();
    if (! frame[0]->getMark())
    {
        for (int j= 1; j < PACKET_BUFFER_SIZE; j++)
        {
            if (frame[j])
            {
                if ((SQ+1)%0xffff != frame[j]->getSequenceNumber())
                {
                    NOTIFY("RTPBuffer_t::getImage: "
                           "[%d] lost SQ, last known %d\n",
                           SSRC,
                           SQ
                          );
                    return -1;
                }
                if (frame[j]->getMark())
                {
                    break;
                }
                SQ= frame[j]->getSequenceNumber();
            }
            else
            {
                NOTIFY("RTPBuffer_t::getImage: "
                       "[%d] lost SQ, last known %d\n",
                       SSRC,
                       SQ
                      );
                return -2;
            }
        }
    }
#endif

    int totalSize = 0;
    if (frame[0]->getPayloadType() == MJPEG_PT)   // for JPEG
    {
        for (int j= 0; j < PACKET_BUFFER_SIZE; j++)
        {
            if (frame[j] != NULL)
            {
                totalSize += frame[j]->getDataLength() - sizeof(JPEGHeader_t);
            }
        }
        totalSize += 623;
    }
    else
    {
        for (int j= 0; j < PACKET_BUFFER_SIZE; j++)
        {
            if (frame[j] != NULL)
            {
                totalSize += frame[j]->getDataLength();
            }
        }
    }
    if (totalSize == 0)   // no packets! maybe we don't understand PT
    {
        NOTIFY("getImage: no packets found, maybe PT is not understood\n");
        return -3;
    }

    // check if there is enough space
    if (totalSize > outBufLen)
    {
        return totalSize;
    }

    //---------------------------
    // recover frame
    //---------------------------

    switch (frame[0]->getPayloadType())
    {
    case H263_PT:
        hr = getH263Image(outBuf, lastW, lastH);
        *fmt= H263_FORMAT;
        break;
    case H263P_PT:
        hr = getH263Image(outBuf, lastW, lastH);
        *fmt= H263P_FORMAT;
        break;
    case H264_PT:
        hr = getH264Image(outBuf, lastW, lastH);
        *fmt= H264_FORMAT;
        break;
    case MPEG4_PT:
        hr = getMPEG4Image(outBuf, lastW, lastH);
        *fmt= MPEG4_FORMAT;
        break;
    case XVID_PT:
        hr = getMPEG4Image(outBuf, lastW, lastH);
        *fmt= XVID_FORMAT;
        break;
    case MPEG1_PT:
        hr = getMPEG1Image(outBuf, lastW, lastH);
        *fmt= MPEG1_FORMAT;
        break;
    case MPEG2_PT:
        hr = getMPEG2Image(outBuf, lastW, lastH);
        *fmt= MPEG2_FORMAT;
        break;
    case MJPEG_PT:
        hr = getJPEGImage(outBuf, lastW, lastH);
        *fmt= MJPEG_FORMAT;
        break;
    default:
        NOTIFY("RTPBuffer_t::getImage: unknown PT [%d]\n",
            frame[0]->getPayloadType()
            );
        hr = false;
    }

    if ( ! (*lastW * *lastH) )   // we haven't got size, nor previous
    {
        if ( (PT == MPEG1_PT) || (PT == MPEG1_PT))
        {
            NOTIFY("RTPBuffer_t::getImage: waiting for INTRA frame\n");
            NOTIFY("\t if MPEG2, put MP2T fragmenter/defragmenter \n");
            return -4;
        }
        else
        {
            NOTIFY("RTPBuffer_t::getImage: ERROR: no size in stream \n");
            return -5;
        }
    }

    if ( ! hr)
    {
        NOTIFY("RTPBuffer_t::getImage: error recovering image\n");
        return -6;
    }

    *gotFrame= true;

    return totalSize;
}

