/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __app_messages_h__
#define __app_messages_h__

#include <windows.h>
#include "Color.h"
#include "gwr\gwr.h"
#include "gwr\displayTask.h"

//Definicion de mensaje de comando de aplicacion

//windowGDI_t messages
#define WM_WGDI_GO_AWAY            WM_APP + 0x0001 //wParam = 0
#define WM_WGDI_SET_TITLE        WM_APP + 0x0002 //wParam = char * title
#define WM_WGDI_SET_GEOMETRY    WM_APP + 0x0003 //wParam = char * geom
#define WM_WGDI_MAP                WM_APP + 0x0004 //wParam = 0
#define WM_WGDI_UNMAP            WM_APP + 0x0005 //wParam = 0
#define WM_WGDI_CLEAR            WM_APP + 0x0006 //wParam = 0
#define WM_WGDI_RESIZE            WM_APP + 0x0007 //wParam = RESIZE_DATA * size
#define WM_WGDI_REPOSITION        WM_APP + 0x0008 //wParam = REPOSITION_DATA * point
#define WM_WGDI_PUT_RGB24        WM_APP + 0x0009 //wParam = RGB24_DATA * data
#define WM_WGDI_PUT_RGB24_BG    WM_APP + 0x000a //wParam = RGB24_DATA * data
#define WM_WGDI_DRAW_LINE_BG    WM_APP + 0x000b //wParam = LINE_DATA * line
#define WM_WGDI_DRAW_STRING        WM_APP + 0x000c //wParam = STRING_DATA * string

//dtGDI_t messages
#define WM_DT_CREATE_WGDI        WM_APP + 0x0020 //wParam = windowGDI_t**; // lParam = WNDGDI_DATA *
#define WM_DT_DESTROY_WGDI        WM_APP + 0x0021 //wParam = 0; // lParam = 0
#define WM_DT_DESTROY_DT        WM_APP + 0x0022 //wParam = dtGDI_t *; // lParam = 0

//lParam --> punteros a las estructuras de informacion
typedef SIZE RESIZE_DATA;

typedef POINT REPOSITION_DATA;

#define STR_LEN 256

typedef struct RGB24_DATA
{
    const u8 *data;
    unsigned  width;
    unsigned  height;
    float     zoomx;
    float     zoomy;
}RGB24_DATA;

typedef struct LINE_DATA 
{
    color_t color;
    float width;
    POINT start;
    POINT end;
}LINE_DATA;

typedef struct STRING_DATA
{
    POINT point;
    char  string[STR_LEN];
    align_e align;
}STRING_DATA;

typedef struct WNDGDI_DATA
{
    char   title[STR_LEN];
    char   geom[STR_LEN];
    displayTask_t::window_t * parent;
    int visualDepth;
    int visualClass;
    unsigned defaultWidth;
    unsigned defaultHeight;
    HANDLE newWindow;
}WNDGDI_DATA;

#endif

