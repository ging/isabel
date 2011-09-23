
#include "defines.h"

#ifdef __BUILD_FOR_LINUX
#include <unistd.h>
#include <string.h>
#endif

#include "tight.h"
#include "rfb.h"
#include "jpeg.h"

using namespace libvnc;

CTight::CTight(CRfbProtocol * rfb)
{
    m_pRfb = rfb;
    m_pJpeg = new CJpeg(rfb);

    memset(&m_DecompStream, 0, sizeof(z_stream));
    m_pRawBuffer = NULL;
    m_RawBufferSize = -1;
    m_DecompStreamInited = 0;
    memset(m_ZlibStreamActive,0,4);    
}

CTight::~CTight(void)
{
    delete m_pJpeg;
}

int
CTight::HandleZlibEncodedMessage(char * buffer, rfbFramebufferUpdateRectHeader rectheader, VncOptions &opt)
{
    rfbZlibHeader hdr;
    int remaining;
    int inflateResult;
    int toRead;

    /* First make sure we have a large enough raw buffer to hold the
    * decompressed data.  In practice, with a fixed BPP, fixed frame
    * buffer size and the first update containing the entire frame
    * buffer, this buffer allocation should only happen once, on the
    * first update.
    */
    if ( m_RawBufferSize < (( rectheader.r.w * rectheader.r.h ) 
        * ( opt.client.bpp / 8 ))) {

            if ( m_pRawBuffer != NULL ) 
            {
                delete[] m_pRawBuffer;
            }

            m_RawBufferSize = (( rectheader.r.w * rectheader.r.h ) 
                * (  opt.client.bpp / 8 ));
            m_pRawBuffer = new char[m_RawBufferSize];

    }

    if (FAILED(m_pRfb->RecvFromRfbServer((char *)&hdr, sz_rfbZlibHeader)))
    {
        return 0;
    }

    remaining = Swap32IfLE(hdr.nBytes);

    /* Need to initialize the decompressor state. */
    m_DecompStream.next_in   = ( Bytef * )buffer;
    m_DecompStream.avail_in  = 0;
    m_DecompStream.next_out  = ( Bytef * )m_pRawBuffer;
    m_DecompStream.avail_out = m_RawBufferSize;
    m_DecompStream.data_type = Z_BINARY;

    /* Initialize the decompression stream structures on the first invocation. */
    if ( m_DecompStreamInited == 0 ) {

        inflateResult = inflateInit( &m_DecompStream );

        if ( inflateResult != Z_OK ) {
            fprintf(stderr,
                "inflateInit returned error: %d, msg: %s\n",
                inflateResult,
                m_DecompStream.msg);
            return 0;
        }

        m_DecompStreamInited = 1;

    }

    inflateResult = Z_OK;

    /* Process buffer full of data until no more to process, or
    * some type of inflater error, or Z_STREAM_END.
    */
    while (( remaining > 0 ) &&
        ( inflateResult == Z_OK )) {

            if ( remaining > CRfbProtocol::DEFAULT_BUFFER_SIZE ) {
                toRead = CRfbProtocol::DEFAULT_BUFFER_SIZE;
            }
            else {
                toRead = remaining;
            }

            /* Fill the buffer, obtaining data from the server. */
            if (FAILED(m_pRfb->RecvFromRfbServer(buffer,toRead)))
            {
                return 0;
            }

            m_DecompStream.next_in  = ( Bytef * )buffer;
            m_DecompStream.avail_in = toRead;

            /* Need to uncompress buffer full. */
            inflateResult = inflate( &m_DecompStream, Z_SYNC_FLUSH );

            /* We never supply a dictionary for compression. */
            if ( inflateResult == Z_NEED_DICT ) {
                fprintf(stderr,"zlib inflate needs a dictionary!\n");
                return 0;
            }
            if ( inflateResult < 0 ) {
                fprintf(stderr,
                    "zlib inflate returned error: %d, msg: %s\n",
                    inflateResult,
                    m_DecompStream.msg);
                return 0;
            }

            /* Result buffer allocated to be at least large enough.  We should
            * never run out of space!
            */
            if (( m_DecompStream.avail_in > 0 ) &&
                ( m_DecompStream.avail_out <= 0 )) {
                    fprintf(stderr,"zlib inflate ran out of space!\n");
                    return 0;
            }

            remaining -= toRead;

    } /* while ( remaining > 0 ) */

    if ( inflateResult == Z_OK ) {

        /* Put the uncompressed contents of the update on the screen. */
        m_pRfb->GetImage()->PutImage((unsigned char*)m_pRawBuffer,rectheader.r.x,rectheader.r.y,rectheader.r.w,rectheader.r.h,CImage::RGB565);
        /*dfb_write_data_to_screen(
        rectheader.r.x, 
        rectheader.r.y, 
        rectheader.r.w, 
        rectheader.r.h, 
        m_pRawBuffer
        );*/
    }else{

        fprintf(stderr,
            "zlib inflate returned error: %d, msg: %s\n",
            inflateResult,
            m_DecompStream.msg);
        return 0;

    }

    return 1;
}

