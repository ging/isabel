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
///////////////////////////////////////////////////////////////////////////////////////////////
//
// $Id:$
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "general.h"
#include "dtGDI.h"
#include "win32_x11.h"
#include "messages.h"
#include "ICF2/notify.hh"
#include <strsafe.h>
#include <GDIplus.h>

using namespace Gdiplus;

dtGDI_t::windowGDI_t::windowGDI_t(const char *title,
                    const char *geom,
                    window_ref  parent,
                    unsigned    visualDepth,
                    int         visualClass,
                    unsigned    defaultWidth,
                    unsigned    defaultHeight
                    )
{
    // Inicializar variables
    m_bReleased = false;
    m_bRoot    = false;
    m_Depth   = visualDepth;
    m_hRegion = NULL;
    m_hMutex  = CreateMutex(NULL,FALSE,NULL);
    m_bkColor = color_t(0,0,0);
    m_fgColor = color_t(255,255,255);
    m_bMapped = false;
    m_bOverrideRedirect = false;
    m_hDC     = NULL;
    m_hWnd    = NULL;
    m_Font    = new Gdiplus::Font(Gdiplus::FontFamily::GenericSansSerif(),10);

    unsigned W = DEFAULT_W;
    unsigned H = DEFAULT_H;
    int      X = DEFAULT_X;
    int      Y = DEFAULT_Y;

    // Si hay padre lo obtenemos
    HWND hParent = NULL;
    if (parent)
    {
        window_t* aux1     = static_cast<window_t*>(parent);
        windowGDI_t * aux2 = dynamic_cast<windowGDI_t *>(aux1);
        hParent = aux2->m_hWnd;
    }

    // Obtenemos la geometria
    if (geom)
    {
        unsigned tmpW = DEFAULT_W;
        unsigned tmpH = DEFAULT_H;
        int      tmpX = DEFAULT_X;
        int      tmpY = DEFAULT_Y;

        int res= XParseGeometry(geom, &tmpX, &tmpY, &tmpW, &tmpH);

        if(res & WidthValue) W = tmpW;
        if(res & HeightValue) H = tmpH;
        if(res & XValue) X = tmpX;
        if(res & YValue) Y = tmpY;
    }

    m_Rect.left  = X;
    m_Rect.right = X + W;
    m_Rect.top = Y;
    m_Rect.bottom = Y + H;

    // Creamos la ventana
    m_hWnd = CreateWindow(MainClassName,             // name of window class
                          title,                     // title-bar string
                          WS_OVERLAPPED|WS_SIZEBOX,  // top-level window
                          X,                         // default horizontal position
                          Y,                         // default vertical position
                          W,                         // default width
                          H,                         // default height
                          hParent,                   // no owner window
                          (HMENU) NULL,              // use class menu
                          hInstance,                 // handle to application instance
                          (LPVOID) NULL              // no window-creation data
                         );

    if (m_hWnd == NULL)
    {
        DisplayError("windowGDI_t::windowGDI_t");
    }
    else
    {
        // Obtenemos los contextos de pintado
        m_hDC = GetWindowDC(this->m_hWnd);
        // Creamos uno compatible en memoria (Background)
        m_hMemDC = CreateCompatibleDC(m_hDC);
        CreateMemRegion();
    }
}

dtGDI_t::windowGDI_t::~windowGDI_t(void)
{
    // Destruimos objetos y ventana
    WaitForSingleObject(m_hMutex,INFINITE);
    if (m_hDC) { ReleaseDC(m_hWnd,m_hDC); }
    if (m_hMemDC) { DeleteDC(m_hMemDC); }
    if (m_hRegion) { DeleteObject(m_hRegion); }
    if (m_hWnd)    { SendMessage(m_hWnd,WM_CLOSE,0,0); }
    delete m_Font;
    CloseHandle(m_hMutex);
}

void
dtGDI_t::windowGDI_t::SetRoot(bool root)
{
    // Hacemos que la ventana tenga propiedades
    // de ventana raiz: sin borde, tamaño de escritorio, no se mueve...
    m_bRoot = root;
    if (m_bRoot)
    {
        RECT rect;
        GetWindowRect(GetDesktopWindow(),&rect);
        setBgColor(0,0,255);
        SetWindowLong(m_Root->m_hWnd,GWL_STYLE,0);
        __reposition(0,0);
        __resize(rect.right-rect.left,rect.bottom-rect.top);
        setOverrideRedirect(true);
    }
};

