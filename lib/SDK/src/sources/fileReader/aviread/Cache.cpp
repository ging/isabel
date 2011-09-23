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

#include <icf2/general.h>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/time.h>
    #include <sys/stat.h>
    #include <errno.h>
#elif defined(__BUILD_FOR_WINXP)
#endif

#include "Cache.h"
#include "avm_cpuinfo.h"
#include "avm_output.h"
#include "utils.h"

// limit the maximum size of each prefetched stream in bytes
#define STREAM_SIZE_LIMIT 3000000

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

// Isn't this bug in NetBSD configuration - it should not have HAVE_LSEEK64
#ifdef __NetBSD__
#define lseek64 lseek
#else
#ifndef HAVE_LSEEK64
#define lseek64 lseek
#endif
#endif


// uncomment if you do not want to use threads for precaching
//#define NOTHREADS               // do not commit with this define

AVM_BEGIN_NAMESPACE;

#define __MODULE__ "StreamCache"
//static float ttt = 0;
//static float ttt1 = 0;

Cache::Cache(uint_t size)
:m_uiSize(size), thread(0), m_pPacket(0), m_iFd(NULL), m_uiId(0),
cache_access(0), cache_right(0), cache_miss(0), m_bQuit(false),
m_bThreaded(true) 
{
}

Cache::~Cache()
{
	mutex.Lock();
	m_bQuit = true;
	cond.Broadcast();
	mutex.Unlock();
	delete thread;
	clear();
	//printf("CACHE A %d  MIS %d  RIGH %d\n", cache_access, cache_miss, cache_right);
	if (cache_access != 0)
		avm_printf(__MODULE__, "Destroy... (Total accesses %d, hits %.2f%%, misses %.2f%%, errors %.2f%%)\n",
		cache_access, 100. * double(cache_right - cache_miss) / cache_access,
		100. * double(cache_miss) / cache_access,
		100. * double(cache_access - cache_right) / cache_access);
	//printf("TTTT %f  %f\n", ttt, ttt1);
}

int Cache::addStream(uint_t id, const std::vector<uint32_t>& table)
{
	avm_dprintf(__MODULE__, 3, "Adding stream, %d chunks\n", table.size());
	mutex.Lock();
	m_streams.push_back(StreamEntry(&table, 0, m_uiSize));
	cond.Broadcast();
	mutex.Unlock();

	return 0;
}

// starts caching thread once we know file descriptor
int Cache::create(FILE* fd)
{
	m_iFd = fd;
	avm_dprintf(__MODULE__, 1, "Creating cache for file descriptor: %d\n", m_iFd);
	if (m_streams.size() > 0)
	{
		if (m_bThreaded)
		{
			mutex.Lock();
			thread = new PthreadTask(0, Cache::startThreadfunc , this);
			cond.Wait(mutex,TIMEOUT);
			mutex.Unlock();		
		}
	}
	else
		avm_printf(__MODULE__, "Warning: No stream for caching!\n");

	return 0;
}

// return true if this stream should be waken and read new data
inline bool Cache::isCachable(StreamEntry& stream, uint_t id)
{
	// trick to allow precaching even very large image sizes
	//printf("ISCHACHE f:%d  s:%d  sum:%d   l: %d ll: %d\n", stream.packets.full(), stream.packets.size(), stream.sum, stream.last, stream.table->size());
	return ((stream.sum < STREAM_SIZE_LIMIT)
		&& stream.last < stream.table->size()
		&& !stream.packets.full()
		&& (stream.filling || 
		   (stream.sum < STREAM_SIZE_LIMIT/2 && stream.packets.size() < m_uiSize/2)));
}

