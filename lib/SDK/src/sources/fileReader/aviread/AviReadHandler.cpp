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

#include "AviReadHandler.h"
#include "AviReadStream.h"
#include "utils.h"
#include "avm_fourcc.h"
#include "formats.h"
#include "avm_output.h"

//#define USE_HACK

AVM_BEGIN_NAMESPACE;

// reset profiler status - just for debuging
//extern "C" void kprof_reset_stats(void);
struct RiffInfoChunk {
    uint_t riff;
    const char* text;
};

static const struct RiffInfoChunk infos[] =
{
    // here will follow various info about the movie file
    // comments here are copied from OpenDML specs by Matrox

    // Indicates where the subject of the file is archived
    { RIFFINFO_IARL, "Archival Location" },
    // Lists the artist of the original subject of the file;
    // for example, "Michaelangelo."
    { RIFFINFO_IART, "Artist" },
    // Lists the name of the person or organization that commissioned
    // the subject of the file; for example "Pope Julian II."
    { RIFFINFO_ICMS, "Commissioned" },
    // Provides general comments about the file or the subject
    // of the file. If the comment is several sentences long, end each
    // sentence with a period. Do not include new-line characters.
    { RIFFINFO_ICMT, "Comments" },
    // Records the copyright information for the file; for example,
    // "Copyright Encyclopedia International 1991." If there are multiple
    // copyrights, separate them by semicolon followed by a space.
    { RIFFINFO_ICOP, "Copyright" },
    // Specifies the date the subject of the file was created. List
    // dates in year-month-day format, padding one-digit months and days with
    // a zero on the left; for example, "1553-05-03" for May 3, 1553.
    { RIFFINFO_ICRD, "Creation Date" },
    // Describes whether an image has been cropped and, if so, how it
    // was cropped; for example, "lower-right corner."
    { RIFFINFO_ICRP, "Cropped" },
    // Specifies the size of the original subject of the file; for
    // example, "8.5 in h, 11 in w."
    { RIFFINFO_IDIM, "Dimensions" },
    // Stores dots per inch setting of the digitizer used to
    // produce the file, such as "300."
    { RIFFINFO_IDPI, "Dots Per Inch" },
    // Stores the of the engineer who worked on the file. If there are
    // multiple engineers, separate the names by a semicolon and a blank;
    // for example, "Smith, John; Adams, Joe."
    { RIFFINFO_IENG, "Engineer" },
    // Describes the original work, such as "landscape,", "portrait,"
    // "still liefe," etc.
    { RIFFINFO_IGNR, "Genre" },
    // Provides a list of keywords that refer to the file or subject of the
    // file. Separate multiple keywords with a semicolon and a blank;
    // for example, "Seattle, aerial view; scenery."
    { RIFFINFO_IKEY, "Keywords" },
    // ILGT - Describes the changes in the lightness settings on the digitizer
    // required to produce the file. Note that the format of this information
    // depends on the hardware used.
    { RIFFINFO_ILGT, "Lightness" },
    // IMED - Decribes the original subject of the file, such as
    // "computer image," "drawing," "lithograph," and so on.
    { RIFFINFO_IMED, "Medium" },
    // INAM - Stores the title of the subject of the file, such as
    // "Seattle from Above."
    { RIFFINFO_INAM, "Name" },
    // IPLT - Specifies the number of colors requested when digitizing
    // an image, such as "256."
    { RIFFINFO_IPLT, "Palette Setting" },
    // IPRD - Specifies the name of title the file was originally intended
    // for, such as "Encyclopedia of Pacific Northwest Geography."
    { RIFFINFO_IPRD, "Product" },
    // ISBJ - Decsribes the contents of the file, such as
    // "Aerial view of Seattle."
    { RIFFINFO_ISBJ, "Subject" },
    // ISFT - Identifies the name of the software packages used to create the
    // file, such as "Microsoft WaveEdit"
    { RIFFINFO_ISFT, "Software" },
    // ISHP - Identifies the change in sharpness for the digitizer
    // required to produce the file (the format depends on the hardware used).
    { RIFFINFO_ISHP, "Sharpness" },
    // ISRC - Identifies the name of the person or organization who
    // suplied the original subject of the file; for example, "Try Research."
    { RIFFINFO_ISRC, "Source" },
    // ISRF - Identifies the original form of the material that was digitized,
    // such as "slide," "paper," "map," and so on. This is not necessarily
    // the same as IMED
    { RIFFINFO_ISRF, "Source Form" },
    // ITCH - Identifies the technician who digitized the subject file;
    // for example, "Smith, John."
    { RIFFINFO_ITCH, "Technician" },

    // ISMP
    { RIFFINFO_ISMP, "Time Code" },
    // IDIT
    { RIFFINFO_IDIT, "Digitization Time" },

    { 0, 0 }
};

