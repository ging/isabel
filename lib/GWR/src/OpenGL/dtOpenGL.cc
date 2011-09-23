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
// $Id: dtOpenGL.cc 10868 2007-09-19 17:20:49Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <sys/time.h>

#include <icf2/dictionary.hh>
#include <icf2/notify.hh>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "dtOpenGL.hh"

dtOpenGL_t::dtOpenGL_t(const char *dpyName)
:dtX11_t(dpyName),
    lastDraw(0),
    usLimit(1000000/25),
    rotation(0),
    realWindow(NULL),
    windowCount(0)
{
    //NOTIFY("Building OpenGL display task with %s\n",glGetString(GL_VERSION));
    memset(texture,0,sizeof(GLuint)*MAX_TEXTURE_ID);
}

dtOpenGL_t::~dtOpenGL_t(void)
{
NOTIFY("CALLED DT_OPENGL_T DESTRUCTOR\n");
    if(mainWindow.ctx)
    {
        if(!glXMakeCurrent(mainWindow.dpy, None, NULL))
        {
            /*an error occurred*/
        }
        glXDestroyContext(mainWindow.dpy, mainWindow.ctx);
        mainWindow.ctx = NULL;
    }

    XCloseDisplay(mainWindow.dpy);
}

window_ref
dtOpenGL_t::createWindow(const char *title,
                         const char *geom,
                         window_ref  parent,
                         unsigned    visualDepth,
                         int         visualClass,
                         unsigned    defaultWidth,
                         unsigned    defaultHeight
                        )
{
    if (realWindow == NULL)
    {
        dtX11_ref myref = (dtX11_ref)this;
        realWindow =
            this->dtX11_t::createWindow(title,
                                        geom,
                                        parent,
                                        visualDepth,
                                        visualClass,
                                        800,
                                        600
                                       );
        window_t *wtp = (window_t *)realWindow;
        windowX11_t *wxtp = dynamic_cast<dtX11_t::windowX11_t *>(wtp);
        windowX11_ref wxr = wxtp;
        int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 8,
                             GLX_GREEN_SIZE, 8,
                             GLX_BLUE_SIZE, 8,
                             GLX_DEPTH_SIZE, 16,
                             None
                            };

        int attrListDbl[] = {GLX_RGBA, GLX_DOUBLEBUFFER,
                             GLX_RED_SIZE, 8,
                             GLX_GREEN_SIZE, 8,
                             GLX_BLUE_SIZE, 8,
                             GLX_DEPTH_SIZE, 16,
                             None
                            };
        XVisualInfo *vi;
        mainWindow.dpy = myref->getDisplay();
        mainWindow.screen = DefaultScreen(mainWindow.dpy);
        vi = glXChooseVisual(mainWindow.dpy, mainWindow.screen, attrListDbl);
        if(NULL == vi)
        {
            vi = glXChooseVisual(mainWindow.dpy, mainWindow.screen, attrListSgl);
        }
        mainWindow.ctx = glXCreateContext(mainWindow.dpy, vi, 0, GL_TRUE);
        mainWindow.win = wxr->getWinId();
        glXMakeCurrent(mainWindow.dpy, mainWindow.win, mainWindow.ctx);

        this->initializeGL();
        this->resizeGLScene(800,600);
    }
    windowCount++;
    windowOpenGL_t *woglr =
        (windowOpenGL_t *)new dtOpenGL_t::windowOpenGL_t(this,
                                                         title,
                                                         geom,
                                                         parent,
                                                         visualDepth,
                                                         visualClass,
                                                         defaultWidth,
                                                         defaultHeight
                                                        );
    return woglr;
}

bool
dtOpenGL_t::destroyWindow(smartReference_t<displayTask_t::window_t> win)
{
    if (realWindow == NULL) // already destroyed
    {
        return false;
    }

    bool res= realWindow->release(); // deactivate window

    realWindow= NULL;

    return res;
}

void
dtOpenGL_t::setLastDraw(unsigned long time)
{
    this->lastDraw = time;
}

unsigned long
dtOpenGL_t::getLastDraw()
{
    return lastDraw;
}

void
dtOpenGL_t::setUsLimit(unsigned fps)
{
    this->usLimit = 1000000/fps;
}

void
dtOpenGL_t::setNoLimitation()
{
    this->usLimit = 1;
}

unsigned long
dtOpenGL_t::getUsLimit()
{
    return this->usLimit;
}

void
dtOpenGL_t::resizeGLScene(unsigned w, unsigned h)
{
    mainWindow.height = h;
    mainWindow.width = w;
    float width,height;
    if(mainWindow.height == 0)
        mainWindow.height = 1;
    if(mainWindow.width == 0)
        mainWindow.width = 1;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,mainWindow.width,mainWindow.height);
    width = (float)w;
    height = (float)h;
    gluPerspective(45.0f, width/height, 0.1f, 50.0f);
    glMatrixMode(GL_MODELVIEW);
}

