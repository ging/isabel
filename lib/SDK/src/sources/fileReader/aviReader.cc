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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include "aviReader.hh"

#define DELETE_PTR(x) if(x){ delete x; x=NULL;}
#define DELETE_ARR(x) if(x){ delete[] x; x=NULL;}

aviReader_t::aviReader_t(sourceDescriptor_ref desc, const char *fileName)
: theSourceDescriptor(desc)
{
    m_hAviFile = NULL;
    m_hVideo   = NULL;
    m_hAudio   = NULL;
    m_VideoID  = 0;
    m_AudioID  = 0;
    m_hVideoDec = NULL;
    m_hAudioDec = NULL;
    m_iBuffSize = 0;
    m_Buff = NULL;
    m_iH = 0;
    m_iW = 0;
    m_ProcessedFrames = 0;
    m_TotalFrames = 0;
    memset(m_szFileName,0,MAX_PATH);
    m_Time = NULL;
    inetAddr_t addr0("127.0.0.1",NULL,SOCK_DGRAM);
    inetAddr_t addr1("127.0.0.1","6789",SOCK_DGRAM);
    m_pSocket = new dgramSocket_t(addr0,addr1);

    if ( ! openAviFile (fileName) )
    {
        throw "aviReader_t:: cannot open file";
    }
}

aviReader_t::~aviReader_t(void)
{
    if (m_hVideoDec) { vDeleteDecoder(m_hVideoDec); m_hVideoDec = NULL; }
    if (m_hAudioDec) { aDeleteDecoder(m_hAudioDec); m_hAudioDec = NULL; }

    DELETE_ARR(m_Buff);
    DELETE_PTR(m_hAviFile);
}

bool
aviReader_t::setFrameRate(double fps)
{
    return false;
}

double
aviReader_t::getFrameRate(void)
{
    if (m_hAviFile)
    {
        AVIMainHeader avi_header = m_hAviFile->GetHeader();
        return 1000000.0/avi_header.dwMicroSecPerFrame;
    }

    return 0; // no movie, no frame rate
}

bool
aviReader_t::setGrabSize(unsigned width, unsigned height)
{
    return false;
}

void
aviReader_t::getGrabSize(unsigned *width, unsigned *height)
{
}

const char *
aviReader_t::getInputPort(void)
{
    return m_szFileName;
}

bool
aviReader_t::setGrabFormat(u32 fcc)
{
    return false;
}

u32
aviReader_t::getGrabFormat(void)
{
    return BGR24_FORMAT;
}