static inline void avm_get_leAVIMainHeader(AVIMainHeader* h)
{
    h->dwMicroSecPerFrame	= avm_get_le32(&h->dwMicroSecPerFrame);
    h->dwMaxBytesPerSec		= avm_get_le32(&h->dwMaxBytesPerSec);
    h->dwPaddingGranularity	= avm_get_le32(&h->dwPaddingGranularity);
    h->dwFlags			= avm_get_le32(&h->dwFlags);
    h->dwTotalFrames		= avm_get_le32(&h->dwTotalFrames);
    h->dwInitialFrames		= avm_get_le32(&h->dwInitialFrames);
    h->dwStreams		= avm_get_le32(&h->dwStreams);
    h->dwSuggestedBufferSize	= avm_get_le32(&h->dwSuggestedBufferSize);
    h->dwWidth			= avm_get_le32(&h->dwWidth);
    h->dwHeight			= avm_get_le32(&h->dwHeight);
    h->dwScale			= avm_get_le32(&h->dwScale);
    h->dwRate			= avm_get_le32(&h->dwRate);
    h->dwStart			= avm_get_le32(&h->dwStart);
    h->dwLength			= avm_get_le32(&h->dwLength);
}

static inline void avm_get_leAVIStreamHeader(AVIStreamHeader* h)
{
    h->fccType			= avm_get_le32(&h->fccType);
    h->fccHandler		= avm_get_le32(&h->fccHandler);
    h->dwFlags			= avm_get_le32(&h->dwFlags);
    h->wPriority 		= avm_get_le16(&h->wPriority);
    h->wLanguage 		= avm_get_le16(&h->wLanguage);
    h->dwInitialFrames		= avm_get_le32(&h->dwInitialFrames);
    h->dwScale			= avm_get_le32(&h->dwScale);
    h->dwRate			= avm_get_le32(&h->dwRate);
    h->dwStart			= avm_get_le32(&h->dwStart);
    h->dwLength			= avm_get_le32(&h->dwLength);
    h->dwSuggestedBufferSize	= avm_get_le32(&h->dwSuggestedBufferSize);
    h->dwQuality 		= avm_get_le32(&h->dwQuality);
    h->dwSampleSize		= avm_get_le32(&h->dwSampleSize);
    h->rcFrame.left		= avm_get_le16(&h->rcFrame.left);
    h->rcFrame.right		= avm_get_le16(&h->rcFrame.right);
    h->rcFrame.top		= avm_get_le16(&h->rcFrame.top);
    h->rcFrame.bottom		= avm_get_le16(&h->rcFrame.bottom);
}


AviReadHandler::AviReadHandler(unsigned int flags)
:m_uiFlags(flags)
{
}

