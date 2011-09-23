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
#ifndef AVIFILE_IVIDEODECODER_H
#define AVIFILE_IVIDEODECODER_H

#include "infotypes.h"
#include "image.h"

AVM_BEGIN_NAMESPACE;

/**
 *
 *  Class for video decompression.
 *
 *  Usage:
 *
 *  IVideoDecoder::Create() call tries to load video codec
 *  corresponding to your format,
 *  Start() starts decompression and Stop() stops it.
 *
 * SetDestFmt() sets desired bit depth and color space of output picture.
 * Returns zero on success, -1 if format is unsupported and
 * +1 if there was no 'native' support for the depth and library
 * is going to perform internal conversion of format. Most decoders
 * support depths 15, 16, 24 or 32. Many of them also allow to use some
 * YUV formats. You can check if your particular decoder is able to decode
 * into the particular YUV format by calling GetCapabilities(),
 * which returns bitwise OR of supported formats.
 *
 * DecodeFrame() decodes a single frame. Returns >=0 on success,
 * of <0 on failure (it is not fatal ( maybe it was
 * just one defective bit in file, after next keyframe everything
 * will be OK ).
 *
 * SetDirection() allows to 'flip' output picture. By default picture
 * is flipped ( DestFmt().biHeight<0 ), so that output image can
 * be simply memcpy()'ed to the X screen. It should be turned on
 * if image will be used for recompression, because some video encoders
 * will refuse to encode images with negative biHeight. Call it before
 * Start().
 *
 * Some decoders allow tweaking of their parameters. There are two ways
 * of doing it. 1) Use Get/SetExtendedAttr() to store parameter values
 * in registry. Decoders read these values during Start(), so you should
 * perform Restart() if you need to change parameters in run-time.
 *
 * There's more convenient way, which is supported by a few DirectShow
 * codecs. If GetRtConfig() returns nonzero, you can
 * use IRtConfig methods to adjust picture properties between Start() and
 * Stop(). These adjustments are applied from the first frame following
 * method call.
 *
 * List of valid parameters is available IRtConfig::GetCodecInfo().decoder_info.
 * See include/infotypes.h for details on accessing it.
 *
 */


class IVideoDecoder
{
public:
    /**
     * internaly used flag to mark that decoder
     * has not produced new picture from given data
     */
    static const int NO_PICTURE = 0x40000000;
    static const int NEXT_PICTURE = 0x20000000;
    enum CAPS
    {
	CAP_NONE = 0,
        // packed
	CAP_IYUV = 1,
	CAP_YUY2 = 2,
	CAP_UYVY = 4,
	CAP_YVYU = 8,
        // planar
	CAP_YV12 = 128,
	CAP_I420 = 256,
	CAP_YVU9 = 512,

	// for DR1 rendering this align is needed
        // this also mean codec will properly handly strides
        // IT'S FOR PRIVATE USAGE and will be changed
	CAP_ALIGN64 = 1 << 30,
	CAP_ALIGN16 = 1 << 31
    };

    IVideoDecoder(const CodecInfo& info, const BITMAPINFOHEADER& format);
    virtual ~IVideoDecoder();

    /* Functions which may be reimplemented */

    /**
     * Codec's curently selected destination format
     */
    virtual const BITMAPINFOHEADER& GetDestFmt() const;
    /**
     * Capability flags for this codec
     */
    virtual CAPS GetCapabilities() const;
    virtual const CodecInfo& GetCodecInfo() const;
    /**
     * Use this method instead of dynamic_cast - some gcc compilers
     * (even the new ones seems to have some serious problems with that)
     */
    virtual IRtConfig* GetRtConfig(); // can't be const

    virtual void Flush(); // call after seek
    virtual int Restart();
    virtual int SetDirection(int dir);
    virtual int Start();
    virtual int Stop();

    /* Functions which must be implemented by each decoder */

    /**
     * Passes the data to decoder.
     * 'render' flag signals decoder if the frame will be rendered.
     * If it is 'false', decoder may speed up the decoding process
     * by not filling CImage object. This feature is available only
     * for a few decoders, such as Indeo Video 5.0.
     * also it will be faster with ffmpeg decoder
     *
     * dest -  points to memory area (size respects BITMAPINFOHEADER.biSize)
     * src  -  source of bitstream data
     * size -  size of bitstream data
     * is_keyframe - flag to make visible to decoder that this is keyframe
     *               most new decoders will decide this from content
     *               of bitstream - but for some older codecs this
     *		     flags is necessary
     * returns <0 if some error appears
     * INTERNAL flags:
     *         | NO_PICTURE - no picture was generated
     *         | NEXT_PICTURE  - another picture is needed
     *         when picture is generated codec might actually return it
     *         in differenet locations (out != 0)
     */
    virtual int DecodeFrame(CImage* dest, const void* src, uint_t size,
			    int is_keyframe, bool render = true,
			    CImage** out = 0)				=0;
    /**
     * Select destination decoder output
     * by default it will be BGR24
     */
    virtual int SetDestFmt(int bits = 24, fourcc_t csp = 0)		=0;

#ifdef AVM_COMPATIBLE
    /** \deprecated backward compatible **/
    int DecodeFrame(const void* src, uint_t size, framepos_t p, double t,
		    int is_keyframe, bool render = true);
    /** \deprecated backward compatible **/
    CImage* GetFrame() const { m_pImage->AddRef(); return m_pImage; }
#endif

protected:
    const CodecInfo& m_Info;
    BITMAPINFOHEADER* m_pFormat;
    BitmapInfo m_Dest;
    CImage* m_pImage; // for backward compatibility
};

// declaration of IRtConfig moved to image.h
// used also by IVideoRenderer

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE
typedef avm::IRtConfig IRtConfig;
typedef avm::IVideoDecoder IVideoDecoder;
#endif

#endif // AVIFILE_IVIDEODECODER_H