//
// currently preffered picking alghorithm
//
// seems to be having good seek strategy
uint_t Cache::pickChunk()
{
	uint_t id = m_uiId;

	do
	{
		StreamEntry& se = m_streams[id];
		// determine next needed chunk in this stream
		se.last = (se.packets.empty()) ?
			se.position : se.packets.back()->position + 1;
		//printf("Pick sid:%d  pos:%d  wants:%d  size:%d\n", id, se.last, se.position, se.packets.size());

		if (isCachable(se, id))
			return id;
		// try next stream
		id = (id + 1)%m_streams.size();
	}
	while (id != m_uiId);

	return WAIT; // nothing for caching found
}

// caching thread
void* Cache::threadfunc()
{
	cond.Broadcast();
	
	int r = 0;
	while (!m_bQuit)
	{
		mutex.Lock();
		m_uiId = pickChunk();
		cond.Broadcast();
		if (m_uiId == WAIT)
		{
			m_uiId = 0;
			cond.Wait(mutex,TIMEOUT);
			mutex.Unlock();
			continue;
		}
		StreamEntry& stream = m_streams[m_uiId];		
		uint_t coffset = (*stream.table)[stream.last];
		char bfr[8];
		mutex.Unlock();
		if (fseek(m_iFd, coffset & ~1, SEEK_SET) == -1 || fread(bfr, 1, 8, m_iFd) != 8)
		{
			mutex.Lock();
			stream.error = stream.last;
			cond.Wait(mutex,TIMEOUT);
			mutex.Unlock();
			continue;
		}

		uint_t ckid = avm_get_le32(bfr);
		uint_t clen = avm_get_le32(bfr + 4);

		if (clen > StreamPacket::MAX_PACKET_SIZE)
		{
			// this is wrong and should be replaced by a better code
			avm_printf(__MODULE__, "Warning: Too large chunk %d\n", clen);
			clen = 10000;
		}
		m_pPacket = new StreamPacket(clen);
		m_pPacket->position = stream.last;

		uint_t rs = 0;
		while (rs < m_pPacket->size)
		{
			int rd = fread(m_pPacket->memory + rs,  1 , m_pPacket->size - rs, m_iFd);
			if (rd <= 0)
			{
				break;
			}
			rs += rd;
		}
		r += rs;
		mutex.Lock();
		// check if we still want same buffer
		if (rs != m_pPacket->size)
		{
			stream.error = stream.last;
			m_pPacket->Release();
			m_pPacket = NULL;
			cond.Wait(mutex,TIMEOUT);
			mutex.Unlock();
			continue;
		}
		stream.error = stream.OK;
		stream.sum += rs;
		m_pPacket->size = rs;
		m_pPacket->flags = (coffset & 1) ? AVIIF_KEYFRAME : 0;
		stream.filling = !(stream.sum > STREAM_SIZE_LIMIT);
		stream.packets.push(m_pPacket);
		m_pPacket = NULL;
		mutex.Unlock();
	}

	return 0;
}

