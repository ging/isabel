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
// $Id: gifDec.cc 10574 2007-07-12 16:08:15Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/item.hh>

#include "gifImpl.hh"

// WARNING:
// This interface requires to get the data buffer aligned
// in memory

// Logical Screen Descriptor Length
#define LOGICALSCREENDESCRL 7
// those headers with identifier mark are substracted 1 from their
// length (the block identifier)
// Image Descriptor Length (removed the identifier (first byte 0x2C)
#define IMAGEDESCRL 9

#define EXTENSION     0x21
#define IMAGESEP      0x2c
#define TRAILER       0x3b
#define INTERLACEMASK 0x40
#define COLORMAPMASK  0x80

struct decodedGif_t {
  u8       *b;
  u8       *p;
  u8       *comment;
  unsigned  numColors;
  unsigned w, h;
};

class gif_t: public virtual item_t
{

public:
  gif_t (void) {
      imageData= Raster= NULL;
  };

  ~gif_t (void) {
      if (imageData != NULL)
        free (imageData);
      if (Raster != NULL)
        free (Raster);
  };


  enum gifType_e {
    GIF87a, GIF89a
  };

  typedef enum gifRes_e {
    GIFOK,
    TRUNCGIF,
    READOK,
    ERRORREAD,
    ERRORREADSIGNATURE,
    ERRORREADLOGICALSCREENDESCRIPTOR,
    ERRORREADCOLORMAP,
    ERRORREADBLOCK,
    ERRORREADIMAGEDESCRIPTOR,
    ERRORREADLOCALCOLORMAP,
    ERRORREADDATABLOCK,
    ERRORREADBLOCKSIZE,
    ERRORREADSBSIZE,
    ERRORREADEXTENSION,
    ERRORREADASPNUM,
    ERRORREADASPDEF,
    ERRORREADMISCBYTE,
    ERRORREADCODESIZE,
    ERRORDISCARDINGIMAGEDESCR,
    ERRORDISCARDINGCOLORMAP,
    ERRORNOCOLORMAP,
    ERRORRUNOUTOFMEMORY,
    BADBLOCKTYPE,
    BADGIFASPECT,
    NOIMAGEDATA,
    NOTGIFFORMAT
  };

  void     printError (gifRes_e error, char *s);
  gifRes_e load (const u8 *buf, int size);
  int      save2File (char *outfilename);  // for debbuging purposes only

  //
  // User should free the buffer from decode
  //
  decodedGif_t  *decodeToRGB24 (void);

  //
  // Access functions
  //
  unsigned  getNumColors (void) {
      return HasLocalColorMap?LocalColorMapSize:GlobalColorMapSize;
  };
  unsigned  getWidth     (void) { return (unsigned)__Width; };
  unsigned  getHeight    (void) { return (unsigned)__Height; };
  //
  // User should free the buffer from getColorMap
  //
  u8        *getColorMap (void);

  virtual const char *className(void) const { return "gif_t"; };

private:

  gifType_e  gifType;

  u8 const  *beginBuf;

  u8         LogicalScreenDescriptor[LOGICALSCREENDESCRL];
  u8         GlobalColorMap[256][3]; // (biggest) Global Color Map
  u8         LocalColorMap[256][3];  // (biggest) Global Color Map
  u8         ImageDescriptor[IMAGEDESCRL];
  u8        *Raster;    // Serialized Compressed Image Data
  u8        *imageData; // Serialized CLUT Image Data
  int        rasterSize; // Size of compressed data
  u8         Background; // Background color

  float      normaspect;
  u8         InitCodeSize,  // Starting code size, used during Clear
             CodeSize;
  int        ClearCode,  // GIF clear code
             EOFCode;    // GIF end-of-information code

  int        __Width, __Height;         // Image Dimensions
  int        ScreenWidth, ScreenHeight; // Screen Dimensions
  bool       Interlaced,
             HasGlobalColorMap,
             HasLocalColorMap;
  u8         GlobalBitsPerPixel,
             LocalBitsPerPixel;
  int        GlobalColorMapSize, // number of colors of GlobalColorMap
             LocalColorMapSize;  // number of colors of LocalColorMap

