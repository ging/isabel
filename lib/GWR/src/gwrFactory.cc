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
// $Id: gwrFactory.cc 10255 2007-05-31 15:44:25Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include <gwr/displayTask.h>
#include <icf2/notify.hh>


#ifdef HAVE_X11
#include "X11/dtX11.hh"
#include "GLX/dtGLX.hh"
#endif
#ifdef HAVE_OPENGL 
#include "OpenGL/dtOpenGL.hh"
#endif
#ifdef HAVE_GLUT 
#include "GLUT/dtGLUT.hh"
#endif
#ifdef HAVE_COCOA
#include "cocoa/dtCocoa.hh"
#endif
#ifdef HAVE_DIRECTX 
#include "directX/dtDirectX.hh"
#endif
#ifdef HAVE_GDI
#include "GDI/dtGDI.h"
#endif

displayTask_ref
createDisplayTask(const char *kind, const char *options)
{
NOTIFY("Trying to create DisplayTask with renderer %s: ", kind);
#ifdef HAVE_X11
    if (strcmp(kind, "X11") == 0)
    {
        if ((options != NULL) && strcmp(options, "GLX") == 0)
        {
            try
            {
                NOTIFY_ND("trying GLX on X11\n");
                return new dtGLX_t();
            }
            catch (char const *e)
            {
                NOTIFY_ND("GLX on X11 NOT available\n");
                NOTIFY_ND("falling back to Shm X11\n");
            }
        }

        NOTIFY_ND("X11 available\n");

        return new dtX11_t();
    }
#endif
#ifdef HAVE_OPENGL 
    if (strcmp(kind, "OpenGL") == 0)
    {
        NOTIFY_ND("OpenGL available\n");
        return new dtOpenGL_t(options);
    }
#endif
#ifdef HAVE_COCOA
    if (strcmp(kind, "Cocoa") == 0)
    {
        NOTIFY_ND("Cocoa available\n");
        return new dtCocoa_t(options);
    }
#endif
#ifdef HAVE_DIRECTX 
    if (strcmp(kind, "DirectX") == 0)
    {
        NOTIFY_ND("DirectX available\n");
        return new dtDirectX_t(options);
    }
#endif
#ifdef HAVE_GDI
    if (strcmp(kind, "GDI") == 0)
    {
        NOTIFY_ND("GDI available\n");
        return new dtGDI_t(options);
    }
#endif
    NOTIFY_ND("Invalid DisplayTask provided: %s is unknown\n", kind);

    throw "Invalid DisplayTask provided";

    return NULL;
}