void
dtGDI_t::windowGDI_t::CreateMemRegion(void)
{
    // Creamos region de memoria en segundo plano
    RECT rect;
    GetWindowRect(this->m_hWnd,&rect);
    int cx = rect.right-rect.left;
    int cy = rect.bottom-rect.top;
    if (!m_bRoot)
    {
        cx -= 4;
        cy -= 4;
    }

    HBITMAP tmp = CreateCompatibleBitmap(m_hDC,cx,cy); // Creamos DC
    HDC tmpDC = CreateCompatibleDC(m_hMemDC);
    SelectObject(tmpDC,tmp);
    __clear(tmpDC);
    if (m_hRegion) // Si existe memoria anterior la copiamos
    {
        StretchBlt(tmpDC,0,0,cx,cy,m_hMemDC,0,0,cx,cy,SRCCOPY);
        DeleteObject(m_hRegion);
    }

    DeleteDC(tmpDC);
    m_hRegion = tmp;
    SelectObject(m_hMemDC,m_hRegion);
}

void
dtGDI_t::windowGDI_t::__setTitle(const char *s)
{
    if (s)
    {
        WaitForSingleObject(m_hMutex,INFINITE);
        SetWindowText(this->m_hWnd,s);
        ReleaseMutex(this->m_hMutex);
    }
}

void
dtGDI_t::windowGDI_t::__setGeometry(const char *g)
{
    if (g)
    {
        unsigned  W,H,tmpW,tmpH;
        int       X,Y,tmpX,tmpY;

        int res= XParseGeometry((char*)g, &tmpX, &tmpY, &tmpW, &tmpH);

        if(res & WidthValue) W = tmpW;
        if(res & HeightValue) H = tmpH;
        if(res & XValue) X = tmpX;
        if(res & YValue) Y = tmpY;

        WaitForSingleObject(m_hMutex,INFINITE);
        m_Rect.left  = X;
        m_Rect.right = X + W;
        m_Rect.top = Y;
        m_Rect.bottom = Y + H;
        SetWindowPos(this->m_hWnd,NULL,X,Y,W,H,0);
        CreateMemRegion();
        ReleaseMutex(this->m_hMutex);
    }
}

void
dtGDI_t::windowGDI_t::__map(void)
{
    ShowWindow(m_hWnd,SW_SHOW);
    WaitForSingleObject(m_hMutex,INFINITE);
    m_bMapped = true;
    ReleaseMutex(this->m_hMutex);
}

void
dtGDI_t::windowGDI_t::__unmap(void)
{
    ShowWindow(m_hWnd,SW_HIDE);
    WaitForSingleObject(m_hMutex,INFINITE);
    m_bMapped = false;
    ReleaseMutex(this->m_hMutex);
}

void
dtGDI_t::windowGDI_t::__clear(HDC MemDC)
{
    if (MemDC==NULL)
    {
        MemDC=m_hMemDC;
    }
    Graphics graphics(m_hDC),BGgraphics(MemDC);
    RECT rect,rect2;
    WaitForSingleObject(m_hMutex,INFINITE);
    GetWindowRect(m_hWnd,&rect);
    GetClientRect(m_hWnd,&rect2);
    int cx = rect2.right  - rect2.left;
    int cy = rect2.bottom - rect2.top;
    int x  = ((rect.right-rect.left)-cx)/2;
    int y  = (rect.bottom-rect.top)-cy-4;
    Gdiplus::Rect gdi_rect(x,y,cx,cy);
    if (this->m_bRoot==true)
    {
        gdi_rect.X = rect.left;
        gdi_rect.Y = rect.top;
        gdi_rect.Width = rect.right - rect.left;
        gdi_rect.Height = rect.bottom - rect.top;
    }
    SolidBrush solidBrush(Color(m_bkColor.a,m_bkColor.r,m_bkColor.g,m_bkColor.b));
    graphics.FillRectangle(&solidBrush,gdi_rect);
    BGgraphics.FillRectangle(&solidBrush,gdi_rect);
    ReleaseMutex(this->m_hMutex);
}

void
dtGDI_t::windowGDI_t::__resize(unsigned w, unsigned h)
{
    RECT rect;
    WaitForSingleObject(m_hMutex,INFINITE);
    GetWindowRect(this->m_hWnd,&rect);
    m_Rect = rect;
    SetWindowPos(this->m_hWnd,NULL,rect.left,rect.top,w,h,0);
    CreateMemRegion();
    ReleaseMutex(this->m_hMutex);
}

