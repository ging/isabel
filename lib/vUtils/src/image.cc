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
/////////////////////////////////////////////////////////////////////////
//
// $Id: image.cc 22395 2011-05-22 13:15:23Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <vCodecs/vUtils.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <vUtils/image.h>

// forward declarations
void flipOrMirrorPlane(u8 *b, int w, int h, bool flip, bool mirror);

//
// crop a memory buffer from wSxhS to wDxhD
// Bpp is Bytes per pixel (1 or 3, usually)
//
void memcrop(u8 *dst, u8 *src,
             unsigned Bpp,
             unsigned wD, unsigned hD,
             unsigned wS, unsigned hS
            );

image_t::image_t(u8  *b,
                 u32  nBytes,
                 u32  nfcc,
                 int  width,
                 int  height,
                 u32  ts
                )
: numBytes(nBytes),
  fcc(nfcc),
  w(width),
  h(height),
  timestamp(ts)
{
    if ((w < 16) || (h < 16)) // illegal image, fake it!
    {
        w= h = 16;

        nfcc= RGB24_FORMAT;
        nBytes= w * h * 3;
        buffer= new u8[nBytes];
        memset(buffer, 0, nBytes);

        return;
    }

    buffer= new u8[nBytes];
    assert(buffer != NULL && "image_t: run out of memory");

    memcpy(buffer, b, nBytes);
}

image_t::image_t(const image_t &other)
{
    numBytes= other.numBytes;

    fcc= other.fcc;
    w= other.w;
    h= other.h;
    timestamp= other.timestamp;

    buffer= new u8[numBytes];
    assert(buffer != NULL && "image_t(other): run out of memory");

    memcpy(buffer, other.buffer, numBytes);
}

image_t::~image_t(void)
{
    delete []buffer;
}

u8*
image_t::getBuff(void) const
{
    return buffer;
}

u32
image_t::getNumBytes(void) const
{
    return numBytes;
}

u32
image_t::getFormat(void) const
{
    return fcc;
}

int
image_t::getWidth (void) const
{
    return w;
}

int
image_t::getHeight(void) const
{
    return h;
}

u32
image_t::getTimestamp(void) const
{
    return timestamp;
}

struct timeval
image_t::getTimeToPaint(void) const
{
    return timeToPaint;
}

void
image_t::setTimestamp(u32 ts)
{
    timestamp = ts;
}

void
image_t::setTimeToPaint(struct timeval time)
{
    timeToPaint.tv_sec  = time.tv_sec;
    timeToPaint.tv_usec = time.tv_usec;
}

void
image_t::flipV(void)
{
    int numPixels= w * h;

    switch (fcc)
    {
    case I420P_FORMAT:
    {
        u8 *luma = buffer;
        u8 *cromU= buffer + numPixels;
        u8 *cromV= buffer + numPixels * 5/4;

        flipOrMirrorPlane(luma,    w,   h, true, false);
        flipOrMirrorPlane(cromU, w/2, h/2, true, false);
        flipOrMirrorPlane(cromV, w/2, h/2, true, false);

        return ;
    }
    default:
        NOTIFY("flipOrMirror: cannot dealt with format 0x%x (%s)\n",
               fcc,
               vGetFormatNameById(fcc)
              );
        return ;
    }
}

void
image_t::mirrorH(void)
{
    int numPixels= w * h;

    switch (fcc)
    {
    case I420P_FORMAT:
    {
        u8 *luma = buffer;
        u8 *cromU= buffer + numPixels;
        u8 *cromV= buffer + numPixels * 5/4;

        flipOrMirrorPlane(luma,    w,   h, false, true);
        flipOrMirrorPlane(cromU, w/2, h/2, false, true);
        flipOrMirrorPlane(cromV, w/2, h/2, false, true);

        return ;
    }
    default:
        NOTIFY("flipOrMirror: cannot dealt with format 0x%x (%s)\n",
               fcc,
               vGetFormatNameById(fcc)
              );
        return ;
    }
}