int AviReadHandler::init(const char* pszFile)
{
    if (m_Input.open(pszFile) <= 0)
    {
	avm_printf("AVI reader", "Stream: %s can't be opened!\n", pszFile);
        return -1;
    }

   if ( m_Input.readDword() != FOURCC_RIFF )
#ifndef USE_HACK
	return -1
#endif
	    ;
    /*int riff_chunk=*/
    m_Input.readDword();
    if (m_Input.readDword() != formtypeAVI)
#ifndef USE_HACK
	return -1
#endif
	    ;

    uint_t movie_chunk = 0;
    uint_t type;
    int64_t nextpos;
    int chunk_size;
    uint32_t chunk_type;
    bool index_chunk_found = false;
    bool valid_list = false;

#ifdef USE_HACK
    AVIStreamHeader ash;
    ash.fccType = streamtypeVIDEO;
    BitmapInfo bih(640,360,24);
    bih.SetDirection(1);
    bih.biCompression = ('3'<<24) | ('V'<<16) | ('I' << 8) | 'D';
    ash.fccHandler = bih.biCompression;
    m_Streams.push_back(new AviReadStream(this, ash, 0, (char*)&bih, sizeof(bih)));
#endif

    for (;;)
    {
	chunk_type = m_Input.readDword();
	if (m_Input.eof())
            break;
	if (!valid_list && chunk_type != FOURCC_LIST)
            continue; // search for next valid LIST header

	chunk_size = m_Input.readDword();
        if (chunk_size < 0)
            continue;

	nextpos = m_Input.pos() + chunk_size + (chunk_size & 1);
	//printf("CHUNK  %.4s  %d    pos:%lld %d\n", (char*)&chunk_type, chunk_size, m_Input.pos(), m_Input.eof());
	switch(chunk_type)
	{
	case FOURCC_LIST:
	    if (chunk_size < 4)
	    {
		avm_printf("AVI reader", "Damaged Avi with LIST chunk size %d detected...\n", chunk_size);
                continue;
	    }
	    else
	    {
                valid_list = true;
		type = m_Input.readDword();
		//printf("LISTTYPE  %.4s   %lld %d\n", (char*)&type, m_Input.pos(), m_Input.eof());
		switch (type)
		{
		case listtypeAVIMOVIE:
		    movie_chunk = m_Input.pos();
		    m_Input.seek(nextpos);
		    // readMovieChunk();
		    //printf("MovieChunk  %d\n", movie_chunk);
		    break;
		case listtypeAVIHEADER:
		    continue;
		case listtypeSTREAMHEADER:
		    if (readAVIStreamHeader() < 0)
			return -1;
		    break;
		case listtypeINFO:
		    readInfoChunk(chunk_size);
		    break;
		}
	    }
	    break;
	case ckidAVIMAINHDR:
	    readAVIMainHeader(chunk_size);
	    break;
	case ckidAVINEWINDEX:
	    if (readIndexChunk(chunk_size, movie_chunk) == 0)
		index_chunk_found = true;
	    break;
	};
	m_Input.seek(nextpos);
    }

    if (m_Streams.size() == 0)
    {
        avm_printf("AVI reader", "No playable stream found in this AVI file!\n");
	return -1;
    }

    if (!index_chunk_found)
	reconstructIndexChunk(movie_chunk);

    for (unsigned i = 0; i < m_Streams.size(); i++)
    {
	m_Streams[i]->fixHeader();
	char b[100];
        int wFormatTag;
	if (m_Streams[i]->GetType() == IStream::Audio)
	{
	    if(m_Streams[i]->m_pcFormat && (m_Streams[i]->m_uiFormatSize >= 2))
		wFormatTag = ((WAVEFORMATEX*)(m_Streams[i]->m_pcFormat))->wFormatTag;
	    else
    		wFormatTag = m_Streams[i]->m_Header.fccHandler;
	    strncpy(b, avm_wave_format_name(wFormatTag), sizeof(b));
            b[sizeof(b) - 1] = 0;
	}
	else
	{
	    wFormatTag = m_Streams[i]->m_Header.fccHandler;
	    strncpy(b, (const char*)&m_Streams[i]->m_Header.fccHandler, 4);
            b[4] = 0;
	}

	//m_Streams[i]->m_Index.resize(m_Streams[i]->m_Index.size());
	m_Streams[i]->m_Offsets.resize(m_Streams[i]->m_Offsets.size());
	m_Streams[i]->m_Positions.resize(m_Streams[i]->m_Positions.size());

	char fcctype[4];
	avm_set_le32(fcctype, m_Streams[i]->m_Header.fccType);

	avm_printf("AVI reader", "Stream %d %.4s : %s (0x%x) %u chunks (%.2fKB)\n",
		  i, fcctype, b, wFormatTag, m_Streams[i]->m_Offsets.size(),
		  (m_Streams[i]->m_Offsets.size() * sizeof(uint32_t)
		   + m_Streams[i]->m_Positions.size() * sizeof(uint32_t))
		  / 1024.0);

#if 0
	avm_printf("AVI reader", "   Idx      Offset   Timestamp Flags\n");
	AviReadStream* stream = m_Streams[i];
	for (unsigned j = 0 ; j < stream->m_Offsets.size(); j++)
	    avm_printf("AVI reader", "%6d  %10d  %10d %d\n",
		      j, stream->m_Offsets[j] & ~0x01,
		      (j < stream->m_Positions.size()) ? stream->m_Positions[j] :
		      (int)(j / stream->m_dAvgBytesPerSec * 1000),
		      stream->m_Offsets[j] & 0x01);
#endif

	//hack   interleaved format  -  iavs
	//if (m_Streams[i]->m_Header.fccType == mmioFOURCC('i', 'a', 'v', 's'))
	//    m_Streams[i]->m_Header.fccType = streamtypeVIDEO;

	//m_Input.addStream(m_Streams[i]->m_iId, m_Streams[i]->m_Index);
	m_Input.addStream(m_Streams[i]->m_iId, m_Streams[i]->m_Offsets);
    }
    m_Input.async();
    return 0;
}

