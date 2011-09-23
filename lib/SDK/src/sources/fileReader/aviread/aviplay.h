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
#ifndef AVIFILE_IAVIPLAYER_H
#define AVIFILE_IAVIPLAYER_H

/**
 *  AVI player, ver. 0.7.X
 *  Copyright 2002 Zdenek Kabelac (kabi@users.sf.net)
 *  Copyright 2001 Eugene Kuznetsov  (divx@euro.ru)
 */

#include "infotypes.h"
#include "subtitle.h"
#include "image.h"
#include "avm_args.h"

typedef void (*KILLHANDLER)(int, void* p);
typedef int (*AUDIOFUNC)(void* srcdata, unsigned int size, void* p);

AVM_BEGIN_NAMESPACE;

class IPlayerWidget;
class IRtConfig;
class IVideoRenderer;
class StreamInfo;

/**
 *
 *   This class is meant to contain all code that is
 * not related to graphical interface of AVI/ASF player: you provide
 * an interface ( buttons, windows, etc. ), this class
 * provides you with video data and performs audio playback.
 *
 *   NEVER call playback-controlling functions ( even indirectly
 * through event processing calls, such as qApp->processEvents
 * in Qt ) from its callbacks.
 *
 *    Usage:
 *
 *   Call CreateAviPlayer(), specifying file name of AVI/ASF file, bit depth
 * of physical screen and optional file name of subtitle file.
 * Catch FatalError if player can't be constructed. Set two callbacks:
 * kill handler ( will be called when player stops ) and draw callback
 * ( called each frame during playback and frequently if paused. Takes
 * non-zero pointer to video data or NULL if frame sholdn't be drawn ).
 *  Passed URL may refer to remote ( MMS://... ) ASF file. Such files are
 * opened asynchronously. It means that constructor only performs sanity
 * checking on the file name, starts actual opening process ( in another
 * thread ) and returns. You should periodically call isOpened() until
 * it returns true, which means that opening process has finished. After that,
 * you can determine if it was successful by calling isValid(). For local
 * files isOpened() and isValid() will return 'true' immediately after
 * successful construction.
 *   URL can point to ASX ( redirector ) file. For such file isRedirector()
 * will return 'true'. If this is the case, read list of redirected URLs
 * with getURLs(). After that, IAviPlayer object may be destroyed because
 * you can't do anything else with it.
 *  Prepare to draw video and call start(). Player immediately starts
 * playing. Its state can be controlled by calls play()/pause()/reseek(),
 * player can be stopped by stop() and started again by start().
 * Once again: don't try to call these functions from draw callback or
 * your program will burn in hell forever.
 *  When playing ASF files over the network and AVI files over slow
 * connections, player may occasionally get into 'buffering' state.
 * It means that player stops to output data and waits until its internal
 * buffers are refilled. You can determine if it's in buffering state
 * by calling GetState(). It'll put buffer fill percentage into variable
 * pointed by its argument if buffering takes place ( here 1 corresponds
 * to full buffers and 0 to empty buffers ).
 *  Player can be destroyed with 'delete'. Deleting automatically stops
 * playback and frees all resources.
 *
 */

class IAviPlayer
{
public:
    enum Property
    {
	PROPERTY_END,		// 0 - marks last property
	AUDIO_CODECS,		// char*
	VIDEO_CODECS,		// char*
	AUDIO_RENDERER,		// char*
	VIDEO_RENDERER,		// char*
	SUBTITLE_RENDERER,	// char*
	USE_YUV,		// bool
	AUTOREPEAT,		// bool
	AUDIO_STREAM,		// int (0..127)
	VIDEO_STREAM,		// int
	SUBTITLE_FONT,		// char*
	SUBTITLE_ASYNC_TIME_MS,	// int
	SUBTITLE_EXTEND_TIME_MS,// int
	SUBTITLE_CODEPAGE,	// char* (iconv --list)
	SUBTITLE_ENABLED,	// int
	SUBTITLE_WRAP,		// int
	SUBTITLE_BGCOLOR,	// int
	SUBTITLE_FGCOLOR,	// int
	SUBTITLE_HPOSITION,	// int
	USE_HTTP_PROXY, 	// bool
	HTTP_PROXY,		// char*
	AUDIO_RESAMPLING,	// bool
	AUDIO_RESAMPLING_RATE,	// int
	AUDIO_PLAYING_RATE,	// int
	AUDIO_MASTER_TIMER,	// bool
	AUDIO_VOLUME,		// int (0..1000 - is standard level)
	AUDIO_BALANCE,		// int (0..1000 - 500 - is standard mid-level)
	AUDIO_URL,		// char* URL
	VIDEO_PRESERVE_ASPECT,	// bool
	VIDEO_BUFFERED, 	// bool
	VIDEO_DIRECT,		// bool
	VIDEO_DROPPING, 	// bool
	VIDEO_QUALITY_AUTO,	// bool
	DISPLAY_FRAME_POS,	// bool
	ASYNC_TIME_MS,		// int

