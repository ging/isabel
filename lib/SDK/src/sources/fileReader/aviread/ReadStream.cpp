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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#include "ReadStream.h"
#include "avm_output.h"
#include "avm_except.h"

AVM_BEGIN_NAMESPACE;

#define __MODULE__ "reader"

StreamPacket::StreamPacket(uint_t bsize, char* mem)
    : memory(mem), size(bsize), read(0), flags(0), timestamp(NO_TIMESTAMP),
    refcount(1)
{
    if (!memory && size > 0)
    {
#ifdef HAVE_MEMALIGN
        memory = (char*) memalign(16, size + 16);
#else
	memory = (char*) malloc(size + 16);
#endif
    }
}

StreamPacket::~StreamPacket()
{
    assert(refcount == 0);
    if (memory) free(memory);
}

IStream::~IStream() {}

ReadStream::ReadStream(IMediaReadStream* stream)
    :m_pStream(stream), m_pPacket(0), m_iEof(0), rem_buffer(0),
    rem_size(0), rem_local(0), rem_limit(0),
    m_uiLastPos(0), m_dLastTime(0.)
{
    assert(m_pStream != 0);

    m_uiFormatSize = m_pStream->GetFormat();
    m_pFormat = new char[m_uiFormatSize];
    m_pStream->GetFormat(m_pFormat, m_uiFormatSize);

    if (m_pStream->GetLength() > 0x7fffffff)
	throw FATAL("Empty stream");

    const char* tname;
    switch (GetType())
    {
    case Video:
	tname = "video";
        break;
    case Audio:
	tname = "audio";
	break;
    default:
	tname = "unknown";
        break;
    }
    avm_printf(__MODULE__, "Initialized %s stream (chunk tblsz: %d, fmtsz: %d)\n",
	      tname, m_pStream->GetLength(), m_uiFormatSize);
}

ReadStream::~ReadStream()
{
    if (rem_buffer) free(rem_buffer);
    delete[] m_pFormat;
    if (m_pPacket)
	m_pPacket->Release();
}

bool ReadStream::Eof() const
{
    //printf("EOF %d  %d\n", m_iEof, GetType());
    return (m_iEof > 1);
    //avm_printf(__MODULE__, "EOF %d  Pos: %d  Len: %d\n", m_iEof, GetPos(), m_pStream->GetLength());
    //if (GetType() == IStream::Audio)
    //    avm_printf(__MODULE__, "%p EOF %d  Pos: %f  Len: %f  t:%d\n", this, m_iEof, GetTime(), m_pStream->GetLengthTime(), GetType());
    // using delta difference as comparing two doubles might have some epsilon problems
    if (m_iEof > 1 || (m_pStream->GetLengthTime() - GetTime()) < 0.001)
    {
	//avm_printf(__MODULE__, "EOF\n");
	return true;
    }
    return false;
}

void ReadStream::Flush()
{
    m_iEof = 0;
    rem_size = rem_local = 0;
    ReadPacket();
}

int ReadStream::GetFrameFlags(int* flags) const
{
    int f = (m_pPacket && m_pPacket->flags) ? KEYFRAME : 0;
    if (flags)
	*flags = f;
    return f;
}

double ReadStream::GetFrameTime() const
{
    return m_pStream->GetFrameTime();
}

uint_t ReadStream::GetHeader(void* pheader, uint_t n) const
{
    return m_pStream->GetHeader(pheader, n);
}

framepos_t ReadStream::GetLength() const
{
    return m_pStream->GetLength();
}

double ReadStream::GetLengthTime() const
{
    return m_pStream->GetLengthTime();
}

framepos_t ReadStream::GetPos() const
{
    return m_uiLastPos;
}

framepos_t ReadStream::GetNextKeyFrame(framepos_t pos) const
{
    avm_dprintf(__MODULE__, 3, "GetNextKeyFrame() %d\n", pos);
    return m_pStream->GetNextKeyFrame((pos == ERR) ? GetPos() : pos);
}

framepos_t ReadStream::GetPrevKeyFrame(framepos_t pos) const
{
    avm_dprintf(__MODULE__, 3, "ReadStream::GetPrevKeyFrame() %d\n", pos);
    return m_pStream->GetPrevKeyFrame((pos == ERR) ? GetPos() : pos);
}

StreamInfo* ReadStream::GetStreamInfo() const
{
    return m_pStream->GetStreamInfo();
}

double ReadStream::GetTime(framepos_t pos) const
{
    return (pos == ERR) ? m_dLastTime : m_pStream->GetTime(pos);
}

ReadStream::StreamType ReadStream::GetType() const
{
    return m_pStream->GetType();
}

