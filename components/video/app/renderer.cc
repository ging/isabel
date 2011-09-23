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
/////////////////////////////////////////////////////////////////////////
//
// $Id: renderer.cc 21743 2010-12-17 10:44:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include "videoApp.hh"
#include "renderer.hh"

renderer_t::renderer_t(u32 nchId, stats_t *myS)
: chId(nchId)
{
    int vDepth, vClass;

    theApp->dpy->getDefaultVisualCombination((unsigned*)&vDepth, &vClass);
    NOTIFY("renderer_t:: using default vDepth= %d, vClass= %d\n",
           vDepth,
           vClass
          );

    win= theApp->dpy->createWindow("ISABEL Video",
                                   NULL,
                                   NULL,
                                   vDepth, vClass,
                                   320, 240
                                  );

    // default color for titles
    win->setFgColor("light blue");

    // loading font
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    const char *fontname= "-*-courier-bold-r-*-*-24-*-*-*-*-*-iso8859-1";
    fontLoaded= win->loadFont(fontname, 0, 0);
#elif defined(__BUILD_FOR_WINXP)
    char *fontname= "Arial";
    fontLoaded= win->loadFont(fontname, 24, 0);
#endif        
    myStats= myS;
}

renderer_t::~renderer_t(void)
{
    theApp->dpy->destroyWindow(win);

    win= NULL;

    myStats= NULL;
}

u32
renderer_t::getWinId(void)
{
    return win->getWinId();
}

bool
renderer_t::isMapped(void)
{
    return win->isMapped();
}

void
renderer_t::mapWindow(void)
{
    win->map();
}

void
renderer_t::unmapWindow(void)
{
    win->unmap();
}

void
renderer_t::clear(void)
{
    win->clear();
}

void
renderer_t::paint(image_t *img, bool followSize)
{
    if ( ! win.isValid())
    {
        NOTIFY("renderer_t::paint: [ch=%d] Invalid window?!?!?!\n", chId);
        delete img;
        return;
    }

    if (followSize)
    {
        win->resize(img->getWidth(), img->getHeight());
    }

#if 0
NOTIFY("Pintando imagen %dx%d en ventana %dx%d\n",
       img->getWidth(), img->getHeight(),
       win->width(), win->height());
#endif

    if (img->getFormat()== RGB24_FORMAT)
    {
        win->putRGB24(img->getBuff(),
                      img->getWidth(), img->getHeight(),
                      -1, -1
                     );

        myStats->accountPaintFrame();

    }
    else
    {
        NOTIFY("renderer_t::paint: unacceptable format %s==0x%x\n",
               vGetFormatNameById(img->getFormat()),
               img->getFormat()
              );
    }

    delete img;
}

void
renderer_t::setGeometry(const char *geom)
{
    win->setGeometry(geom);
}

void
renderer_t::setOverrideRedirect(bool ORFlag)
{
    win->setOverrideRedirect(ORFlag);

    // sometimes, OverrideRedirect take a time to have effect,
    // mapping again the window (if mapped) makes it visible at once
    if (win->isMapped())
    {
        win->unmap();
        win->map();
    }
}

void
renderer_t::setTitle(const char *title)
{
    win->setTitle(title);
}

void
renderer_t::drawMsg(const char *acro)
{
    int x, y;

    if ( ! fontLoaded )
    {
        NOTIFY("renderer_t::drawMsg: cannot drawMsg, no fontInfo\n");
        return;
    }

    x= (win->getWidth() * 95) / 100;  // 95% (*0.95 int comp.)
    y= (win->getHeight() * 95) / 100;

    if ( acro && acro[0] )
    {
        win->drawString(x, y, acro, RIGHT);
    }
}