AviReadHandler::~AviReadHandler()
{
    m_Input.close();
    for (unsigned i = 0; i < m_Streams.size(); i++)
        delete m_Streams[i];
}

uint_t AviReadHandler::GetHeader(void* pheader, uint_t size)
{
    if (pheader && (size >= sizeof(AVIMainHeader)))
    {
	memset(pheader, 0, size);
	memcpy(pheader, &m_MainHeader, sizeof(AVIMainHeader));
    }
    return sizeof(AVIMainHeader);
}

AviReadStream * AviReadHandler::GetStream(uint_t stream_id,
					    IStream::StreamType type)
{
    uint_t match = 0;
    std::vector<AviReadStream*>::iterator it;
    for (it = m_Streams.begin(); it != m_Streams.end(); it++)
	if ((*it)->GetType() == type)
	{
	    if (match == stream_id)
		return (*it);
	    match++;
	}
    return 0;
}

uint_t AviReadHandler::GetStreamCount(IStream::StreamType type)
{
    uint_t cnt = 0;
    std::vector<AviReadStream*>::const_iterator it;
    for (it = m_Streams.begin(); it != m_Streams.end(); it++)
	if ((*it)->GetType() == type)
            cnt++;

    return cnt;
}

void AviReadHandler::readInfoChunk(uint_t data_size)
{
    uint_t rs = 0;
    while (rs < data_size && !m_Input.eof())
    {
	uint32_t type = m_Input.readDword();
	uint32_t size = m_Input.readDword();	
	if ((rs + size) < data_size)
	{
	    size += (size & 1);
	    char * b = new char[size + 10];
	    m_Input.read(b, size);
		
	    for (const RiffInfoChunk* pr = infos; pr->riff; pr++)
		if (pr->riff == type)
		{
		    avm_printf("AVI reader", "InfoChunk %s:  %s\n", pr->text, b);
		    break;
		}
		delete []b;
	}
	rs += size;
    }
}

int AviReadHandler::readAVIMainHeader(uint_t data_size)
{
//    checkInt(ckidAVIMAINHDR);//avih
//    uint_t data_size=m_Input.readInt();
    data_size += (data_size & 1);
    uint_t read_size = sizeof(AVIMainHeader);

    memset(&m_MainHeader, 0, read_size);
    if (data_size > read_size)
	avm_printf("AVI reader", "WARNING: unexpected main header size\n");
    else
	read_size = data_size;

    m_Input.read(&m_MainHeader, read_size);
    if (read_size < data_size)
        m_Input.seekCur(data_size - read_size);

    avm_get_leAVIMainHeader(&m_MainHeader);
    PrintAVIMainHeader(&m_MainHeader);

    return data_size + 8;
}