	// fill new entires here
	// add appropriate line into avifileprop.cpp
	/** for internal usage only */
	LAST_PROPERTY,

	/** average drop in % (int*) */
	QUERY_AVG_DROP = 10000,
	/** average image quality in %	(int*) */
	QUERY_AVG_QUALITY,
	/** */
	QUERY_AUDIO_STREAMS,	// int	  number of audio streams
	QUERY_VIDEO_STREAMS,	// int	  number of video streams
	QUERY_AUDIO_RENDERERS,
	QUERY_VIDEO_RENDERERS,
	QUERY_SUBTITLE_RENDERERS,
	QUERY_VIDEO_WIDTH,	// int
	QUERY_VIDEO_HEIGHT,	// int
	QUERY_EOF		// bool
#ifdef AVM_COMPATIBLE
	// backward compatible
	, DEFAULT_AUDIO_STREAM =	AUDIO_STREAM
#endif
    };

    enum State
    {
	Opening,
	Buffering,
	Playing,
	Stopped,
	Paused,
	Invalid
    };

    /** options for aviplay */
    static const Args::Option options[];

    /** filename of the currently played stream */
    virtual const char* GetFileName() const =0;
    /** retrieve player state */
    virtual State GetState(double* percent) =0; // player state
    virtual bool GetURLs(avm::vector<std::string>& urls) const =0;

    virtual double GetAudioLengthTime() const =0;
    virtual const char* GetAudioFormat() const =0;
    virtual StreamInfo* GetAudioStreamInfo() const =0;

    virtual double GetVideoLengthTime() const =0;
    virtual const char* GetVideoFormat() const =0;
    virtual StreamInfo* GetVideoStreamInfo() const =0;
    virtual double GetFps() const =0;		// video frames per second
    virtual int GetWidth() const =0;		// video width
    virtual int GetHeight() const =0;		// video height

    virtual double GetLengthTime() const = 0;	// length of longer stream
    virtual double GetTime() const =0;
    virtual framepos_t GetFramePos() const =0;	// playing frame position

    virtual const subtitle_line_t* GetCurrentSubtitles() =0;
    virtual bool HasSubtitles() const =0;
    virtual int InitSubtitles(const char* filename) =0;

    virtual bool IsOpened() const =0;		// true if stream is opened
    virtual bool IsRedirector() const =0;
    virtual bool IsValid() const =0;

    virtual bool IsPaused() const =0;		// true -> paused
    virtual bool IsPlaying() const =0;		// true -> playing || paused
    virtual bool IsStopped() const =0;		// true -> !playing && !paused

    virtual int NextFrame() =0;		// to next frame ( mostly useful in pause mode )
    virtual int NextKeyFrame() =0;	// to next keyframe
    virtual int PrevFrame() =0;		// to prev frame - might take a lot of time!!!
    virtual int PrevKeyFrame() =0;	// to prev keyframe ( the one before frame drawn last time )

    virtual void Pause(bool state) =0;
    virtual void Play() =0;			// start playing

    virtual double Reseek(double timepos) =0;
    virtual int ReseekExact(double timepos) =0;  // nonzero if unable

