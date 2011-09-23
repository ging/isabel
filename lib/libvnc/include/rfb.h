#ifndef __FRB_H__
#define __FRB_H__

#include "image.h"
#include "jpeg.h"
#include "tight.h"
#include "rfbproto.h"

namespace libvnc
{

class CRfbProtocol
{
public:
    static const int DEFAULT_BUFFER_SIZE = 640*480*3;

private:
        
    bool m_Connected;
    int m_Sock;
    unsigned int m_BufSize;    
    char *m_pBuf;
    char *m_pBufOut;
    unsigned int m_nBuffered;

    CTight * m_pTight;    
    CImage * m_pImage;
    
private:

    HRESULT NegotiateProtocol(void);
    HRESULT Authenticate( VncOptions& opt);
    HRESULT InitClient( VncOptions& opt);
    HRESULT InitServer(VncOptions& opt);

    HRESULT HandleRawEncodedMessage(rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt);
    HRESULT HandleCopyrectEncodedMessage(rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt);
    HRESULT HandleRreEncodedMessage(rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt);
    HRESULT HandleCorreEncodedMessage(rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt);
    HRESULT HandleHextileEncodedMessage(char* buffer, rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt);        
    
public:

    CRfbProtocol();
    ~CRfbProtocol();

    HRESULT RecvFromRfbServer(char *out, unsigned int n);
    HRESULT SendToRfbServer(char *buf, unsigned int n);
    
    HRESULT ConnectTo(const char *host, int port);
    HRESULT Disconnect(void);
    HRESULT InitConnection(VncOptions& opt);    
    HRESULT SetFormatAndEncodings(VncOptions& opt);
    HRESULT SendUpdateRequest(int incremental, VncOptions &opt);
    HRESULT HandleServerMessages(VncOptions& opt);
    HRESULT UpdateMouse(MouseState& mousestate, VncOptions& opt);
    HRESULT SendKeyEvent(int key, int down_flag);
    void GetRgbFromData(int *r, int *g, int *b, char *data, VncOptions &opt);
    CImage * GetImage(void);
    inline bool IsConnected(void){ return m_Connected; }   
};

}

#endif