void
dtGDI_t::windowGDI_t::__reposition(int x, int y)
{
    RECT rect;
    WaitForSingleObject(m_hMutex,INFINITE);
    GetWindowRect(this->m_hWnd,&rect);
    m_Rect = rect;
    SetWindowPos(this->m_hWnd,NULL,x,y,rect.right-rect.left,rect.bottom-rect.top,0);
    CreateMemRegion();
    ReleaseMutex(this->m_hMutex);
}

void
dtGDI_t::windowGDI_t::__putRGB24(const u8 *data,
                                 unsigned  width,
                                 unsigned  height,
                                 float     zoomx,
                                 float     zoomy,
                                 bool         BG
                                )
{
    // Convertimos los datos a la profundidad de
    // color de la ventana
    unsigned char * image = NULL;
    int in_len = width*height*3;
    unsigned char * gamma_image = (unsigned char *)data;

    if (m_Gamma.IsActive()) // Si hay gamma, lo aplicamos
    {
        gamma_image = new unsigned char[in_len];
        m_Gamma.GammaTransform((unsigned char *)data,gamma_image,in_len);
    }

    int nPlanes = 0;
    switch (m_Depth)
    {
    case 32:
        nPlanes = 4;
        color_t::Convert24To32(gamma_image,&image,width,height,zoomx,zoomy);
        break;
    case 16:
        nPlanes = 3;
        color_t::Convert24To16(gamma_image,&image,width,height,zoomx,zoomy);
        break;
    default:
        MessageBox(NULL,
                   (LPCTSTR)L"Display Manager only supports 16 and 32 bits color depth, exiting...",
                   TEXT("Error"),
                   MB_OK
                  );
        abort();
    }

    if (gamma_image != data) // si hay gamma, borramos el buffer
    {
        delete[] gamma_image;
    }

    if (image)
    {
        // Ponemos la imagen en la ventana
        HBITMAP bitmap = CreateBitmap(width,height,nPlanes,m_Depth/nPlanes,image);
        HDC tmpDC = CreateCompatibleDC(m_hDC);
        SelectObject(tmpDC,bitmap);
        if (this->m_bRoot==true) // Si somos root pintamos sobre toda la ventana
        {
            HDC destDC = BG?m_hMemDC:m_hDC;
            StretchBlt(destDC,0,0,width,height,tmpDC,0,0,width,height,SRCCOPY);            
        }else{ // Si no, pintamos sobre la parte cliente
            WINDOWINFO wInfo;
            GetWindowInfo(m_hWnd,&wInfo);
            int clientW = wInfo.rcClient.right - wInfo.rcClient.left;
            int clientH = wInfo.rcClient.bottom - wInfo.rcClient.top;
            int x = (width - clientW)-wInfo.cxWindowBorders;
            int y = (height - clientH)-wInfo.cyWindowBorders;
            HDC destDC = BG?m_hMemDC:m_hDC;
            StretchBlt(destDC,x,y,clientW,clientH,tmpDC,0,0,clientW,clientH,SRCCOPY);            
        }
        DeleteObject(bitmap);
        DeleteDC(tmpDC);
        delete[] image;
    }
}

void
dtGDI_t::windowGDI_t::__drawLineInBG(color_t color,float width,int x1,int y1,int x2,int y2)
{
    Graphics graphics(m_hDC),BGgraphics(m_hMemDC);

    WaitForSingleObject(m_hMutex,INFINITE);
    Color gdi_color(color.a,color.r,color.g,color.b);
    ReleaseMutex(this->m_hMutex);

    // Create a Pen object.
    Pen pen(gdi_color, width);

    // Create two Point objects that define the line.
    Point point1(x1, y1);
    Point point2(x2, y2);

    // Draw the line.
    graphics.DrawLine(&pen,point1,point2);
    BGgraphics.DrawLine(&pen,point1,point2);
}

void
dtGDI_t::windowGDI_t::__drawString(int x, int y, const char *msg, align_e a)
{
    Graphics graphics(m_hDC);

    WaitForSingleObject(m_hMutex,INFINITE);
    Color gdi_color(m_fgColor.a,m_fgColor.r,m_fgColor.g,m_fgColor.b);
    StringFormat format;
    switch (a)
    {
        case align_e::CENTER:
        {
            format.SetAlignment(StringAlignment::StringAlignmentCenter);
        }break;
        case align_e::RIGHT:
        {
            format.SetAlignment(StringAlignment::StringAlignmentFar);
        }break;
        case align_e::LEFT:
        {
            format.SetAlignment(StringAlignment::StringAlignmentNear);
        }break;
    }
    ReleaseMutex(this->m_hMutex);
    // Draw the line.
    wchar_t * unicode_msg = new wchar_t[strlen(msg)+1];
    int i = mbstowcs(unicode_msg,msg,strlen(msg));
    unicode_msg[i] = 0;
    graphics.DrawString(unicode_msg,i,this->m_Font,PointF(REAL(x),REAL(y)),&format,&SolidBrush(gdi_color));
}