int
CTight::HandleTightEncodedMessage(char * buffer, rfbFramebufferUpdateRectHeader rectheader, VncOptions &opt)
{
    CARD8 comp_ctl;
    CARD8 filter_id;
    CARD16 fill_colour;
    int r=0, g=0, b=0;
    filterPtr filterFn;
    int err, stream_id, compressedLen, bitsPixel;
    int bufferSize, rowSize, numRows, portionLen, rowsProcessed, extraBytes;
    void *dst;
    z_streamp zs;

    /* read the compression type */
    if (FAILED(m_pRfb->RecvFromRfbServer((char*)&comp_ctl, 1)))
    {
        return 0;
    }

    /* The lower 4 bits are apparently used as active flags for the zlib
    * streams. Iterate over them and right shift 1, so the encoding ends up in
    * the first 4 bits. */
    for (stream_id = 0; stream_id < 4; stream_id++) {
        if ((comp_ctl & 1) && m_ZlibStreamActive[stream_id]) {
            if (inflateEnd (&m_ZlibStream[stream_id]) != Z_OK &&
                m_ZlibStream[stream_id].msg != NULL)
                printf("inflateEnd: %s\n", m_ZlibStream[stream_id].msg);
            m_ZlibStreamActive[stream_id] = 0;
        }
        comp_ctl >>= 1;
    }

    /* Handle solid rectangles. */
    if (comp_ctl == rfbTightFill) {

        if (FAILED(m_pRfb->RecvFromRfbServer((char*)&fill_colour, sizeof(fill_colour))))
        {
            return 0;
        }
        m_pRfb->GetRgbFromData(&r, &g, &b, (char*)&fill_colour,opt);
        m_pRfb->GetImage()->DrawRect(rectheader.r.x,rectheader.r.y,rectheader.r.w,rectheader.r.h,r,g,b);
        /*dfb_draw_rect_with_rgb(
        rectheader.r.x,
        rectheader.r.y,
        rectheader.r.w,
        rectheader.r.h,
        r,g,b
        );*/
        return 1;
    }

    /* Handle jpeg compressed rectangle */
    if (comp_ctl == rfbTightJpeg) {
        return m_pJpeg->DecompressJpegRect(
            buffer, opt,
            rectheader.r.x, 
            rectheader.r.y, 
            rectheader.r.w, 
            rectheader.r.h);
    }

    /* Quit on unsupported subencoding value. */
    if (comp_ctl > rfbTightMaxSubencoding) {
        printf("Tight encoding: bad subencoding value received.\n");
        return 0;
    }

    /*
    * Here primary compression mode handling begins.
    * Data was processed with optional filter + zlib compression.
    */

    /* First, we should identify a filter to use. */
    if ((comp_ctl & rfbTightExplicitFilter) != 0) {
        if (FAILED(m_pRfb->RecvFromRfbServer((char*)&filter_id, 1)))
        {
            return 0;
        }
        switch (filter_id) {
    case rfbTightFilterCopy:
        filterFn = &CTight::FilterCopy;
        bitsPixel = InitFilterCopy(rectheader.r.w, rectheader.r.h,opt.client.bpp);
        break;
    case rfbTightFilterPalette:
        filterFn = &CTight::FilterPalette;
        bitsPixel = InitFilterPalette(rectheader.r.w, rectheader.r.h,opt.client.bpp);
        break;
    case rfbTightFilterGradient:
        filterFn = &CTight::FilterGradient;
        bitsPixel = InitFilterGradient(rectheader.r.w, rectheader.r.h,opt.client.bpp);
        break;
    default:
        printf("Tight encoding: unknown filter code received.\n");
        return 0;
        }
    } else {
        filterFn = &CTight::FilterCopy;
        bitsPixel = InitFilterCopy(rectheader.r.w, rectheader.r.h,opt.client.bpp);
    }
    if (bitsPixel == 0) {
        printf("Tight encoding: error receiving palette.\n");
        return 0;
    }

    /* Determine if the data should be decompressed or just copied. */
    rowSize = (rectheader.r.w * bitsPixel + 7) / 8;

    /* rect is to small to be compressed reasonably, simply copy */
    if (rectheader.r.h * rowSize < TIGHT_MIN_TO_COMPRESS) {
        if (FAILED(m_pRfb->RecvFromRfbServer((char*)buffer, rectheader.r.h * rowSize)))
        {
            return 0;
        }

        dst = (void *) &buffer[TIGHT_MIN_TO_COMPRESS * 4];
        (this->*filterFn)(rectheader.r.h, buffer, dst, opt);
        m_pRfb->GetImage()->PutImage((unsigned char*)dst,rectheader.r.x,rectheader.r.y,rectheader.r.w,rectheader.r.h,CImage::RGB565);
        /*dfb_write_data_to_screen(
        rectheader.r.x, 
        rectheader.r.y, 
        rectheader.r.w, 
        rectheader.r.h, 
        dst
        );*/

        return 1;
    }

    /* Read the length (1..3 bytes) of compressed data following. */
    compressedLen = (int)m_pJpeg->ReadCompactLen();
    if (compressedLen <= 0) {
        printf("Incorrect data received from the server.\n");
        return 0;
    }

    /* Now let's initialize compression stream if needed. */
    stream_id = comp_ctl & 0x03;
    zs = &m_ZlibStream[stream_id];
    if (!m_ZlibStreamActive[stream_id]) {
        zs->zalloc = Z_NULL;
        zs->zfree = Z_NULL;
        zs->opaque = Z_NULL;
        err = inflateInit(zs);
        if (err != Z_OK) {
            if (zs->msg != NULL)
                printf("InflateInit error: %s.\n", zs->msg);
            return 0;
        }
        m_ZlibStreamActive[stream_id] = 1;
    }

    /* Read, decode and draw actual pixel data in a loop. */
    bufferSize = 640*480 * bitsPixel / (bitsPixel + opt.client.bpp) & 0xFFFFFFFC;
    dst = &buffer[bufferSize];
    if (rowSize > bufferSize) {
        /* Should be impossible when BUFFER_SIZE >= 16384 */
        printf("Internal error: incorrect buffer size.\n");
        return 0;
    }

    rowsProcessed = 0;
    extraBytes = 0;

    while (compressedLen > 0) {
        if (compressedLen > ZLIB_BUFFER_SIZE)
            portionLen = ZLIB_BUFFER_SIZE;
        else
            portionLen = compressedLen;

        if (FAILED(m_pRfb->RecvFromRfbServer((char*)m_pZlibBuff, portionLen)))
        {
            return 0;
        }
        compressedLen -= portionLen;

        zs->next_in = (Bytef *)m_pZlibBuff;
        zs->avail_in = portionLen;

        do {
            zs->next_out = (Bytef *)&buffer[extraBytes];
            zs->avail_out = bufferSize - extraBytes;

            err = inflate(zs, Z_SYNC_FLUSH);
            if (err == Z_BUF_ERROR)   /* Input exhausted -- no problem. */
                break;
            if (err != Z_OK && err != Z_STREAM_END) {
                if (zs->msg != NULL) {
                    printf("Inflate error: %s.\n", zs->msg);
                } else {
                    printf("Inflate error: %d.\n", err);
                }
                return 0;
            }

            numRows = (bufferSize - zs->avail_out) / rowSize;

            (this->*filterFn)(numRows, buffer, dst, opt);

            extraBytes = bufferSize - zs->avail_out - numRows * rowSize;
            if (extraBytes > 0)
                memcpy(buffer, &buffer[numRows * rowSize], extraBytes);

            m_pRfb->GetImage()->PutImage((unsigned char*)dst,rectheader.r.x,rectheader.r.y + rowsProcessed,rectheader.r.w,numRows,CImage::RGB565);
            /*dfb_write_data_to_screen(
            rectheader.r.x, 
            rectheader.r.y + rowsProcessed, 
            rectheader.r.w, 
            numRows, 
            dst);*/
            rowsProcessed += numRows;
        }
        while (zs->avail_out == 0);
    }

    if (rowsProcessed != rectheader.r.h) {
        printf("Incorrect number of scan lines after decompression.\n");
        return 0;
    }

    return 1;
}