int AviReadHandler::readAVIStreamHeader()
{
    if (m_Input.readDword() != ckidSTREAMHEADER)
	return -1;//strh

    AVIStreamHeader ash;
    uint_t read_size = sizeof(ash);
    memset(&ash, 0, read_size);

    uint_t data_size = m_Input.readDword();
    data_size += (data_size & 1);
    if (data_size > read_size)
	avm_printf("AVI reader", "WARNING: unexpected stream header size (%d)\n", data_size);
    else
        read_size = data_size;

    m_Input.read(&ash, read_size);
    if (read_size < data_size)
        m_Input.seekCur(data_size - read_size);

    avm_get_leAVIStreamHeader(&ash);
    //if (m_Header.dwStart && m_Header.dwStart)
    //    cout << "Calc starttime " << m_Header.dwStart * (m_Header.dwRate / (double)m_Header.dwStart) << endl;

    if (m_Input.readDword() != ckidSTREAMFORMAT)
	return -1;//strf

    data_size = m_Input.readDword();
    uint_t fsize = data_size;
    data_size += (data_size & 1);

    char* format = new char[data_size];
    m_Input.read(format, data_size);

    if (ash.fccType == streamtypeVIDEO)
    {
	const uint_t bihs = sizeof(BITMAPINFOHEADER);
        BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*) format;
	if (data_size < bihs)
	{
	    char *t = new char[bihs];
	    memset(t, 0, bihs);
	    memcpy(t, format, data_size);
	    delete[] format;
	    format = t;
            fsize = data_size = bihs;
	}

	avm_get_leBITMAPINFOHEADER(bih);
	//avm_printf("AVI reader", "format %x  %.4s  %d\n",
	//       s.m_Header.fccHandler, (char*)&s.m_Header.fccHandler, data_size);
	if (bih->biCompression != ash.fccHandler)
	{
	    avm_printf("AVI Reader", "Info: fccHandler differs from biCompression!\n");
	    ash.fccHandler = bih->biCompression;
	}
	ash.dwSampleSize = 0;
    }
    else if (ash.fccType == streamtypeAUDIO)
    {
	uint_t wfs = sizeof(WAVEFORMATEX) + ((WAVEFORMATEX*)format)->cbSize;
	if (data_size < wfs)
	{
	    char *t = new char[wfs];
	    memcpy(t, format, data_size);
	    memset(t + data_size, 0, wfs - data_size);
	    delete[] format;
	    format = t;
	    fsize = data_size = wfs;
	}
        avm_get_leWAVEFORMATEX((WAVEFORMATEX*) format);
	if (!ash.dwSampleSize && ash.dwLength > 800000)
	{
	    // ((*(ashort*)s.m_pcFormat == WAVE_FORMAT_MPEGLAYER3)
	    // only if we have a lot of chunks
	    // if the stream would be VBR then ther would be far
	    // less chunks
	    avm_printf("AVI reader", "WARNING: setting SampleSize=1 (Length was %d)\n", ash.dwLength);
	    ash.dwSampleSize = 1;
	}
    }

#if 0
    if (ash.dwScale > 100)
        ash.dwScale = 9600;
    avm_printf("AVI reader", "out rate: %d    scale: %d\n", ash.dwRate, ash.dwScale);
#endif

    uint_t id = m_Streams.size();
    PrintAVIStreamHeader(&ash);
    m_Streams.push_back(new AviReadStream(this, ash, id, format, fsize));
    delete[] format;
    return 0;
}

int AviReadHandler::readIndexChunk(uint_t index_size, uint_t movie_chunk_offset)
{
    bool zero_based_offsets = false;

    //for (unsigned i = 0; i < m_Streams.size(); i++)
    //    positions[i] = m_Streams[i]->m_Header.dwStart;

    avm_printf("AVI reader", "Reading index from offset: %d\n", m_Input.pos());

    for (unsigned ip = 0; ip <= (index_size/sizeof(AVIINDEXENTRY))
	 && !m_Input.eof(); ip++)
    {
	uint_t ckid = m_Input.readDword();
	uint_t cflags = m_Input.readDword();
	uint_t coffset = m_Input.readDword();
	uint_t clen = m_Input.readDword();

	uint_t id = StreamFromFOURCC(ckid);
	if (id >= m_Streams.size())
	    continue;
	AviReadStream* stream = m_Streams[id];

	//printf ("Id  %u    off: %u   len:%u\n", ip, entry.dwChunkOffset, entry.dwChunkLength);
	if (clen > StreamPacket::MAX_PACKET_SIZE)
	{
	    avm_printf("AVI reader", "WARNING: invalid index entry %u -- id: %u  offset: %u size:  %u (pos: %u)\n",
		      ip, ckid, coffset, clen, m_Input.pos());
	    return reconstructIndexChunk(stream->m_Offsets.back() & ~1);
	}

	if (coffset < movie_chunk_offset)
	    zero_based_offsets = true;

	if (zero_based_offsets)
	    coffset += movie_chunk_offset - 4;

	stream->addChunk(coffset, clen, cflags & AVIIF_KEYFRAME);
    }
    //avm_printf("AVI reader", "Avi Index entries: %d\n", m_Streams[0].m_Index.size());
    //kprof_reset_stats();
    return 0;
}

