#ifndef __JPEG_H__
#define __JPEG_H__

#include "defines.h"

#ifdef __BUILD_FOR_LINUX
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <X11/Xmd.h>
extern "C"
{
#include <jpeglib.h>
}
#endif
#ifdef __BUILD_FOR_WINXP
#include <stdio.h>
extern "C"
{
#include "./jpeglib/jpeglib.h"
}

#include "rfbproto.h"
#endif

#include "locker.h"

namespace libvnc
{

class CRfbProtocol;

class CJpeg
{
private:
        
    CRfbProtocol* rfb;
    static CPthreadMutex * mutex;
    static int jpegError;
    static struct jpeg_source_mgr jpegSrcManager;
    static JOCTET *jpegBufferPtr;
    static size_t  jpegBufferLen;

    void JpegSetSrcManager(j_decompress_ptr cinfo, CARD8 *compressedData,int compressedLen);
    static void JpegInitSource(j_decompress_ptr cinfo);
    static boolean JpegFillInputBuffer(j_decompress_ptr cinfo);
    static void JpegSkipInputData(j_decompress_ptr cinfo, long num_bytes);
    static void JpegTermSource(j_decompress_ptr cinfo);

public:

    CJpeg(CRfbProtocol* rfb);
    ~CJpeg(void);
        
    int DecompressJpegRect(char * buffer, VncOptions& opt, int x, int y, int w, int h);
    long ReadCompactLen ();
};

}

#endif