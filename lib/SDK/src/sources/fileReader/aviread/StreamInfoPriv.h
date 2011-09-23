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
#ifndef STREAMINFOPRIV_H
#define STREAMINFOPRIV_H

#include "infotypes.h"

AVM_BEGIN_NAMESPACE;

struct StreamInfo::StreamInfoPriv
{
    Type m_Type;
    uint32_t m_uiFormat;	// stream format

    double m_dLengthTime;	// time in seconds for all packets
    uint_t m_uiBps;		// bits per second
    uint_t m_uiMaxKfFrameSize;	// biggest keyframe chunk size
    uint_t m_uiMinKfFrameSize;	// smallest keyframe chunk size
    uint_t m_uiKfFrames;	// keyframe chunks
    int64_t m_uiKfFramesSize;	// total keyframe chunk size

    // should make sence probably only for video stream
    uint_t m_uiMaxFrameSize;	// biggest non-keyframe chunk size
    uint_t m_uiMinFrameSize;	// smallest non-keyframe chunk size
    uint_t m_uiFrames;		// non-keyframe chunks
    int64_t m_uiFramesSize;	// total non-keyframe chunk size
    int m_iQuality;             // quality of the stream (0..10000)
    int m_iSampleSize;		// sample size

    union
    {
	struct
	{
	    int	m_iChannels;
	    int	m_iSamplesPerSec;
	    int	m_iBitsPerSample;
            bool m_bVbr;
	} aud;
	struct
	{
	    int m_iWidth;
	    int m_iHeight;
	    float m_fAspect;
	} vid;
    };

    void setKfFrames(uint_t max, uint_t min, uint_t chunks, int64_t size)
    {
	m_uiMaxKfFrameSize = max;
        m_uiMinKfFrameSize = min;
        m_uiKfFrames = chunks;
        m_uiKfFramesSize = size;
    }
    void setFrames(uint_t max, uint_t min, uint_t chunks, int64_t size)
    {
	m_uiMaxFrameSize = max;
        m_uiMinFrameSize = min;
        m_uiFrames = chunks;
        m_uiFramesSize = size;
    }
    void setVideo(int width, int height, int bps = 0, float aspect = 0.0)
    {
	vid.m_iWidth = width;
	vid.m_iHeight = height;
        vid.m_fAspect = aspect;
	m_uiBps = bps;
    }
    void setAudio(int channels, int freq, int bits, int bps = 0)
    {
	if (bits == 0)
	    bits = 16;
	aud.m_iChannels = channels;
	aud.m_iSamplesPerSec = freq;
	aud.m_iBitsPerSample = bits;
        m_uiBps = bps;
    }
};

AVM_END_NAMESPACE;

#endif