  // Shortcuts for colormap and colormapsize, as they are often used,
  int  colMapSize; // HasLocalColorMap?LocalColorMapSize:GlobalColorMapSize
  u8  *colMap;     // HasLocalColorMap?LocalColorMap:GlobalColorMap

  gifRes_e readImage (u8 const *buf, u8 const *bufEnd);

  // auxiliar functions
  void readByte (u8 &dst, u8 const *&src) const;
  void readWord (u16 &dst, u8 const *&src) const;
  void readNBytes (u8 *dst, u8 const *&src, int size) const;
  void discardNBytes (u8 const *&src, int size) const;

  // auxiliar functions to unLZW
  int  readCode (void);
  void putPixel(u8 *pic8, u8 index, bool Interlaced, bool do24);

  //
  // By default, unLZW decompress the image leaving the codes as
  // indexes onto the colorMap
  // If do24 is true, it generates the real pixels, though getting
  // a RGB24 image.
  // else, it generates a CLUT image
  int   unLZW (bool do24);

  int BitOffset,                // Bit Offset of next code
      XC, YC,                   // Output X and Y coords of current pixel
      Pass,                     // Used by output routine if interlaced pic
      OutCount,                 // Decompressor output 'stack count'
      BytesPerScanline,         // bytes per scanline in output raster
      MaxCode,                  // limiting value for current code size
      CurCode, OldCode, InCode, // Decompressor variables
      FirstFree,                // First free code, generated per GIF spec
      FreeCode,                 // Decompressor,next free slot in hash table
      FinChar,                  // Decompressor variable
      ReadMask;                 // Code AND mask for current code size


    /* The hash table used by the decompressor */
  int Prefix[4096];
  int Suffix[4096];

    /* An output array used by the decompressor */
  int OutCode[4097];

};

void
gif_t::readByte (u8 &dst, u8 const *&src) const
{
  dst= *src;
  src++;
}

void
gif_t::readWord (u16 &dst, u8 const *&src) const
{
  u16 *p=(u16*)src;

  dst=*p;
  src+= 2;
}

void
gif_t::readNBytes (u8 *dst, u8 const *&src, int size) const
{
  memcpy (dst, src, size);
  src+= size;
}

void
gif_t::discardNBytes (u8 const *&src, int size) const
{
  src+= size;
}