// called by stream reader - most of the time this read should
// be satisfied from already precached chunks
StreamPacket* Cache::readPacket(uint_t id, framepos_t position)
{
	//avm_dprintf(__MODULE__, 4, "Cache: read(id %d, pos %d)\n", id, position);
	int rsize = 1;
	cache_access++;
	if (id >= m_streams.size()) {
		avm_dprintf(__MODULE__, 1, "stream:%d  out ouf bounds (%d)\n",
			id, m_streams.size());
		return 0;
	}

	StreamEntry& stream = m_streams[id];
	if (position >= stream.table->size()) {
		avm_dprintf(__MODULE__, 1, "to large stream:%d pos: %d (of %d)\n",
			id, position, stream.table->size());
		return 0;
	}
#if 0
	if (!m_bThreaded)
	{
		// code path for single threaded reading
		//int64_t sss = longcount();
		Locker locker(mutex);
		char bfr[8];
		if (fseek(m_iFd, (*stream.table)[position] & ~1, SEEK_SET) == -1
			|| fread(bfr, 1, 8, m_iFd) != 8) {
				avm_printf(__MODULE__, "Warning: Read error\n");
				return 0;
		}

		uint_t ckid = avm_get_le32(bfr);
		uint_t clen = avm_get_le32(bfr + 4);
		if (clen > StreamPacket::MAX_PACKET_SIZE)
		{
			avm_printf(__MODULE__, "Warning: Too large chunk %d\n", clen);
			clen = 100000;
		}
		StreamPacket* p = new StreamPacket(clen);
		if (p->size > 0)
		{
			rsize = fread(p->memory, 1, p->size, m_iFd);
			//printf("read_packet: id:%x   len:%d   rsize:%d  %d  m:%x\n", ckid, clen, rsize, p->size, *(int*)(p->memory));
			if (rsize <= 0)
			{
				p->Release();
				return 0;
			}
		}
		p->flags = (*stream.table)[position] & 1 ? AVIIF_KEYFRAME : 0;
		p->position = position;
		//ttt1 += to_float(longcount(), sss);
		return p;
	}
#endif
	mutex.Lock();

	if (stream.packets.size() < (CACHE_SIZE / 2)){
		cond.Broadcast(); // wakeup only when buffers are getting low...
	}
	
	//while (stream.actual != position || stream.packets.size() == 0)
	StreamPacket* p = 0;
	while (!m_bQuit)
	{
		//printf("STREAMPOS:%d  sp:%d  id:%d  ss:%d\n", position, stream.position, id, stream.packets.size());
		if (!stream.packets.empty())
		{
			p = stream.packets.front();
			stream.packets.pop();
			stream.sum -= p->size;
			if (p->position == position)
			{
				//avm_dprintf(__MODULE__, 4, "id: %d bsize: %d memory: %p pp: %d\n",
				//	  id, stream.packets.size(), p->memory, p->position);
				cache_right++;
				break;
			}
			//avm_dprintf(__MODULE__, 4, "position: 0x%x want: 0x%x\n", p->position, position);
			// remove this chunk
			//printf("delete chunkd %d   (wants %d)\n", p->position, position);
			p->Release();
			p = 0;
			continue;
		}
		if (stream.error == position) {
			//printf("READERROR  e:%d   p:%d   pl:%d\n", stream.error, position, stream.last);
			break;
		}
		cache_miss++;
		rsize = 0;
		m_uiId = id;
		stream.position = position;

		//avm_dprintf(__MODULE__, 4, "--- actual: %d size: %d\n", id, stream.packets.size());
		//int64_t w = longcount();
		//printf("ToWait read  sid:%d  pos:%d  size:%d\n", id, position, stream.packets.size());
		cond.Wait(mutex,TIMEOUT);
		//printf("----------- DoneWait read  size:%d\n", stream.packets.size());
		//ttt += to_float(longcount(), w);
		//avm_dprintf(__MODULE__, 4, "--- actual: %d done - size: %d\n", id, stream.packets.size());
	}

	
	
	mutex.Unlock();
	//printf("RETURN packet %p\n", p);
	return p;
}

int Cache::clear()
{
	avm_dprintf(__MODULE__, 4, "*** CLEAR ***\n");

	mutex.Lock();
	for (unsigned i = 0; i < m_streams.size(); i++)
	{
		StreamEntry& stream = m_streams[i];
		while (stream.packets.size())
		{
			StreamPacket* r = stream.packets.front();
			stream.packets.pop();
			r->Release();
		}
		stream.sum = 0;
		stream.position = 0;
	}
	m_uiId = 0;
	cond.Broadcast();
	mutex.Unlock();

	return 0;
}

double Cache::getSize()
{
	/*
	int status=0;
	for(int i=0; i<m_uiSize; i++)
	if(req_buf[i].st==req::BUFFER_READY)status++;
	return (double)status/m_uiSize;
	*/
	return 1.;
}

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
void* Cache::startThreadfunc(void* arg)
#elif defined(__BUILD_FOR_WINXP)
DWORD WINAPI Cache::startThreadfunc(void* arg)
#endif
{
	Cache* c = (Cache*)arg;
	c->threadfunc();
	return 0;
}


