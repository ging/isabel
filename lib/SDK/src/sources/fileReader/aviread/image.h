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
#ifndef AVIFILE_IMAGE_H
#define AVIFILE_IMAGE_H

#include "infotypes.h"
#include "formats.h"

AVM_BEGIN_NAMESPACE;

/**
 * Class that hides some details of proper BITMAPINFOHEADER
 * initialization methods.
 */
class BitmapInfo : public BITMAPINFOHEADER
{
public:
    uint32_t m_iColors[3];

    BitmapInfo();
    BitmapInfo(int width, int height, int bpp);
    BitmapInfo(const BITMAPINFOHEADER & hdr);
    BitmapInfo(const BitmapInfo & bi);
    BitmapInfo(const BitmapInfo * bi);
    static int BitCount(int csp);
    void SetBitFields15(); // by default SetBits will use BI_RGB & 16bit
    void SetBitFields16();
    void SetRGB();
    // set top-down RGB image
    void SetBits(int bits);
    // set top-down YUV image
    void SetSpace(int csp);
    void SetDirection(int dir)
    {
	if (biHeight < 0) biHeight = -biHeight; //labs
	biHeight = (dir) ? biHeight : -biHeight;
    }
    int Bpp() const;
    bool operator== (const BitmapInfo & bi) const
    {
	return (biWidth == bi.biWidth && biHeight == bi.biHeight
		&& Bpp() == bi.Bpp()
		&& (biCompression == bi.biCompression
		    || (IsRGB() && bi.IsRGB())
		   )
	       );
    }
    bool IsRGB() const
    {
	return biCompression == BI_RGB || biCompression == BI_BITFIELDS;
    }
    void Print() const;
};

/**
 * This class describes an uncompressed image in one of several supported
 * formats. These formats currently include RGB ( 15, 16, 24 and 32 bits )
 * and packed YUV ( non-subsampled 24-bit YUV and 16-bit YUY2 formats ).
 * It is capable of converting between all supported formats. Some of
 * conversions are optimized, but it is generally not recommended to use
 * these conversions in time-critical code.
 */

/**
 * mplayer compatible surface declaration
 * used to allow development of common conversion routines
 */

// for internal usage  BGR/RGB,Y | U | V | Alpha
static const uint_t CIMAGE_MAX_PLANES = 4;

struct ci_surface_t {
    unsigned int m_iFormat;	// fourcc
    int m_iWidth, m_iHeight;    // surface's real width & height
    struct window_s
    {
	int x;
	int y;
	int w;
	int h;
    } m_Window;			// visible area
    uint8_t* m_pPlane[CIMAGE_MAX_PLANES];
    int m_iStride[CIMAGE_MAX_PLANES];
    int m_iBpp;                 // bytes per plane
    int m_iFlip; //  1 - upside-down   -1 downside-up (usually RGB)
    // mostly private extension -
    // do not even think about using them now
    int* m_iQScale;
    int m_iQStride;
    int m_iAge;                 // mainly for ffmpeg
    int m_iType; 		// i/p/b frame
    float m_fAspectRatio;

    // select area of visible image
    // the rest of the image will not be converted, transfered,....
    void SetWindow(int x = 0, int y = 0, int w = 0, int h = 0);
    // pointer to start of visible data
    uint8_t* GetWindow(uint_t idx = 0);
    const uint8_t* GetWindow(uint_t idx = 0) const;
};

class IImageAllocator;

class CImage : public ci_surface_t
{
public:
    //       non-conversion constructors
    /**
     *  Creates new image in format 'header' from specified memory area.
     *  Either allocates its own memory area & copies src data into it, or reuses
     *  parent data. - Note: data can't be specified as void* here to distinguish
     *  different constructors
     */
    CImage(const BitmapInfo* header, const uint8_t* data = 0, bool copy = true);
    CImage(const BitmapInfo* header, const uint8_t* planes[CIMAGE_MAX_PLANES],
	   const int strides[CIMAGE_MAX_PLANES], bool copy = true);

    /* Creates 24-bit RGB image from 24-bit RGB 'data' */
    CImage(const uint8_t* data, int width, int height);

    /* Creates a copy of image 'im' */
    CImage(const CImage* im);

    // Conversion constructors