gif_t::gifRes_e
gif_t::load (const u8 *buf, int size)
{
  u8 block;
  beginBuf= buf; // remember the start, just in case
  u8 const *endBuf= buf+size; // locate the end, to check for errors
  char ch;
  u8 GIFSignature[6];
  float aspectRatio=0;

  colMap= NULL; colMapSize= 0;

  BitOffset = 0;         // Bit Offset of next code
  XC = 0; YC = 0;        // Output X and Y coords of current pixel
  Pass = 0;              // Used by output routine if interlaced pic
  OutCount = 0;          // Decompressor output 'stack count'

  HasGlobalColorMap = false; // needed for proper settings of shortcuts
  GlobalColorMapSize= false; // paranoid
  HasLocalColorMap  = false; // needed for proper settings of shortcuts
  LocalColorMapSize = 0;     // paranoid
  Raster = NULL;

  // read the GIF Signature
  if (buf+6 > endBuf)
    return ERRORREADSIGNATURE;
  readNBytes ((u8*)GIFSignature, buf, 6);

  if (strncmp((char *) GIFSignature, "GIF87a", (size_t) 6)==0) {
    gifType = GIF87a;
  } else if (strncmp((char *) GIFSignature, "GIF89a", (size_t) 6)==0) {
    gifType = GIF89a;
  } else {
    return NOTGIFFORMAT;
  }

  // Get GIF screen descriptor
  if (buf+LOGICALSCREENDESCRL > endBuf)
    return ERRORREADLOGICALSCREENDESCRIPTOR;
  readNBytes ((u8*)LogicalScreenDescriptor, buf, LOGICALSCREENDESCRL);

  // Get variables from the GIF screen descriptor

  ScreenWidth = (unsigned(LogicalScreenDescriptor[1])<<8)
                |unsigned(LogicalScreenDescriptor[0]);
  ScreenHeight= (unsigned(LogicalScreenDescriptor[3])<<8)
                |unsigned(LogicalScreenDescriptor[2]);
  ch= LogicalScreenDescriptor[4];
  HasGlobalColorMap = ((ch & COLORMAPMASK) ? true : false);

  GlobalBitsPerPixel= (ch & 0x07) + 1;
  GlobalColorMapSize= 1 << GlobalBitsPerPixel;

  Background = LogicalScreenDescriptor[5];  // background color... not used.

  u8 aspect = LogicalScreenDescriptor[6];
  if (aspect) {
    if (gifType == GIF87a)
      return BADGIFASPECT;
    else
      aspectRatio = (float) (aspect + 15) / 64.0;   // gif89 aspect ratio
  }
  // Done with Screen Description

  // Read in global colormap.

  if (HasGlobalColorMap) {
    int nentries= 3*GlobalColorMapSize;

    if (buf + nentries > endBuf)
      return ERRORREADCOLORMAP;
    readNBytes ((u8*)GlobalColorMap, buf, nentries);
  }

  debugMsg(dbg_App_Normal,
           "load",
           "GIF %s ScreenSize=%dx%d %s"
           " Global ColorMap"
           "\n\t\t%d colors (BitsPerPixel=%d)"
           "\n\t\taspect=%d (aspect ratio=%.2f)\n",
           gifType==GIF87a?"87a":"89a",
           ScreenWidth, ScreenHeight,
           HasGlobalColorMap?"with":"without",
           GlobalColorMapSize, GlobalBitsPerPixel,
           aspect, aspectRatio
          );

  // possible things at this point are:
  //   an application extension block
  //   a comment extension block
  //   an (optional) graphic control extension block
  //       followed by either an image
  //       or a plaintext extension
  //

  bool gotimage= false; // Take only first image if several
  while (1) {
    if (buf > endBuf) return ERRORREADBLOCK;
    readByte (block, buf);

    if (block == EXTENSION) {  // parse extension blocks
      u8 fn, blocksize;
      u16 aspnum, aspden;

      // read extension block
      if (buf > endBuf) return ERRORREADEXTENSION;
      readByte (fn, buf);

      if (fn == 'R') {  // 0x52 TBC        // GIF87 aspect extension
        u8 sbsize;

        if (buf > endBuf) return ERRORREADBLOCKSIZE;
        readByte (blocksize, buf);
        if (blocksize == 2) {
          if (buf+2 > endBuf)
            return ERRORREADASPDEF;
          readWord (aspden, buf);
          if (aspden>0 && aspnum>0)
            normaspect = (float) aspnum / (float) aspden;
          else { normaspect = 1.0;  aspnum = aspden = 1; }
        }
        else {
          discardNBytes (buf, blocksize);
          if (buf > endBuf) return ERRORREADBLOCK;
        }

        while (1) {  // eat any following data subblocks
          if (buf > endBuf) return ERRORREADSBSIZE;
          readByte (sbsize, buf);
          if (sbsize == 0) break;
          discardNBytes (buf, sbsize); // eat any following data subblocks
          if (buf > endBuf) return ERRORREADBLOCK;
        }
      }


      else if (fn == 0xFE) {  // Comment Extension **IGNORED**
        u8 sbsize;

        while (1) {
          if (buf > endBuf) return ERRORREADSBSIZE;
          readByte (sbsize, buf);     // length of comment subblock
          if (sbsize == 0) break;
          discardNBytes (buf, sbsize); // eat any following data subblocks
          if (buf > endBuf) return ERRORREADBLOCK;
        }


      }


      else if (fn == 0x01) {  // PlainText Extension **IGNORED**
        u8 sbsize;

        while (1) {
          if (buf > endBuf) return ERRORREADSBSIZE;
          readByte (sbsize, buf);      // length of comment subblock
          if (sbsize == 0) break;
          discardNBytes (buf, sbsize);  // eat any following data subblocks
          if (buf > endBuf) return ERRORREADBLOCK;
        }

      }


      else if (fn == 0xF9) {  // Graphic Control Extension **IGNORED**
        u8 sbsize;

        while (1) {
          if (buf > endBuf) return ERRORREADSBSIZE;
          readByte (sbsize, buf);      // length of comment subblock
          if (sbsize == 0) break;
          discardNBytes (buf, sbsize);  // eat any following data subblocks
          if (buf > endBuf) return ERRORREADBLOCK;
        }

      }


      else if (fn == 0xFF) {  // Application Extension **IGNORED**
        u8 sbsize;

        while (1) {
          if (buf > endBuf) return ERRORREADSBSIZE;
          readByte (sbsize, buf);     // length of comment subblock
          if (sbsize == 0) break;
          discardNBytes (buf, sbsize); // eat any following data subblocks
          if (buf > endBuf) return ERRORREADBLOCK;
        }

      }


      else { // unknown extension **OBVIOUSLY IGNORED**
        u8 sbsize;

        while (1) {
          if (buf > endBuf) return ERRORREADSBSIZE;
          readByte (sbsize, buf);     // length of comment subblock
          if (sbsize == 0) break;
          discardNBytes (buf, sbsize); // eat any following data subblocks
          if (buf > endBuf) return ERRORREADBLOCK;
        }

      }
    }


    else if (block == IMAGESEP) {
      //if (DEBUG) NOTIFY("imagesep (got=%d)  ", gotimage);

      if (gotimage) {   // just skip over remaining images
        u8 misc;

        // skip image header
        discardNBytes (buf, IMAGEDESCRL-1);
            // -1 porque el siguiente byte me interesa para saber
            // si tengo que descartar mas cosas
        if (buf > endBuf) return ERRORDISCARDINGIMAGEDESCR;
        if (buf > endBuf) return ERRORREADMISCBYTE;
        readByte (misc, buf);     // misc. byte

        if (misc & COLORMAPMASK) {    // image has local colormap. Skip it
          discardNBytes (buf, 3 * (1 << ((misc&0x07)+1)));
          if (buf > endBuf) return ERRORDISCARDINGCOLORMAP;
        }

        while (1) {
          u8 sbsize;

          // figure out length of comment
          if (buf > endBuf) return ERRORREADSBSIZE;
          readByte (sbsize, buf);       // length of subblock
          if (sbsize == 0) break;
          discardNBytes (buf, sbsize);   // eat any following data subblocks
          if (buf > endBuf) return ERRORREADBLOCK;
        }

      }

      else {
        gifRes_e res= readImage(buf, endBuf);
        if (res == READOK)
          gotimage = true;
        else
          return res;
      }
    }


    else if (block == TRAILER) {      /* stop reading blocks */
      //if (DEBUG) NOTIFY("trailer");
      break;
    }

    else {      /* unknown block type */
      //if (DEBUG) NOTIFY("block type 0x%02x  ", block);

      if (!gotimage)
        return BADBLOCKTYPE;

      break;
    }

    //if (DEBUG) NOTIFY("\n");
  }

  if (!gotimage)
     return NOIMAGEDATA;

  // shortcuts
  colMapSize= HasLocalColorMap?LocalColorMapSize:GlobalColorMapSize;
  colMap= (u8*)(HasLocalColorMap?LocalColorMap:GlobalColorMap);

  return GIFOK;
}