void
dtGDI_t::windowGDI_t::__repaint(void)
{
    RECT rect,rect2;
    // GetUpdateRect(this->m_hWnd,&rect,FALSE);
    GetWindowRect(m_hWnd,&rect);
    GetClientRect(m_hWnd,&rect2);
    int cx = rect2.right  - rect2.left;
    int cy = rect2.bottom - rect2.top;
    int x  = ((rect.right-rect.left)-cx)/2;
    int y  = (rect.bottom-rect.top)-cy-4;
    if (this->m_bRoot==true)
    {
        x = rect.left;
        y = rect.top;
        cx = rect.right - rect.left;
        cy = rect.bottom - rect.top;
    }
    StretchBlt(m_hDC,x,y,cx,cy,m_hMemDC,x,y,cx,cy,SRCCOPY);
}

u32
dtGDI_t::windowGDI_t::getWinId(void) const
{
    u32 id = 0;
    id = (u32)this->m_hWnd;
    return id;
}

unsigned
dtGDI_t::windowGDI_t::getWidth(void) const
{
    RECT rect;
    WaitForSingleObject(m_hMutex,INFINITE);
    GetWindowRect(this->m_hWnd,&rect);
    ReleaseMutex(this->m_hMutex);
    return (rect.right-rect.left);
}

unsigned
dtGDI_t::windowGDI_t::getHeight(void) const
{
    RECT rect;
    WaitForSingleObject(m_hMutex,INFINITE);
    GetWindowRect(this->m_hWnd,&rect);
    ReleaseMutex(this->m_hMutex);
    return (rect.bottom - rect.top);
}

int
dtGDI_t::windowGDI_t::getXPos(void) const
{
    RECT rect;
    WaitForSingleObject(m_hMutex,INFINITE);
    GetWindowRect(this->m_hWnd,&rect);
    ReleaseMutex(this->m_hMutex);
    return rect.left;
}

int
dtGDI_t::windowGDI_t::getYPos(void) const
{
    RECT rect;
    WaitForSingleObject(m_hMutex,INFINITE);
    GetWindowRect(this->m_hWnd,&rect);
    ReleaseMutex(this->m_hMutex);
    return rect.top;
}

bool
dtGDI_t::windowGDI_t::isMapped(void) const
{
    WaitForSingleObject(m_hMutex,INFINITE);
    bool mapped = m_bMapped;
    ReleaseMutex(this->m_hMutex);
    return mapped;
}

bool
dtGDI_t::windowGDI_t::loadFont(const char *family, float size, int style)
{
    WCHAR w_family[MAX_PATH];
    memset(w_family,0,sizeof(WCHAR)*MAX_PATH);
    mbstowcs(w_family,family,MAX_PATH);
    FontFamily fontFamily(w_family);
    WaitForSingleObject(m_hMutex,INFINITE);
    delete m_Font;
    m_Font = new Gdiplus::Font(w_family, size, style);
    ReleaseMutex(this->m_hMutex);
    return true;
}

bool
dtGDI_t::windowGDI_t::setBgColor(const char *color)
{
    WaitForSingleObject(m_hMutex,INFINITE);
    bool ret = color_t::GetColorByName(color,&m_bkColor);
    ReleaseMutex(this->m_hMutex);
    return ret;
}

bool
dtGDI_t::windowGDI_t::setBgColor(u8 r,u8 g,u8 b)
{
    WaitForSingleObject(m_hMutex,INFINITE);
    m_bkColor = color_t(r,g,b);
    ReleaseMutex(this->m_hMutex);
    SendMessage(this->m_hWnd,WM_WGDI_CLEAR,0,0);
    return true;
}

bool
dtGDI_t::windowGDI_t::setFgColor(const char *color)
{
    WaitForSingleObject(m_hMutex,INFINITE);
    bool ret = color_t::GetColorByName(color,&m_fgColor);
    ReleaseMutex(this->m_hMutex);
    return ret;
}

bool
dtGDI_t::windowGDI_t::setFgColor(u8 r,u8 g,u8 b)
{
    WaitForSingleObject(m_hMutex,INFINITE);
    m_fgColor = color_t(r,g,b);
    ReleaseMutex(this->m_hMutex);
    return false;
}