    /**
     * Creates RGB image in depth 'depth' from image 'im'
     * or image with given color space
     */
    CImage(const CImage* im, uint_t csp_bits);

    /** Creates new image in format 'header' from image 'im' */
    CImage(const CImage* im, const BitmapInfo* header);

    ~CImage();
    void AddRef() const { m_iRefcount++; }
    void Release() const;
    uint8_t* Data(uint_t idx = 0) { return m_pPlane[idx]; }
    const uint8_t* Data(uint_t idx = 0) const { return m_pPlane[idx]; }
    uint_t Stride(uint_t idx = 0) const { return m_iStride[idx]; }

    // for RGB images only!
    uint8_t *At(int i) { return Data() + i * m_iBpl; }
    const uint8_t *At(int i) const { return Data() + i * m_iBpl; }
    uint8_t *At(int i, int j) { return Offset(i) + j * m_iBpl; }
    uint8_t *Offset(int i) { return Data() + i * m_iBpp; }
    const uint8_t *Offset(int i) const { return Data() + i * m_iBpp; }
    // returns bytes per the first plane
    int Bpp() const { return m_iBpp; }
    // returns bytes per line (width * bpp)
    int Bpl() const { return m_iBpl; }
    int Depth() const { return m_iDepth; }


    fourcc_t Format() const { return m_iFormat; }
    uint_t Bytes(uint_t idx = 0) const { return m_iBytes[idx]; }
    int Pixels() const { return m_iPixels; }
    int Width() const { return m_iWidth; }
    int Height() const { return m_iHeight; }
    float Aspect() const { return m_fAspectRatio; }
    int AspectWidth() const { return (int)(m_fAspectRatio * m_iWidth + 0.5); }
    // returns true for upside-down image
    bool Direction() const { return !(m_pInfo.biHeight < 0); }
    /**
     *  Is it in format 'hdr'?
     */
    bool IsFmt(const BitmapInfo* hdr) const { return (m_pInfo == *hdr); }
    BitmapInfo* GetFmt() { return &m_pInfo; }
    const BitmapInfo* GetFmt() const { return &m_pInfo; }

    /**
     * Translations 24-bit RGB <-> Non-subsampled packed 24-bit YUV
     * ( for image processing purposes )
     */
    void ToYUV();
    void ToRGB();

    /**
     * Fast MMX blur filter ( blur over the square with 2^range*2^range dimensions )
     * Blur(x); Blur(y, x) has the same effect as Blur(y)
     */
    void Blur(int range, int from = 0);

    /**
     * Writes 24-bit uncompressed bitmap with name 'filename'.
     */
    void Dump(const char *filename);

    /**
     * RGB<->BGR translation for 24-bit images
     */
    void ByteSwap();

    float GetQuality() const { return m_fQuality; }
    void SetQuality(float q) { m_fQuality = q; }
    void* GetUserData() const { return m_pUserData; }
    void SetUserData(void* userData) { m_pUserData = userData; }
    IImageAllocator* GetAllocator() const { return m_pAllocator; }
    void SetAllocator(IImageAllocator* pAllocator) { m_pAllocator = pAllocator; }

    // set image to black color
    void Clear();

    static bool Supported(const BITMAPINFOHEADER& bi);
    static bool Supported(int csp, int bitcount = 0);
    // convert some common fourcc to those internaly used
    // i.e.  Y422 -> YUY2, IYUV -> I420
    static fourcc_t StandardFOURCC(fourcc_t csp);

    /**
     * General way of converting is as follows.
     * a) There's one function for converting anything to 24-bit RGB and back.
     * b) There is a number of 'shortcuts' between formats,
     *    e.g. YUY2<->16-bit RGB.
     * c) If there's no shortcut between particular two formats, the conversion
     *    is done in two steps through temporary image.
     */
    void Convert(const CImage* from_img);
    void Convert(const uint8_t* from_data, const BitmapInfo* from_fmt);
    // do not use - it's for internal usage and will be removed!
    void Slice(const ci_surface_t* ci, int alpha = 0);

protected:
    void fillMembers();
    void setPlanes();
    mutable int m_iRefcount;
    BitmapInfo m_pInfo;
    uint_t m_iBytes[CIMAGE_MAX_PLANES];
    int m_iDepth;
    int m_iBpl;
    int m_iPixels;
    float m_fQuality;
    void* m_pUserData;
    IImageAllocator* m_pAllocator;
    bool m_bDataOwner[CIMAGE_MAX_PLANES];
public:
    uint_t m_uiPosition;        // decoded position
    int64_t m_lTimestamp;       // 64bit timestamp (us)
private:
    /* copying disabled */
    CImage& operator= (CImage&) { return *this; }
    CImage(const CImage&) {}
};

