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
// $Id: videoTrans.cc 20770 2010-07-07 13:19:44Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/defragmenter.hh>

#include <icf2/general.h>
#include <icf2/dictionary.hh>

#include "videoTrans.hh"
#include "output.hh"

#define MIN_ACTIVE 5
#define MTU_SIZE 900

const int MAX_BUF_FRAMES= 4;

dictionary_t<videoTranscodeInfo_t, videoTranscoder_t*> *videoTranscoderList =
    new dictionary_t<videoTranscodeInfo_t, videoTranscoder_t*>;

tgtInfo_t::tgtInfo_t(void *newTgt)
: tgt(newTgt)
{
}

tgtInfo_t::~tgtInfo_t(void)
{
    while (outputBuffer.len() > 0)
    {
        RTPPacket_t *rtp= outputBuffer.head();
        outputBuffer.behead();
        delete rtp;
    }
}


u32
videoTranscoder_t::processBW(u8 *data, u32 dataLen, u32 TS)
{
    u32 retBW = 0;

    int mayReset = (( t2 - t1 ) / t1 ) > 1000; // milliseconds

    if (mayReset && t1 && t2 && t1 != t2)
    {
        // Procesamos el ancho de banda y reiniciamos la cuenta
        // timeInc in milliseconds
        double timeInc = (t2-t1)/90000.0;

        // BW in Kbits per second
        retBW = (u32)((dataCounter*8.0)/(timeInc*1024.0));
        dataCounter = dataLen;
        t1 = TS;
    }
    else
    {
        // no reseteamos la cuenta
        dataCounter+= dataLen;
        if (!t1)
        {
            t1=TS;
        }
        t2 = TS;
    }
    return retBW;
}

videoTranscoder_t::videoTranscoder_t(videoTranscodeInfo_t videoTranscodeInfo)
{
   actualBW    = 0;
   lastSeqNum  = 0;
   lastTS      = 0;
   t1          = 0;
   t2          = 0;
   dataCounter = 0;
   nActive     = 0;
   working     = false;
   Info        = videoTranscodeInfo;

   rtp2image= new rtp2image_t(2); // maximun two frames stored

   fragmenter = new fragmenter_t;    

   lastFmt = 0; actualFmt= 0; // illegal for video

   seqNum = (int)((10000.0*(double)rand())/((double)RAND_MAX+1.0));
   
   coder = NULL;
   vCodecInit();
}

videoTranscoder_t::~videoTranscoder_t(void)
{
    delete rtp2image;

    if (fragmenter)
    {
        delete(fragmenter);
    }
    if (coder)
    {
        delete(coder);
    }
    vCodecRelease();
}


