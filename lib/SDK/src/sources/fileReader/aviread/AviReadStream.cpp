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
#include <stdio.h>

#include "AviReadStream.h"
#include "AviReadHandler.h"
#include "formats.h"
#include "avm_output.h"
#include "avm_cpuinfo.h"
#include "utils.h"


AVM_BEGIN_NAMESPACE;
//static float ttt = 0;
AviReadStream::AviReadStream(AviReadHandler* handler,
			     const AVIStreamHeader& hdr,
			     uint_t id, const void* format, uint_t fsize)
    :m_pHandler(handler), m_iId(id), m_uiChunk(0),
    m_Header(hdr), m_uiFormatSize(fsize),
    m_uiStart(0), m_uiStreamSize(0), m_uiKeyChunks(0),
    m_uiKeySize(0),  m_uiKeyMaxSize(0), m_uiKeyMinSize(~0U),
    m_uiDeltaSize(0), m_uiDeltaMaxSize(0), m_uiDeltaMinSize(~0U)
{
    m_pcFormat = new char[m_uiFormatSize];
    memcpy(m_pcFormat, format, m_uiFormatSize);

    if (m_Header.fccType == streamtypeAUDIO && m_Header.dwSampleSize)
    {
	// even thought VBR audio has SampleSize=0
        // it's a weird type of audio stream
	m_dAvgBytesPerSec = ((WAVEFORMATEX*)m_pcFormat)->nAvgBytesPerSec;
	m_Positions.reserve(16384);
    }
    else
    {
	m_dAvgBytesPerSec = (m_Header.dwScale) ? m_Header.dwRate / (double) m_Header.dwScale : 1.0;
	//printf("AVG %f\n", m_dAvgBytesPerSec);

	// even thought VBR audio has SampleSize=0
	// it's a weird type of audio stream
        if (m_Header.fccType == streamtypeAUDIO)
	    m_Positions.reserve(16384);
    }
    m_uiPosition = m_Header.dwStart;
    m_Offsets.reserve(16384);
}

AviReadStream::~AviReadStream()
{
    delete[] m_pcFormat;
    //printf("TOTAL %f\n", ttt);
}

double AviReadStream::CacheSize() const
{
    return m_pHandler->m_Input.cacheSize();
}

void AviReadStream::ClearCache()
{
    m_pHandler->m_Input.clear();
}

uint_t AviReadStream::GetHeader(void* pHeader, uint_t size) const
{

    if (pHeader && size >= sizeof(m_Header))
    {
	memset(pHeader, 0, size);
	memcpy(pHeader, &m_Header, sizeof(m_Header));
    }
    return sizeof(m_Header);
}

uint_t AviReadStream::GetFormat(void *pFormat, uint_t lSize) const
{
    if (pFormat)
	memcpy(pFormat, m_pcFormat, (lSize < m_uiFormatSize) ? lSize : m_uiFormatSize);

    return m_uiFormatSize;
}

framepos_t AviReadStream::GetLength() 
{
    framepos_t l = m_Header.dwLength;
    if (m_Header.dwSampleSize)
	l *= m_Header.dwSampleSize;
    return l + m_Header.dwStart;
}

double AviReadStream::GetLengthTime() 
{
    return GetTime(GetLength());
}

framepos_t AviReadStream::GetNearestKeyFrame(framepos_t pos) const
{
    if (pos == ERR)
	pos = m_uiPosition;

    framepos_t ipos = pos - m_Header.dwStart;
    if (ipos >= m_Offsets.size() || ipos == ERR)
	return 0;

    if (m_Offsets[(int)ipos] & 1)
	return pos;

    framepos_t prev = GetPrevKeyFrame(pos);
    framepos_t next = GetNextKeyFrame(pos);

    return ((pos - prev) < (next - pos)) ? prev : next;
}

framepos_t AviReadStream::GetNextKeyFrame(framepos_t pos) const
{
    if (pos == ERR)
	pos = m_uiPosition;

    if (m_Header.dwSampleSize)
	return pos;

    pos -= m_Header.dwStart;
    while (pos < m_Offsets.size())
    {
	if (m_Offsets[(int)pos] & 1)
	    return pos + m_Header.dwStart;
        pos++;
    }

    return ERR;
}

framepos_t AviReadStream::GetPrevKeyFrame(framepos_t pos) const
{
    if (pos == ERR)
	pos = m_uiPosition;

    if (m_Header.dwSampleSize)
	return pos;

    pos -= m_Header.dwStart;
    if (pos > m_Offsets.size() || pos == ERR)
	return ERR;
    if (pos <= 1)
        return 1;

    pos--;
    while (pos > 0 && !(m_Offsets[(int)(--pos)] & 1))
        ;

    return pos + m_Header.dwStart;
}