void
dtGDI_t::windowGDI_t::setGamma(float rGamma, float gGamma, float bGamma)
{
    WaitForSingleObject(m_hMutex,INFINITE);
    m_Gamma.SetGamma(rGamma, gGamma, bGamma);
    ReleaseMutex(this->m_hMutex);
}

void
dtGDI_t::windowGDI_t::setOverrideRedirect(bool ORflag)
{
    WaitForSingleObject(m_hMutex,INFINITE);
    m_bOverrideRedirect = ORflag;
    ReleaseMutex(this->m_hMutex);
}

bool     
dtGDI_t::windowGDI_t::release(void)
{
    m_bReleased = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//    Los siguientes metodos son llamados por el thread de la aplicacion y
//  sirven para mandar mensajes al thread del gestor de ventanas
//
///////////////////////////////////////////////////////////////////////////////////////////////


void
dtGDI_t::windowGDI_t::setTitle(const char *s)
{
    SendMessage(this->m_hWnd,WM_WGDI_SET_TITLE,(WPARAM)strdup(s),0);
}

void
dtGDI_t::windowGDI_t::setGeometry(const char *g)
{
    SendMessage(this->m_hWnd,WM_WGDI_SET_GEOMETRY,(WPARAM)strdup(g),0);
}

void
dtGDI_t::windowGDI_t::map(void)
{
    SendMessage(this->m_hWnd,WM_WGDI_MAP,0,0);
}

void
dtGDI_t::windowGDI_t::unmap(void)
{
    SendMessage(this->m_hWnd,WM_WGDI_UNMAP,0,0);
}

void
dtGDI_t::windowGDI_t::clear(void)
{
    SendMessage(this->m_hWnd,WM_WGDI_CLEAR,0,0);
}

void
dtGDI_t::windowGDI_t::resize(unsigned w, unsigned h)
{
    RESIZE_DATA * data = new RESIZE_DATA;
    data->cx = w;
    data->cy = h;
    SendMessage(this->m_hWnd,WM_WGDI_RESIZE,(WPARAM)data,0);
}

void
dtGDI_t::windowGDI_t::reposition(int x, int y)
{
    REPOSITION_DATA * data = new REPOSITION_DATA;
    data->x = x;
    data->y = y;
    SendMessage(this->m_hWnd,WM_WGDI_REPOSITION,(WPARAM)data,0);
}

void
dtGDI_t::windowGDI_t::putRGB24(const u8 *data,
                               unsigned  width,
                               unsigned  height,
                               float     zoomx,
                               float     zoomy
                              )
{
    RGB24_DATA * param = new RGB24_DATA;
    param->data = data;
    param->width = width;
    param->height = height;
    param->zoomx = zoomx < 0 ? this->getWidth()  / (float)width  : zoomx;
    param->zoomy = zoomy < 0 ? this->getHeight() / (float)height : zoomy;
    SendMessage(this->m_hWnd,WM_WGDI_PUT_RGB24,(WPARAM)param,0);
}

void
dtGDI_t::windowGDI_t::putRGB24inBG(const u8 *data,
                                   unsigned  width,
                                   unsigned  height,
                                   float     zoomx,
                                   float     zoomy
                                  )
{
    RGB24_DATA * param = new RGB24_DATA;
    param->data = data;
    param->width = width;
    param->height = height;
    param->zoomx = zoomx < 0 ? this->getWidth()  / width  : zoomx;
    param->zoomy = zoomy < 0 ? this->getHeight() / height : zoomy;
    SendMessage(this->m_hWnd,WM_WGDI_PUT_RGB24_BG,(WPARAM)param,0);
}

void
dtGDI_t::windowGDI_t::drawLineInBG(int x1, int y1, int x2, int y2, int lwidth)
{
    LINE_DATA * data = new LINE_DATA;
    data->color = color_t(0,0,0);
    data->width = (REAL)lwidth;
    data->start.x = x1;
    data->start.y = y1;
    data->end.x = x2;
    data->end.y = y2;
    SendMessage(this->m_hWnd,WM_WGDI_DRAW_LINE_BG,(WPARAM)data,0);
}

void
dtGDI_t::windowGDI_t::drawString(int x, int y, const char *msg, align_e a)
{
    STRING_DATA * data = new STRING_DATA;
    data->point.x = x;
    data->point.y = y;
    data->align = a;
    strcpy_s(data->string,STR_LEN,msg);
    SendMessage(this->m_hWnd,WM_WGDI_DRAW_STRING,(WPARAM)data,0);
}

