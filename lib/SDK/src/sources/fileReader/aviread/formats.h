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
#ifndef AVIFILE_FORMATS_H
#define AVIFILE_FORMATS_H

#ifdef __BUILD_FOR_WINXP
#include <windows.h>
#include <wingdi.h>
#include <mmsystem.h>
#else
#include "avm_default.h"	/* to get the HAVE_xxx_H defines */

#ifndef GUID_TYPE
#define GUID_TYPE
/**
 * GUID is unique 16-byte indentifier
 *
 * Usualy being shown this way:
 * AAAAAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE
 *
 * DDDD however is not uint16 but two uint8
 * This representation allows to use it in the natural
 * human readable way
 */
typedef struct __attribute__((__packed__))
{
    uint32_t	f1;
    uint16_t	f2;
    uint16_t	f3;
    uint8_t	f4[8];
} GUID;
#endif /* !GUID_TYPE */

enum BI_FMT {
    BI_RGB = 0,
    BI_RLE8 = 1,
    BI_RLE4 = 2,
    BI_BITFIELDS = 3
    //BI_JPEG = XXXX
};

#ifndef _BITMAPINFOHEADER_
#define _BITMAPINFOHEADER_


/**
 *  The BITMAPINFOHEADER contains all the details about the video stream such
 * as the actual image dimensions and their pixel depth. A source filter may
 * also request that the sink take only a section of the video by providing a
 * clipping rectangle in rcSource. In the worst case where the sink filter
 * forgets to check this on connection it will simply render the whole thing
 * which isn't a disaster. Ideally a sink filter will check the rcSource and
 * if it doesn't support image extraction and the rectangle is not empty then
 * it will reject the connection. A filter should use SetRectEmpty to reset a
 * rectangle to all zeroes (and IsRectEmpty to later check the rectangle).
 * The rcTarget specifies the destination rectangle for the video, for most
 * source filters they will set this to all zeroes, a downstream filter may
 * request that the video be placed in a particular area of the buffers it
 * supplies.
 */