void
image_t::crop16(void)
{
    if (((h % 16) == 0) && ((w % 16) == 0))
    {
        return ;
    }

    // size to crop
    unsigned wC = w & ~0xF;
    unsigned hC = h & ~0xF;

    switch (fcc)
    {
    case RGB24_FORMAT:
    case BGR24_FORMAT:
        {
        u8 *orig= buffer;
        u8 *pdest= buffer;

#if 0
GABRIEL: esto seguro que funciona, congelalo cuando puedas
        memcrop(pdest, orig, 3, wC, hC, w, h);
#else
        for (unsigned y= 0; y < hC; y++)
        {
            memcpy(&pdest[y*wC*3], &orig[y*w*3], wC*3);
            //memcpy(pdest, orig, wC*3);
            //pdest += wC*3;
            //orig += w*3;
        }
#endif
        }
        break;
    case I420P_FORMAT:
        {
        int size= w * h;

        u8 *lumaS = buffer;
        u8 *cromUS= lumaS + size;
        u8 *cromVS= lumaS + size * 5 / 4;

        int sizeC= wC * hC;

        u8 *lumaD = buffer;
        u8 *cromUD= lumaD + sizeC;
        u8 *cromVD= lumaD + sizeC * 5 / 4;

        memcrop(lumaD,  lumaS,  1, wC,   hC,   w,   h);
        memcrop(cromUD, cromUS, 1, wC/2, hC/2, w/2, h/2);
        memcrop(cromVD, cromVS, 1, wC/2, hC/2, w/2, h/2);

        }
        break;
    default:
        NOTIFY("crop16: cannot crop 0x%x\n", fcc);
        throw "crop16: cannot crop format";
    }

    w= wC;
    h= hC;
}

void
image_t::reduceBy2(void)
{
    int nw= w / 2;
    int nh= h / 2;

    switch (fcc)
    {
    case RGB24_FORMAT:
    case BGR24_FORMAT:
        {
            u8 *l1, *l2, *pdest;
            l1= buffer;
            l2= l1 + w*3;
            pdest= buffer;
            for (int y= 0; y < nh; y++)
            {
                for (int x= 0; x < nw; x++)
                {
                    *pdest= (*l1 + *(l1+3) + *l2 + *(l2 +3)) / 4;
                    pdest++; l1++; l2++;
                    *pdest= (*l1 + *(l1+3) + *l2 + *(l2 +3)) / 4;
                    pdest++; l1++; l2++;
                    *pdest= (*l1 + *(l1+3) + *l2 + *(l2 +3)) / 4;
                    pdest++; l1++; l2++;
                    l1 += 3;
                    l2 += 3;
                }
                l1 = l2;
                l2 = l1 + w * 3;
            }
        }
        break;
    case I420P_FORMAT:
    {

        return ;
    }
    default:
        NOTIFY("reduceBy2: cannot dealt with format 0x%x (%s)\n", fcc);
        return ;
    }
}

void
image_t::zoom(int nw, int nh)
{
    assert ((nw > 0) && (nh > 0) && "zoom: w, h must be positive\n");

    int srcBytes= vFrameSize(w, h, fcc);
    u8 *srcBuf= buffer;

    int dstBytes= vFrameSize(nw, nh, fcc);
    u8 *dstBuf= new u8[dstBytes];

    if ( ! dstBuf)
    {
        NOTIFY("image_t::zoom: run out of memory, trying to continue\n");
        return;
    }

    int retVal= vRescale(srcBuf,
                         srcBytes,
                         dstBuf,
                         dstBytes,
                         w, h,
                         nw, nh,
                         fcc
                        );

    if (retVal < 0)
    {
        NOTIFY("image_t::zoom: error rescaling, trying to continue\n");
        delete []dstBuf;
        return;
    }

    buffer= dstBuf;
    numBytes= retVal;

    delete []srcBuf;

    w= nw;
    h= nh;
}

