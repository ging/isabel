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
#include "general.h"
#include "dtGDI.h"
#include "win32_x11.h"
#include "messages.h"
#include "ICF2/notify.hh"
#include <strsafe.h>
#include <GDIplus.h>

using namespace Gdiplus;
dictionary_t<HWND,windowGDI_ref> windows;
dtGDI_t::windowGDI_t* dtGDI_t::m_Root = NULL;
bool windowClassRegistered = false;

dtGDI_t::dtGDI_t(const char * options)
{
    m_Root = NULL;
    hInstance = NULL;

    //Init GDIplus
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    //Get DISPLAY DEPTH
    m_Depth = GetDisplayDepth();

    //Save app instance
       if (options == NULL)
    {
        //Try to get hinstance from console window
        char title[500];  // to hold title
        // get title of console window
        GetConsoleTitleA( title, 500 );
        // get HWND of console, based on its title
        HWND hwndConsole = FindWindowA( NULL, title );
        // get HINSTANCE of console, based on HWND
        hInstance = (HINSTANCE)GetWindowLong(hwndConsole, GWL_HINSTANCE);
    }else{
        hInstance = *((HINSTANCE*)options);
    }
    //Start dtGDI
    DWORD thID;
    m_hThread = CreateThread(NULL,NULL,Run,this,0,&thID);
    while (!m_Root)
    {
        Sleep(100);
    }
}

unsigned short 
dtGDI_t::GetDisplayDepth(void)
{
    HDC hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);     
    unsigned short depth = GetDeviceCaps(hScrDC, BITSPIXEL);     // get screen depth
    DeleteDC(hScrDC);
    return depth;
}

DWORD WINAPI
dtGDI_t::Run(void * param)
{
    dtGDI_t * dtGDI = (dtGDI_t *)param;
    dtGDI->MainLoop();
    return 0;
}

void
dtGDI_t::MainLoop(void)
{

    // Register the main window class.     
    if (windowClassRegistered == false)
    {
        WNDCLASS wndclass;
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = (WNDPROC)dtGDI_t::MsgProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hIcon = NULL;
        wndclass.hInstance = hInstance;
        wndclass.hCursor = NULL;
        wndclass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0,0,255));
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName =  MainClassName;

        if (!RegisterClass(&wndclass))
        {
            DisplayError("dtGDI_t::dtGDI_t");
        }else{
            windowClassRegistered = true;
        }
    }

    m_Root = new windowGDI_t("ROOT","100x100+0+0",NULL,32,CLASS_TRUECOLOR,100,100);
    windows.insert(m_Root->m_hWnd,m_Root);
    m_Root->SetRoot(true);
    m_Root->__map();
    
    //Process messages
    MSG msg;
    while(GetMessage(&msg,NULL,0,0))
    { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
}

dtGDI_t::~dtGDI_t(void)
{
    SendMessage(m_Root->m_hWnd,WM_DT_DESTROY_DT,(WPARAM)this,0);        
}

void
dtGDI_t::Destroy(void)
{
    ql_t<HWND> *hWnds = windows.getKeys();
    while(hWnds->len() > 0)
    {
        HWND hWnd= hWnds->head();
        windows.remove(hWnd);
        hWnds->behead();
    }
    delete hWnds;
    UnregisterClass(MainClassName,hInstance);
    windowClassRegistered = false;
}

void   
dtGDI_t::DisplayError(LPCTSTR function)
{
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)function)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf),
        TEXT("%s failed with error %d: %s"), 
        function, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

void 
dtGDI_t::getDefaultVisualCombination(unsigned *visualDepth, int *visualClass)
{
    *visualDepth = m_Depth;
    *visualClass = CLASS_TRUECOLOR;
}


bool 
dtGDI_t::supportedVisualCombination(unsigned int visualDepth,int visualClass)
{
    if (visualDepth == m_Depth && visualClass == CLASS_TRUECOLOR)
    {
        return true;
    }else{
        return false;
    }
}

dtGDI_t::window_t* 
dtGDI_t::rootWindow(void)
{
    return m_Root;
}

window_ref 
dtGDI_t::createWindow(const char *title,
                      const char *geom,
                      window_ref  parent,
                      unsigned    visualDepth,
                      int         visualClass,
                      unsigned    defaultWidth,
                      unsigned    defaultHeight
                     )
{
    if (supportedVisualCombination(visualDepth,visualClass))
    {
        windowGDI_t * newWindow = NULL;
        WNDGDI_DATA * data = new WNDGDI_DATA;  // JOE! HAZ UN CONSTRUCTOR!!
        data->defaultHeight = defaultHeight;
        data->defaultWidth  = defaultWidth;
        memset(data->geom, 0, STR_LEN);
        if (geom != NULL)
            strcpy_s(data->geom,STR_LEN,geom);
        data->parent = m_Root;//parent;
        memset(data->title, 0, STR_LEN);
        if (title != NULL)
            strcpy_s(data->title,STR_LEN,title);
        data->visualClass = visualClass;
        data->visualDepth = visualDepth;
        data->newWindow = CreateEvent(NULL,TRUE,FALSE,NULL);
        ResetEvent(data->newWindow);
        SendMessage(m_Root->m_hWnd,WM_DT_CREATE_WGDI,(WPARAM)&newWindow,(LPARAM)data);
        WaitForSingleObject(data->newWindow,INFINITE);
        CloseHandle(data->newWindow);
        delete data;

        windows.insert(newWindow->m_hWnd,newWindow);
        return (window_ref)newWindow;
    }else{
        return NULL;
    }
}