int AviReadHandler::reconstructIndexChunk(uint_t offset)
{
    m_Input.seek(offset);
    int64_t * positions = new int64_t[m_Streams.size()];
    int * isdivx = new int[m_Streams.size()];

    avm_printf("AVI reader", "Reconstructing index from offset: %d\n", offset);
    // fixing KeyFrames for DivX movies by checking flag bit
    for (unsigned i = 0; i < m_Streams.size(); i++)
    {
	isdivx[i] = 0;

	if (m_Streams[i]->GetType() == IStream::Video)
	    switch (m_Streams[i]->m_Header.fccHandler)
	    {
	    case fccDIV3:
	    case fccdiv3:
	    case fccDIV4:
	    case fccdiv4:
	    case fccDIV5:
	    case fccdiv5:
	    case fccDIV6:
	    case fccdiv6:
	    case fccMP43:
	    case fccmp43:
	    case fccMP42:
	    case fccmp42:
	    case fccAP41:
		isdivx[i] = 1;
                break;
	    case fccDX50:
	    case fccdx50:
	    case fccDIVX:
	    case fccdivx:
	    case fccDIV1:
	    case fccdiv1:
	    case fccMP4S:
	    case fccmp4s:
	    case fccXVID:
	    case 0x4:
		isdivx[i] = 2;
                break;
	    case fccWMV1:
	    case fccwmv1:
		isdivx[i] = 3;
                break;
	    }
    }

    // set when trying to reconstruct even very broken .avi files
    bool seriouslyBroken = false;
    while (!m_Input.eof())
    {
	uint_t ckid = m_Input.readDword();
	uint_t id = StreamFromFOURCC(ckid);
	//uint_t ckidt = TWOCCFromFOURCC(ckid);

	if (ckid == ckidAVINEWINDEX)
            break;

	//printf("pos: %llu   %u   %u    %.4s\n", m_Input.pos(), id, m_Streams.size(), (char*)&ckid);
	if (id >= m_Streams.size())
	{
	    //printf("index type  0x%x   0x%x", (ckid & 0xffff), cktypeINDEX);
	    if ((ckid & 0xffff) == cktypeINDEX)
	    {
                // skip indexes
		uint32_t size = m_Input.readDword();
		//printf("SKEEEEEP %d\n", size);
		m_Input.seekCur(size);
		continue;
	    }

	    if (m_Input.eof())
		break;
	    m_Input.seekCur(-2);
            if (!seriouslyBroken)
		avm_printf("AVI reader", "Trying to reconstruct broken avi stream (could take a lot of time)\n");
	    seriouslyBroken = true;
	    continue;
	}

	AviReadStream* stream = m_Streams[id];
	uint_t coffset = m_Input.pos() - 4;
	uint_t clen = m_Input.readDword();
	if (clen > ((seriouslyBroken) ? MAX_CHUNK_SIZE : 0xfffffffU))
	    continue;

	//printf ("Id  %u    ps: %d  len:%u   0x%x\n", id, coffset, clen, ckid);

	uint_t size = clen + (clen & 1); // round
	bool keyframe = (size > 0);

	if (isdivx[id] && size)
	{
	    uint_t a = m_Input.readDword();
	    switch (isdivx[id])
	    {
	    case 1:
		if (a & 0x40)
		    keyframe = false;
		break;
	    case 2:
		// DivX4 keyframe detection
		//avm_printf("AVI reader", "Dword 0x%x\n", a);
		if (a == 0xb6010000 || a == 0xb0010000)
		{
		    // looks like new DivX4/5 frame
                    int b = m_Input.readByte();
		    keyframe = ((b & 0xc0) == 0);
		    // 0x00 Key frame,  0x40 P-frame
		    //printf("Byte 0x%x  %d", b, b);
		    // for b001 it gives false keyframes -
		    // parser from ffmpeg will be needed
		    // to do a better job :(...
		    size--;
		}
		//else if (a == 0xb0010000) // old OpenDivX
		//    keyframe = true;
		else if (a != 0x00010000) // old OpenDivX
		    keyframe = false;
		break;
	    case 3:
		if (!(a & 0x1))
		    keyframe = false;
		//printf("keyframe %d  %d\n", keyframe, size);
		break;
	    }
	    size -= 4;
	}

	m_Input.seekCur(size);
	if (m_Input.pos() > int64_t(coffset + size))
	    // do not insert incomplete chunks
	    stream->addChunk(coffset, clen, keyframe);
    }
	delete [] isdivx;
	delete [] positions;
    return 0;
}

