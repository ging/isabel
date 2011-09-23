#include "defines.h"

#ifdef __BUILD_FOR_LINUX
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <zlib.h>
#endif
#ifdef __BUILD_FOR_WINXP
#include "xmd.h"
#include <errno.h>
#include ".\zlib\zlib.h"
#include <winsock2.h>
#endif

#include "rfb.h"
#include <math.h>
#include <stdio.h>
#include "vncauth.h"
#include "extras.h"
#include <memory.h>
#include <stdlib.h>

#define OPER_SAVE     0
#define OPER_RESTORE  1

#ifdef __BUILD_FOR_WINXP
#pragma comment(lib,"ws2_32.lib")
#define close(x) closesocket(x)
#define EWOULDBLOCK             WSAEWOULDBLOCK
#endif

using namespace libvnc;
CRfbProtocol::CRfbProtocol(void)
{    
    m_Connected= false;
    m_BufSize = DEFAULT_BUFFER_SIZE;
    m_pBuf = new char[m_BufSize];

    m_pImage = NULL;    
    m_pTight = new CTight(this);

    m_Sock = 0;
    m_pBufOut = m_pBuf;
    m_nBuffered = 0;    
}

CRfbProtocol::~CRfbProtocol(void)
{
    Disconnect();
    delete[] m_pBuf;
    delete m_pTight;
    if (m_pImage)
    {
        delete m_pImage;
    }
}

/*
* ConnectToRFBServer.
*/

HRESULT 
CRfbProtocol::Disconnect(void)
{
    HRESULT ret = S_OK;
    printf("Disconnect: closing connection...\n");
    m_Connected= false;
    if (close(m_Sock)!=0)
    {
        ret = E_FAIL;
    }
    return ret;
}

HRESULT 
CRfbProtocol::ConnectTo(const char *host, int port)
{
	struct hostent *he=NULL;
    int one=1;
    struct sockaddr_in s;

    if ((m_Sock = (int)socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
#ifdef __BUILD_FOR_WINXP
        int error = WSAGetLastError();
#endif
#ifdef __BUILD_FOR_LINUX
        int error = errno;
#endif
        printf("Error creating communication socket: %d\n", error);
        return E_FAIL;
    }

    /* if the server wasnt specified as an ip address, look it up */
    if (!inet_aton(host, &s.sin_addr)) 
    {
        if ( (he = gethostbyname(host)) )
        {
            memcpy (&s.sin_addr.s_addr, he->h_addr, he->h_length);
        }else{
            printf("Couldnt resolve host!\n");
            Disconnect();
            return E_FAIL;
        }
    }

    s.sin_port = htons(port);
    s.sin_family = AF_INET;

    if (connect(m_Sock,(struct sockaddr*) &s, sizeof(s)) < 0)
    {
        printf("Connect error\n");
        Disconnect();
        return E_FAIL;
    }
    if (setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(one)) < 0)
    {
        printf("Error setting socket options\n");
        Disconnect();
        return E_FAIL;
    }

    //int timeout = 500;
    //if (setsockopt(m_Sock, SOL_SOCKET , SO_RCVTIMEO, (char *)&timeout, sizeof(int)) < 0)
    //{
    //    printf("Error setting socket options\n");
    //    Disconnect();
    //    return E_FAIL;
    //}
    m_Connected= true;
    return S_OK;
}

HRESULT
CRfbProtocol::InitConnection (VncOptions& opt)   
{
    if (FAILED(NegotiateProtocol())) return E_FAIL;
    if (FAILED(Authenticate(opt))) return E_FAIL;
    if (FAILED(InitClient(opt))) return E_FAIL;
    if (FAILED(InitServer(opt))) return E_FAIL;
    if (m_pImage)
    {
        delete m_pImage;
    }
    m_pImage = new CImage(opt.server.width,opt.server.height);
    return S_OK;
}

