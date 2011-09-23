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
#ifndef AVIFILE_CACHE_H
#define AVIFILE_CACHE_H

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#ifdef AVIFILE_AVIFMT_H
#error Cache.h has to be the first inlude file you use!!!
#endif

// These classes provide basic caching abilities for files
// in AVI format. This is useful when files are played
// over NFS or SMB. Relies heavily on AVI structure.

#include "avifmt.h"
#include "ReadHandlers.h"
#include "avm_locker.h"
#include "avm_stl.h"
#include <stdio.h>
#include <vector>
#include <string>

AVM_BEGIN_NAMESPACE;


class Cache
{
public:
	static const int TIMEOUT = 5000;
    // FIXME: add methods for setting/getting size of cache
    Cache(uint_t size = CACHE_SIZE);
    ~Cache();
    //int addStream(uint_t id, const std::vector<AVIINDEXENTRY2>& table);
    int addStream(uint_t id, const std::vector<uint32_t>& table);
    int clear();
    int create(FILE * fd);
    double getSize();
    uint_t pickChunk();
    StreamPacket* readPacket(uint_t id, framepos_t pos);

protected:

    struct StreamEntry
    {
	const std::vector<uint32_t>* table;
	avm::qring<StreamPacket*> packets;
	uint_t position;       // needed chunk

	// handled internaly
	uint_t sum;             // cached bytes in stream
	int64_t offset;
	uint_t last;
	uint_t error;
        bool filling;
        static const uint_t OK = ~0U;
	//StreamEntry(const std::vector<AVIINDEXENTRY2>* t = 0,
	StreamEntry(const std::vector<uint32_t>* t = 0,
				uint_t pos = 0, uint_t size = CACHE_SIZE)
			:table(t), packets(size), position(pos), sum(0),
	    offset(0), last(~0U), error(OK), filling(false) {}
    };

    void* threadfunc(); // main thread for cache handling
    inline bool isCachable(StreamEntry& stream, uint_t id);  // new data could be readed
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    static void* startThreadfunc(void* arg);
#elif defined(__BUILD_FOR_WINXP)
    static DWORD WINAPI startThreadfunc(void* arg);
#endif
    
    static const uint_t WAIT = ~0U;
    static const uint_t CACHE_SIZE = 10;

    uint_t m_uiSize;		// preallocated chunks for one stream
    std::vector<StreamEntry> m_streams;

    PthreadTask* thread;
    PthreadMutex mutex;
    PthreadCond cond;
    StreamPacket* m_pPacket;
    FILE * m_iFd;			// cached file descritor
    uint_t m_uiId;		// currently processed stream
    uint_t cache_access;	// total amount of cache access
    uint_t cache_right;		// accesses satisfied by cache
    uint_t cache_miss;		// how many times we had to wait
    bool m_bQuit;		// quit caching thread
    bool m_bThreaded;
};

class InputStream
{
public:
    InputStream() : m_iFd(NULL), cache(0), m_bEof(false) {}
    ~InputStream();
    FILE* open(const char* file);
    void close();
    int64_t seek(int64_t offset);
    int64_t seekCur(int64_t offset);
    int64_t pos() const;
    int64_t len() const;
    bool eof() const { return m_bEof; }

    //enter asynchronous mode
    int async();

    //int addStream(uint_t id, const std::vector<AVIINDEXENTRY2>& table)
    int addStream(uint_t id, const std::vector<uint32_t>& table);
    int read(void* buffer, uint_t size);
    uint8_t readByte();
    uint32_t readDword();
    uint16_t readWord();

    //from cache:
    StreamPacket* readPacket(uint_t id, framepos_t fpos)
    {
	return (cache) ? cache->readPacket(id, fpos) : 0;
    }

    int clear() { return (cache) ? cache->clear() : -1; }
    double cacheSize() const { return (cache) ? cache->getSize() : 0.0; }
protected:
    static const int BFRSIZE = 512;
    FILE* m_iFd;
    Cache* cache;
    uint_t m_iPos;
    uint_t m_iBuffered;
    bool m_bEof;
    char bfr[BFRSIZE];
};

AVM_END_NAMESPACE;

#endif // AVIFILE_CACHE_H