int ReadStream::ReadDirect(void* buffer, uint_t bufsize, uint_t samples,
			   uint_t& samples_read, uint_t& bytes_read,
			   int* flags)
{
    if (!rem_size)
    {
	if (rem_buffer) free(rem_buffer);
        if (!m_pPacket)
	    ReadPacket();
	if (!m_pPacket)
	{
	    rem_buffer = 0;
            rem_limit = 0;
	    m_iEof++;
	    return -1;
	}
	rem_buffer = m_pPacket->memory;
	m_pPacket->memory = 0; // transfer allocated memory
	rem_size = m_pPacket->size;
        rem_limit = rem_size / 2;
	rem_flags = m_pPacket->flags;
	//printf("FLAGS  %d  SIZE %d\n", rem_flags, rem_size);
	rem_local = 0;
	m_pPacket->read = m_pPacket->size;
        ReadPacket();
    }

    if (buffer)
    {
        bytes_read = bufsize;
	if (bytes_read > rem_size)
            bytes_read = rem_size;

	memcpy(buffer, rem_buffer + rem_local, bytes_read);
	rem_size -= bytes_read;
        rem_local += bytes_read;
        samples_read = bytes_read;
    }
    else
    {
	bytes_read = rem_size;
        samples_read = rem_size;
    }

    int ssize = m_pStream->GetSampleSize();
    if (ssize > 1)
        samples_read /= ssize;

    if (flags)
	*flags = rem_flags;

    //printf("ReadDirect br:%d  sr:%d   ssize:%d   flg:%d\n", bytes_read, samples_read, ssize, rem_flags);
    return 0;
}

StreamPacket* ReadStream::ReadPacket()
{
    if (m_pPacket && m_pPacket->read >= m_pPacket->size)
    {
	m_pPacket->Release();
	m_pPacket = 0;
    }

    if (!m_pPacket)
	m_pPacket = m_pStream->ReadPacket();

    if (m_pPacket)
    {
	if (m_pPacket->timestamp != StreamPacket::NO_TIMESTAMP)
	{
	    m_uiLastPos = m_pPacket->position;
	    m_dLastTime = m_pPacket->timestamp / 1000000.0;
	}
    }
    else
    {
        double t = m_pStream->GetTime();
	if (t != m_dLastTime)
	{
	    m_dLastTime = t;
	    m_uiLastPos++;
	}
    }
    return m_pPacket;
}

int ReadStream::Seek(framepos_t pos)
{
    avm_dprintf(__MODULE__, 2, "Seek(%d)\n", pos);
    int hr = m_pStream->Seek(pos);
    if (hr == 0)
    {
	if (m_pPacket) m_pPacket->read = m_pPacket->size;
	Flush();
    }
    return hr;
}

int ReadStream::SeekTime(double timepos)
{
    avm_dprintf(__MODULE__, 2, "SeekTime(%f) (%f)\n", timepos, m_dLastTime);
    int hr = m_pStream->SeekTime(timepos);
    if (hr == 0)
    {
	if (m_pPacket) m_pPacket->read = m_pPacket->size;
	Flush();
    }
    return hr;
}

framepos_t ReadStream::SeekToKeyFrame(framepos_t pos)
{
    return (Seek(pos) == 0) ? GetPos() : ERR;
}

double ReadStream::SeekTimeToKeyFrame(double timepos)
{
    return (SeekTime(timepos) == 0) ? GetTime() : -1.0;
}

int ReadStream::SkipFrame()
{
    avm_dprintf(__MODULE__, 2, "SkipFrame()\n");
    return m_pStream->SkipFrame();
}

int ReadStream::SkipTo(double pos)
{
    avm_dprintf(__MODULE__, 2, "SkipTo()\n");
    return m_pStream->SkipTo(pos);
}

framepos_t ReadStream::SeekToNextKeyFrame()
{
    avm_dprintf(__MODULE__, 3, "SeekToNextKeyFrame()\n");
    framepos_t newpos = GetNextKeyFrame();
    if (newpos == ERR)
	return newpos;

    Seek(newpos);
    return GetPos();
}

framepos_t ReadStream::SeekToPrevKeyFrame()
{
    avm_dprintf(__MODULE__, 3, "SeekToPrevKeyFrame()\n");
    framepos_t newpos = GetPrevKeyFrame();
    if (newpos == ERR)
	newpos = 0;
    Seek(newpos);
    return GetPos();
}

uint_t ReadStream::GetAudioFormat(void* bi, uint_t size) const
{
    avm_printf(__MODULE__, "WARNING: GetAudioFormat() for non-audio stream\n");
    return 0;
}

uint_t ReadStream::GetVideoFormat(void* bi, uint_t size) const
{
    avm_printf(__MODULE__, "WARNING: GetVideoFormat() for non-video stream\n");
    return 0;
}

IAudioDecoder* ReadStream::GetAudioDecoder() const
{
    avm_printf(__MODULE__, "WARNING: Getting audio decoder for non-audio stream\n");
    return 0;
}

IVideoDecoder* ReadStream::GetVideoDecoder() const
{
    avm_printf(__MODULE__, "WARNING: Getting video decoder for non-video stream\n");
    return 0;
}

CImage* ReadStream::GetFrame(bool readframe)
{
    avm_printf(__MODULE__, "WARNING: GetFrame() called for non-video stream\n");
    return 0;
}

uint_t ReadStream::GetFrameSize() const
{
    avm_printf(__MODULE__, "WARNING: GetFrameSize() called for non-video stream\n");
    return 0;
}

int ReadStream::ReadFrame(bool render)
{
    avm_printf(__MODULE__, "WARNING: ReadFrame() called for non-video stream\n");
    return -1;
}

int ReadStream::ReadFrames(void* buffer, uint_t bufsize, uint_t samples,
				  uint_t& samples_read, uint_t& bytes_read)
{
    avm_printf(__MODULE__, "WARNING: ReadFrames() called for non-audio stream\n");
    return -1;
}

int ReadStream::SetOutputFormat(void* bi, uint_t size)
{
    avm_printf(__MODULE__, "WARNING: SetOutputFormat() called\n");
    return -1;
}

#undef __MODULE__

AVM_END_NAMESPACE;
