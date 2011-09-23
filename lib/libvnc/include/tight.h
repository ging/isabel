#ifndef __TIGHT_H__
#define __TIGHT_H__

#include "defines.h"
#include "rfbproto.h"
#ifdef __BUILD_FOR_WINXP
#include ".\zlib\zlib.h"
#else
#include <zlib.h>
#endif

namespace libvnc
{

class CRfbProtocol;
class CJpeg;

class CTight
{
private:    

    CRfbProtocol * m_pRfb;
    CJpeg        * m_pJpeg;
    /*
    * Variables for the ``tight'' encoding implementation.
    */
    static const int TIGHT_MIN_TO_COMPRESS = 12;
    static const int ZLIB_BUFFER_SIZE = 512;

    char m_pZlibBuff[ZLIB_BUFFER_SIZE];

    /* Four independent compression streams for zlib library. */
    z_stream m_ZlibStream[4];
    int m_ZlibStreamActive[4];

    /* Filter stuff. Should be initialized by filter initialization code. */
    int m_RectWidth, m_RectColors;
    char m_TightPalette[256*4];
    CARD8 m_TightPrevRow[2048*3*sizeof(CARD16)];

    /* zlib stuff */
    int m_RawBufferSize;
    char *m_pRawBuffer;
    z_stream m_DecompStream;
    int m_DecompStreamInited;

private:

    typedef void (CTight:: *filterPtr)(int, void *, void *,VncOptions&);

    int InitFilterCopy (int rw, int rh, int bpp);
    int InitFilterPalette (int rw, int rh, int bpp);
    int InitFilterGradient (int rw, int rh, int bpp);
    void FilterCopy (int numRows, void* srcBuffer, void *destBuffer, VncOptions &opt);
    void FilterPalette (int numRows, void* srcBuffer, void *destBuffer, VncOptions &opt);
    void FilterGradient (int numRows, void* srcBuffer, void *destBuffer, VncOptions &opt);

public:

    CTight(CRfbProtocol * rfb);
    ~CTight(void);    

    int HandleTightEncodedMessage(char * buffer, rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt);
    int HandleZlibEncodedMessage(char * buffer, rfbFramebufferUpdateRectHeader rectheader, VncOptions &opt);
};

}

#endif