StreamInfo* AviReadStream::GetStreamInfo()
{
    if (m_StreamInfo.m_p->m_dLengthTime == 0.0)
    {

		m_StreamInfo.m_p->setKfFrames(m_uiKeyMaxSize, m_uiKeyMinSize,
						  m_uiKeyChunks, m_uiKeySize);

		m_StreamInfo.m_p->setFrames(m_uiDeltaMaxSize,
						(m_uiDeltaMinSize > m_uiDeltaMaxSize)
										// usually no delta frames
						? m_uiDeltaMaxSize : m_uiDeltaMinSize,
						m_Offsets.size() - m_uiKeyChunks,
						m_uiStreamSize - m_uiKeySize);

		this->GetLengthTime();
		m_StreamInfo.m_p->m_dLengthTime = 0;//
		m_StreamInfo.m_p->m_iQuality = m_Header.dwQuality;
		m_StreamInfo.m_p->m_iSampleSize = m_Header.dwSampleSize;

		if (m_Header.fccType == streamtypeVIDEO)
		{
			BITMAPINFOHEADER* h = (BITMAPINFOHEADER*) m_pcFormat;
			m_StreamInfo.m_p->setVideo(h->biWidth, h->biHeight);
			m_StreamInfo.m_p->m_Type = StreamInfo::Video;
			m_StreamInfo.m_p->m_uiFormat = h->biCompression;
		}
		else if (m_Header.fccType == streamtypeAUDIO)
		{
			WAVEFORMATEX* w = (WAVEFORMATEX*) m_pcFormat;

			m_StreamInfo.m_p->setAudio(w->nChannels, w->nSamplesPerSec,
						   w->wBitsPerSample);
			m_StreamInfo.m_p->m_Type = StreamInfo::Audio;
			m_StreamInfo.m_p->m_uiFormat = w->wFormatTag;
		}
    }

    return new StreamInfo(m_StreamInfo);
}

double AviReadStream::GetTime(framepos_t pos)
{
    if (pos == ERR)
	pos = m_uiPosition;
    if (m_Header.dwSampleSize || !m_Positions.size())
    {
	framepos_t l = GetLength();
	if (pos > l)
	    pos = l;
    }
    else
	pos = (pos < m_Positions.size()) ? m_Positions[pos] : (m_Positions.back() + 1);
    //printf("AVIGETTIME %d    %f   (%f,  %d,   %.4s   s: %d)\n", pos, pos / m_dAvgBytesPerSec, m_dAvgBytesPerSec, m_Header.dwStart, (const char*)&m_Header.fccType, m_Positions.size());
    return pos / m_dAvgBytesPerSec;
}

IStream::StreamType AviReadStream::GetType()
{
    switch (m_Header.fccType)
    {
    case streamtypeAUDIO: return IStream::Audio;
    case streamtypeVIDEO: return IStream::Video;
    default: return IStream::Other;
    }
}

bool AviReadStream::IsKeyFrame(framepos_t pos) const
{
    if (m_Header.dwSampleSize)
	return true; //audio

    if (pos == ERR)
        pos = m_uiPosition;
    pos -= m_Header.dwStart;
    if (pos >= m_Offsets.size() || pos == ERR)
	return true; //or whatever

    return m_Offsets[pos] & 1;
}

StreamPacket* AviReadStream::ReadPacket()
{
    //int64_t st = longcount();
    StreamPacket* p = m_pHandler->m_Input.readPacket(m_iId, m_uiChunk++);
    //printf("ReadPacket: %p  id:%d  chunk:%d  pos:%d\n", p, m_iId, m_uiChunk, m_uiPosition);
    //ttt += to_float(longcount(), st);
    if (p)
    {
	p->position = m_uiPosition;
	p->timestamp = (int64_t)(GetTime() * 1000000.);
	//if (m_Header.dwSampleSize) printf("readpacketpos  %d  %d   id:%d  %lld   %p   pos:%lld\n", m_uiPosition, p->size, m_iId, p->timestamp, p, m_pHandler->m_Input.pos());
	m_uiPosition = (m_Header.dwSampleSize && m_Positions.size() > m_uiChunk) ? m_Positions[m_uiChunk] : m_uiChunk;
	m_uiPosition += m_Header.dwStart;
	//printf("SETNEW RP %d  ss:%d   ts: %lld\n", m_uiPosition, m_Header.dwSampleSize, p->timestamp);
    }
    return p;
}

int AviReadStream::Seek(framepos_t pos)
{
    m_uiChunk = find(pos);
    avm_dprintf("AVI reader", 3, "AviReadStream::Seek(%u) -> %d  (%d)\n", pos, m_uiChunk, m_iId);
    m_uiPosition = (m_Header.dwSampleSize) ? m_Positions[m_uiChunk] : m_uiChunk;
    m_uiPosition += m_Header.dwStart;
    //ClearCache(); // ?? not needed - cache will do this when necessary
    return 0;
}

