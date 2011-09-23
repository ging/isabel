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
#ifndef AVIFILE_AVIFMT_H
#define AVIFILE_AVIFMT_H

/****************************************************************************
 *
 *  AVIFMT - AVI file format definitions
 *
 ****************************************************************************/

#include "utils.h"

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)

#define LOBYTE(w)              ((uint8_t)(uint16_t)(w))
#define HIBYTE(w)              ((uint8_t)((uint16_t)(w) >> 8))

#define LOWORD(l)              ((uint16_t)(uint32_t)(l))
#define HIWORD(l)              ((uint16_t)((uint32_t)(l) >> 16))

#define MAKELONG(low,high)     ((int32_t)(((uint16_t)(low)) | (((uint32_t)((uint16_t)(high))) << 16)))

typedef struct __attribute__((__packed__))
{
    int16_t	left;
    int16_t	top;
    int16_t	right;
    int16_t	bottom;
} RECT, *PRECT, *LPRECT;
#endif
#ifdef _MSC_VER
#pragma warning(disable:4200)
#endif

/* The following is a short description of the AVI file format.
 * Please see the accompanying documentation for a full explanation.
 *
 * An AVI file is the following RIFF form:
 *
 *	RIFF('AVI'
 *	      LIST('hdrl'
 *		    avih(<AVIMainHeader>)
 *			LIST ('strl'
 *			strh(<Stream header>)
 *			strf(<Stream format>)
 *			[junk(data)]
 *			... additional header data
 *		LIST('movi'
 *			{ LIST('rec'
 *			       SubChunk...
 *			       )
 *				| SubChunk } ....
 *	          )
 *	      [ <AVIIndex> ]
 *	    )
 *
 *	The main file header specifies how many streams are present.  For
 *	each one, there must be a stream header chunk and a stream format
 *	chunk, enlosed in a 'strl' LIST chunk.  The 'strf' chunk contains
 *	type-specific format information; for a video stream, this should
 *	be a BITMAPINFO structure, including palette.  For an audio stream,
 *	this should be a WAVEFORMAT (or PCMWAVEFORMAT) structure.
 *
 *	The actual data is contained in subchunks within the 'movi' LIST
 *	chunk.  The first two characters of each data chunk are the
 *	stream number with which that data is associated.
 *
 *	Some defined chunk types:
 *           Video Streams:
 *                  ##db:	RGB DIB bits
 *                  ##dc:	RLE8 compressed DIB bits
 *                  ##pc:	Palette Change
 *
 *           Audio Streams:
 *                  ##wb:	waveform audio bytes
 *
 * The grouping into LIST 'rec' chunks implies only that the contents of
 * the chunk should be read into memory at the same time.  This grouping
 * is used for files specifically intended to be played from CD-ROM.
 *
 * The index chunk at the end of the file should contain one entry for
 * each data chunk in the file.
 *
 * Limitations for the current software:
 *	Only one video stream and one audio stream are allowed.
 *	The streams must start at the beginning of the file.
 *
 *
 * To register codec types please obtain a copy of the Multimedia
 * Developer Registration Kit from:
 *
 *  Microsoft Corporation
 *  Multimedia Systems Group
 *  Product Marketing
 *  One Microsoft Way
 *  Redmond, WA 98052-6399
 *
 */

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 ) \
    ( (int)(unsigned char)(ch0) | ( (int)(unsigned char)(ch1) << 8 ) | \
    ( (int)(unsigned char)(ch2) << 16 ) | ( (int)(unsigned char)(ch3) << 24 ) )
#endif

/* Macro to make a TWOCC out of two characters */
#ifndef aviTWOCC
#define aviTWOCC(ch0, ch1) ((unsigned short)(unsigned char)(ch0) | ((unsigned short)(unsigned char)(ch1) << 8))
#endif

//typedef uint16_t TWOCC;

/* form types, list types, and chunk types */
#define formtypeAVI             mmioFOURCC('A', 'V', 'I', ' ')

#define listtypeAVIHEADER       mmioFOURCC('h', 'd', 'r', 'l')
#define listtypeSTREAMHEADER    mmioFOURCC('s', 't', 'r', 'l')
#define listtypeAVIMOVIE	mmioFOURCC('m', 'o', 'v', 'i')
#define listtypeAVIRECORD	mmioFOURCC('r', 'e', 'c', ' ')
#define listtypeINFO		mmioFOURCC('I', 'N', 'F', 'O')

