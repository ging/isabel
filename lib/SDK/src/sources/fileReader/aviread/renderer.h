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
#ifndef AVIFILE_RENDERER_H
#define AVIFILE_RENDERER_H

#include "avm_stl.h"
#include "image.h"
#include "subtitle.h"

AVM_BEGIN_NAMESPACE;

class IPlayerWidget;

/** Structure describes video mode */
struct VideoMode
{
    /// width of the screen
    int width;
    /// height of the screen
    int height;
    /// vertical refresh frequency
    float freq;
    /// textual representation for this mode
    std::string name;
};


/**
 * Abstract interface for video rendering
 *
 *  One day we may support more renderers
 *  We do not want to have any XFree specific stuff here
 */
class IVideoRenderer : public IImageAllocator
{
public:
    enum Property {
	SYNC,
	REFRESH,
	FULLSCREEN,		// true - maximize with FULLSCREEN toggle
	DRAW_CIMAGE,            // CImage*
	DRAW_SUBTITLES,

        WINDOW_SIZE = 100,      // int* width, int* height
	WINDOW_POSITION,        // get*/set int* x, int* y
        WINDOW_CAPTION,         // char*
        SUBTITLE_FONT,          // char*
        SUBTITLE_POSITION,      // get*/set int* y
        SUBTITLE_VISIBLE,
	VIDEO_MODES,		// avm::vector<VideoMode> - list of video modes
	VIDEO_OVERLAYS,		// avm::vector<fcctype_t> - list of overlay

        LAST
    };
    enum Cursor {
        NORMAL,
	ZOOMLU,
        ZOOMRB
    };
    /**
     * Allow YUV renderer to use software YUV->RGB conversion
     * ( slow, only for testing purposes )
     * SDL has very fast implementation - used now
     */
    static bool allow_sw_yuv;

    virtual ~IVideoRenderer() {}

    // possible new interface???
    virtual int Set(...) 					= 0;
    virtual int Get(...) const					= 0;
#if 1

    /**
     * Issues the request to draw the image in the window memory
     * might be empty when direct rendering is being used
     */
    virtual int Draw(const CImage* data)                        = 0;
    /**
     * Draws a line of subtitles below the picture.
     */
    virtual int DrawSubtitles(const subtitle_line_t* sl) { return -1; }
    /**
     * Returns actual size of the rendered image
     */
    virtual int GetSize(int& width, int& height) const	{ return -1; }
    /**
     * Returns actual x,y coordinates of the window.
     */
    virtual int GetPosition(int& x, int& y) const	{ return -1; }
    /**
     * Returns available video modes
     */
    virtual const avm::vector<VideoMode>& GetVideoModes() const	= 0;
    virtual IRtConfig* GetRtConfig() const { return 0; }
    /**
     * Called to blit image on the screen
     */
    virtual int Sync()						= 0;
    /**
     * Redraws image (resize, move with pause)
     */
    virtual int Refresh()                                       = 0;
    /**
     * Resizes renderer window and stretch the image in it.
     */
    virtual int Resize(int& new_w, int& new_h)		{ return -1; }
    /**
     * Set window titlebar
     * \param title window title name
     * \param icon  pathname to icon of the window
     */
    virtual int SetCaption(const char* title, const char* icon) { return -1; }
    /**
     * Set font for subtitle
     */
    virtual int SetFont(const char* font)		{ return -1; }
    /**
     * Set window position
     * \param x coordinate
     * \param y coordinate
     */
    virtual int SetPosition(int x, int y)		{ return -1; }
    /**
     * Toggle on/off fullscreen mode. maximize=true means that
     * renderer should stretch the picture to dimensions of 
     * screen when going fullscreen without changing screen mode.
     */
    virtual int ToggleFullscreen(bool maximize = false)	{ return -1; }
    /**
     * Pick area in the image which is displayed in the given window
     * \param x  coordinate for the begining of zoomed area
     * \param y  coordinate for the begining of zoomed area
     * \param width width of zoomed area (0 disables zooming)
     * \param height height of zoomed area (0 disables zooming)
     */
    virtual int Zoom(int x, int y, int width, int height) { return -1; }
    virtual int SetCursor(Cursor cursor) { return -1; }
#endif
    virtual int Lock() const = 0;
    virtual int TryLock() const = 0;
    virtual int Unlock() const = 0;
};

#ifndef X_DISPLAY_MISSING
/**
 * Creates SDL video renderer for RGB data in current depth of
 * display from 2nd argument. Last argument specifies whether
 * this renderer should reserve place for subtitles or not.
 */
IVideoRenderer* CreateFullscreenRenderer(IPlayerWidget*, void* dpy,
					int width, int height, bool sub = false);
/**
 * Creates SDL video renderer for YUV data in format yuv_fmt.
 * If it fails ( e.g. because yuv_fmt is not supported by X server
 * and VideoRenderer::allow_sw_yuv==false ), returns pointer to
 * 'ordinary' fullscreen renderer and puts 0 into yuv_fmt.
 */    
IVideoRenderer* CreateYUVRenderer(IPlayerWidget*, void* dpy,
				 int width, int height,
				 fourcc_t yuv_fmt, bool sub = false);
/*
VideoRenderer* CreateXvYUVRenderer(PlayerWidget*, void* dpy,
				   int width, int height,
				   fourcc_t yuv_fmt, bool sub = false);
 */
/**
 * Function that returns actual bit depth of specified X display.
 * Should be used instead of any other functions with similar
 * purpose ( DefaultDepth(), x11Depth() in Qt ) when determining
 * bit depth of data to pass to renderer.
 */
int GetPhysicalDepth(void* dpy);

#else
static inline int GetPhysicalDepth(void* dpy) { return 0; }
#endif // X_DISPLAY_MISSING



#ifdef HAVE_LIBXXF86DGA
//VideoRenderer* CreateDGARenderer();
#endif

AVM_END_NAMESPACE;


#ifdef AVM_COMPATIBLE
typedef avm::VideoMode VideoMode;
typedef avm::IVideoRenderer VideoRenderer;

#ifndef X_DISPLAY_MISSING
static inline VideoRenderer* CreateFullscreenRenderer(avm::IPlayerWidget* pw, void* dpy,
						      int width, int height, bool sub = false)
{
    return avm::CreateFullscreenRenderer(pw, dpy, width, height, sub);
}

static inline VideoRenderer* CreateYUVRenderer(avm::IPlayerWidget* pw, void* dpy,
					       int width, int height,
					       fourcc_t yuv_fmt, bool sub = false)
{
    return avm::CreateYUVRenderer(pw, dpy, width, height, yuv_fmt, sub);
}

#endif // X_DISPLAY_MISSING

static inline int GetPhysicalDepth(void* dpy)
{
    return avm::GetPhysicalDepth(dpy);
}

#endif

#endif // AVIFILE_RENDERER_H
