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
// $Id: pnmLoad.cc 10573 2007-07-12 16:06:28Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include "pnmImpl.hh"

#define RAW   0
#define ASCII 1

u8*
eatUntilChar(u8 *p, u8 *end, char c)
{
    while ((p <= end) && (*p != c))
    {
        p++;
    }
    return (p > end) ? (u8*)NULL : p;
}

u8*
getInt (unsigned *d, u8 *p, u8 *end)
{
    int   i;

    while (1)
    {
        if (p>end) return NULL;

        if (*p == '#')  // eat comments
            if ((p=eatUntilChar(p, end, '\n')) == NULL)
                return NULL;

        if ((*p>='0') && (*p<='9')) break;

        if (!isspace(*p)) return NULL;
        p++;
    }
    i=0;
    while ((p<end) && isdigit(*p))
    {
        i= i*10 + (*p - '0');
        p++;
    }
    *d= i;
    return p;
}

u8*
imagePBM(u8 *p, u8 *endp, int w, int h, int rawOrAscii)
{
    unsigned  npixels= w*h;
    u8       *buffer= (u8*)malloc (3*npixels);
    u8       *pbuffer= buffer, *endbuffer= buffer + 3*npixels;
    unsigned  x, numBytes, i, j, bit;

    numBytes= (rawOrAscii == RAW) ?  npixels / 8 : npixels;

    if (rawOrAscii == RAW)
    {
        for (i= 0; i < h; i++)
        {
            bit= 0;
            for (j= 0; j < w; j++)
            {
                if (p > endp)
                {
                    NOTIFY("imagePBM: PBM file seems to be truncated\n");
                    return buffer;
                }

                if (bit == 0)   // read a new byte each 8 times
                {
                    x= *p;
                    p++;
                }
                bit ++;
                bit %= 8; // proccessed 8 bit, read next byte

                *pbuffer= (x & 0x80) ? 0 : 255; pbuffer++;
                *pbuffer= (x & 0x80) ? 0 : 255; pbuffer++;
                *pbuffer= (x & 0x80) ? 0 : 255; pbuffer++;
                x= x << 1;
            }
        }
    }
    else
    {
        for (i= 0; i < npixels; i++)
        {
            if (p > endp)
            {
                NOTIFY("imagePBM: PBM file seems to be truncated\n");
                return buffer;
            }
            p= getInt (&x, p, endp);
            *pbuffer= x == 1 ? 0 : 255; pbuffer++;
            *pbuffer= x == 1 ? 0 : 255; pbuffer++;
            *pbuffer= x == 1 ? 0 : 255; pbuffer++;
        }
    }

    return buffer;
}

u8*
imagePGM(u8 *p, u8 *endp, int ctrl, int w, int h, int rawOrAscii)
{
    unsigned  npixels= w*h;
    u8       *buffer= (u8*)malloc (3*npixels);
    unsigned  x;

    for (unsigned i=0; i < npixels && p < endp; i++)
    {
        if (p > endp)
        {
            NOTIFY("imagePGM: No bytes enough reading image data\n");
            free (buffer);
            return NULL;
        }
        if (rawOrAscii == RAW)
        {
            x= *p;
            p++;
        }
        else
        {
            p= getInt (&x, p, endp);
        }
        buffer[3*i+0]= (u8)x ;
        buffer[3*i+1]= (u8)x ;
        buffer[3*i+2]= (u8)x ;
    }

    return buffer;
}

u8*
imagePPM(u8 *p, u8 *endp, int ctrl, int w, int h, int rawOrAscii)
{
    unsigned  numBytes= 3*w*h;
    u8       *buffer= (u8*)malloc (numBytes);
    int       bitshift;

    bitshift= 0;
    while (ctrl > 255)
    {
        ctrl >>= 1;
        bitshift++;
    }

    for (unsigned i=0; i < numBytes && p < endp; i++)
    {
        if (p > endp)
        {
            NOTIFY("imagePPM: No bytes enough reading image data\n");
            free (buffer);
            return NULL;
        }
        if (rawOrAscii == RAW)
        {
            buffer[i]= (u8)*p >> bitshift;
            p++;
        }
        else
        {
            unsigned x;
            p= getInt (&x, p, endp);
            buffer[i]= (u8)x >> bitshift;
        }
    }

    return buffer;
}