#define ckidAVIMAINHDR          mmioFOURCC('a', 'v', 'i', 'h')
#define ckidSTREAMHEADER        mmioFOURCC('s', 't', 'r', 'h')
#define ckidSTREAMFORMAT        mmioFOURCC('s', 't', 'r', 'f')
#define ckidSTREAMHANDLERDATA   mmioFOURCC('s', 't', 'r', 'd')
#define ckidSTREAMNAME		mmioFOURCC('s', 't', 'r', 'n')
#define ckidAVINEWINDEX		mmioFOURCC('i', 'd', 'x', '1')
/* Chunk id to use for extra chunks for padding. */
#define ckidAVIPADDING		mmioFOURCC('J', 'U', 'N', 'K')

/*
 ** Stream types for the <fccType> field of the stream header.
 */
#define streamtypeVIDEO		mmioFOURCC('v', 'i', 'd', 's')
#define streamtypeAUDIO		mmioFOURCC('a', 'u', 'd', 's')
#define streamtypeMIDI		mmioFOURCC('m', 'i', 'd', 's')
#define streamtypeTEXT		mmioFOURCC('t', 'x', 't', 's')

/* Basic chunk types */
#define cktypeDIBbits		aviTWOCC('d', 'b')
#define cktypeDIBcompressed	aviTWOCC('d', 'c')
#define cktypePALchange		aviTWOCC('p', 'c')
#define cktypeWAVEbytes		aviTWOCC('w', 'b')
#define cktypeINDEX		aviTWOCC('i', 'x')


/*
 ** Useful macros
 **
 ** Warning: These are nasty macro, and MS C 6.0 compiles some of them
 ** incorrectly if optimizations are on.  Ack.
 */

/* Macro to get stream number out of a FOURCC ckid */
#define FromHex(n)	(((n) >= 'A') ? ((n) + 10 - 'A') : ((n) - '0'))
#define StreamFromFOURCC(fcc) \
    ((uint16_t) ((FromHex(LOBYTE(LOWORD(fcc))) << 4) + \
		  (FromHex(HIBYTE(LOWORD(fcc))))))

/* Macro to get TWOCC chunk type out of a FOURCC ckid */
#define TWOCCFromFOURCC(fcc)    HIWORD(fcc)

/* Macro to make a ckid for a chunk out of a TWOCC and a stream number
 * from 0-255.
 */
#define ToHex(n)	((uint8_t) (((n) > 9) ? ((n) - 10 + 'A') : ((n) + '0')))
#define MAKEAVICKID(tcc, stream) \
    MAKELONG((ToHex((stream) & 0x0f) << 8) | (ToHex(((stream) & 0xf0) >> 4)), tcc)

/*
 * Main AVI File Header
 */

/* flags for use in <dwFlags> in AVIFileHdr */
#define AVIF_HASINDEX		0x00000010	// Index at end of file?
#define AVIF_MUSTUSEINDEX	0x00000020
#define AVIF_ISINTERLEAVED	0x00000100
#define AVIF_TRUSTCKTYPE	0x00000800	// Use CKType to find key frames?
#define AVIF_WASCAPTUREFILE	0x00010000
#define AVIF_COPYRIGHTED	0x00020000

/* The AVI File Header LIST chunk should be padded to this size */
#define AVI_HEADERSIZE  2048                    // size of AVI header list

typedef struct __attribute__((__packed__))
{
    uint32_t	dwMicroSecPerFrame;	// frame display rate (or 0L)
    uint32_t	dwMaxBytesPerSec;	// max. transfer rate
    uint32_t	dwPaddingGranularity;	// pad to multiples of this
					// size; normally 2K.
    uint32_t	dwFlags;		// the ever-present flags
    uint32_t	dwTotalFrames;		// # frames in file
    uint32_t	dwInitialFrames;
    uint32_t	dwStreams;
    uint32_t	dwSuggestedBufferSize;

    uint32_t	dwWidth;
    uint32_t	dwHeight;

    uint32_t	dwScale;
    uint32_t	dwRate;
    uint32_t	dwStart;
    uint32_t	dwLength;
} AVIMainHeader;

/*
 * Stream header
 */

#define AVISF_DISABLED			0x00000001
#define AVISF_VIDEO_PALCHANGES		0x00010000

