
#include "defines.h"
#include "rfb.h"
#include "vncconn.h"
#include "extras.h"

using namespace libvnc;

CVncConn::CVncConn(std::string server_name, int port, std::string password)
{
    InitSockets();
        
    m_pRfb = new CRfbProtocol();

    m_Opt.servername = server_name;
    m_Opt.password = password;
    m_Opt.port = port;

    /* Add supported encodings*/    
    m_Opt.encodings.push_back("zlib");
    m_Opt.encodings.push_back("tight");
    m_Opt.encodings.push_back("hextile");
    m_Opt.encodings.push_back("copyrect");
    m_Opt.encodings.push_back("corre");
    m_Opt.encodings.push_back("rre");
    m_Opt.encodings.push_back("raw");
    
    /* set some default values */
    m_Opt.client.bpp = 16;
    m_Opt.client.depth = 16;
    m_Opt.client.bigendian = 0;
    m_Opt.client.truecolour = 1;
    m_Opt.client.redmax = 31;
    m_Opt.client.greenmax = 63;
    m_Opt.client.bluemax = 31;
    m_Opt.client.redshift =11;
    m_Opt.client.greenshift = 5;
    m_Opt.client.blueshift = 0;
    m_Opt.client.compresslevel = 99;
    m_Opt.client.quality = 99;
    m_Opt.shared = 1;    
    m_Opt.poll_freq = 1000;
    m_Opt.h_ratio = 1;
    m_Opt.v_ratio = 1;

    /* Connect to server */
    if (FAILED(m_pRfb->ConnectTo(m_Opt.servername.c_str(), 5900 + m_Opt.port)))
    {
        printf("CVncConn::CVncConn: Couldnt establish connection with the VNC server. Exiting\n");                
        return;
    }

    /* initialize the connection */
    if (FAILED(m_pRfb->InitConnection(m_Opt)))
    {
        printf("Connection with VNC server couldnt be initialized. Exiting\n");
        m_pRfb->Disconnect();      
        return;
    }

    /* Tell the VNC server which pixel format and encodings we want to use */
    if (FAILED(m_pRfb->SetFormatAndEncodings(m_Opt)))
    {
        printf("Error negotiating format and encodings. Exiting.\n");
        m_pRfb->Disconnect();
        return;
    }
    m_pRfb->SendUpdateRequest(0,m_Opt);    
    SetPeriod(m_Opt.poll_freq);
}

CVncConn::~CVncConn(void)
{
    Stop();
    m_pRfb->Disconnect();
    delete m_pRfb;
}

void 
CVncConn::InitSockets(void)
{
#ifdef __BUILD_FOR_WINXP
    WORD version;
    WSADATA data;
    version=MAKEWORD(1,1);
    WSAStartup(version,&data); // iniciamos Winsock
#endif
}

bool 
CVncConn::IsReady(void)
{
    return m_pRfb->IsConnected();
}

void 
CVncConn::Run(void)
{
    if (IsReady())
    {        
		ThreadTask::Run();
    }
}

void 
CVncConn::DoWork(void)
{
    if (IsReady())
    {        
        m_pRfb->SendUpdateRequest(1,m_Opt);
        m_pRfb->HandleServerMessages(m_Opt);        
    }
}

CImage * 
CVncConn::GetImage(void)
{
    if (IsReady())
    {
        return m_pRfb->GetImage();
    }else{
        return NULL;
    }
}

HRESULT 
CVncConn::UpdateMouse(int x, int y, EMouseButtons buttons)
{
    if (IsReady())
    {
        MouseState state;
        state.x = x;
        state.y = y;
        state.buttonmask = buttons;
        return m_pRfb->UpdateMouse(state,m_Opt);
    }else{
        return E_FAIL;
    }
}

HRESULT 
CVncConn::SendKeyEvent(int key, bool pressed)
{
    if (IsReady())
    {
        return m_pRfb->SendKeyEvent(key,pressed?1:0);
    }else{
        return E_FAIL;
    }
}

