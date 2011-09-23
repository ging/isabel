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
#ifndef __UDP_SOUND_H__
#define __UDP_SOUND_H__

#include <icf2/sockIO.hh>
#include <icf2/lockSupport.hh>

#include "ringBuffer.h"
#include "mixerbuffer.h"

class UdpSound: virtual public simpleTask_t
{
private:
    static const int MAX_UDP_LEN= 1500;     // ethernet datagram size
    static const int MAX_BUFF_LEN= 48*1000; // 1000 ms at 48000 Hz

    RingBuffer_t ringBuffer;

    /*
    * buffer to read PCM 16 bits audio samples from network
    * (just to avoid automatic variable in IOReady method)
    */
    u8 data[MAX_UDP_LEN];

    /**
    * Mixer to copy network samples (locallly player)
    */
    MixerBuffer *mixerBuffer;

    /**
    * Write offset of mixer
    */
    int offset;

public:

    UdpSound(MixerBuffer *mixer);
    virtual ~UdpSound(void);

    /**
     * read audio samples from UDP socket
     */
    virtual void IOReady(io_ref& io);

    /**
    * Copy readed audio samples into 'pBuffer'
    * @param pBuffer where to mix network samples
    * @param length Length of 'pBuffer'
    * @return Number of readed bytes
    */
    int read(unsigned char *pBuffer, int length);
};

#endif