gif_t::gifRes_e
gif_t::readImage(u8 const *buf, u8 const *endBuf)
{
  u8  sbsize, *ptr1;
  int Misc;

  // Get image descriptor
  if (buf+IMAGEDESCRL > endBuf) return ERRORREADIMAGEDESCRIPTOR;
  readNBytes (ImageDescriptor, buf, IMAGEDESCRL);

  // Get variables from the image descriptor
  // which does not include the Image Separator

  __Width = (unsigned(ImageDescriptor[5])<<8)|unsigned(ImageDescriptor[4]);
  __Height= (unsigned(ImageDescriptor[7])<<8)|unsigned(ImageDescriptor[6]);

  Misc = ImageDescriptor[8];
  Interlaced = ((Misc & INTERLACEMASK) ? true : false);

  HasLocalColorMap = ((Misc & COLORMAPMASK) ? true : false);
  LocalBitsPerPixel= (Misc & 0x07)+1;
  LocalColorMapSize= 1 << LocalBitsPerPixel;

  if (HasLocalColorMap) {
    int nentries= 3*LocalColorMapSize;

    if (buf + nentries > endBuf) return ERRORREADLOCALCOLORMAP;
    readNBytes ((u8*)LocalColorMap, buf, nentries);
  }

  if (!HasGlobalColorMap && !HasLocalColorMap) {
    // no global nor local colormap

    //if (debug) NOTIFY("Warning: GIF file has no colormap\n");
    return ERRORNOCOLORMAP;
  }

  debugMsg(dbg_App_Normal,
           "readImage",
           "ImageSize=%dx%d %sinterlaced %s Local ColorMap"
           "\n\t\t%d colors (BitsPerPixel=%d)\n",
           __Width, __Height,
           Interlaced? "":"non ",
           HasLocalColorMap?"with":"without",
           LocalColorMapSize, LocalBitsPerPixel
          );

  // Start reading the raster data. First we get the initial code size
  // and compute decompressor constant values, based on this code size.
  //

  if (buf > endBuf) return ERRORREADCODESIZE;
  readByte (CodeSize, buf);
  ClearCode = (1 << CodeSize);
  EOFCode = ClearCode + 1;
  FreeCode = FirstFree = ClearCode + 2;

  // The GIF spec has it that the code size is the code size used to
  // compute the above values is the code size given in the file, but the
  // code size used in compression/decompression is the code size given in
  // the file plus one. (thus the ++).
  //

  CodeSize++;
  InitCodeSize = CodeSize;
  MaxCode = (1 << CodeSize);
  ReadMask = MaxCode - 1;

  // UNBLOCK:
  // Read the raster data.  Here we just transpose it from the GIF array
  // to the Raster array, turning it from a series of blocks into one long
  // data stream, which makes life much easier for readCode().
  //

  // LZW will no produce a compressed string larger
  // than twice the original string

  Raster= (u8*)malloc (2*__Width*__Height);
  if (!Raster) {
      return ERRORRUNOUTOFMEMORY;
  }

  ptr1 = Raster;
  rasterSize= 0;
  while (1) {
    if (buf > endBuf) return ERRORREADBLOCKSIZE;
    readByte (sbsize, buf);
    if (sbsize == 0) {
        break;
    }
    if (buf+sbsize > endBuf) return ERRORREADDATABLOCK;
    readNBytes (ptr1, buf, sbsize);
    ptr1 += sbsize;
    rasterSize += sbsize;
  }


  return READOK;
}