image_t*
pnmLoadFile(FILE *f)
{
    size_t fileSize;
    unsigned w, h, ctrl;
    u8 *buffer, *p, *endp;
    char format;

    // find the size of the file
    fseek(f, 0L, SEEK_SET); // paranoid, not needed
    fseek(f, 0L, SEEK_END);
    fileSize= ftell(f);
    fseek(f, 0L, SEEK_SET);

    u8  *readBuffer= (u8*)malloc (fileSize);

    if (fread(readBuffer, 1, fileSize, f) != fileSize)
    {
        NOTIFY("pnmLoadFile: No bytes enough reading header: %s",
               strerror(errno)
              );
        free(readBuffer);
        return NULL;
    }

    p= readBuffer;
    endp= readBuffer+fileSize;

    if (p[0] != 'P' || p[1] < '1' || p[1] > '6')
    {
        free(readBuffer);
        return NULL;
    }
    format= (char)(p[1]);

    if ((p= eatUntilChar (p, endp, '\n')) == NULL)
    {
        NOTIFY("pnmLoadFile: No bytes enough "
               "after format identifier (P%c)\n",
               format
              );
        free(readBuffer);
        return NULL;
    }

    if ((p= getInt (&w, p, endp)) == NULL)
    {
        NOTIFY("pnmLoadFile: No bytes enough or syntax error reading width\n");
        free(readBuffer);
        return NULL;
    }
    if (w <= 0)
    {
        NOTIFY("pnmLoadFile: Width 0 not allowed\n");
        free(readBuffer);
        return NULL;
    }
    if ((p= getInt (&h, p, endp)) == NULL)
    {
        NOTIFY("pnmLoadFile: No bytes enough or syntax error reading height\n");
        free(readBuffer);
        return NULL;
    }
    if (h <= 0)
    {
        NOTIFY("pnmLoadFile: Height 0 not allowed\n");
        free(readBuffer);
        return NULL;
    }

    if (!(format == '1' || format == '4'))
    {
        if ((p= getInt (&ctrl, p, endp)) == NULL)
        {
            NOTIFY("pnmLoadFile: No bytes enough or syntax error reading MAX\n");
            free(readBuffer);
            return NULL;
        }

        if (ctrl != 255)
        {
            NOTIFY("pnmLoadFile: Bad 255 at supposedly P6 file\n");
            free(readBuffer);
            return NULL;
        }
    }
    if (p > endp)
    {
        NOTIFY("pnmLoadFile: No bytes enough before reading data\n");
        free(readBuffer);
        return NULL;
    }
    if (*p != '\n')
    {
        NOTIFY("pnmLoadFile: syntax error in file before reading data\n");
        free(readBuffer);
        return NULL;
    }
    p++;

    switch (format)
    {
    case '1': buffer= imagePBM(p, endp, w, h, ASCII);       break;
    case '2': buffer= imagePGM(p, endp, ctrl, w, h, ASCII); break;
    case '3': buffer= imagePPM(p, endp, ctrl, w, h, ASCII); break;
    case '4': buffer= imagePBM(p, endp, w, h, RAW);         break;
    case '5': buffer= imagePGM(p, endp, ctrl, w, h, RAW);   break;
    case '6': buffer= imagePPM(p, endp, ctrl, w, h, RAW);   break;
    default:
        NOTIFY("pnmLoadFile: imposible to reach this point\n");
        abort();
    }

    free(readBuffer);
    readBuffer= NULL;

    if ( ! buffer)
    {
        NOTIFY("pnmLoadFile: Error reading image data\n");
        return NULL;
    }

    image_t *img= new image_t(buffer,
                              3 * w * h,
                              RGB24_FORMAT,
                              w,
                              h,
                              0
                             );

    free(buffer);

    return img;
}


