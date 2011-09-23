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
// $Id: displayTask.h 10644 2007-08-23 15:39:53Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr__display_task__hh__
#define __gwr__display_task__hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/task.hh>

#include <gwr/gwr.h>

enum align_e
{
    CENTER,
    LEFT,
    RIGHT
};

class displayTask_t: public virtual threadedTask_t
{
public:
    class window_t: public virtual collectible_t, public virtual item_t
    {
    public:
        virtual ~window_t(void);

        virtual bool     release(void)= 0;

        virtual u32      getWinId(void) const = 0;

        virtual void     setTitle(const char *s)= 0;
        virtual void     setGeometry(const char *g)= 0;

        virtual unsigned getWidth(void) const = 0;
        virtual unsigned getHeight(void) const = 0;
        virtual int      getXPos(void) const= 0;
        virtual int      getYPos(void) const= 0;

        virtual bool     isMapped(void) const= 0;

        virtual void map(void)= 0;
        virtual void unmap(void)= 0;
        virtual void clear(void)= 0;
        virtual void resize(unsigned w, unsigned h)= 0;
        virtual void reposition(int x, int y)= 0;

        virtual void putRGB24(const u8 *data,
                              unsigned  width,
                              unsigned  height,
                              float     zoomx,
                              float     zoomy
                             )= 0;

        virtual void putRGB24inBG(const u8 *data,
                                  unsigned  width,
                                  unsigned  height,
                                  float     zoomx,
                                  float     zoomy
                                 )= 0;

        virtual bool loadFont(const char *fontname, float size, int style)= 0;
        virtual bool setBgColor(const char *color)= 0;
        virtual bool setBgColor(u8 r, u8 g, u8 b)= 0;
        virtual bool setFgColor(const char *color)= 0;
        virtual bool setFgColor(u8 r, u8 g, u8 b)= 0;

        virtual void drawLineInBG(int x1, int y1, int x2, int y2, int lw)= 0;
        virtual void drawString(int x, int y, const char *msg, align_e a)= 0;

        virtual void setGamma(float rGamma, float gGamma, float bGamma)= 0;
        virtual void setOverrideRedirect(bool ORflag)= 0;

        friend class smartReference_t<window_t>;

        virtual const char *className(void) const { return "window_t"; };
    };


public:
    displayTask_t(const char *dpyName= NULL);

protected:
    virtual ~displayTask_t(void);

public:
    virtual void getDefaultVisualCombination(unsigned *visualDepth, int *visualClass)= 0;

    virtual bool supportedVisualCombination(unsigned visualDepth, int visualClass)= 0;

    // getDisplay: to be removed (or "abstracted")
//    virtual Display  *getDisplay(void);
    virtual window_t *rootWindow(void)= 0;

    virtual smartReference_t<displayTask_t::window_t>
        createWindow(const char *title,
                     const char *geom,
                     smartReference_t<displayTask_t::window_t> parent,
                     unsigned    visualDepth,
                     int         visualClass,
                     unsigned    defaultWidth,
                     unsigned    defaultHeight
                    )= 0;

    virtual bool destroyWindow(smartReference_t<displayTask_t::window_t> win)= 0;

protected:
    virtual void IOReady(io_ref &)= 0;

public:
    virtual char const *className(void) const { return "displayTask_t"; };


    friend class displayTask_t::window_t;
    friend class smartReference_t<displayTask_t>;

};
typedef smartReference_t<displayTask_t          > displayTask_ref;
typedef smartReference_t<displayTask_t::window_t> window_ref;

displayTask_ref createDisplayTask(const char *kind, const char *options= NULL);

#endif