int
CTight::InitFilterCopy (int rw, int rh, int bpp)
{
    m_RectWidth = rw;
    return bpp;
}

void
CTight::FilterCopy (int numRows, void *src, void *dst, VncOptions& opt)
{
    memcpy (dst, src, numRows * m_RectWidth * (opt.client.bpp / 8));
}

int
CTight::InitFilterGradient (int rw, int rh, int bpp)
{
    int bits;
    bits = InitFilterCopy(rw, rh, bpp);
    memset(m_TightPrevRow, 0, rw * 3 * sizeof(CARD16));
    return bits;
}

void
CTight::FilterGradient (int numRows, void* buffer, void *buffer2, VncOptions &opt)
{
    int x, y, c;
    CARD16 *src = (CARD16 *)buffer;
    CARD16 *dst = (CARD16 *)buffer2;
    CARD16 *thatRow = (CARD16 *)m_TightPrevRow;
    CARD16 thisRow[2048*3];
    CARD16 pix[3];
    CARD16 max[3];
    int shift[3];
    int est[3];


    max[0] = opt.client.redmax;
    max[1] = opt.client.greenmax;
    max[2] = opt.client.bluemax;

    shift[0] = opt.client.redshift;
    shift[1] = opt.client.greenshift;
    shift[2] = opt.client.blueshift;

    for (y = 0; y < numRows; y++) {

        /* First pixel in a row */
        for (c = 0; c < 3; c++) {
            pix[c] = (CARD16)(((src[y*m_RectWidth] >> shift[c]) + thatRow[c]) & max[c]);
            thisRow[c] = pix[c];
        }
        dst[y*m_RectWidth] = RGB24_TO_PIXEL(opt.client.bpp, pix[0], pix[1], pix[2]);

        /* Remaining pixels of a row */
        for (x = 1; x < m_RectWidth; x++) {
            for (c = 0; c < 3; c++) {
                est[c] = (int)thatRow[x*3+c] + (int)pix[c] - (int)thatRow[(x-1)*3+c];
                if (est[c] > (int)max[c]) {
                    est[c] = (int)max[c];
                } else if (est[c] < 0) {
                    est[c] = 0;
                }
                pix[c] = (CARD16)(((src[y*m_RectWidth+x] >> shift[c]) + est[c]) & max[c]);
                thisRow[x*3+c] = pix[c];
            }
            dst[y*m_RectWidth+x] = RGB24_TO_PIXEL(opt.client.bpp, pix[0], pix[1], pix[2]);
        }
        memcpy(thatRow, thisRow, m_RectWidth * 3 * sizeof(CARD16));
    }
}