    virtual int SetColorSpace(fourcc_t csp, bool test_only) =0;
    virtual void SetKillHandler(KILLHANDLER handler, void* argument=0) =0;
    virtual void SetAudioFunc(AUDIOFUNC func, void* argument=0) =0;

    virtual void Start() =0;
    virtual void Stop() =0;

    // reading and setting list of video renderers - should work in runtime later
    virtual const avm::vector<IVideoRenderer*>& GetVideoRenderers() const =0;
    virtual int SetVideoRenderers(avm::vector<IVideoRenderer*>) =0;
    //to be added!!!  virtual const std::vector<AudioRenderer*>& GetAudioRenderers() const =0;
    //to be added!!!  virtual int SetAudioRenderers(vector<AudioRenderer*>) =0;

    // General interface to control various things at runtime
    // - list of properties with arguments is finished with 0 (PROPERTY_END)
    // WARNING: passed pointers has to match its type!!!
    virtual int Get(...) const = 0;
    virtual int Set(...) = 0;

    /**
     * type could be AUDIO_CODECS, VIDEO_CODECS
     */
    virtual const CodecInfo& GetCodecInfo(int type = VIDEO_CODECS) const =0;
    /**
     * type could be AUDIO_CODECS, VIDEO_CODECS, AUDIO_RENDERER, VIDEO_RENDERER
     */
    virtual IRtConfig* GetRtConfig(int type = VIDEO_CODECS) const =0;
    virtual void Restart() =0;

    virtual bool GetVideo() const =0;
    virtual void SetVideo(bool enabled) =0;

    virtual bool GetAudio() const =0;
    virtual void SetAudio(bool enabled) =0;

#ifdef AVM_COMPATIBLE
    //
    // Obsolete backward compatible calls
    // Audio/video sync correction,  argument in seconds
    //
    virtual float GetAsync() const =0;
    virtual void SetAsync(float asynctime) =0;

    // \deprecated
    int PageUp() { return NextKeyFrame(); }
    // \deprecated
    int PageDown() { return PrevKeyFrame(); }
    // \deprecated
    IRtConfig* GetRuntimeConfig() const { return GetRtConfig(); }
    // \deprecated
    double GetPos() const  { return GetTime(); }
#endif
};

/**
 * This object has interface very similar to IAviPlayer, but
 * it does drawing by itself. You do not need to provide
 * pointer to kill handler, but you can still do it. Attempts
 * to install draw callback will be ignored.
 * Create an object with this interface by calling CreateAviPlayer2().
 * Its arguments:
 * parent - pointer to interface which handles a few specific
 * key & mouse events in movie window.
 * Optional. Pass it if you want to do this handling by yourself.
 * dpy - pointer to opened X display.
 * Optional. If it's non-NULL,
 * movie window will be created on this display. If it's NULL,
 * movie window won't be created at all.
 * filename & subname - same as when constructing IAviPlayer.
 */
class IAviPlayer2: public IAviPlayer
{
public:
    virtual ~IAviPlayer2() {}
    virtual int Refresh()					=0;
    virtual int Resize(int& new_w, int& new_h)			=0;
    virtual int Zoom(int x, int y, int w, int h)		=0;
    virtual int ToggleFullscreen(bool maximize = false)		=0;
};

IAviPlayer* CreateAviPlayer(const char* filename, int bitdepth,
			    const char* subfile = 0, unsigned int flags = 0,
			    // vcodec & acodec are going to be supported
			    const char* vcodec = 0, const char* acodec = 0);

//void* instead of Display*
IAviPlayer2* CreateAviPlayer2(IPlayerWidget*, void* dpy, const char* filename,
			      const char* subname, unsigned int flags = 0,
			      // vcodec & acodec are going to be supported
			      const char* vcodec = 0, const char* acodec = 0);

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE
#include "avm_except.h"
typedef avm::IAviPlayer IAviPlayer;
typedef avm::IAviPlayer2 IAviPlayer2;
#endif // AVM_COMPATIBLE

extern "C" int GetAvifileVersion() ;

#endif //AVIFILE_IAVIPLAYER_H