void
gif_t::printError (gifRes_e error, char *s)
{
  switch (error) {
    case GIFOK:
      strcpy (s, "GIF OK");
      break;
    case TRUNCGIF:
      strcpy (s, "TRUNC GIF");
      break;
    case READOK:
      strcpy (s, "READ OK");
      break;
    case ERRORREAD:
      strcpy (s, "ERROR reading data");
      break;
    case ERRORREADSIGNATURE:
      strcpy (s, "ERROR reading signature");
      break;
    case ERRORREADLOGICALSCREENDESCRIPTOR:
      strcpy (s, "ERROR reading screen descriptor");
      break;
    case ERRORREADCOLORMAP:
      strcpy (s, "ERROR reading color map");
      break;
    case ERRORREADBLOCK:
      strcpy (s, "ERROR reading block");
      break;
    case ERRORREADIMAGEDESCRIPTOR:
      strcpy (s, "ERROR reading image descriptor");
      break;
    case ERRORREADLOCALCOLORMAP:
      strcpy (s, "ERROR reading local color map");
      break;
    case ERRORREADDATABLOCK:
      strcpy (s, "ERROR reading data block");
      break;
    case ERRORREADBLOCKSIZE:
      strcpy (s, "ERROR reading block size");
      break;
    case ERRORREADSBSIZE:
      strcpy (s, "ERROR reading SB size");
      break;
    case ERRORREADEXTENSION:
      strcpy (s, "ERROR reading extension");
      break;
    case ERRORREADASPNUM:
      strcpy (s, "ERROR reading aspnum");
      break;
    case ERRORREADASPDEF:
      strcpy (s, "ERROR reading aspdef");
      break;
    case ERRORREADMISCBYTE:
      strcpy (s, "ERROR reading misc byte");
      break;
    case ERRORREADCODESIZE:
      strcpy (s, "ERROR reading code size");
      break;
    case ERRORDISCARDINGIMAGEDESCR:
      strcpy (s, "ERROR discarding image descriptor");
      break;
    case ERRORDISCARDINGCOLORMAP:
      strcpy (s, "ERROR discarding color map");
      break;
    case ERRORNOCOLORMAP:
      strcpy (s, "ERROR no color map available");
      break;
    case ERRORRUNOUTOFMEMORY:
      strcpy (s, "ERROR run out of memory");
      break;
    case BADBLOCKTYPE:
      strcpy (s, "ERROR bad block type");
      break;
    case BADGIFASPECT:
      strcpy (s, "ERROR bad GIF aspect");
      break;
    case NOIMAGEDATA:
      strcpy (s, "ERROR no image data");
      break;
    case NOTGIFFORMAT:
      strcpy (s, "ERROR not GIF format");
      break;
    default:
      strcpy (s, "ERROR UNKOWN");
  }
}