HRESULT
CRfbProtocol::NegotiateProtocol(void)
{
    rfbProtocolVersionMsg msg;

    /* read the protocol version the server uses */
    if (FAILED(RecvFromRfbServer( (char*)&msg, sz_rfbProtocolVersionMsg)))
    {
        return E_FAIL;
    }
    /* FIXME actually do something with that information ;) */

    /* send the protocol version we want to use */
    sprintf(msg, rfbProtocolVersionFormat, 
            rfbProtocolMajorVersion, 
            rfbProtocolMinorVersion);
    if (FAILED(SendToRfbServer ( msg, sz_rfbProtocolVersionMsg)))
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT
CRfbProtocol::Authenticate(VncOptions& opt)
{
    CARD32 authscheme;
    if (FAILED(RecvFromRfbServer((char *)&authscheme, 4)))
    {
        return E_FAIL;
    }
    authscheme = Swap32IfLE(authscheme);
    switch (authscheme)
    {
        CARD32 reason_length;
        CARD8 *reason_string;
        CARD8 challenge_and_response[CVncAuth::CHALLENGESIZE];
        CARD32 auth_result;

    case rfbConnFailed:	 	
        printf("DIRECTVNC: Connection to VNC server failed\n");
        if (FAILED(RecvFromRfbServer((char *)&reason_length, 4))){  return E_FAIL; }
        reason_length = Swap32IfLE(reason_length);
        reason_string = new CARD8[reason_length];
        if (FAILED(RecvFromRfbServer((char *)reason_string, reason_length))) { return E_FAIL; }
        printf("Errormessage: %s\n", reason_string); 
        return E_FAIL;
    case rfbVncAuth:
        /* we didnt get a password on the command line, so go get one */
        if (opt.password.length() == 0)
        {
            printf("No password!\n");   
        }
        if (FAILED(RecvFromRfbServer((char*)challenge_and_response, CVncAuth::CHALLENGESIZE)))
        {
            return E_FAIL;
        }
        CVncAuth::vncEncryptBytes(challenge_and_response, opt.password.c_str());
        if (FAILED(SendToRfbServer((char*)challenge_and_response, CVncAuth::CHALLENGESIZE)))
        {
            return E_FAIL;
        }
        if (FAILED(RecvFromRfbServer((char*)&auth_result, 4)))
        {
            return E_FAIL;
        }
        auth_result = Swap32IfLE(auth_result);
        switch (auth_result)
        {
        case rfbVncAuthFailed:
            printf("Authentication Failed\n");
            return E_FAIL;
        case rfbVncAuthTooMany:
            printf("Too many connections\n");
            return E_FAIL;
        case rfbVncAuthOK:
            printf("Authentication OK\n");
            break;
        }
        break;
    case rfbNoAuth:
        break;
    } 
    return S_OK;
}

HRESULT
CRfbProtocol::InitClient(VncOptions& opt)
{
    rfbClientInitMsg cl;
    cl.shared = opt.shared;
    if (FAILED(SendToRfbServer((char *) &cl, sz_rfbClientInitMsg)))
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT
CRfbProtocol::InitServer(VncOptions& opt)
{
    HRESULT ret = S_OK;
    int len;
    rfbServerInitMsg si;

    if (FAILED(RecvFromRfbServer((char *)&si, sz_rfbServerInitMsg)))
    {
        return E_FAIL;
    }
    opt.server.width = Swap16IfLE(si.framebufferWidth);
    opt.server.height = Swap16IfLE(si.framebufferHeight);
    opt.server.bpp = si.format.bitsPerPixel;
    opt.server.depth = si.format.bigEndian;
    opt.server.truecolour = si.format.trueColour;

    opt.server.redmax = Swap16IfLE(si.format.redMax);
    opt.server.greenmax = Swap16IfLE(si.format.greenMax);
    opt.server.bluemax = Swap16IfLE(si.format.blueMax);
    opt.server.redshift = si.format.redShift;
    opt.server.greenshift = si.format.greenShift;
    opt.server.blueshift = si.format.blueShift;

    len = Swap32IfLE(si.nameLength);

    char * server_name = new char[len+1];
    if (FAILED(RecvFromRfbServer( server_name,len)))
    {
        opt.server.name = server_name;
        ret = E_FAIL;
    }
    delete[] server_name;        
    return ret;
}

HRESULT
CRfbProtocol::SetFormatAndEncodings(VncOptions& opt)
{
    int num_enc =0;
    rfbSetPixelFormatMsg pf;
    rfbSetEncodingsMsg em;
    CARD32 enc[MAX_ENCODINGS];

    pf.type = 0;
    pf.format.bitsPerPixel = opt.client.bpp;
    pf.format.depth = opt.client.depth;
    pf.format.bigEndian = opt.client.bigendian;
    pf.format.trueColour = opt.client.truecolour;
    pf.format.redMax = Swap16IfLE(opt.client.redmax);
    pf.format.greenMax = Swap16IfLE(opt.client.greenmax);
    pf.format.blueMax = Swap16IfLE(opt.client.bluemax);
    pf.format.redShift =opt.client.redshift;
    pf.format.greenShift = opt.client.greenshift;
    pf.format.blueShift = opt.client.blueshift;

    if (FAILED(SendToRfbServer( (char*)&pf, sz_rfbSetPixelFormatMsg)))
    {
        return E_FAIL;
    }

    em.type = rfbSetEncodings;
    /* figure out the encodings string given on the command line */
    for (std::vector<std::string>::iterator iter = opt.encodings.begin();
        iter != opt.encodings.end();++iter)
    {
        if (*iter == "raw")
        {
            enc[num_enc++] = Swap32IfLE(rfbEncodingRaw);
        }
        if (*iter == "tight")
        {
            enc[num_enc++] = Swap32IfLE(rfbEncodingTight);
        }
        if (*iter == "hextile")
        {
            enc[num_enc++] = Swap32IfLE(rfbEncodingHextile);
        }
        if (*iter == "zlib")
        {
            enc[num_enc++] = Swap32IfLE(rfbEncodingZlib);
        }
        else if (*iter == "copyrect")
        {
            enc[num_enc++] = Swap32IfLE(rfbEncodingCopyRect);
        }
        else if (*iter == "corre")
        {
            enc[num_enc++] = Swap32IfLE(rfbEncodingCoRRE);
        }
        else if (*iter == "rre")
        {
            enc[num_enc++] = Swap32IfLE(rfbEncodingRRE);
        }
        em.nEncodings = Swap16IfLE(num_enc);
    }
    if (!em.nEncodings)
    {
        enc[num_enc++] = Swap32IfLE(rfbEncodingTight);
        enc[num_enc++] = Swap32IfLE(rfbEncodingHextile);
        enc[num_enc++] = Swap32IfLE(rfbEncodingZlib);
        enc[num_enc++] = Swap32IfLE(rfbEncodingCopyRect);
        enc[num_enc++] = Swap32IfLE(rfbEncodingRRE);
        enc[num_enc++] = Swap32IfLE(rfbEncodingCoRRE); 
        enc[num_enc++] = Swap32IfLE(rfbEncodingRaw);
    }
    
    if (opt.client.compresslevel <= 9)
        enc[num_enc++] = Swap32IfLE(rfbEncodingCompressLevel0 + 
        opt.client.compresslevel);
    if (opt.client.quality <= 9)
        enc[num_enc++] = Swap32IfLE(rfbEncodingQualityLevel0 + 
        opt.client.quality);

    em.nEncodings = Swap16IfLE(num_enc);

    if (FAILED(SendToRfbServer((char*)&em, sz_rfbSetEncodingsMsg))){ return E_FAIL; }
    if (FAILED(SendToRfbServer((char*)&enc, num_enc * 4))){ return E_FAIL; }

    return S_OK;
}


HRESULT
CRfbProtocol::SendUpdateRequest(int incremental, VncOptions &opt)
{
    rfbFramebufferUpdateRequestMsg urq;

    urq.type = rfbFramebufferUpdateRequest;
    urq.incremental = incremental;
    urq.x = 0;
    urq.y = 0;
    urq.w = opt.server.width;
    urq.h = opt.server.height;

    urq.x = Swap16IfLE(urq.x);
    urq.y = Swap16IfLE(urq.y);
    urq.w = Swap16IfLE(urq.w);
    urq.h = Swap16IfLE(urq.h);

    if (FAILED(SendToRfbServer((char*)&urq, sz_rfbFramebufferUpdateRequestMsg)))
    {
        return E_FAIL;
    }
    return S_OK;
}


HRESULT
CRfbProtocol::HandleServerMessages(VncOptions& opt)
{
    char buffer[DEFAULT_BUFFER_SIZE];
    rfbServerToClientMsg msg;
    rfbFramebufferUpdateRectHeader rectheader;

    if (FAILED(RecvFromRfbServer((char*)&msg, 1)))
    {
        return E_FAIL;
    }
    switch (msg.type)
    {        
    case rfbFramebufferUpdate:
        if (FAILED(RecvFromRfbServer(((char*)&msg.fu)+1, sz_rfbFramebufferUpdateMsg-1)))
        {
            return E_FAIL;
        }
        msg.fu.nRects = Swap16IfLE(msg.fu.nRects);
        for (int i=0;i< msg.fu.nRects;i++)
        {
            if (FAILED(RecvFromRfbServer((char*)&rectheader,sz_rfbFramebufferUpdateRectHeader)))
            {
                return E_FAIL;
            }
            rectheader.r.x = Swap16IfLE(rectheader.r.x);
            rectheader.r.y = Swap16IfLE(rectheader.r.y);
            rectheader.r.w = Swap16IfLE(rectheader.r.w);
            rectheader.r.h = Swap16IfLE(rectheader.r.h);
            rectheader.encoding = Swap32IfLE(rectheader.encoding);
            switch (rectheader.encoding)
            {
            case rfbEncodingRaw:
                HandleRawEncodedMessage(rectheader,opt);		  
                break;
            case rfbEncodingCopyRect:
                HandleCopyrectEncodedMessage(rectheader,opt); 
                break;
            case rfbEncodingRRE:
                HandleRreEncodedMessage(rectheader,opt);
                break;
            case rfbEncodingCoRRE:
                HandleCorreEncodedMessage(rectheader,opt);
                break;
            case rfbEncodingHextile:
                HandleHextileEncodedMessage(buffer,rectheader,opt);
                break;
            case rfbEncodingTight:
                m_pTight->HandleTightEncodedMessage(buffer,rectheader, opt);
                break;
            case rfbEncodingZlib:
                m_pTight->HandleZlibEncodedMessage(buffer,rectheader,opt);
                break;
            case rfbEncodingRichCursor:
                printf("Encoding Rich Cursor: Unimplemented.\n");
                break;
            case rfbEncodingLastRect:
                printf("Encoding Last Rect\n");
                break;
            default:
                printf("Unknown encoding\n");
                return E_FAIL;
                break;
            }
        }
        break;
    case rfbSetColourMapEntries:
        printf("SetColourMapEntries. Unimplemented\n");
        RecvFromRfbServer(((char*)&msg.scme)+1, sz_rfbSetColourMapEntriesMsg-1);
        break;
    case rfbBell:
        printf("Bell message. Unimplemented.\n");
        break;
    case rfbServerCutText:
        printf("ServerCutText. Unimplemented.\n");
        break;
    default:
        printf("Unknown server message. Type: %i\n", msg.type);
        return E_FAIL;
        break;
    }
    return S_OK;
}

HRESULT
CRfbProtocol::UpdateMouse(MouseState& mousestate, VncOptions& opt)
{
    rfbPointerEventMsg msg;

    msg.type = rfbPointerEvent;
    msg.buttonMask = mousestate.buttonmask;

    /* scale to server resolution */
    msg.x = (CARD16)rint(mousestate.x * opt.h_ratio);
    msg.y = (CARD16)rint(mousestate.y * opt.v_ratio);
    msg.x = Swap16IfLE(msg.x);
    msg.y = Swap16IfLE(msg.y);

    return(SendToRfbServer( (char*)&msg, sz_rfbPointerEventMsg));
}

HRESULT
CRfbProtocol::SendKeyEvent(int key, int down_flag)
{
    rfbKeyEventMsg ke;

    ke.type = rfbKeyEvent;
    ke.down = down_flag;
    ke.key = Swap32IfLE(key);

    return (SendToRfbServer((char*)&ke, sz_rfbKeyEventMsg));
}

HRESULT
CRfbProtocol::HandleRawEncodedMessage(rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt)
{
    int size;
    char *buf;
    size = (opt.client.bpp/8 * rectheader.r.w) * rectheader.r.h;
    buf = new char[size];
    if (FAILED(RecvFromRfbServer(buf,size)))
    { 
        return E_FAIL;
    }
    m_pImage->PutImage((unsigned char*)buf,rectheader.r.x,rectheader.r.y,rectheader.r.w,rectheader.r.h, CImage::RGB565);    
    delete[] buf;
    return S_OK;
}

HRESULT
CRfbProtocol::HandleCopyrectEncodedMessage(rfbFramebufferUpdateRectHeader rectheader,VncOptions& opt)
{
    int src_x, src_y;
    if (FAILED(RecvFromRfbServer((char*)&src_x, 2))) { return  E_FAIL; }
    if (FAILED(RecvFromRfbServer((char*)&src_y, 2))) { return  E_FAIL; }
    m_pImage->CopyRect(Swap16IfLE(src_x),Swap16IfLE(src_y),rectheader.r.x,rectheader.r.y,rectheader.r.w,rectheader.r.h);
    return S_OK;
}

HRESULT
CRfbProtocol::HandleRreEncodedMessage(rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt)
{
    HRESULT ret = S_OK;
    rfbRREHeader header;
    char *colour;
    CARD16 rect[4];
    unsigned int i;
    int r=0, g=0, b=0;

    colour = new char[opt.client.bpp/8];
    if (FAILED(RecvFromRfbServer( (char *)&header, sz_rfbRREHeader)))
    {
        ret = E_FAIL;
        goto end;
    }
    
    header.nSubrects = Swap32IfLE(header.nSubrects);

    /* draw background rect */
    if (FAILED(RecvFromRfbServer( colour, opt.client.bpp/8)))
    {
        ret = E_FAIL;
        goto end;
    }
    GetRgbFromData(&r, &g, &b, colour,opt);
    m_pImage->DrawRect(rectheader.r.x, rectheader.r.y, rectheader.r.w, rectheader.r.h, r, g, b, 0xff);
    
    /* subrect pixel values */
    for (i=0;i<header.nSubrects;i++)
    {
        if (FAILED(RecvFromRfbServer( colour, opt.client.bpp/8)))
        {
            ret = E_FAIL;
            goto end;
        }
        GetRgbFromData(&r, &g, &b, colour,opt);
        if (FAILED(RecvFromRfbServer( (char *)&rect, sizeof(rect))))
        {
            ret = E_FAIL;
            goto end;
        }
        m_pImage->DrawRect(Swap16IfLE(rect[0]) + rectheader.r.x,
            Swap16IfLE(rect[1]) + rectheader.r.y,
            Swap16IfLE(rect[2]),
            Swap16IfLE(rect[3]),
            r,g,b,0xff);
    }   
end:
    delete[] colour;
    return ret;
}

HRESULT
CRfbProtocol::HandleCorreEncodedMessage(rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt)
{
    HRESULT ret = S_OK;
    rfbRREHeader header;
    char *colour;
    CARD8 rect[4];
    unsigned int i;
    int r=0, g=0, b=0;

    colour = new char[opt.client.bpp/8];
    
    if (FAILED(RecvFromRfbServer((char *)&header, sz_rfbRREHeader)))
    {
        ret = E_FAIL;
        goto end;
    }
    header.nSubrects = Swap32IfLE(header.nSubrects);

    /* draw background rect */
    if (FAILED(RecvFromRfbServer( colour, opt.client.bpp/8)))
    {
        ret = E_FAIL;
        goto end;
    }
    GetRgbFromData(&r, &g, &b, colour, opt);
    m_pImage->DrawRect(rectheader.r.x, rectheader.r.y, rectheader.r.w, rectheader.r.h, r,g,b);
    /* subrect pixel values */
    for (i=0;i<header.nSubrects;i++)
    {
        if (FAILED(RecvFromRfbServer( colour, opt.client.bpp/8)))
        {
            ret = E_FAIL;
            goto end;
        }
        GetRgbFromData(&r, &g, &b, colour, opt);
        if (FAILED(RecvFromRfbServer( (char *)&rect, sizeof(rect))))
        {
            ret = E_FAIL;
            goto end;
        }
        m_pImage->DrawRect(rect[0] + rectheader.r.x, 
            rect[1] + rectheader.r.y, 
            rect[2], rect[3], r,g,b);
    }   
end:
    delete[] colour;
    return ret;
}

HRESULT
CRfbProtocol::HandleHextileEncodedMessage(char* buffer, rfbFramebufferUpdateRectHeader rectheader, VncOptions& opt)
{
    int rect_x, rect_y, rect_w,rect_h, i=0,j=0, n;
    int tile_w = 16, tile_h = 16;
    int remaining_w, remaining_h;
    CARD8 subrect_encoding;
    int bpp = opt.client.bpp / 8;
    int nr_subr = 0;		  
    int x,y,w,h;
    int r=0, g=0, b=0;
    int fg_r=0, fg_g=0, fg_b=0;
    int bg_r=0, bg_g=0, bg_b=0;
    rect_w = remaining_w = rectheader.r.w;
    rect_h = remaining_h = rectheader.r.h;
    rect_x = rectheader.r.x;
    rect_y = rectheader.r.y;

    /* the rect is divided into tiles of width and height 16. Iterate over
    * those */
    while ((double)i < (double) rect_h/16)
    {
        /* the last tile in a column could be smaller than 16 */
        if ( (remaining_h -=16) <= 0) tile_h = remaining_h +16;

        j=0;
        while ((double)j < (double) rect_w/16)
        {
            /* the last tile in a row could also be smaller */
            if ( (remaining_w -= 16) <= 0 ) tile_w = remaining_w +16;

            if (FAILED(RecvFromRfbServer((char*)&subrect_encoding, 1)))
            {
                return E_FAIL;
            }
            /* first, check if the raw bit is set */
            if (subrect_encoding & rfbHextileRaw)
            {
                if (FAILED(RecvFromRfbServer( buffer, bpp*tile_w*tile_h)))
                {
                    return E_FAIL;
                }
                m_pImage->PutImage((unsigned char*)buffer,rect_x+(j*16),rect_y+(i*16),tile_w,tile_h, CImage::RGB565);                
            } 
            else  /* subrect encoding is not raw */
            {
                /* check whether theres a new bg or fg colour specified */
                if (subrect_encoding & rfbHextileBackgroundSpecified)
                {
                    if (FAILED(RecvFromRfbServer( buffer, bpp)))
                    {
                        return E_FAIL;
                    }
                    GetRgbFromData(&bg_r, &bg_g, &bg_b, buffer, opt);
                }
                if (subrect_encoding & rfbHextileForegroundSpecified)
                {
                    if (FAILED(RecvFromRfbServer(buffer, bpp)))
                    {
                        return E_FAIL;
                    }
                    GetRgbFromData(&fg_r, &fg_g, &fg_b, buffer, opt);
                }
                /* fill the background */
                m_pImage->DrawRect(rect_x+(j*16), rect_y+(i*16), tile_w, tile_h, bg_r, bg_g, bg_b);
                
                if (subrect_encoding & rfbHextileAnySubrects)
                {
                    if (FAILED(RecvFromRfbServer((char*)&nr_subr, 1)))
                    {
                        return E_FAIL;
                    }
                    for (n=0;n<nr_subr;n++)
                    {
                        if (subrect_encoding & rfbHextileSubrectsColoured)
                        {
                            if (FAILED(RecvFromRfbServer(buffer, bpp)))
                            {
                                return E_FAIL;
                            }
                            GetRgbFromData(&r, &g, &b, buffer, opt);
                            if (FAILED(RecvFromRfbServer(buffer, 2)))
                            {
                                return E_FAIL;
                            }                            
                            x = rfbHextileExtractX( (CARD8) *buffer);
                            y = rfbHextileExtractY( (CARD8) *buffer);
                            w = rfbHextileExtractW( (CARD8)*(buffer+1));
                            h = rfbHextileExtractH( (CARD8)*(buffer+1));
                            m_pImage->DrawRect(x+(rect_x+(j*16)), y+(rect_y+(i*16)), w, h, r,g,b);
                        }else{
                            if (FAILED(RecvFromRfbServer(buffer, 2)))
                            {
                                return E_FAIL;
                            }                            
                            x = rfbHextileExtractX( (CARD8) *buffer);
                            y = rfbHextileExtractY( (CARD8) *buffer);
                            w = rfbHextileExtractW( (CARD8)* (buffer+1));
                            h = rfbHextileExtractH( (CARD8)* (buffer+1));
                            m_pImage->DrawRect(x+(rect_x+(j*16)), y+(rect_y+(i*16)), w, h, fg_r,fg_g,fg_b);
                        }
                    }
                }
            }
            j++;
        }
        remaining_w = rectheader.r.w;
        tile_w = 16; /* reset for next row */
        i++;
    }	
    return S_OK;
}

void
CRfbProtocol::GetRgbFromData(int *r, int *g, int *b, char *data, VncOptions &opt)
{
    CARD16 foo16;

    switch (opt.client.bpp)
    {
    case 8:
        printf("FIXME unimplemented\n");	 
        break;
    case 16:
        memcpy(&foo16, data, 2);
        *r = (( foo16 >> opt.client.redshift ) & opt.client.redmax) <<3;
        *g = (( foo16 >> opt.client.greenshift ) & opt.client.greenmax)<<2;
        *b = (( foo16 >> opt.client.blueshift ) & opt.client.bluemax)<<3;
        break;
    case 24:
        printf("FIXME unimplemented\n");
        break;
    case 32:
        printf("FIXME unimplemented\n");
        break;
    }
}


HRESULT
CRfbProtocol::RecvFromRfbServer(char *out, unsigned int n)
{
    if (!m_Connected)
    {
        return E_FAIL;
    }

    if (n <= m_nBuffered)
    {
        memcpy(out, m_pBufOut, n);
        m_pBufOut += n;
        m_nBuffered -= n;
        return S_OK;
    }

    memcpy(out, m_pBufOut, m_nBuffered);
    out += m_nBuffered;
    n -= m_nBuffered;
    
    if (n > m_BufSize)
    {
        m_BufSize = n;
        char * tmp = new char[n];
        if (m_pBuf)
        {
            delete[] m_pBuf;
        }
        m_pBuf = tmp;        
    }
    
    m_pBufOut = m_pBuf;
    m_nBuffered = 0;

    while (m_nBuffered < n)
    {
        memset(m_pBuf + m_nBuffered, 0,  m_BufSize - m_nBuffered);
        int i = recv(m_Sock, m_pBuf + m_nBuffered, m_BufSize - m_nBuffered,0);

        if (i <= 0)
        {
#ifdef __BUILD_FOR_WINXP
            int error = WSAGetLastError();
#endif
#ifdef __BUILD_FOR_LINUX
            int error = errno;
#endif
                printf("VNC server closed connection\n");
                Disconnect();
                return E_FAIL;                   
        }
        m_nBuffered += i;
    }
    memcpy(out, m_pBufOut, n);
    m_pBufOut += n;
    m_nBuffered -= n;
    return S_OK;    
}

HRESULT
CRfbProtocol::SendToRfbServer(char *buf, unsigned int n)
{
    if (!m_Connected)
    {
        return E_FAIL;
    }

    fd_set fds;
    unsigned int i = 0;
    int j;

    while (i < n)
    {
        j = send(m_Sock, buf + i, (n - i), 0);
        if (j <= 0)
        {
            if (j < 0)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    FD_ZERO(&fds);
                    FD_SET(m_Sock, &fds);

                    if (select(m_Sock + 1, NULL, &fds, NULL, NULL) <= 0)
                    {
                        printf("SendToRfbServer");
                        perror(": select");
                        return E_FAIL;
                    }
                    j = 0;
                }
                else
                {
                    printf("SendToRfbServer");
                    perror(": write");
                    return E_FAIL;
                }
            }
            else
            {
                printf("SendToRfbServer: write failed\n");
                return E_FAIL;
            }
        }
        i += j;
    }
    return S_OK;
}

CImage * 
CRfbProtocol::GetImage(void)
{
    return m_pImage;
}
