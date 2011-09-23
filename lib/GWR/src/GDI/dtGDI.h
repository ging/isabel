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
/////////////////////////////////////////////////////////////////////////
//
// $Id: dtGDI.hh,v 1.1.2.1 2007/05/31 15:44:25 gabriel Exp $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2007. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr__dtGDI__hh__
#define __gwr__dtGDI__hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/task.hh>
#include <icf2/dictionary.hh>

#include <gwr/gwr.h>
#include <gwr/displayTask.h>
#include <gdiplus.h>

#include "color.h"

class dtGDI_t: public virtual displayTask_t
{
public:
    class windowGDI_t: public virtual collectible_t, public virtual displayTask_t::window_t 
    {
    private:

    	bool m_bReleased;
        unsigned short m_Depth;
        RECT m_Rect;
        color_t m_bkColor,m_fgColor;
        gamma_t m_Gamma;
        HDC  m_hDC,m_hMemDC;
        HBITMAP m_hRegion;
        HWND m_hWnd;
        bool m_bMapped,m_bOverrideRedirect,m_bRoot;
        HANDLE m_hMutex;
        Gdiplus::Font * m_Font;

    	static const int DEFAULT_X = 10;
    	static const int DEFAULT_Y = 10;
    	static const int DEFAULT_W = 320;
    	static const int DEFAULT_H = 240;
        
    private:

        void CreateMemRegion(void);
        void SetRoot(bool);
        
        void __setTitle(const char *s);
        void __setGeometry(const char *g);
        void __map(void);
        void __unmap(void);
        void __clear(HDC MemDC = NULL);
        void __resize(unsigned w, unsigned h);
        void __reposition(int x, int y);
        void __putRGB24(const u8 *data,
                          unsigned  width,
                          unsigned  height,
                          float     zoomx,
                          float     zoomy,
                          bool BG
                         );
        void __drawLineInBG(color_t color,float width,int x1,int y1,int x2,int y2);
        void __drawString(int x, int y, const char *msg, align_e a);
        void __repaint(void);
                
    public:

        windowGDI_t(const char *title,
                    const char *geom,
                    window_ref  parent,
                    unsigned    visualDepth,
                    int         visualClass,
                    unsigned    defaultWidth,
                    unsigned    defaultHeight
                    );

        virtual ~windowGDI_t(void);

    	virtual bool     release(void);
        void goAway(void);

        u32      getWinId(void) const ;

        void     setTitle(const char *s);
        void     setGeometry(const char *g);

        unsigned getWidth(void) const ;
        unsigned getHeight(void) const ;
        int      getXPos(void) const;
        int      getYPos(void) const;

        bool     isMapped(void) const;

        void map(void);
        void unmap(void);
        void clear(void);
        void resize(unsigned w, unsigned h);
        void reposition(int x, int y);

        void putRGB24(const u8 *data,
                      unsigned  width,
                      unsigned  height,
                      float     zoomx,
                      float     zoomy
                     );

        void putRGB24inBG(const u8 *data,
                                  unsigned  width,
                                  unsigned  height,
                                  float     zoomx,
                                  float     zoomy
                                 );

        bool loadFont(const char *family, float size, int style);
        bool setBgColor(const char *color);
        bool setBgColor(u8 r,u8 g,u8 b);
        bool setFgColor(const char *color);
        bool setFgColor(u8 r,u8 g,u8 b);
    
        void drawLineInBG(int x1, int y1, int x2, int y2, int lwidth);
        void drawString(int x, int y, const char *msg, align_e a);

        void setGamma(float rGamma, float gGamma, float bGamma);
        void setOverrideRedirect(bool ORflag);

        friend class dtGDI_t;
        friend class smartReference_t<windowGDI_t>;
        virtual const char *className(void) const { return "windowGDI_t"; };
    };

private:

    unsigned short m_Depth;
    HANDLE    m_hThread;
    static windowGDI_t* m_Root;
    ULONG_PTR m_gdiplusToken;

    void MainLoop(void);

    static DWORD WINAPI Run(void * lParam);
    static void DisplayError(LPCTSTR function);
    static LRESULT WINAPI MsgProc(HWND hWnd,UINT message,WPARAM wparam,LPARAM lparam);
    unsigned short GetDisplayDepth(void);

public:

    dtGDI_t(const char *options= NULL);

protected:

    virtual ~dtGDI_t(void);
    virtual void IOReady(io_ref &){};
    virtual void heartBeat(void){};

public:

    void getDefaultVisualCombination(unsigned *visualDepth, int *visualClass);
    bool supportedVisualCombination (unsigned int,int);
    void Destroy(void);

    window_t *rootWindow(void);

    window_ref createWindow(const char *title,
                            const char *geom,
                            window_ref  parent,
                            unsigned    visualDepth,
                            int         visualClass,
                            unsigned    defaultWidth,
                            unsigned    defaultHeight
                           );
    bool destroyWindow(smartReference_t<displayTask_t::window_t> win);
public:

    char const *className(void) const { return "dtGDI_t"; };
    friend class dtGDI_t::windowGDI_t;
    friend class smartReference_t<dtGDI_t>;
};

typedef smartReference_t<dtGDI_t          > dtGDI_ref;
typedef smartReference_t<dtGDI_t::windowGDI_t> windowGDI_ref;

#endif