int AviReadStream::SeekTime(double timepos)
{
    avm_dprintf("AVI reader", 3, "AviReadStream::SeekTime(%f)\n", timepos);
    return Seek((framepos_t)(timepos * m_dAvgBytesPerSec));
}

// takes normal pos
// returns pos without dwStart!
framepos_t AviReadStream::find(framepos_t pos)
{
    framepos_t len = GetLength();
    if (pos <= m_Header.dwStart)
	return 0;
    if (pos >= len || pos == ERR)
	pos = len;

    pos -= m_Header.dwStart;
    if (!m_Header.dwSampleSize && !m_Positions.size())
        return pos;

    //avm_printf("AVI reader", "FIND %d   %d\n", pos, m_Index.size());
    framepos_t low_limit = 0;
    framepos_t high_limit = m_Positions.size() - 1;
    while (low_limit != high_limit)
    {
	framepos_t middle = (low_limit + high_limit) / 2;
	if (pos >= m_Positions[middle])
	{
            // this fix is necessary to avoid deadlock
	    if (middle == low_limit)
                break;

	    low_limit = middle;
	    if (pos < m_Positions[middle + 1])
		break;
	}
	else
	    high_limit = middle;
    }

    return low_limit;
}

void AviReadStream::addChunk(uint_t coffset, uint_t clen, bool iskf)
{
    if (iskf)
    {
	m_uiKeyChunks++;
	m_uiKeySize += clen;
	if (m_uiKeyMaxSize < clen)
	    m_uiKeyMaxSize = clen;
	if (m_uiKeyMinSize > clen)
	    m_uiKeyMinSize = clen;
	// avi chunks begins on 'even' positions
	// so use this the lowest bit as keyframe flag
	coffset |= 1;
    }
    else
    {
	m_uiDeltaSize += clen;
	if (m_uiDeltaMaxSize < clen)
	    m_uiDeltaMaxSize = clen;
	if (m_uiDeltaMinSize > clen)
	    m_uiDeltaMinSize = clen;
    }
    if (m_Header.dwSampleSize)
	// we probably don't need bytes position
        // for chunk stream - so use them only for wave audio
	m_Positions.push_back(m_uiStreamSize);
    else if (m_Header.fccType == streamtypeAUDIO && m_Header.dwScale)
    {
	// VBR audio magic - we have to recalculate chunk position
	// if the chunk is bigger then dwScale - it represents
	// more then one chunk in that case - IMHO what a stupid idea
        // basicaly it violates all the standards
	uint_t p = (m_Positions.size()) ? m_Positions.back() : 0;
	p += (clen + m_Header.dwScale - 1) / m_Header.dwScale;
	// yep that's all
	m_Positions.push_back(p);
    }

    m_uiStreamSize += clen;

    m_Offsets.push_back(coffset);
    //if (m_iId > 0) printf("Id:%d  offset:%10d  sp:%10d  len:%7d  p:%d\n", m_iId, coffset, m_uiStreamSize, clen, m_Offsets.size());
    //if (m_iId > 0) printf("Id:%d  offset:%10d  sp:%10d  len:%6d  p:%5d (%d) -> %.3f\n", m_iId, coffset, m_uiStreamSize, clen, m_Positions.back(), m_Positions.size(),
    //    		  m_Positions.back() * m_Header.dwScale /  (double)m_Header.dwRate);
    //else printf("Id:%d  offset:%10d  sp:%10d  len:%6d  p:%5d -> %.3f\n", m_iId, coffset, m_uiStreamSize, clen, m_Offsets.size(),
    //    	m_Offsets.size() * m_Header.dwScale /  (double)m_Header.dwRate);
}

void AviReadStream::fixHeader()
{
    uint_t n = (m_Header.dwSampleSize)
	? m_uiStreamSize / m_Header.dwSampleSize: m_Offsets.size();
    if (n != m_Header.dwLength)
    {
	avm_printf("AVI reader", "WARNING: stream header has incorrect dwLength (%d != %d)\n", m_Header.dwLength, n);
	m_Header.dwLength = n;
    }
}

int AviReadStream::FixAvgBytes(uint_t bps)
{
    if (bps > 8000)
    {
	m_dAvgBytesPerSec = bps;
	return 0;
    }
    // ok some broken files with vbr stream and nonvbr headers
    // are weird and we can't easily recognize them - they usually
    // have 32kbps blocks at the begining
    // also this is hack to make playable several broken sample files
    // I have thus it might fail for others...
    return -1;
}

AVM_END_NAMESPACE;
