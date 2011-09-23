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
#include <string.h>

#include "StreamInfoPriv.h"
#include "utils.h"

AVM_BEGIN_NAMESPACE;

StreamInfo::StreamInfo() : m_p(0)
{
    m_p = new StreamInfoPriv;

    m_p->m_dLengthTime = 0.0;
    m_p->m_uiMaxKfFrameSize = 0;
    m_p->m_uiMinKfFrameSize = 0;
    m_p->m_uiKfFrames = 0;
    m_p->m_uiKfFramesSize = 0;

    m_p->m_uiMaxFrameSize = 0;
    m_p->m_uiMinFrameSize = 0;
    m_p->m_uiFrames = 0;
    m_p->m_uiFramesSize = 0;

    m_p->m_Type = Video;
}

StreamInfo::StreamInfo(const StreamInfo& si) : m_p(0)
{
    m_p = new StreamInfoPriv;

    memcpy(m_p, si.m_p, sizeof(StreamInfoPriv));
}

StreamInfo::~StreamInfo()
{
    delete m_p;
}

StreamInfo::Type StreamInfo::GetType() const
{
    return m_p->m_Type;
}

uint_t StreamInfo::GetFormat() const
{
    return m_p->m_uiFormat;
}

const char* StreamInfo::GetFormatName() const
{
    return (m_p->m_Type == Audio) ?
	avm_wave_format_name(m_p->m_uiFormat) : avm_fcc_name(m_p->m_uiFormat);
}

double StreamInfo::GetBps() const
{
    return (m_p->m_dLengthTime && m_p->m_dLengthTime < 0x7fffffff && GetStreamSize() > 0)
	? GetStreamSize() / m_p->m_dLengthTime : (double) m_p->m_uiBps;
}

double StreamInfo::GetFps() const
{
    return (m_p->m_dLengthTime) ? GetStreamFrames() /  m_p->m_dLengthTime : 0.0;
}

double StreamInfo::GetLengthTime() const
{
    return m_p->m_dLengthTime;
}

int64_t StreamInfo::GetStreamSize() const
{
    return m_p->m_uiKfFramesSize + m_p->m_uiFramesSize;
}

uint_t StreamInfo::GetStreamFrames() const
{
    return m_p->m_uiKfFrames + m_p->m_uiFrames;
}

uint_t  StreamInfo::GetMaxKfFrameSize() const
{
    return m_p->m_uiMaxKfFrameSize;
}

uint_t  StreamInfo::GetMinKfFrameSize() const
{
    return m_p->m_uiMinKfFrameSize;
}

uint_t  StreamInfo::GetAvgKfFrameSize() const
{
    return (m_p->m_uiKfFrames > 0) ? m_p->m_uiKfFramesSize / m_p->m_uiKfFrames : 0;
}

int64_t StreamInfo::GetKfFramesSize() const
{
    return m_p->m_uiKfFramesSize;
}

uint_t StreamInfo::GetKfFrames() const
{
    return m_p->m_uiKfFrames;
}

uint_t  StreamInfo::GetMaxFrameSize() const
{
    return m_p->m_uiMaxFrameSize;
}

uint_t  StreamInfo::GetMinFrameSize() const
{
    return m_p->m_uiMinFrameSize;
}

uint_t  StreamInfo::GetAvgFrameSize() const
{
    return (m_p->m_uiFrames > 0) ? m_p->m_uiFramesSize / m_p->m_uiFrames : 0;
}

int64_t StreamInfo::GetFramesSize() const
{
    return m_p->m_uiFramesSize;
}

uint_t StreamInfo::GetFrames() const
{
    return m_p->m_uiFrames;
}

int StreamInfo::GetSampleSize() const
{
    return m_p->m_iSampleSize;
}

int StreamInfo::GetQuality() const
{
    return m_p->m_iQuality;
}

int StreamInfo::GetVideoWidth() const
{
    return (m_p->m_Type == Video) ? m_p->vid.m_iWidth : -1;
}

int StreamInfo::GetVideoHeight() const
{
    return (m_p->m_Type == Video) ? m_p->vid.m_iHeight : -1;
}