void
gif_t::putPixel(u8 *imageData, u8 index, bool Interlaced, bool do24)
{
    u8   *ptr = NULL;
    long posPixel= YC*__Width + XC;

    if (do24) posPixel *=3;

    ptr = imageData + posPixel;

    if (do24) {
        ptr[0]= colMap[index*3+0];
        ptr[1]= colMap[index*3+1];
        ptr[2]= colMap[index*3+2];
    } else {
        ptr[0]= index;
    }

    XC++;
    if (XC == __Width) {
        XC = 0;

        if(Interlaced) {
            switch (Pass) {
                case 0:
                    YC += 8;
                    if (YC >= __Height) { Pass++; YC = 4; }
                    break;
                case 1:
                    YC += 8;
                    if (YC >= __Height) { Pass++; YC = 2; }
                    break;
                case 2:
                    YC += 4;
                    if (YC >= __Height) { Pass++; YC = 1; }
                    break;
                case 3:
                    YC += 2;  break;
            }
        } else {
            YC++;
        }
    }
}


int
gif_t::unLZW (bool do24)
{
  int i;
  int Code;    // Value returned by ReadCode
  long npixels=0;
  long maxpixels= __Width*__Height;

  /* Allocate the 'pic' */
  long imageDataSize= do24?maxpixels*3:maxpixels;
  imageData= (u8*)malloc(imageDataSize);

  if (!Raster) {
      return ERRORRUNOUTOFMEMORY;
  }

  /* Decompress the file, continuing until you see the GIF EOF code.
   * One obvious enhancement is to add checking for corrupt files here.
   */

  Code = readCode();
  while (Code != EOFCode) {
//NOTIFY("Freecode=%d, npixels=%d\n", FreeCode, npixels);
    /* Clear code sets everything back to its initial value, then reads the
     * immediately subsequent code as uncompressed data.
     */

    if (Code == ClearCode) {
      CodeSize = InitCodeSize;
      MaxCode = (1 << CodeSize);
      ReadMask = MaxCode - 1;
      FreeCode = FirstFree;
      Code = readCode();
      CurCode = OldCode = Code;
      FinChar = CurCode;
      putPixel(imageData, FinChar, Interlaced, do24);
      npixels++;
    } else {
      /* If not a clear code, must be data: save same as CurCode and InCode */
      /* if we're at maxcode and didn't get a clear, stop loading */
      if (FreeCode>=4096) {
        NOTIFY("gif_t::un-LZW " "freecode blew up\n");
        break;
      }

      CurCode = InCode = Code;

      /* If greater or equal to FreeCode, not in the hash table yet;
       * repeat the last character decoded
       */

      if (CurCode >= FreeCode) {
        CurCode = OldCode;
        if (OutCount > 4096) {
          NOTIFY("gif_t::un-LZW " "outcount blew up\n");
          break;
        }
        OutCode[OutCount++] = FinChar;
      }
      /* Unless this code is raw data, pursue the chain pointed to by CurCode
       * through the hash table to its end; each code in the chain puts its
       * associated output code on the output queue.
       */

      while (CurCode >= FirstFree) {
        if (OutCount > 4096) {
          NOTIFY("gif_t::un-LZW " "corrupt input\n");
          break;   /* corrupt file */
        }
        OutCode[OutCount++] = Suffix[CurCode];
        CurCode = Prefix[CurCode];
      }

      if (OutCount > 4096) {
        NOTIFY("gif_t::un-LZW " "outcount blew up\n");
        break;
      }

      /* The last code in the chain is treated as raw data. */

      FinChar = CurCode;
      OutCode[OutCount++] = FinChar;

      /* Now we put the data out to the Output routine.
       * It's been stacked LIFO, so deal with it that way...
       */

      /* safety thing:  prevent exceeding range of 'imageData' */
      if (npixels + OutCount > maxpixels) OutCount = maxpixels-npixels;

      npixels += OutCount;
      for (i=OutCount-1; i>=0; i--) {
          putPixel(imageData, OutCode[i], Interlaced, do24);
      }
      OutCount = 0;

      /* Build the hash table on-the-fly. No table is stored in the file. */

      Prefix[FreeCode] = OldCode;
      Suffix[FreeCode] = FinChar;
      OldCode = InCode;

      /* Point to the next slot in the table.  If we exceed the current
       * MaxCode value, increment the code size unless it's already 12.  If it
       * is, do nothing: the next code decompressed better be CLEAR
       */

      FreeCode++;
      if (FreeCode >= MaxCode) {
        if (CodeSize < 12) {
          CodeSize++;
          MaxCode *= 2;
          ReadMask= MaxCode - 1;
        }
      }
    }
    Code = readCode();
    if (npixels >= maxpixels) {
      if (npixels > maxpixels)
        NOTIFY("gif_t::un-LZW more pixels than width*heigth");
      break;
    }
  }

  if (npixels < maxpixels) {
    NOTIFY("gif_t::un-LZW GIF truncated");
    NOTIFY("gif_t::un-LZW This GIF seems to be truncated. Winging it.");
    if (!Interlaced)  /* clear->EOBuffer */
      memset((char*) imageData+npixels, 0, maxpixels-npixels);
    return TRUNCGIF;
  }
  if (npixels > maxpixels) {
    NOTIFY("gif_t::un-LZW " "GIF beyond data image, ignoring");
  }
  return GIFOK;
}