/**
 *  Interface for Image allocators
 *
 *  Usually implemented by VideoRenderer and used for DirectRendering support
 */
class IImageAllocator
{
public:
    // number of allocated buffers
    virtual uint_t GetImages() const					=0;
    virtual CImage* ImageAlloc(const BITMAPINFOHEADER& bh, uint_t idx, uint_t align)	=0;
    virtual void ReleaseImages()					=0;
    //virtual int ImageResize(int w, int h, uint_t idx)			=0;
};

struct yuv;

struct lookuptable
{
    int m_plY[256];
    int m_plRV[256];
    int m_plGV[256];
    int m_plGU[256];
    int m_plBU[256];
    lookuptable();
};

/**
 * Structure that represents 24-bit RGB pixel.
 */
struct __attribute__((__packed__)) col
{
    static lookuptable t;
    uint8_t b,g,r;
    col() {}
    col(uint8_t B, uint8_t G, uint8_t R) :b(B),g(G),r(R) {}
    inline col(yuv YUV);
    int CREF() const { return (int(b) << 16) + (int(g) << 8) + int(r); }
    uint_t Y() const
    {
	int _Y= 6392l * b + 33055l * g + 16853l * r + 0x8000;
	return (_Y>>16)+16;
    }
    uint16_t bgr15() const {
	return (uint16_t) (((((((r<<5)&0xff00)|g)<<5)&0xfff00)|b)>>3);
    }
    uint16_t bgr16() const {
	return (uint16_t) (((((((r<<5)&0xff00)|g)<<6)&0xfff00)|b)>>3);
    }
    uint32_t bgr32() const { return (uint32_t) ((r<<24) | (g<<16) | b); }
};

/**
 * Structure that represents 24-bit ( 888 ) YUV pixel.
 */
struct __attribute__((__packed__)) yuv
{
    uint8_t Y;
    uint8_t Cb;/* aka U */
    uint8_t Cr;/* aka V */
    yuv() {}
    yuv(uint8_t _Y, uint8_t _Cb, uint8_t _Cr)
	:Y(_Y), Cb(_Cb), Cr(_Cr) {}
    yuv(col Col)
    {
	int _Y = ((25*Col.b + 129*Col.g + 66*Col.r) >> 8) + 0x10;
	if (_Y < 0x10) _Y = 0x10;
	else if (_Y > 0xef) _Y = 0xef;
	Y = _Y;

	int _Cb = ((112*Col.b -74*Col.g -38*Col.r) >> 8) + 0x80;
	if (_Cb < 0x10) _Cb = 0x10;
	else if (_Cb > 0xef) _Cb = 0xef;
	Cb = _Cb;

	int _Cr = ((-18*Col.b -94*Col.g + 112*Col.r) >> 8) + 0x80;
	if (_Cr < 0x10) _Cr = 0x10;
        else if (_Cr > 0xef) _Cr = 0xef;
	Cr = _Cr;
    }
};

inline col::col(yuv YUV)
{
    int y = t.m_plY[YUV.Y];

    int B = (y + t.m_plBU[YUV.Cb]) >> 8;
    if (B < 0) B = 0;
    else if (B > 0xff) B = 0xff;
    b = B;

    int G = (y + t.m_plGU[YUV.Cb] + t.m_plGV[YUV.Cr]) >> 8;
    if (G < 0) G = 0;
    else if (G > 0xff) G = 0xff;
    g = G;

    int R = (y + t.m_plRV[YUV.Cr]) >> 8;
    if (R < 0) R = 0;
    else if (R > 0xff) R = 0xff;
    r = R;
}

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE
typedef avm::CImage CImage;
typedef avm::BitmapInfo BitmapInfo;
typedef avm::yuv yuv;
typedef avm::col col;
#endif

#endif // AVIFILE_IMAGE_H
