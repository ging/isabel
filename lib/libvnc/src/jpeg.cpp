#include "defines.h"
#include "jpeg.h"
#include "rfb.h"
#include <stdlib.h>

using namespace libvnc;

int CJpeg::jpegError;
struct jpeg_source_mgr CJpeg::jpegSrcManager;
JOCTET *CJpeg::jpegBufferPtr;
size_t  CJpeg::jpegBufferLen;
CPthreadMutex * CJpeg::mutex = new CPthreadMutex();

CJpeg::CJpeg(CRfbProtocol* rfb)
{
    this->rfb = rfb;
}

CJpeg::~CJpeg()
{

}

void
CJpeg::JpegInitSource(j_decompress_ptr cinfo)
{
    jpegError = 0;
}

boolean
CJpeg::JpegFillInputBuffer(j_decompress_ptr cinfo)
{
    jpegError = 1;
    jpegSrcManager.bytes_in_buffer = jpegBufferLen;
    jpegSrcManager.next_input_byte = jpegBufferPtr;
    return TRUE;
}

void
CJpeg::JpegSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
    if (num_bytes < 0 || (size_t)num_bytes > jpegSrcManager.bytes_in_buffer) {
        jpegError = 1;
        jpegSrcManager.bytes_in_buffer = (int)(jpegBufferLen);
        jpegSrcManager.next_input_byte = (JOCTET *)jpegBufferPtr;
    } else {
        jpegSrcManager.next_input_byte += (size_t) num_bytes;
        jpegSrcManager.bytes_in_buffer -= (size_t) num_bytes;
    }
}

void
CJpeg::JpegTermSource(j_decompress_ptr cinfo)
{
    /* No work necessary here. */
}

void
CJpeg::JpegSetSrcManager(j_decompress_ptr cinfo, CARD8 *compressedData,
                         int compressedLen)
{    
    jpegBufferPtr = (JOCTET *)compressedData;
    jpegBufferLen = (size_t)compressedLen;

    jpegSrcManager.init_source = JpegInitSource;
    jpegSrcManager.fill_input_buffer = JpegFillInputBuffer;
    jpegSrcManager.skip_input_data = JpegSkipInputData;
    jpegSrcManager.resync_to_restart = jpeg_resync_to_restart;
    jpegSrcManager.term_source = JpegTermSource;
    jpegSrcManager.next_input_byte = jpegBufferPtr;
    jpegSrcManager.bytes_in_buffer = (int)jpegBufferLen;

    cinfo->src = &jpegSrcManager;
}

int
CJpeg::DecompressJpegRect(char * buffer, VncOptions& opt, int x, int y, int w, int h)
{
    CLocker lock(*mutex);
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    int compressedLen;
    CARD8*  compressedData;
    CARD16* pixelPtr;
    JSAMPROW rowPointer[1];
    int dx, dy;

    compressedLen = (int)ReadCompactLen();
    if (compressedLen <= 0) {
        fprintf(stderr, "Incorrect data received from the server.\n");
        return 0;
    }

    compressedData = new CARD8[compressedLen];
    if (compressedData == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return 0;
    }

    if (FAILED(rfb->RecvFromRfbServer((char*)compressedData, compressedLen)))
    {
        delete[] compressedData;
        return 0;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    JpegSetSrcManager(&cinfo, compressedData, compressedLen);

    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_RGB;

    jpeg_start_decompress(&cinfo);
    if (cinfo.output_width != w || cinfo.output_height != h ||
        cinfo.output_components != 3) { 
            fprintf(stderr, "Tight Encoding: Wrong JPEG data received.\n");
            jpeg_destroy_decompress(&cinfo);
            delete[] compressedData;
            return 0;
    }

    rowPointer[0] = (JSAMPROW)buffer;
    dy = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, rowPointer, 1);
        if (jpegError) {
            break;
        }
        /* FIXME 16 bpp hardcoded */
        /* Fill the second half of our global buffer with the uncompressed data */
        pixelPtr = (CARD16 *)&buffer[CRfbProtocol::DEFAULT_BUFFER_SIZE/2];
        for (dx = 0; dx < w; dx++) {
            *pixelPtr++ =
                RGB24_TO_PIXEL(16, buffer[dx*3], buffer[dx*3+1], buffer[dx*3+2]);
        }

        /* write scanline to screen */
        rfb->GetImage()->PutImage((u8*)&buffer[CRfbProtocol::DEFAULT_BUFFER_SIZE/2], x, y + dy, w, 1, CImage::RGB565);
        //dfb_write_data_to_screen(x, y + dy, w, 1,);
        dy++;
    }

    if (!jpegError)
        jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);
    delete[] compressedData;

    return !jpegError;
}

long
CJpeg::ReadCompactLen (void)
{    
    long len;
    CARD8 b;

    if (FAILED(rfb->RecvFromRfbServer((char *)&b, 1)))
    {
        return -1;
    }
    len = (int)b & 0x7F;
    if (b & 0x80) {
        if (FAILED(rfb->RecvFromRfbServer((char *)&b, 1)))
        {
            return -1;
        }
        len |= ((int)b & 0x7F) << 7;
        if (b & 0x80) {
            if (FAILED(rfb->RecvFromRfbServer((char *)&b, 1)))
            {
                return -1;
            }
            len |= ((int)b & 0xFF) << 14;
        }
    }
    return len;
}