bool
aviReader_t::openAviFile(const char *fileName)
{
    locker_t the_lock = lock();

    // Check that is a AVI file.
    bool ret = false;
    if (fileName !=  NULL &&
        (strstr(fileName,".avi") != NULL || strstr(fileName,".AVI") != NULL)
       )
    {
        m_hVideo = NULL;
        m_hAudio = NULL;
        m_iW = 0;
        m_iH = 0;
        m_Time = 0;

        if (m_hVideoDec) { vDeleteDecoder(m_hVideoDec); m_hVideoDec = NULL; }
        if (m_hAudioDec) { aDeleteDecoder(m_hAudioDec); m_hAudioDec = NULL; }
        DELETE_ARR(m_Buff);
        DELETE_PTR(m_hAviFile);
        m_hAviFile = CreateAviReadHandler(fileName, 0);
        if (m_hAviFile)
        {
            m_ProcessedFrames = 0;
            m_TotalFrames = m_hAviFile->GetHeader().dwTotalFrames;
            NOTIFY("aviReader_t::openAviFile: %s openned\n",fileName);
            m_hVideo = m_hAviFile->GetStream(m_VideoID,avm::IStream::Video);
            m_hAudio = m_hAviFile->GetStream(m_AudioID,avm::IStream::Audio);
            if (m_hAudio)
            {
                int size = m_hAudio->GetFormat();
                if (size > 0)
                {
                    u8 *format = new u8[size];
                    m_hAudio->GetFormat(format,size);
                    WAVEFORMATEX *wave = (WAVEFORMATEX*)format;
                    m_hAudioDec = aGetDecoder((audioID_e)wave->wFormatTag,
                                              wave->nSamplesPerSec
                                             );
                    if (wave->wBitsPerSample == 0)
                    {
                        wave->wBitsPerSample = 16;
                    }
                    m_Rate = wave->nSamplesPerSec;
                    delete []format;
                }
            }
            if (m_hVideo)
            {
                vDecoderArgs_t params;
                params.format = BGR24_FORMAT;
                params.height = m_iW;
                params.width  = m_iH;
                m_hVideoDec = vGetDecoderByFmt(m_hVideo->GetHeader().fccHandler,&params);
                if (m_hVideoDec)
                {
                    m_iBuffSize = 0;
                    m_iW = m_hAviFile->GetHeader().dwWidth;
                    m_iH = m_hAviFile->GetHeader().dwHeight;
                    NOTIFY("aviReader_t::openAviFile: Video decoder found (%dx%d)!\n",m_iW,m_iH);
                    m_iBuffSize = (m_iW*(m_iH+1))*3;
                    if (m_iBuffSize > 0)
                    {
                        m_Buff = new u8[m_iBuffSize];
                        strcpy(m_szFileName,fileName);
                        m_TimePerFrame = m_hAviFile->GetHeader().dwMicroSecPerFrame;
                        ret = true;
                    }
                }
                else
                {
                    NOTIFY("aviReader_t::openAviFile: Unknown video codec!\n");
                }
            }
            else
            {
                NOTIFY("aviReader_t::openAviFile: No video stream found!\n");
            }
        }
        else
        {
            NOTIFY("aviReader_t::openAviFile: can't open %s!\n", fileName);
        }
    }
    return ret;
}

image_t*
aviReader_t::getImage(void)
{
    image_t *img = NULL;
    locker_t the_lock = lock();
    if (m_TotalFrames > m_ProcessedFrames)
    {
        m_ProcessedFrames++;

        if (m_hAudio && m_hAudioDec)
        {
            StreamPacket * audio_packet = m_hAudio->ReadPacket();
            if (audio_packet && audio_packet->size > 0)
            {
                int decoded_len  = 48000*2;
                u8 *decoded_buff = new u8[decoded_len];
                int n = aDecode(m_hAudioDec,
                                (u8*)audio_packet->memory,
                                audio_packet->size,
                                decoded_buff
                               );
                if (n > 0)
                {
                    double ratio = (double)OUTPUT_SAMPLE_RATE/(double)m_Rate + 1.0;
                    int resampled_len = n*(int)(ratio);
                    u8 *resampled_buff = new u8[resampled_len];
                    n = m_Resampler.resample(decoded_buff,
                                             n,
                                             m_Rate,
                                             resampled_buff,
                                             resampled_len,
                                             OUTPUT_SAMPLE_RATE
                                            );
                    if (n > 0)
                    {
                        u8 *data = resampled_buff;
                        int len = n;
                        while (len > 0)
                        {
                            int pkt_len = 900;
                            if (len < pkt_len)
                            {
                                pkt_len = len;
                            }
                            m_pSocket->write(data,pkt_len);
                            data += pkt_len;
                            len  -= pkt_len;
                        }
                    }
                    delete []resampled_buff;
                }
                delete []decoded_buff;
                audio_packet->Release();
            }
        }

        if (m_hVideo && m_hVideoDec)
        {
            m_Time += m_TimePerFrame;

            StreamPacket *video_packet = m_hVideo->ReadPacket();
            if (video_packet && m_hVideoDec && m_iBuffSize > 0)
            {
                memset(m_Buff,0xff,m_iBuffSize);
                int i = vDecode(m_hVideoDec,
                                (u8*)video_packet->memory,
                                video_packet->size,
                                m_Buff,
                                m_iBuffSize
                               );
                if (i > 0)
                {
                    img= new image_t(m_Buff,
                                     i,
                                     BGR24_FORMAT,
                                     m_iW,
                                     m_iH,
                                     m_Time
                                    );
                }
                video_packet->Release();
            }
        }
    }
    return img;
}

