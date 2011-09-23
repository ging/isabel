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
#ifndef AVIREADHANDLER_H
#define AVIREADHANDLER_H

#include <vector>

#include "Cache.h"
#include "ReadHandlers.h"

AVM_BEGIN_NAMESPACE;

class AviReadStream;

class AviStreamPacket : public StreamPacket
{
public:
    int64_t offset;
    uint_t id;

    AviStreamPacket(uint_t off = 0, uint_t i = 0, uint_t bsize = 0)
	: StreamPacket(bsize), offset(off), id(i) {}
};

class AviReadHandler 
{
    friend class AviReadStream;
    static const uint_t MAX_CHUNK_SIZE = 10000000;
public:
    AviReadHandler(unsigned int flags = 0);
    virtual ~AviReadHandler();
    virtual uint_t GetHeader(void* pheader, uint_t size);
	virtual inline AVIMainHeader GetHeader(void) { return m_MainHeader; };
    virtual AviReadStream * GetStream(uint_t stream_id,
					IStream::StreamType type);
    virtual uint_t GetStreamCount(IStream::StreamType type);
    virtual bool GetURLs(std::vector<std::string>& urls) { return false; }
    virtual void Interrupt() {}
    /* avi files are opened synchronously */
    virtual bool IsOpened() { return true; }
    virtual bool IsValid() { return true; }
    virtual bool IsRedirector() { return false; }

    int init(const char* pszFile);

    static void PrintAVIMainHeader(const AVIMainHeader* hdr);
    static void PrintAVIStreamHeader(const AVIStreamHeader* hdr);
    static char* GetAviFlags(char* buffer, uint_t flags);

protected:
    int readAVIMainHeader(uint_t);
    int readAVIStreamHeader();
    void readStreams(uint_t);
    void readInfoChunk(uint_t);
    int readIndexChunk(uint_t, uint_t movie_chunk_offset);

    //AviStreamPacket* readPacket(bool fill = true);
    int reconstructIndexChunk(uint_t);

    AVIMainHeader m_MainHeader;
	std::vector<avm::AviReadStream*> m_Streams;
    InputStream m_Input;
    uint_t m_uiFlags;
};

AviReadHandler* CreateAviReadHandler(const char *pszFile, unsigned int flags);

AVM_END_NAMESPACE;

#endif // AVIREADHANDLER_H