void
dtOpenGL_t::initializeGL(void)
{
    glDisable (GL_LINE_SMOOTH);

    glDisable (GL_LIGHTING);

    glClearColor (0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth (1.0f);
    glDepthFunc (GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glShadeModel (GL_SMOOTH);
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

    surface = glGenLists(1);              // generate display list
    glNewList(surface, GL_COMPILE_AND_EXECUTE);  // fill display list
        glBegin(GL_QUADS);
            glNormal3f( 0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f,  1.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, -0.5f,  1.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,  0.5f,  1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  1.0f);
        glEnd();
    glEndList();

    glFlush();
}

void
dtOpenGL_t::drawGLScene(void)
{
    unsigned long elapsed = 0;
    struct timeval currt;

    gettimeofday (&currt, NULL);
    elapsed = (currt.tv_sec*1000000) + currt.tv_usec - lastDraw;

    if (elapsed <= usLimit)
    {
        return;
    }

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float phase = 360.0f/((float)windowCount);

    for (unsigned a = 0; a< MAX_TEXTURE_ID; a++)
    {
        if (texture[a] != 0)
        {
            glLoadIdentity();
            glBindTexture(GL_TEXTURE_2D, texture[a]);
            glTranslatef (0.0f, 0.0f, -4.0f);
            glRotatef(rotation+phase*a,0.0f,1.0f,0.0f);
            glCallList(surface);
        }
    }
    rotation +=2.0f;

    glXSwapBuffers(mainWindow.dpy, mainWindow.win);
    gettimeofday(&currt, NULL);
    lastDraw = (currt.tv_sec*1000000) + currt.tv_usec;
}

window_ref
dtOpenGL_t::getRealWindow(void)
{
    return realWindow;
}

GLuint
dtOpenGL_t::buildTexture(void)
{
    /*Let's search for an empty slot to store our texture*/
    for (unsigned a = 0; a< MAX_TEXTURE_ID; a++)
    {
        if (texture[a] == 0)
        {
            /*Found empty slot, building a new texture and binding it to a texture2D type*/
            glGenTextures(1, &texture[a]);
            glBindTexture( GL_TEXTURE_2D, texture[a]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            return texture[a];
        }
    }
    throw ("I cannot believe it!!! 1024 people in one Isabel Session!!!");
}

void
dtOpenGL_t::refreshTexture(GLuint id, const u8* data, unsigned width, unsigned height)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0,
                  GL_RGB, GL_UNSIGNED_BYTE, data
                 );
}

dtOpenGL_t::windowOpenGL_t::windowOpenGL_t(dtOpenGL_t *d,
                                           const char *t,
                                           const char *geomStr,
                                           window_ref  theparent,
                                           unsigned    visualDepth,
                                           int         visualClass,
                                           unsigned    defaultWidth,
                                           unsigned    defaultHeight
                                          )
:windowX11_t(d,
             t,
             geomStr,
             theparent,
             visualDepth,
             visualClass,
             800,
             600
            )
{
    this->texId = d->buildTexture();
    dtParent = d;
}

dtOpenGL_t::windowOpenGL_t::windowOpenGL_t(dtOpenGL_t *d)
:windowX11_t(d)
{
    dtParent = d;
}

dtOpenGL_t::windowOpenGL_t::~windowOpenGL_t(void)
{
}

//
// the GUI window has been destroy, release data
//
bool
dtOpenGL_t::windowOpenGL_t::release(void)
{
    return dtParent->getRealWindow()->release();
}

void
dtOpenGL_t::windowOpenGL_t::resize(unsigned w, unsigned h)
{
}

void
dtOpenGL_t::windowOpenGL_t::putRGB24(const u8 *data,
                                     unsigned width,
                                     unsigned height,
                                     float zoomx,
                                     float zoomy
                                    )
{
    dtParent->refreshTexture(texId, data, width, height);
    dtParent->drawGLScene();
}

void
dtOpenGL_t::windowOpenGL_t::putRGB24inBG(const u8 *data,
                                  unsigned width,
                                  unsigned height,
                                  float zoomx,
                                  float zoomy
                                 )
{
    putRGB24(data,width,height,zoomx,zoomy);
}

u32
dtOpenGL_t::windowOpenGL_t::getWinId(void) const
{
    return dtParent->getRealWindow()->getWinId();
}

void
dtOpenGL_t::windowOpenGL_t::setTitle(const char *s)
{
    dtParent->getRealWindow()->setTitle(s);
}
void
dtOpenGL_t::windowOpenGL_t::setGeometry(const char *g)
{
    dtParent->getRealWindow()->setGeometry(g);
}

unsigned
dtOpenGL_t::windowOpenGL_t::getWidth(void) const
{
    return dtParent->getRealWindow()->getWidth();
}
unsigned
dtOpenGL_t::windowOpenGL_t::getHeight(void) const
{
    return dtParent->getRealWindow()->getHeight();
}
int
dtOpenGL_t::windowOpenGL_t::getXPos(void) const
{
    return dtParent->getRealWindow()->getXPos();
}
int
dtOpenGL_t::windowOpenGL_t::getYPos(void) const
{
    return dtParent->getRealWindow()->getYPos();
}

bool
dtOpenGL_t::windowOpenGL_t::isMapped(void) const
{
    return dtParent->getRealWindow()->isMapped();
}

void
dtOpenGL_t::windowOpenGL_t::map(void)
{
    dtParent->getRealWindow()->map();
}
void
dtOpenGL_t::windowOpenGL_t::unmap(void)
{
    dtParent->getRealWindow()->unmap();
}
void
dtOpenGL_t::windowOpenGL_t::clear(void)
{
    dtParent->getRealWindow()->clear();
}

void
dtOpenGL_t::windowOpenGL_t::reposition(int x, int y)
{
    dtParent->getRealWindow()->reposition(x,y);
}

void
dtOpenGL_t::windowOpenGL_t::drawString(int x, int y, const char *msg, align_e a)
{
    dtParent->getRealWindow()->drawString(x, y, msg, a);
}

void
dtOpenGL_t::windowOpenGL_t::setGamma(float rGamma, float gGamma, float bGamma)
{
    dtParent->getRealWindow()->setGamma(rGamma, gGamma, bGamma);
}
void
dtOpenGL_t::windowOpenGL_t::setOverrideRedirect(bool ORflag)
{
    dtParent->getRealWindow()->setOverrideRedirect(ORflag);
}