float StreamInfo::GetAspectRatio() const
{
    return (m_p->m_Type == Video) ? m_p->vid.m_fAspect : -1;
}

int StreamInfo::GetAudioChannels() const
{
    return (m_p->m_Type == Audio) ? m_p->aud.m_iChannels : -1;
}

int StreamInfo::GetAudioSamplesPerSec() const
{
    return (m_p->m_Type == Audio) ? m_p->aud.m_iSamplesPerSec : -1;
}

int StreamInfo::GetAudioBitsPerSample() const
{
    return (m_p->m_Type == Audio) ? m_p->aud.m_iBitsPerSample : -1;
}

std::string StreamInfo::GetString() const
{
    double keyproc = 0, delproc = 0;
    double skeyproc = 0, sdelproc = 0;
    char buffer[2048];
    if (GetStreamFrames() > 0)
    {
	keyproc = GetKfFrames()/(double)GetStreamFrames() * 100.0;
	delproc = 100.0 - keyproc;
	skeyproc = (GetStreamSize() > 0) ? GetKfFramesSize()/(double)GetStreamSize() : 0.0;
	skeyproc *= 100.0;
	sdelproc = 100.0 - skeyproc;
    }

    int i = 0;
    if (m_p->m_Type == Audio)
    {
	i = sprintf(buffer, " AudioInfo - %s (0x%x) %dHz %db  Channels: %d  Sample Size: %d\n",
		    avm_wave_format_name(GetFormat()), GetFormat(),
		    GetAudioSamplesPerSec(),
		    GetAudioBitsPerSample(), GetAudioChannels(),
		    GetSampleSize());
    }
    else if (m_p->m_Type == Video)
    {
	char b[4];
	avm_set_le32(b, GetFormat());
	i = sprintf(buffer, " VideoInfo - %.4s (0x%x)  %dx%d  Fps: %.3f  Quality: %d\n",
		    b, GetFormat(), GetVideoWidth(), GetVideoHeight(), GetFps(),
		    GetQuality());
    }

    char tlen[50];
    sprintf(tlen, "%.2f", GetLengthTime());
    if (GetLengthTime() >= 0x7fffffff)
        strcpy(tlen, "Live");

    i += sprintf(buffer + i, "  Time length: %s  Size: %.0f bytes (%.2fKB)\n"
		 "  Frames: %d    Bps: %.2f kbps (%.2fKB/s)\n",
		 tlen, (double)GetStreamSize(),  GetStreamSize() / 1024.0,
		 GetStreamFrames(), GetBps() * 8 / 1000, GetBps() / 1024.0);

    if (m_p->m_Type == Video)
    {
	if (GetMinKfFrameSize() != ~0U || GetMinFrameSize() != ~0U)

	    i += sprintf(buffer + i,
			 "  Key frames/size %.2f%% (%.2f%%)  delta frames/size %.2f%%\n"
			 "  Min/avg/max/total key frame size: %d/%d/%d (%.2fKB) in %d frames\n"
			 "  Min/avg/max/total delta frame size: %d/%d/%d (%.2fKB) in %d frames\n",

			 keyproc, skeyproc, sdelproc,

			 GetMinKfFrameSize(), GetAvgKfFrameSize(), GetMaxKfFrameSize(),
			 GetKfFramesSize() / 1024.0, GetKfFrames(),

			 GetMinFrameSize(), GetAvgFrameSize(), GetMaxFrameSize(),
			 GetFramesSize() / 1024.0, GetFrames());
    }
    else if (m_p->m_Type == Audio)
    {
	if (GetMinKfFrameSize() != ~0U || GetMinFrameSize() != ~0U)
	    i += sprintf(buffer + i,
			 "  Min/avg/max/total frame size: %d/%d/%d (%.2fKB) in %d frames\n",
			 GetMinKfFrameSize(), GetAvgKfFrameSize(), GetMaxKfFrameSize(),
			 GetKfFramesSize() / 1024.0, GetKfFrames());
    }

    return std::string(buffer);
}

AVM_END_NAMESPACE;