typedef struct //__attribute__((__packed__))
{
    /** Specifies the number of bytes required by the structure. */
    uint32_t	biSize;
    /** Specifies the width of bitmap, in pixels. */
    uint32_t	biWidth;
    /**
     * Specifies the height of bitmap, in pixels.
     *
     * For RGB surfaces - negative orientation means top-down image
     * for all other codecs the sign is meangless and should be ignored
     * the FOURCC code uniquely identifies the compression and orientation
     * It is not valid to describe orientation with the sign of biHeight
     *
     * Common YUV format such as YV12, YUY2 are top-down oriented.
     * The sign of biHeight for such formats must always be set positive
     * by driver producing such formats and the sign must be ignored by any
     * driver receiving such formats. For proprietary compression formats with
     * an associated FOURCC, any orientation is acceptable, but must be
     * always the same for all bitmaps of the FOURCC.
     */
    int32_t	biHeight;
    /** Specifies the number of planes for targete device. (Must be 1) */
    uint16_t	biPlanes;
    /**
     * Specifies the number of bits-per-pixel.
     *
     * - 0
     *		The number is specified or is implied by the JPEG or PNG
     *
     * - 1
     *		The bitmap is monochrome and the bmiColors member contains
     *          two entries. Each bit in the bitmap array represents a pixel.
     *          If the bit is clear, the pixel is displayed with the color
     *          of the first entry in the bmiColors table; if thebit is set,
     *          the pixel has the color of the second in the table.
     *
     * - 4
     *		The bitmap has a maximum of 16 colors and the bmiColors member
     *          contains up to 16 entries. Each pixel in the bitmap is represented
     *          by a 4-bit index into the color table. For example, if the
     *          first pixel contains the color in the second table entry, and
     *          the second pixel constains the color in the sixteenth table entry
     *
     * - 8
     *		The bitmap has a maximum of 256 color, and the bmiColors member
     *          constains up to 256 entries. In this case, each byt in the
     *          array represents a single pixel.
     *
     * - 16
     *		The bitmap has a maximum of 2^16 color. If the biCompression member
     *          of the BITMPAINFOHEADER is BI_RGB, the bmiColors member is NULL.
     *          Each WORD in the bitmap array represents a single pixel.
     *          The relative intensities of red, green and blue are represented
     *          with five bits for each color component 5-5-5.
     *          When the biCompression member is BI_BITFIELDS, the system supports
     *          only the following 16bpp color masks: A 5-5-5 16bit image,
     *          (b:0x001f, g:0x03e0, r:0x7c00) and 5-6-5 16bit image
     *          (b:0x001f  g:0x07e0  r:0xf800)
     *
     * - 24
     *		Bitmap has a maximum of 2^24 colors, and the bmiColors member
     *          is NULL, Each 3-byte triplet in the bitmap array represents
     *          the realtive intensities of blue, green, and red, respecitively,
     *          for each a pix. The bmiColors color table is used for optimizing
     *          colors used on palette-based devices, and must contain the number
     *          of entries specified by the biClrUsed member of BITMAPINFOHEADER.
     *
     * - 32
     *		The bitmap has a maximum of 2^32. If the biCompression member
     *          of the BITMPAINFOHEADER is BI_RGB, the bmiColors member is NULL.
     *          Each DWORD in the bitmap array represents the relative intensities
     *          of the blue, green and read, respectively, for a pixel. The high
     *          byte in each DWORD is not used. The bmiColors color table is used
     *          for optimizing colors used on palette-based devices,
     *          and must constain the number of entries specified by the biClrUsed
     *          member of the BITMAPINFOHEADER.
     *
     * \todo append remaining info here
     */
    uint16_t	biBitCount;
    /**
     * Specifies the compression format
     *
     * - BI_RGB
     *		An uncompressed format.
     *
     * - BI_RLE8
     *		A run-length encoded (RLE) format for bitmaps with 8bpp.
     *          Tge cinoressuib format is a 2-byt format consisting of a count
     *          byte followed by a byte containg a color index.
     *
     * - BI_RLE4
     *		An RLE format for bitmaps with 4bpp. The compression format
     *          is a 2-byte format consisting of a count byte followed
     *          by two word-length color indexes.
     *
     * - BI_BITFIELDS
     *          Specifies that the bitmap is not compressed and that the
     *          color table consists of three DWORD color masks that
     *          specify the red, green, and vlue components, respectively,
     *          of each pixel. This is valid when used with 16- and
     *          32-bpp bitmaps.
     *
     * - BI_JPEG
     *          Image is a JPEG image
     *
     * - BI_PNG
     *          Image is PNG image
     */
    uint32_t	biCompression;
    /**
     * Specifies the size, in bytes, of the image. This may be set to zero
     * for BI_RGB bitmaps.
     */
    uint32_t	biSizeImage;
    /** Specifies the horizontal resolution, in pixels-pre-meter */
    uint32_t	biXPelsPerMeter;
    /** Specifies the vertical resolution, in pixels-pre-meter */
    uint32_t	biYPelsPerMeter;
    /**
     * Specifies the number of color indexes in the color table that
     * are actually used by the bitmap. If this value is zero, the bitmap
     * uses the maximum number of colors corresponding to the value of
     * the biBitCount member for the compression mode specified by
     * biCompression.
     *
     * If biClrUsed is nonzero and the biBitCount member is less then 16,
     * the biClrUsed member specifies the actual number of colors the graphics
     * engine or device driver accesses. If the biBitCount is 16 or greater,
     * the biClrUsed member specifies the size of the color table used to
     * optimize performance of the system color palettes. If biBitCount
     * equals 16 or 32, the optimal color palette starts immediately
     * following the free DWORD mask.
     *
     * When the bitmap array immediatelly follos the BITMAPINFO structure,
     * it is a packed bitmap. Packed bitmaps are referenced by a single
     * pointer. Packed bitmaps require that the biClrUsed member
     * must be either zero or the actual size of the color table.
     */
    uint32_t	biClrUsed;
    /**
     * Specifies the number of color indexes that are required for the displaying
     * the bitmap. If this value is zero, all colors are required.
     */
    uint32_t	biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;

typedef struct __attribute__((__packed__))
{
	BITMAPINFOHEADER bmiHeader;
	uint32_t bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO;

#endif /* _BITMAPINFOHEADER */


#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct __attribute__((__packed__)) _WAVEFORMATEX
{
    /**
     * Waveform-audio format type. Format tags are registred with
     * Microsoft Corporation for many compression algorithms.
     */
    uint16_t	wFormatTag; // using uint - less problems
    /** Number of channels in the waveformat audio data. */
    uint16_t	nChannels;
    /**
     * Sample rate, in samples per seconds (Hertz), that each channel
     * should be played or recorded.
     */
    uint32_t	nSamplesPerSec;
    /**
     * Required average data-transfer rate, in bytes per second, for
     * format tag. For PCM it should be nAvgBytesPerSec * nBlockAlign
     */
    uint32_t	nAvgBytesPerSec;
    /**
     * Block alignment in bytes. The block alignment is the minimum
     * atomic using of the data for the wFormatTag format type.
     * For PCM it should be nChannels * wBitsPerPixel/8.
     * For Mp3 this is 1 because of the Windows (there is no logic in this)
     */
    uint16_t	nBlockAlign;
    /**
     * Bits per sample for the wFormatTag format type. If wFormatTag
     * is WAVE_FORMAT_PCM then wBitsPerSample should be equal to 8 or 16.
     * Note that some compression schemes cannot define a value for
     * wBitsPerSample, so this member can be zero.
     */
    uint16_t	wBitsPerSample;
    /**
     * Size, in bytes, of extra format information appended to the end
     * of the WAVEFORMATEX structure. This information can be used by
     * non-PCM formats to store extra attributes for the wFormatTag.
     * If no extra information od required by the wFormatTag, this member
     * must be set to zero. For PCM format this member is ignored.
     */
    uint16_t	cbSize;
} WAVEFORMATEX;

typedef WAVEFORMATEX *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;
#endif /* !_WAVEFORMATEX_ */

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
/**
 *
 */
typedef struct __attribute__((__packed__))
{
    WAVEFORMATEX    Format;
    union {
	uint16_t wValidBitsPerSample;	/**< bits of precision  */
	uint16_t wSamplesPerBlock;	/**< valid if wBitsPerSample==0 */
	uint16_t wReserved; 		/** If neither applies, set to zero. */
    } Samples;
    uint32_t dwChannelMask;		/** which channels are
    					 * present in stream  */
    GUID	SubFormat;
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
#endif /* !_WAVEFORMATEXTENSIBLE_ */

typedef WAVEFORMATEXTENSIBLE	WAVEFORMATPCMEX; /* Format.cbSize = 22 */

#endif /* !WIN32 */

/** WAVE format with extended structure which describes mp3 track */
/* here are some flags pertaining to the structure below */
typedef enum
{
    MP3WFMTX_ID_UNKNOWN			= 0,
    MP3WFMTX_ID_MPEG			= 1,
    MP3WFMTX_ID_CONSTANTFRAMESIZE	= 2,

    MP3WFMTX_FLAG_PADDING_ISO		= 0,
    MP3WFMTX_FLAG_PADDING_ON		= 1,
    MP3WFMTX_FLAG_PADDING_OFF		= 2
} MP3WFMTX;

typedef struct// __attribute__((__packed__))
{
    WAVEFORMATEX wf;
    uint16_t wID;
    uint32_t fdwFlags;
    uint16_t nBlockSize;
    uint16_t nFramesPerBlock;
    uint16_t nCodecDelay;
} MP3WAVEFORMATEX; // 30 bytes

/** WAVE form wFormatTag IDs */
#define WAVE_FORMAT_UNKNOWN 0x0000      /**< Microsoft Corporation */
#define WAVE_FORMAT_ADPCM 0x0002	/**< Microsoft Corporation */
#define WAVE_FORMAT_IEEE_FLOAT 0x0003	/**< Microsoft Corporation */
#define WAVE_FORMAT_VSELP 0x0004	/**< Compaq Computer Corp. */
#define WAVE_FORMAT_IBM_CVSD 0x0005	/**< IBM Corporation */
#define WAVE_FORMAT_ALAW 0x0006		/**< Microsoft Corporation */
#define WAVE_FORMAT_MULAW 0x0007	/**< Microsoft Corporation */
#define WAVE_FORMAT_DTS 0x0008		/**< Microsoft Corporation */
#define WAVE_FORMAT_OKI_ADPCM 0x0010	/**< OKI */
#define WAVE_FORMAT_DVI_ADPCM 0x0011	/**< Intel Corporation */
#define WAVE_FORMAT_IMA_ADPCM (WAVE_FORMAT_DVI_ADPCM) /**< Intel Corporation */
#define WAVE_FORMAT_MEDIASPACE_ADPCM 0x0012 /**< Videologic */
#define WAVE_FORMAT_SIERRA_ADPCM 0x0013	/**< Sierra Semiconductor Corp */
#define WAVE_FORMAT_G723_ADPCM 0x0014	/**< Antex Electronics Corporation */
#define WAVE_FORMAT_DIGISTD 0x0015	/**< DSP Solutions, Inc. */
#define WAVE_FORMAT_DIGIFIX 0x0016	/**< DSP Solutions, Inc. */
#define WAVE_FORMAT_DIALOGIC_OKI_ADPCM 0x0017	/**< Dialogic Corporation */
#define WAVE_FORMAT_MEDIAVISION_ADPCM 0x0018	/**< Media Vision, Inc. */
#define WAVE_FORMAT_CU_CODEC 0x0019	/**< Hewlett-Packard Company */
#define WAVE_FORMAT_YAMAHA_ADPCM 0x0020	/**< Yamaha Corporation of America */
#define WAVE_FORMAT_SONARC 0x0021	/**< Speech Compression */
#define WAVE_FORMAT_DSPGROUP_TRUESPEECH 0x0022	/**< DSP Group, Inc */
#define WAVE_FORMAT_ECHOSC1 0x0023	/**< Echo Speech Corporation */
#define WAVE_FORMAT_AUDIOFILE_AF36 0x0024 /**< Virtual Music, Inc. */
#define WAVE_FORMAT_APTX 0x0025		/**< Audio Processing Technology */
#define WAVE_FORMAT_AUDIOFILE_AF10 0x0026 /**< Virtual Music, Inc. */
#define WAVE_FORMAT_PROSODY_1612 0x0027	/**< Aculab plc */
#define WAVE_FORMAT_LRC 0x0028		/**< Merging Technologies S.A. */
#define WAVE_FORMAT_DOLBY_AC2 0x0030	/**< Dolby Laboratories */
#define WAVE_FORMAT_GSM610 0x0031	/**< Microsoft Corporation */
#define WAVE_FORMAT_MSNAUDIO 0x0032	/**< Microsoft Corporation */
#define WAVE_FORMAT_ANTEX_ADPCME 0x0033	/**< Antex Electronics Corporation */
#define WAVE_FORMAT_CONTROL_RES_VQLPC 0x0034 /**< Control Resources Limited */
#define WAVE_FORMAT_DIGIREAL 0x0035	/**< DSP Solutions, Inc. */
#define WAVE_FORMAT_DIGIADPCM 0x0036	/**< DSP Solutions, Inc. */
#define WAVE_FORMAT_CONTROL_RES_CR10 0x0037 /**< Control Resources Limited */
#define WAVE_FORMAT_NMS_VBXADPCM 0x0038	/**< Natural MicroSystems */
#define WAVE_FORMAT_CS_IMAADPCM 0x0039	/**< Crystal Semiconductor IMA ADPCM */
#define WAVE_FORMAT_ECHOSC3 0x003A	/**< Echo Speech Corporation */
#define WAVE_FORMAT_ROCKWELL_ADPCM 0x003B /**< Rockwell International */
#define WAVE_FORMAT_ROCKWELL_DIGITALK 0x003C /**< Rockwell International */
#define WAVE_FORMAT_XEBEC 0x003D	/**< Xebec Multimedia Solutions Limited */
#define WAVE_FORMAT_G721_ADPCM 0x0040	/**< Antex Electronics Corporation */
#define WAVE_FORMAT_G728_CELP 0x0041	/**< Antex Electronics Corporation */
#define WAVE_FORMAT_MSG723 0x0042	/**< Microsoft Corporation */
#define WAVE_FORMAT_MPEG 0x0050		/**< Microsoft Corporation */
#define WAVE_FORMAT_RT24 0x0052		/**< InSoft, Inc. */
#define WAVE_FORMAT_PAC 0x0053		/**< InSoft, Inc. */
#define WAVE_FORMAT_MPEGLAYER3 0x0055	/**< ISO/MPEG Layer3 Format Tag */
#define WAVE_FORMAT_LUCENT_G723 0x0059	/**< Lucent Technologies */
#define WAVE_FORMAT_CIRRUS 0x0060	/**< Cirrus Logic */
#define WAVE_FORMAT_ESPCM 0x0061	/**< ESS Technology */
#define WAVE_FORMAT_VOXWARE 0x0062	/**< Voxware Inc */
#define WAVE_FORMAT_CANOPUS_ATRAC 0x0063 /**< Canopus, co., Ltd. */
#define WAVE_FORMAT_G726_ADPCM 0x0064	/**< APICOM */
#define WAVE_FORMAT_G722_ADPCM 0x0065	/**< APICOM */
#define WAVE_FORMAT_DSAT_DISPLAY 0x0067	/**< Microsoft Corporation */
#define WAVE_FORMAT_VOXWARE_BYTE_ALIGNED 0x0069	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC8 0x0070	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC10 0x0071	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC16 0x0072	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC20 0x0073	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_RT24 0x0074	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_RT29 0x0075	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_RT29HW 0x0076 /**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_VR12 0x0077	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_VR18 0x0078	/**< Voxware Inc */
#define WAVE_FORMAT_VOXWARE_TQ40 0x0079	/**< Voxware Inc */
#define WAVE_FORMAT_SOFTSOUND 0x0080	/**< Softsound, Ltd. */
#define WAVE_FORMAT_VOXWARE_TQ60 0x0081	/**< Voxware Inc */
#define WAVE_FORMAT_MSRT24 0x0082	/**< Microsoft Corporation */
#define WAVE_FORMAT_G729A 0x0083	/**< AT&T Labs, Inc. */
#define WAVE_FORMAT_MVI_MVI2 0x0084	/**< Motion Pixels */
#define WAVE_FORMAT_DF_G726 0x0085	/**< DataFusion Systems (Pty) (Ltd) */
#define WAVE_FORMAT_DF_GSM610 0x0086	/**< DataFusion Systems (Pty) (Ltd) */
#define WAVE_FORMAT_ISIAUDIO 0x0088	/**< Iterated Systems, Inc. */
#define WAVE_FORMAT_ONLIVE 0x0089	/**< OnLive! Technologies, Inc. */
#define WAVE_FORMAT_SBC24 0x0091	/**< Siemens Business Communications Sys */
#define WAVE_FORMAT_DOLBY_AC3_SPDIF 0x0092 /**< Sonic Foundry */
#define WAVE_FORMAT_MEDIASONIC_G723 0x0093 /**< MediaSonic */
#define WAVE_FORMAT_PROSODY_8KBPS 0x0094 /**< Aculab plc */
#define WAVE_FORMAT_ZYXEL_ADPCM 0x0097	/**< ZyXEL Communications, Inc. */
#define WAVE_FORMAT_PHILIPS_LPCBB 0x0098 /**< Philips Speech Processing */
#define WAVE_FORMAT_PACKED 0x0099	/**< Studer Professional Audio AG */
#define WAVE_FORMAT_MALDEN_PHONYTALK 0x00A0 /**< Malden Electronics Ltd. */
#define WAVE_FORMAT_RHETOREX_ADPCM 0x0100 /**< Rhetorex Inc. */
#define WAVE_FORMAT_IRAT 0x0101		/**< BeCubed Software Inc. */
#define WAVE_FORMAT_VIVO_G723 0x0111	/**< Vivo Software */
#define WAVE_FORMAT_VIVO_SIREN 0x0112	/**< Vivo Software */
#define WAVE_FORMAT_DIGITAL_G723 0x0123	/**< Digital Equipment Corporation */
#define WAVE_FORMAT_SANYO_LD_ADPCM 0x0125 /**< Sanyo Electric Co., Ltd. */
#define WAVE_FORMAT_SIPROLAB_ACEPLNET 0x0130	/**< Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_ACELP4800 0x0131	/**< Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_ACELP8V3 0x0132	/**< Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_G729 0x0133	/**< Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_G729A 0x0134	/**< Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_KELVIN 0x0135	/**< Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_G726ADPCM 0x0140	/**< Dictaphone Corporation */
#define WAVE_FORMAT_QUALCOMM_PUREVOICE 0x0150	/**< Qualcomm, Inc. */
#define WAVE_FORMAT_QUALCOMM_HALFRATE 0x0151	/**< Qualcomm, Inc. */
#define WAVE_FORMAT_TUBGSM 0x0155	/**< Ring Zero Systems, Inc. */
#define WAVE_FORMAT_MSAUDIO1 0x0160	/**< Microsoft Corporation */
#define WAVE_FORMAT_CREATIVE_ADPCM 0x0200 /**< Creative Labs, Inc */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH8 0x0202	/**< Creative Labs, Inc */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH10 0x0203 /**< Creative Labs, Inc */
#define WAVE_FORMAT_UHER_ADPCM 0x0210	/**< UHER informatic GmbH */
#define WAVE_FORMAT_QUARTERDECK 0x0220	/**< Quarterdeck Corporation */
#define WAVE_FORMAT_ILINK_VC 0x0230	/**< I-link Worldwide */
#define WAVE_FORMAT_RAW_SPORT 0x0240    /**< Aureal Semiconductor */
#define WAVE_FORMAT_IPI_HSX 0x0250      /**< Interactive Products, Inc. */
#define WAVE_FORMAT_IPI_RPELP 0x0251	/**< Interactive Products, Inc. */
#define WAVE_FORMAT_CS2 0x0260          /**< Consistent Software */
#define WAVE_FORMAT_SONY_SCX 0x0270	/**< Sony Corp. */
#define WAVE_FORMAT_FM_TOWNS_SND 0x0300	/**< Fujitsu Corp. */
#define WAVE_FORMAT_BTV_DIGITAL 0x0400	/**< Brooktree Corporation */
#define WAVE_FORMAT_QDESIGN_MUSIC 0x0450 /**< QDesign Corporation */
#define WAVE_FORMAT_VME_VMPCM 0x0680	/**< AT&T Labs, Inc. */
#define WAVE_FORMAT_TPC 0x0681		/**< AT&T Labs, Inc. */
#define WAVE_FORMAT_OLIGSM 0x1000	/**< Ing C. Olivetti & C., S.p.A. */
#define WAVE_FORMAT_OLIADPCM 0x1001	/**< Ing C. Olivetti & C., S.p.A. */
#define WAVE_FORMAT_OLICELP 0x1002	/**< Ing C. Olivetti & C., S.p.A. */
#define WAVE_FORMAT_OLISBC 0x1003	/**< Ing C. Olivetti & C., S.p.A. */
#define WAVE_FORMAT_OLIOPR 0x1004	/**< Ing C. Olivetti & C., S.p.A. */
#define WAVE_FORMAT_LH_CODEC 0x1100	/**< Lernout & Hauspie */
#define WAVE_FORMAT_NORRIS 0x1400	/**< Norris Communications, Inc. */
#define WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS 0x1500 /**< AT&T Labs, Inc. */
#define WAVE_FORMAT_DVM 0x2000	/**< FAST Multimedia AG */

/**
 * the WAVE_FORMAT_DEVELOPMENT format tag can be used during the
 * development phase of a new wave format.  Before shipping, you MUST
 * acquire an official format tag from Microsoft.
 */
#define WAVE_FORMAT_DEVELOPMENT (0xFFFF)

#if !defined(WAVE_FORMAT_PCM)
#define  WAVE_FORMAT_PCM  0x0001
#endif /* !WAVE_FORMAT_PCM */

#if !defined(WAVE_FORMAT_EXTENSIBLE)
#define  WAVE_FORMAT_EXTENSIBLE 0xFFFE	/**< Microsoft extensible */
#endif /* !WAVE_FORMAT_EXTENSIBLE */


/**
 * Image format tag:
 * For more details see: http://www.webartz.com/fourcc/fccyuv.htm
 */
/* RGB/BGR Formats */

#define IMG_FMT_RGB (('R'<<24)|('G'<<16)|('B'<<8))
#define IMG_FMT_RGB8 (IMG_FMT_RGB|8)
#define IMG_FMT_RGB15 (IMG_FMT_RGB|15)
#define IMG_FMT_RGB16 (IMG_FMT_RGB|16)
#define IMG_FMT_RGB24 (IMG_FMT_RGB|24)
#define IMG_FMT_RGB32 (IMG_FMT_RGB|32)

#define IMG_FMT_BGR 0 // so normal BGR image has just number
#define IMG_FMT_BGR8 (IMG_FMT_BGR|8)
#define IMG_FMT_BGR15 (IMG_FMT_BGR|15)
#define IMG_FMT_BGR16 (IMG_FMT_BGR|16)
#define IMG_FMT_BGR24 (IMG_FMT_BGR|24)
#define IMG_FMT_BGR32 (IMG_FMT_BGR|32)

/***
 *** Planar YUV Formats    U (Cb)  V (Cr)
 ***/

/** Similar to YV12 but including a level of indirection */
#define IMG_FMT_CLPL 0x4C504C43
/** 8 bit Y plane followed by 8 bit 4x4 subsampled V and U planes. */
#define IMG_FMT_YVU9 0x39555659
/** As YVU9 but an additional 4x4 subsampled plane is appended and
 *  constains delta information relative to the last frame - bpp is 9.
 *  Intel Intermediate YUV9 */
#define IMG_FMT_IF09 0x39304649
/** 8 bit Y plane followed 8 bit 2x2 subsampled U and V planes */
#define IMG_FMT_I420 0x30323449
/** Duplicate FOURCC, identical to I420. */
#define IMG_FMT_IYUV 0x56555949
/** 8 bit Y plane follwed 8 bit 2x2 subsampled V and U planes.
 *  similar to I420 except U, V planes swaped */
#define IMG_FMT_YV12 0x32315659
/** 8 bit Y plane followed 8 bit 4x4 subsampled U and V planes
 *  so there is 1/4 width and 1/4 height of Y pixels for each line
 *  this is UNREGISTERED extension used by avifile (YUV411P) */
#define IMG_FMT_I410 0x30313449
/** 8 bit Y plane followed 8 bit 4x1 subsampled U and V planes
 *  so there is 1/4 width of Y pixels for each line
 *  this is UNREGISTERED extension used by avifile (YUV411P) */
#define IMG_FMT_I411 0x31313449
/** 8 bit Y plane followed 8 bit 2x1 subsampled U and V planes
 *  this is UNREGISTERED extension used by avifile (YUV422P) */
#define IMG_FMT_I422 0x32323449
/** 8 bit Y U V planes
 *  this is UNREGISTERED extension used by avifile (YUV444P) */
#define IMG_FMT_I444 0x34343449
/** Single Y plane for monochrome images. */
#define IMG_FMT_Y800 0x30303859
/** Duplicate FOURCC of Y800 */
#define IMG_FMT_Y8 0x20203859

/***
 *** Packed YUV Formats
 ***/

/** Cirrus Logic format wit 4 pixels packed into uint32
 *  A form of YUV 4:1:1 with less than 8 bits per Y, U and V sample. */
#define IMG_FMT_CLJR 0x524A4C43
/** Interlaced version of UYVY (line order 0,2,4,... 1,3,5,...). */
#define IMG_FMT_IUYV 0x56595549
/** Interlaced version of Y41P (line order 0,2,4,... 1,3,5,...). */
#define IMG_FMT_IY41 0x31435949
/** 12 bit format used in Digital Camera IEEE 1394
 *  lsb: U0,Y0,Y1,V0,Y2,Y3 */
#define IMG_FMT_IYU1 0x31555949
/** YUV 4:2:2 as for UYVY but with different component ordering
 *  within the uint32 macropixel identical to YUNV & V422
 *  lsb: Y0,U0,Y1,V0; Y2,U2,Y3,V2; Y4,U4,Y5,V4; */
#define IMG_FMT_YUY2 0x32595559
/** Duplicate of YUY2. */
#define IMG_FMT_YUYV 0x49595559
/** Duplicate of YUY2. */
#define IMG_FMT_V422 0x32323456
/** Duplicate of YUY2 and V422 */
#define IMG_FMT_YUNV 0x564E5559
/** 24 bit format
 *  YUY 4:4:4  lsb: U0,Y0,V0; U1,Y1,V1; */
#define IMG_FMT_YUV  0x20565559
/** duplicate of YUV used in Digital Camera IEEE 1394 */
#define IMG_FMT_IYU2 0x32555949
/** YUV 4:2:2 (Y sample at every pixel, U and V sampled every second
 *  pixel horizontaly on each line). A macropixel constains 2 pixels
 *  in one uint32 - also known as UYNV and UYVY
 *  lsb: U0,Y0,V0,Y1; U2,Y2,V2,Y3; U4,Y4,V4,Y5 */
#define IMG_FMT_Y422 0x32323459
/** Duplicate of Y422. */
#define IMG_FMT_UYVY 0x59565955
/** Duplicate of Y422. */
#define IMG_FMT_CPLA 0x414C5543
/** Duplicate of UYVY - used by some Nvidia */
#define IMG_FMT_UYNV 0x564E5955
/** Format as UYVY but lsb of each Y component is used
 *  to signal pixel transparency. */
#define IMG_FMT_Y42T 0x54323459
/** YCbCr 4:2:2 extended precision 10bits per component - lsb: U0,Y0,V0,Y1 */
#define IMG_FMT_UYVP 0x50565955
/** YCbCr 4:2:2 extended precision 10-bits per component in Y0U0Y1V0 order. */
#define IMG_FMT_YUVP 0x50565559
/** 16 but YUV 4:2:2 - registered by Vitec Multimedia. */
#define IMG_FMT_V655 0x35353656
/** Packed YUV format with Y sampled at every second pixel across each
 *  line and U and V sampled at every fourth pixel. */
#define IMG_FMT_Y211 0x31313259
/** YUV 4:1:1 similar to Y41P
 *  within the 3x uint32 macropixels there are 8 image pixels
 *  lsb: U0,Y0,V1,Y1; U4,Y2,V4,Y3; Y4,Y5,Y6,Y7; */
#define IMG_FMT_Y411 0x31315559
/** Y41P is basically the same as IY41 with the exception that
 *  the data is interlaced. Lines are ordered 0,2,4,....,1,3,5....
 *  instead of 0,1,2,3,4,5,.... */
#define IMG_FMT_Y41P 0x50313459
/** Identical to Y41P except that the least significant bit of each Y
 *  component forms a chromakey channel. (1 means show pixel) */
#define IMG_FMT_Y41T 0x54313459
/** Reodered YUY2  lsb: Y0,V0,Y1,U0; Y2,V2,Y3,U2; Y4,V4,Y5,U5 */
#define IMG_FMT_YVYU 0x55595659
/** Similar to UYVY - but the image is fliped vertically
 *  also is made from lowercase letters ! */
#define IMG_FMT_cyuv 0x76757963

/** Compressed Formats */
#define IMG_FMT_MPEGPES (('M'<<24)|('P'<<16)|('E'<<8)|('S'))

#endif /* AVIFILE_FORMATS_H */