RTPPacketList_ref
videoTranscoder_t::processData(RTPPacket_t *pkt, void *tgt)
{
    // lista de paquetes que se producen al transcodificar
    RTPPacketList_ref packetList = new ql_t <RTPPacket_t *>;

    if (pkt->getSequenceNumber() != lastSeqNum ||
        pkt->getTimestamp()      != lastTS
       )
    {  
        lastSeqNum = pkt->getSequenceNumber();
        lastTS = pkt->getTimestamp();

        actualBW = processBW(pkt->getData(),
                             pkt->getDataLength(),
                             pkt->getTimestamp()
                            );
        if (actualBW)
        {
            // si tenemos un refresco del BW
            // comprobamos si tenemos que activar la transcodificacion	 
            checkActive(getFmtByPT(pkt->getPayloadType()));
            NOTIFY("\rvideoTranscoder_t::SSRC = %d, incomingBW = %d Kbps",
                   Info.SSRC,
                   actualBW
                  );
        }

        if (working)
        {  
#ifdef __BUILD_FOR_LINUX

            image_t *newImg= rtp2image->addPkt(pkt);

            if (newImg == NULL)
            {
                // no frame yet
                return packetList;
            }

            int newImgWidth = newImg->getWidth();
            int newImgHeight= newImg->getHeight();
            u32 newImgFormat= newImg->getFormat();

            if (coder == NULL || newImgFormat != coder->getInputFormat())
            {
	        // se construye un nuevo coderWrapper
	        // cuando cambian los parametros
                try
                {
	             delete coder;
		     coder = NULL;

                     coderArgs.width= newImgWidth;
                     coderArgs.height= newImgHeight;
                     coderArgs.bitRate= Info.BW;
                     coderArgs.frameRate= 25;
                     coderArgs.maxInter= 25;
                     coderArgs.quality= 80;

                     coder= new coderWrapper_t(newImgFormat,
                                               Info.lineFmt,
                                               &coderArgs
                                              );
		     //NOTIFY("videoTrans::processData Built coderWrapper for codec %i\n", Info.Codec);

                }
                catch (const char *error)
                {
                    NOTIFY("videoTranscoder_t::processData "
                           "Encoder not created due to an error: %s\n",
                           error
                          );
                    exit(1);
                }
            }
            coder->getCoderArgs(&coderArgs);
            if (   coderArgs.width  != newImgWidth
                || coderArgs.height != newImgHeight
               )
            {
                coderArgs.width=  newImgWidth;
                coderArgs.height= newImgHeight;
                if ( ! coder->setCoderArgs(&coderArgs))
                {
                     NOTIFY("videoTranscoder_t::processData: "
                            " setSize went wrong, returning\n"
                           );
                     delete newImg;
                     return packetList;
                }
            }      
            u32 timestamp = pkt->getTimestamp();

	    int length = 0;
            int lineimagesize = newImgWidth*newImgHeight*4;
            unsigned char *lineimage = (unsigned char*)malloc (lineimagesize);
	    length = coder->encode(lineimage,
                                   lineimagesize,
                                   newImg->getBuff(),
                                   newImgWidth,
                                   newImgHeight
                                  ); 

	    if (length < 0)
            {
	        NOTIFY("videoTranscoder_t::processData: unable to encode\n");
	        delete newImg;
	        free (lineimage);
	    }

            u8 linePT= fragmenter->setFrame(lineimage,
                                            length,
                                            Info.lineFmt,
                                            newImgWidth,
                                            newImgHeight
                                           );	   
            int size = MTU_SIZE;  
            u8 fragment[3*MTU_SIZE];
            int n = 0;	   
            u8 *RTPData = new u8[3*MTU_SIZE+sizeof(RTPHeader_t)];

            while ((n = fragmenter->getFragment(fragment, size)) >= 0)
            {
                //-------------------------------------------
                // create RTPPacket
                //-------------------------------------------

                memcpy(RTPData+sizeof(RTPHeader_t),fragment,size);
 
                RTPHeader_t *header  = (RTPHeader_t *)RTPData;

                header->version     = RTP_VERSION;
                header->extension   = 0;
                header->marker      = n?0:1;
                header->padding     = 0;
                header->cc          = 0;
                header->seqnum      = htons(seqNum++);
                header->timestamp   = htonl(timestamp); 
                header->ssrc        = htonl(Info.SSRC);
                header->payloadtype = linePT;

                RTPPacket_t * pkte   = new RTPPacket_t(RTPData,size+sizeof(RTPHeader_t));
                packetList->insert(pkte);

            }
            delete []RTPData;
            free(lineimage); 
#endif	           
        }
        else
        {
            RTPPacket_t *auxPacket = new RTPPacket_t(*pkt);
            packetList->insert(auxPacket);
        }
    }
   
    return packetList;
}

void
videoTranscoder_t::checkActive(u32 lineFmt)
{
    // si transcodificamos de un Codec a otro
    // siempre activo
    if (lineFmt != Info.lineFmt)
    {
        working = true;
        return;
    }

    // si no tenemos que realizar una histéresis
    if (working)
    {
        // si esta ya trabajando
        // tiene que bajar un 10% del deseado
        // que deje de funcionar, y ademas debe
        // darse la situación en MIN_ACTIVE veces
        if (actualBW <= (Info.BW - 0.1*Info.BW))
        {
            nActive++;
        } else {
            nActive = 0;
        }

    }
    else
    {
        if (actualBW >= (Info.BW + 0.1*Info.BW))
        {
            nActive++;
        }
        else
        {
            nActive = 0;
        }
    }

    if (nActive >= MIN_ACTIVE)
    {
        working = !working;
        NOTIFY("\nvideoTranscoder_t::transcoder working = %d\n", working);
    }

    return;
}

videoTranscodeInfo_t
videoTranscoder_t::getInfo(void)
{
    return Info;
}

bool
videoTranscoder_t::isWorking(void)
{
    return working;
}

