#ifndef __VNC_CONNECTION_H__
#define __VNC_CONNECTION_H__

#include "defines.h"
#include "threadtask.h"
#include "image.h"
#include "rfb.h"
#include "locker.h"
#include "image.h"

namespace libvnc
{

class CVncConn: public ThreadTask
{
private:

    CRfbProtocol * m_pRfb;    
    VncOptions     m_Opt;
            
private:

    void InitSockets();
    void DoWork(void);
    
public:

    enum EMouseButtons { LEFT_BUTTON = 0x01, MIDDLE_BUTTON = 0x02, RIGHT_BUTTON = 0x04, WHEEL_UP = 0x08, WHEEL_DOWN = 0x10 };

    CVncConn(std::string server_name, int port, std::string password);
    virtual ~CVncConn(void);

    HRESULT UpdateMouse(int x, int y, EMouseButtons buttons);
    HRESULT SendKeyEvent(int key, bool pressed);

    bool IsReady(void);
    void Run(void);
    CImage * GetImage(void);
};

}

#endif