typedef struct __attribute__((__packed__))
{
    uint32_t	fccType;
    uint32_t	fccHandler;
    uint32_t	dwFlags;	/* Contains AVISF_* flags */
    uint16_t	wPriority;	/* dwPriority - splited for audio */
    uint16_t	wLanguage;
    uint32_t	dwInitialFrames;
    uint32_t	dwScale;
    uint32_t	dwRate;		/* dwRate / dwScale == samples/second */
    uint32_t	dwStart;
    uint32_t	dwLength;	/* In units above... */
    uint32_t	dwSuggestedBufferSize;
    int32_t	dwQuality;
    uint32_t	dwSampleSize;
    RECT	rcFrame;
} AVIStreamHeader;

/* Flags for index */
#define AVIIF_LIST	0x00000001L // chunk is a 'LIST'
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.

#define AVIIF_NOTIME	0x00000100L // this frame doesn't take any time
#define AVIIF_COMPUSE	0x0FFF0000L // these bits are for compressor use

#define FOURCC_RIFF	mmioFOURCC('R', 'I', 'F', 'F')
#define FOURCC_LIST	mmioFOURCC('L', 'I', 'S', 'T')


// The following comes from the OpenDML 1.0 spec for extended AVI files

// bIndexType codes
//

#define AVI_INDEX_OF_INDEXES 0x00	// when each entry in aIndex
// array points to an index chunk

#define AVI_INDEX_OF_CHUNKS 0x01	// when each entry in aIndex
// array points to a chunk in the file

#define AVI_INDEX_IS_DATA	0x80	// when each entry is aIndex is
// really the data

// bIndexSubtype codes for INDEX_OF_CHUNKS

#define AVI_INDEX_2FIELD	0x01	// when fields within frames

typedef struct __attribute__((__packed__))
{
    uint32_t	ckid;
    uint32_t	dwFlags;
    uint32_t	dwChunkOffset;		// Position of chunk
    uint32_t	dwChunkLength;		// Length of chunk
} AVIINDEXENTRY;

typedef struct __attribute__((__packed__))
{
    uint64_t	qwOffset;		// absolute file offset, offset 0 is
    // unused entry??
    uint32_t	dwSize;			// size of index chunk at this offset
    uint32_t	dwDuration;		// time span in stream ticks
} avisuperindexentry;

typedef struct __attribute__((__packed__))
{
    uint32_t	fccType;
    uint32_t	cb;
    uint16_t	wLongsPerEntry;		// must be 4 (size of each entry in aIndex array)
    uint8_t	bIndexSubType;		// must be 0 or AVI_INDEX_2FIELD
    uint8_t	bIndexType;		// must be AVI_INDEX_OF_INDEXES
    uint32_t	nEntriesInUse;		// number of entries in aIndex array that
    // are used
    uint32_t	dwChunkId;		// ’##dc’ or ’##db’ or ’##wb’, etc
    uint32_t	dwReserved[3];		// must be 0
    avisuperindexentry* aIndex;

    uint32_t	dwChunkOffset;		// Position of chunk
    uint32_t	dwChunkLength;		// Length of chunk
} AVISUPERINDEX;

typedef struct __attribute__((__packed__))
{
    int32_t	dwOffset;		// qwBaseOffset + this is absolute file offset
    int32_t	dwSize;			// bit 31 is set if this is NOT a keyframe
} avistdindexentry;

typedef struct __attribute__((__packed__))
{
    uint32_t	fcc;					// ’ix##’
    uint32_t	cb;
    uint16_t	wLongsPerEntry;		// must be sizeof(aIndex[0])/sizeof(DWORD)
    uint8_t	bIndexSubType;		// must be 0
    uint8_t	bIndexType;		// must be AVI_INDEX_OF_CHUNKS
    uint32_t	nEntriesInUse;		//
    uint32_t	dwChunkId;		// ’##dc’ or ’##db’ or ’##wb’ etc..
    uint64_t	qwBaseOffset;		// all dwOffsets in aIndex array are
    // relative to this
    uint64_t	dwReserved3;		// must be 0
    avistdindexentry* aIndex;
} AVISTDINDEX;

#define AVISTREAMREAD_CONVENIENT	(-1L)

/*
 * Palette change chunk
 *
 * Used in video streams.
 */

#endif /* AVIFILE_AVIFMT_H */