bool 
dtGDI_t::destroyWindow(smartReference_t<displayTask_t::window_t> win)
{
    bool ret = false;
    if (windows.lookUp((HWND)win->getWinId()))
    {
        SendMessage((HWND)win->getWinId(),WM_DT_DESTROY_WGDI,NULL,NULL);
        ret = true;
    }
    return ret;
}

LRESULT WINAPI 
dtGDI_t::MsgProc(HWND hWnd,UINT message,WPARAM wparam,LPARAM lparam)
{
    //Obtener window
    windowGDI_ref window = windows.lookUp(hWnd);
    
    switch (message)
    {
        case WM_WGDI_SET_TITLE:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                char * title = (char *)wparam;
                window->__setTitle(title);
                delete []title;
            }
            return 0;
        }
        case WM_WGDI_SET_GEOMETRY:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                char * geom = (char *)wparam;
                window->__setGeometry(geom);
                delete[] geom;
            }
            return 0;
        }
        case WM_WGDI_MAP:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                window->__map();
            }
            return 0;
        }        
        case WM_WGDI_UNMAP:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                window->__unmap();        
            }
            return 0;
        }
        case WM_WGDI_CLEAR:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                window->__clear();
            }
            return 0;
        }    
        case WM_WGDI_RESIZE:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                RESIZE_DATA * data = (RESIZE_DATA *)wparam;
                window->__resize(data->cx,data->cy);
                delete data;
            }
            return 0;
        }
        case WM_WGDI_REPOSITION:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                REPOSITION_DATA * data = (REPOSITION_DATA *)wparam;
                window->__reposition(data->x,data->y);
                delete data;
            }
            return 0;
        }
        case WM_WGDI_PUT_RGB24:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                RGB24_DATA * data = (RGB24_DATA *)wparam;
                window->__putRGB24(data->data,
                                   data->width,
                                   data->height,
                                   data->zoomx,
                                   data->zoomy,
                                   false);
                delete data;
            }
            return 0;
        }
        case WM_WGDI_PUT_RGB24_BG:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                RGB24_DATA * data = (RGB24_DATA *)wparam;
                window->__putRGB24(data->data,
                                   data->width,
                                   data->height,
                                   data->zoomx,
                                   data->zoomy,
                                   true);
                delete data;
            }
            return 0;
        }
        case WM_WGDI_DRAW_LINE_BG:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                LINE_DATA * data = (LINE_DATA *)wparam;
                window->__drawLineInBG(data->color,
                                   data->width,
                                   data->start.x,
                                   data->start.y,
                                   data->end.x,
                                   data->end.y);
                delete data;
            }
            return 0;
        }
        case WM_WGDI_DRAW_STRING:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                STRING_DATA * data = (STRING_DATA *)wparam;
                window->__drawString(data->point.x,data->point.y,data->string,data->align);
                delete data;
            }
            return 0;
        }

        //dtGDI_t messages
        case WM_DT_CREATE_WGDI:
        {
            windowGDI_t** window = (windowGDI_t**)wparam;
            WNDGDI_DATA * data  = (WNDGDI_DATA*)lparam;
            *window = new windowGDI_t(data->title,
                data->geom,
                data->parent,
                data->visualDepth,
                data->visualClass,
                data->defaultWidth,
                data->defaultWidth);
            SetEvent(data->newWindow);
            //data se elimina en el hilo que manda el mensaje
            return 0;
        }
        case WM_DT_DESTROY_WGDI:
        {
            windows.remove(hWnd);
            return 0;
        }
        case WM_DT_DESTROY_DT:
        {
            dtGDI_t * dtGDI = (dtGDI_t *)wparam;
            dtGDI->Destroy();
            return 0;
        }

        //Windows messages
        case WM_PAINT:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window)
            {
                window->__repaint();
            }
            break;
        }
        case WM_SIZE:
            {
                windowGDI_ref window = windows.lookUp(hWnd);
                if (window)
                {
                    window->CreateMemRegion();
                }
                return 0;
            }
        case WM_CLOSE:
        {
            windows.remove(hWnd);
            DefWindowProc (hWnd, message, wparam, lparam); //Ejecutamos close de ventana
            //Si se cierra la ventana raiz terminamos programa
            if (hWnd == m_Root->m_hWnd) 
            {
                PostQuitMessage(0);
            }
            return 0;
        }
        case WM_DESTROY:
        {
            return 0;
        }
        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
        {
            windowGDI_ref window = windows.lookUp(hWnd);
            if (window && window->m_bOverrideRedirect == true)
            {
                WINDOWPOS * pos = (WINDOWPOS*)lparam;
                pos->x = window->m_Rect.left;
                pos->y = window->m_Rect.top;
                pos->cx = window->m_Rect.right-window->m_Rect.left;
                pos->cy = window->m_Rect.bottom-window->m_Rect.top;
            }
            break;
        }
    }
    return DefWindowProc (hWnd, message, wparam, lparam);
}