#if 0
AviStreamPacket* AviReadHandler::readPacket(bool fill)
{
    int ckid = m_Input.readDword();
    uint_t id = StreamFromFOURCC(ckid);
    uint_t ckidt = TWOCCFromFOURCC(ckid);

    int len = m_Input.readDword();
    if (fill)
    {
    }
    return 0;
}
#endif

void AviReadHandler::PrintAVIMainHeader(const AVIMainHeader* h)
{
    char buffer[200];
    avm_printf("AVI reader", "MainHeader: MicroSecPerFrame=%d MaxBytesPerSec=%d\n"
	      " PaddingGranularity=%d Flags=[%s] TotalFrames=%d\n"
	      " InitialFrames=%d Streams=%d SuggestedBufferSize=%d WxH=%dx%d\n"
	      " Scale=%d Rate=%d Start=%d Length=%d\n",
	      h->dwMicroSecPerFrame, h->dwMaxBytesPerSec,

	      h->dwPaddingGranularity, GetAviFlags(buffer, h->dwFlags),
	      h->dwTotalFrames,

	      h->dwInitialFrames, h->dwStreams,
	      h->dwSuggestedBufferSize, h->dwWidth, h->dwHeight,
	      h->dwScale, h->dwRate, h->dwStart, h->dwLength);
}

void AviReadHandler::PrintAVIStreamHeader(const AVIStreamHeader* h)
{
    char buffer[200];

    char sb[10];
    if (h->fccType == streamtypeVIDEO)
    {
	avm_set_le32(sb, h->fccHandler);
	sb[4] = 0;
    }
    else
	sprintf(sb, "0x%x", h->fccHandler);
    char ft[4];
    avm_printf("AVI reader", "StreamHeader: Type=%.4s Handler=%s Flags=[%s]\n"
	   " InitialFrames=%d Scale=%d Rate=%d Start=%d Length=%d\n"
	   " SuggestedBufferSize=%d Quality=%d SampleSize=%d"
	   " Rect l,r,t,b=%d,%d,%d,%d\n",
	   avm_set_le32(ft, h->fccType), sb,

	   GetAviFlags(buffer, h->dwFlags),
	   h->dwInitialFrames, h->dwScale,
	   h->dwRate, h->dwStart, h->dwLength,

	   h->dwSuggestedBufferSize, h->dwQuality, h->dwSampleSize,
	   h->rcFrame.left, h->rcFrame.right, h->rcFrame.top,
	   h->rcFrame.bottom);
}

char* AviReadHandler::GetAviFlags(char* buffer, uint_t flags)
{
    sprintf(buffer, "%s%s%s%s%s%s ",
	    (flags & AVIF_HASINDEX) ? " HAS_INDEX" : "",
	    (flags & AVIF_MUSTUSEINDEX) ? " MUST_USE_INDEX" : "",
	    (flags & AVIF_ISINTERLEAVED) ? " IS_INTERLEAVED" : "",
            (flags & AVIF_TRUSTCKTYPE) ? " TRUST_CKTYPE" : "",
	    (flags & AVIF_COPYRIGHTED) ? " COPYRIGHTED" : "",
	    (flags & AVIF_WASCAPTUREFILE) ? " WAS_CAPTURED_FILE" : ""
	   );
    return buffer;
}

AviReadHandler* CreateAviReadHandler(const char *pszFile, unsigned int flags)
{
    AviReadHandler* r = new AviReadHandler(flags);
    if (r->init(pszFile) == 0)
	return r;

    delete r;
    return 0;
}

AVM_END_NAMESPACE;
