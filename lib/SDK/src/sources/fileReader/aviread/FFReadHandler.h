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
#ifndef FFREADHANDLER_H
#define FFREADHANDLER_H

#include "ReadHandlers.h"
#include "avm_locker.h"

struct AVInputStream;
struct AVFormatContext;

AVM_BEGIN_NAMESPACE;

class FFReadStream;

class FFReadHandler : public IMediaReadHandler
{
friend class FFReadStream;
public:
    FFReadHandler();
    virtual ~FFReadHandler();
    int Init(const char* url);
    virtual uint_t GetHeader(void* pheader, uint_t size);
    virtual IMediaReadStream* GetStream(uint_t stream_id,
					IStream::StreamType type);
    virtual uint_t GetStreamCount(IStream::StreamType type);
    virtual bool GetURLs(std::vector<std::string>& urls) { return false; }
    virtual void Interrupt() {}
    /* avi files are opened synchronously */
    virtual bool IsOpened() { return true; }
    virtual bool IsValid() { return true; }
    virtual bool IsRedirector() { return false; }

protected:
    void flush();
    int seek(framepos_t pos);
    int readPacket();

    AVFormatContext* m_pContext;
    std::vector<FFReadStream*> m_Streams;
    AVInputStream* m_pInput;
    PthreadMutex m_Mutex;
};

AVM_END_NAMESPACE;

#endif // FFREADHANDLER_H