decodedGif_t*
gif_t::decodeToRGB24 ()
{
  decodedGif_t *retVal= new decodedGif_t;

  int   npixels= __Width*__Height;
  u8   *b= (u8*)malloc (npixels*3);

  unLZW(true);  // uncompress from Raster to imageData

  memcpy (b, imageData, npixels*3);

  retVal->b= b;
  retVal->p= NULL;
  retVal->comment= NULL;
  retVal->numColors= 0;
  retVal->w= __Width;
  retVal->h= __Height;

  return retVal;
}

int
gif_t::save2File (char *filename)
{
  int   i;
  char  head[256];
  void *ColorMap;
  int   npixels= __Width*__Height;
  FILE *out;

  if ((out= fopen(filename, "w")) == NULL) {
    NOTIFY("Error opening output file %s\n", filename);
    return 1;
  }

  if (!imageData) {
    if (!Raster) {
      NOTIFY("No GIF image was load for saving to a file\n");
      return 1;
    }
    unLZW(false);  // uncompress from Raster to imageData
  }

  // imprime en formato P6
  sprintf (head, "P6\n%d %d\n255\n", __Width, __Height);
  fwrite (head, 1, strlen (head), out);

  if (HasLocalColorMap)
    ColorMap= LocalColorMap;
  else
    ColorMap= GlobalColorMap;

  for (i=0; i < npixels; i++) {
    int pixel= 3*imageData[i];
    fwrite((char*)(ColorMap)+pixel+0, 1, 1, out);
    fwrite((char*)(ColorMap)+pixel+1, 1, 1, out);
    fwrite((char*)(ColorMap)+pixel+2, 1, 1, out);
  }

  fclose (out);
  return 0;
}