void
image_t::smooth(int nw, int nh)
{
    assert ((nw > 0) && (nh > 0) && "smooth: w, h must be positive\n");

    u8 *srcBuf= buffer;

    int dstBytes= vFrameSize(nw, nh, fcc);
    u8 *dstBuf= new u8[dstBytes];

    if ( ! dstBuf)
    {
        NOTIFY("image_t::smooth: run out of memory, trying to continue\n");
        return;
    }

    vSmooth(srcBuf,
            dstBuf,
            w, h,
            nw, nh,
            fcc
           );

    buffer= dstBuf;
    numBytes= dstBytes;

    delete []srcBuf;

    w= nw;
    h= nh;
}

void
image_t::crop(int fromX, int fromY, int nW, int nH)
{
    assert(    (fromX >= 0) && (fromX < w) 
            && (fromY >= 0) && (fromY < h) 
            && (nW > 0) && (nH > 0) 
            && (fromX + nW <= w)
            && (fromY + nH <= h) 
            && "crop: invalid parameters\n"
          );

    u8 *srcBuf= buffer;

    int dstBytes= vFrameSize(nW, nH, fcc);
    u8 *dstBuf= new u8[dstBytes];

    if ( ! dstBuf)
    {
        NOTIFY("image_t::smooth: run out of memory, trying to continue\n");
        return;
    }

    u8 *src, *dst; // we need the originals later

    switch (fcc)
    {
    case RGB24_FORMAT:
    case BGR24_FORMAT:
        src= buffer + (fromY * w + fromX) * 3;
        dst= dstBuf;
        for (int j= 0; j < nH; j++)
        {
            memcpy(dst, src, nW * 3);
            src +=  w * 3;
            dst += nW * 3;
        }
        break;
    case I420P_FORMAT:
        // luma
        src= buffer + (fromY * w + fromX);
        dst= dstBuf;
        for (int j= 0; j < nH; j++)
        {
            memcpy(dst, src, nW);
            src +=  w;
            dst += nW;
        }
        // cromU
        src= buffer + w * h + (fromY * w / 2 + fromX) / 2;
        for (int j= 0; j < nH / 2; j++)
        {
            memcpy(dst, src, nW / 2);
            src +=  w / 2;
            dst += nW / 2;
        }
        // cromV
        src= buffer + w * h + w * h / 4 + (fromY * w / 2 + fromX) / 2;
        for (int j= 0; j < nH / 2; j++)
        {
            memcpy(dst, src, nW / 2);
            src +=  w / 2;
            dst += nW / 2;
        }
        break;
    default:
        NOTIFY("image_t::crop: not implemented for 0x%x (%d)\n",
               fcc,
               vGetFormatNameById(fcc)
               );
        abort();
    }

    buffer= dstBuf;
    numBytes= dstBytes;

    delete []srcBuf;

    w= nW;
    h= nH;
}

//
// flip Vertically or mirror Horizontally
//
void
flipOrMirrorPlane(u8 *b, int w, int h, bool flip, bool mirror)
{
    u8 *rowB, *rowE;

    // Mirror: runs over a row, swaping pixels
    if (mirror)
    {
        for (int lh= 0; lh < h; lh++)
        {
            rowB = &b[lh*w];       // begin of the row
            rowE = &b[(lh+1)*w-1]; // end of the same row
            while (rowB < rowE)
            {
                int s= *rowE; *rowE= *rowB; *rowB= s; // SWAP
                rowB++;
                rowE--;
            }
        }
    }

    // Flip
    if (flip)
    {
        u8 *sb= new u8[w];
        for (int lh= 0; lh < h/2; lh++)
        {
            rowB = &b[lh*w];
            rowE = &b[(h-1-lh)*w];
            memcpy(sb, rowB, w);
            memcpy(rowB, rowE, w);
            memcpy(rowE, sb, w);
        }
        delete []sb;
    }
}


void
memcrop(u8 *dst, u8 *src,
        unsigned Bpp,
        unsigned wD, unsigned hD,
        unsigned wS, unsigned hS
       )
{
    for (unsigned y= 0; y < hD; y++)
    {
        memcpy(&dst[y*wD*Bpp], &src[y*wS*Bpp], wD*Bpp);
        //memcpy(dst, src, w*3);
        //dst += wD*3;
        //src += wS*3;
    }
}