/*************************************************************/

InputStream::~InputStream()
{
	close();
}

FILE* InputStream::open(const char *pszFile)
{
	m_iFd = fopen(pszFile, "rb");
	if (m_iFd <= 0)
	{
printf("No abro fichero");		return NULL;
	}
printf("ABRO FICHERO!!!");
	m_iPos = ~0U;
	m_iBuffered = 0;
	return m_iFd;
}

void InputStream::close()
{
	delete cache;
	cache = 0;
	if (m_iFd > 0)
		fclose(m_iFd);
	m_iFd = NULL;
}

int InputStream::async()
{
	if (!cache)
		cache = new Cache();
	return (cache) ? cache->create(m_iFd) : -1;
}

int InputStream::addStream(uint_t id, const std::vector<uint32_t>& table)
{
	if (!cache)
		cache = new Cache();
	return (cache) ? cache->addStream(id, table) : -1;
}

int64_t InputStream::len() const
{
	long pos = ftell(m_iFd);
	fseek(m_iFd,0,SEEK_END);
	long size = ftell(m_iFd);
	fseek(m_iFd,pos,SEEK_SET);
	return size;
}

int64_t InputStream::seek(int64_t offset)
{
	m_iBuffered = 0;
	m_bEof = false;
	m_iPos = 0;
	return fseek(m_iFd, offset, SEEK_SET);
}

int64_t InputStream::seekCur(int64_t offset)
{
	//cout << "seekcur " << offset << "   " << m_iPos << endl;
	m_bEof = false;
	if (m_iPos >= m_iBuffered)
		return fseek(m_iFd, offset, SEEK_CUR);

	if (offset >= 0)
	{
		m_iPos += offset;
		if (m_iPos >= m_iBuffered)
			return fseek(m_iFd, m_iPos - m_iBuffered, SEEK_CUR);
	}
	else
	{
		if (m_iPos < -offset)
		{
			offset += m_iBuffered - m_iPos;
			m_iBuffered = 0;
			return fseek(m_iFd, offset, SEEK_CUR);
		}
		m_iPos += offset;
	}
	return pos();
}

int64_t InputStream::pos() const
{
	int64_t o = ftell(m_iFd);
	//printf("POS: %lld   %d  %d   %lld\n", o, m_iPos, m_iBuffered, len());
	if (m_iPos < m_iBuffered)
		o -= (m_iBuffered - m_iPos);
	if (o > len())
		o = len();
	return o;
}

int InputStream::read(void* buffer, uint_t size)
{
	int r = 0;
	if (m_iBuffered > 0)
	{
		uint_t copy = m_iBuffered - m_iPos;
		if (size < copy)
			copy = size;
		memcpy(buffer, bfr + m_iPos, copy);
		m_iPos += copy;
		r = copy;
		size -= copy;
		buffer = (char*) buffer + copy;
	}
	if (size > 0)
	{
		int s = fread(buffer, 1,size, m_iFd);
		if (s <= 0)
		{
			m_bEof = true;
			return -1;
		}
		r += s;
	}

	return r;
}

uint8_t InputStream::readByte()
{
	if (m_iPos >= m_iBuffered)
	{
		int r = fread(bfr,1, sizeof(bfr), m_iFd);
		if (r <= 0)
		{
			m_bEof = true;
			return 255;
		}
		m_iBuffered = r;
		m_iPos = 0;
	}
	return bfr[m_iPos++];
#if 0
	uint8_t c;
	::read(m_iFd, &c, 1);

	return c;
#endif
}

uint32_t InputStream::readDword()
{
	uint32_t dword = readByte() | (readByte() << 8) | (readByte() << 16) | (readByte() << 24);
	return dword;
}

uint16_t InputStream::readWord()
{
	return readByte() | (readByte() << 8);
}


#undef __MODULE__

AVM_END_NAMESPACE;