int
CTight::InitFilterPalette (int rw, int rh, int bpp)
{
    CARD8 numColors;
    m_RectWidth = rw;

    if ((m_pRfb->RecvFromRfbServer((char*)&numColors, 1)))
    {
        return 0;
    }
    m_RectColors = (int)numColors;
    if (++m_RectColors < 2)
        return 0;

    if (FAILED(m_pRfb->RecvFromRfbServer((char*)&m_TightPalette, m_RectColors * (bpp / 8))))
    {
        return 0;
    }
    return (m_RectColors == 2) ? 1 : 8;
}

void
CTight::FilterPalette (int numRows, void *buffer, void *buffer2, VncOptions& opt)
{
    /* FIXME works only in 16 bpp. The dst and palette pointers would need to
    * be adjusted in size. Maybe after dinner ;) */
    int x, y, b, w;
    CARD8 *src = (CARD8 *)buffer;
    CARD16 *dst = (CARD16 *)buffer2;
    CARD16 *palette = (CARD16 *)m_TightPalette;

    if (m_RectColors == 2) {
        w = (m_RectWidth + 7) / 8;
        for (y = 0; y < numRows; y++) {
            for (x = 0; x < m_RectWidth / 8; x++) {
                for (b = 7; b >= 0; b--)
                    dst[y*m_RectWidth+x*8+7-b] = palette[src[y*w+x] >> b & 1];
            }
            for (b = 7; b >= 8 - m_RectWidth % 8; b--) {
                dst[y*m_RectWidth+x*8+7-b] = palette[src[y*w+x] >> b & 1];
            }
        }
    } else {
        for (y = 0; y < numRows; y++)
            for (x = 0; x < m_RectWidth; x++)
                dst[y*m_RectWidth+x] = palette[(int)src[y*m_RectWidth+x]];
    }
}

