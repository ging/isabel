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
#include "udpsound.h"

UdpSound::UdpSound(MixerBuffer *mixer)
: ringBuffer(MAX_BUFF_LEN, MAX_BUFF_LEN / 2)
{
    mixerBuffer = mixer;
    offset = mixerBuffer->getMinOffset();
}

UdpSound::~UdpSound(void)
{
}

int 
UdpSound::read(unsigned char *pBuffer, int length)
{
    return ringBuffer.Dequeue((i16*)pBuffer, length / 2, true) * 2;
}

void 
UdpSound::IOReady(io_ref& io)
{
    int n = io->read(data,MAX_UDP_LEN);

    if (n > 0)
    {
        if (mixerBuffer)
        {
           offset = mixerBuffer->write(offset, (u8*)data, n, true);
        }
        ringBuffer.Enqueue((i16*)data, n/2);
    }
}
