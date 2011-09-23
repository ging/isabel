/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVIFILE_READSTREAM_H
#define AVIFILE_READSTREAM_H

#include "ReadHandlers.h"

AVM_BEGIN_NAMESPACE;

class AviReadFile;

class ReadStream : public IReadStream
{
    friend class AviReadFile;
public:
    ReadStream(IMediaReadStream* stream);
    virtual ~ReadStream();
    virtual uint_t GetBuffering(uint_t*) const { return 0; }
    virtual StreamType GetType() const;
    virtual uint_t GetHeader(void* pheader = 0, uint_t size = 0) const;
    virtual uint_t GetAudioFormat(void* format = 0, uint_t size = 0) const;
    virtual uint_t GetVideoFormat(void* format = 0, uint_t size = 0) const;
    virtual IAudioDecoder* GetAudioDecoder() const;
    virtual IVideoDecoder* GetVideoDecoder() const;
    //Size of stream and one frame
    virtual framepos_t GetLength() const;
    virtual double GetLengthTime() const;
    virtual int GetFrameFlags(int* flags) const;
    virtual double GetFrameTime() const;
    virtual framepos_t GetPos() const;
    virtual StreamInfo* GetStreamInfo() const;
    virtual double GetTime(framepos_t frame = ERR) const;
    //Positioning in stream
    virtual int Seek(framepos_t pos);
    virtual int SeekTime(double pos);
    virtual framepos_t SeekToKeyFrame(framepos_t pos);
    virtual double SeekTimeToKeyFrame(double pos);
    virtual int SkipFrame();
    virtual int SkipTo(double pos);

    //Reading decompressed data
    virtual int SetDirection(bool d) { return -1; }
    virtual int SetBuffering(uint_t maxsz = 1, IImageAllocator* ia = 0) { return -1; }
    virtual int SetOutputFormat(void* bi, uint_t size);
    virtual int ReadFrame(bool render = true);
    virtual CImage* GetFrame(bool readframe = false);
    virtual uint_t GetFrameSize() const;

    virtual int ReadFrames(void* buffer, uint_t bufsize, uint_t samples,
			   uint_t& samples_read, uint_t& bytes_read);
    virtual int ReadDirect(void* buffer, uint_t bufsize, uint_t samples,
			   uint_t& samples_read, uint_t& bytes_read,
			   int* flags = 0);

    virtual framepos_t GetNextKeyFrame(framepos_t frame = ERR) const;
    virtual framepos_t GetPrevKeyFrame(framepos_t frame = ERR) const;

    virtual framepos_t SeekToNextKeyFrame();
    virtual framepos_t SeekToPrevKeyFrame();

    virtual bool Eof() const;

    virtual double CacheSize() const { return m_pStream->CacheSize(); }

protected:
    StreamPacket* ReadPacket();
    virtual void Flush();
    IMediaReadStream* m_pStream;
    StreamPacket* m_pPacket;
    char* m_pFormat;
    uint_t m_uiFormatSize;
    int m_iEof;
    char* rem_buffer;
    uint_t rem_size;
    uint_t rem_local;
    uint_t rem_limit;
    uint_t rem_flags;
    uint_t m_uiLastPos;
    double m_dLastTime;
};

AVM_END_NAMESPACE;

#endif  // AVIPLAY_READSTREAM_H