u8*
gif_t::getColorMap()
{
    u8 *retVal= (u8*) malloc (256*3);
    memset(retVal, 0, 256*3);

    if (HasLocalColorMap) {
        memcpy (retVal, LocalColorMap, LocalColorMapSize*3);
    } else {
        memcpy (retVal, GlobalColorMap, GlobalColorMapSize*3);
    }

    return retVal;
}

/* Fetch the next code from the raster data stream.  The codes can be
 * any length from 3 to 12 bits, packed into 8-bit bytes, so we have to
 * maintain our location in the Raster array as a BIT Offset.  We compute
 * the byte Offset into the raster array by dividing this by 8, pick up
 * three bytes, compute the bit Offset into our 24-bit chunk, shift to
 * bring the desired code to the bottom, then mask it off and return it.
 */

int
gif_t::readCode()
{
  u32 RawCode;
  int ByteOffset;

  ByteOffset = BitOffset / 8;
  RawCode = (u32)Raster[ByteOffset] | ((u32)Raster[ByteOffset + 1] << 8);
  if (CodeSize >= 8)
    RawCode |= ( ((u32)Raster[ByteOffset + 2]) << 16);
  RawCode >>= (BitOffset % 8);
  BitOffset += CodeSize;

  return(RawCode & ReadMask);
}

image_t*
gifLoadFile(FILE *f)
{
    size_t filesize;

    // find the size of the file
    fseek(f, 0L, SEEK_SET); // unnecessary
    fseek(f, 0L, SEEK_END);
    filesize= ftell(f);
    fseek(f, 0L, SEEK_SET);

    u8 *buffer= (u8*)malloc (filesize+1024);
    if ( ! buffer)
    {
        NOTIFY("pngLoadFile: run out of memory\n");
        return NULL;
    }

    // fill buffer
    if (fread (buffer, 1, filesize, f) != filesize)
    {
        NOTIFY("gifLoadFile: error reading %d bytes from file [%s]\n",
               filesize,
               strerror(errno)
              );
        free(buffer);
        return NULL;
    }

    gif_t gif;
    gif_t::gifRes_e res= gif.load(buffer, filesize);

    if (res != gif_t::GIFOK)
    {
        char sres[1024];
        gif.printError(res, sres);
        NOTIFY("gifLoadFile: failed to load file, %s\n", sres);

        free(buffer);

        return NULL;
    }

    decodedGif_t *b= gif.decodeToRGB24();

    image_t *img= new image_t(b->b,
                              3 * b->w * b->h,
                              RGB24_FORMAT,
                              b->w,
                              b->h,
                              0
                             );

    free(b->b);
    delete b;

    free(buffer);

    return img;
}

