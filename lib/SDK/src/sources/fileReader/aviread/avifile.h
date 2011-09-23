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
#ifndef AVIFILE_AVIFILE_H
#define AVIFILE_AVIFILE_H

#include "videodecoder.h"
#include "avm_stl.h"

AVM_BEGIN_NAMESPACE;

class CImage;
class StreamInfo;
class IVideoRenderer;
class PthreadMutex;
class IAudioDecoder;
class IVideoDecoder;

/**
 *       Classes for reading & writing movie files.
 *   Interfaces are the same for all supported formats:
 *      AVI, local and networked ASF 1.0. Writing is
 *           possible only into AVI files.
 */

/**
 * Abstract class for reading streams
 */
class IStream
{
public:
    static const uint_t ERR = ~0U;
    static const uint_t KEYFRAME = 0x00000010L;
    enum StreamType
    {
	Audio,
	Video,
	Other
    };
    enum StreamFormat
    {
	Avi = 1,
	Wav = 10
    };
    virtual ~IStream();
};

/**
 *    This class provides means of sequential access to data from one
 * stream, usually video or soundtrack. Though AVI files rarely contain
 * more than one soundtrack, they are also supported. Streams of other
 * kind are not handled.
 *
 *   It maintains an internal position counter, which can be read with
 * @ref GetPos()/@ref GetTime() and changed with @ref Seek()/@ref SeekToKeyframe().
 * This counter is modified by all reading functions; function that reads
 * 10 frames will increase counter by 10.
 *
 *   IReadStream is capable of implicit video and audio decompression,
 * so that you won't need to work with decoders directly. All you need is
 * to call @ref StartStreaming(), and if it succeeds ( does not throw exception ),
 * call members that decompress data into internal buffer ( @ref ReadFrame() ),
 * return this buffer ( @ref GetFrame() ) or decompress data into your memory
 * ( @ref ReadFrames() ).
 *
 *   If you want to have direct access to compressed data, use @ref ReadDirect().
 * Note that this call is incompatible with streaming, so calls to @ref ReadDirect()
 * between @ref StartStreaming() and @ref StopStreaming() will fail. It is done so
 * because dropping even one data frame during sequence decompression will ruin
 * current picture.
 *
 *   Since version 0.6, these interfaces are also able to handle ASF 1.0 local
 * files, pre-recorded and broadcast MMS URLs. Special care is needed when
 * you want to deal with such files.
 *   First of all, you shouldn't assume that all video and audio frames have
 * the same duration ( result of GetFrameTime() ). It is not true for ASF files.
 * Instead, use GetTime().
 *   For MMS URLs, do not rely on GetPos() and Seek(framepos_t) at all.
 * These operations are simply unavailable in MMS protocol.
 * Use only SeekTime(double), and only when it's definitely needed, because it
 * is very lengthy operation. For live broadcast streams SeekTime(double)
 * is not available, either.
 *   There's an important issue with Seek() and multiple streams in MMS.
 * For the sake of efficiency library does not allow streams to be seekable
 * independently. Each Seek() operation on one stream automatically repositions
 * pointer for others approximately to the same position.
 *
 */
class IReadStream : public IStream
{
public:
    /** Checks for end of stream. */
    virtual bool Eof() const 						=0;
    virtual StreamType GetType() const					=0;
    /**
     * might disappear - use StreamInfo - this might only useful for stream copy
     * - size query - pheader == 0
     */
    virtual uint_t GetHeader(void* pheader = 0, uint_t size = 0) const	=0;
    /**
     * For audio stream returns audio format information. Stores main
     * format info in wf - usually WAVEFORMATEX structure
     *
     * if wf == 0 -- query for the size of structure
     * For other stream types fails.
     */
    virtual uint_t GetAudioFormat(void* format = 0, uint_t size = 0) const =0;
    /**
     * For video stream returns video format information
     * ( usually in BITMAPINFOHEADER format ).
     * For other stream types fails.
     *
     * if format == 0 -- query for the size of structure
     */
    virtual uint_t GetVideoFormat(void* format = 0, uint_t size = 0) const =0;
    /** Total length of stream in samples. */
    virtual framepos_t GetLength() const				=0;
    /** Total length of stream in seconds. */
    virtual double GetLengthTime() const				=0;
    /**
     * Returns various informations in StreamInfo structure like
     * stream size in bytes, avg chunk size and some others...
     * It is users responsibility to free this object
     * when stream information is no longer needed!
     *
     * THIS IS PREFFERED method to get information about stream!
     * GetVideoFormatInfo & GetAudioFormatInfo should be used only
     * when you know what you are doing - mainly for data copying
     */
    virtual StreamInfo* GetStreamInfo() const				=0;
    /** Duration of one frame/sample. */
    virtual double GetFrameTime() const 				=0;

    /**
     * next four method are only meaningful for video streams,
     * but no checking is done
     * value ERR means current frame
     */
    virtual framepos_t GetNextKeyFrame(framepos_t frame = ERR) const	=0;
    virtual framepos_t GetPrevKeyFrame(framepos_t frame = ERR) const	=0;

    virtual framepos_t SeekToNextKeyFrame() 				=0;
    virtual framepos_t SeekToPrevKeyFrame() 				=0;
    /**
     * Seeks to position  'pos'  in stream. In video streams, if
     * streaming is started, you will need to decompress all
     * video frames since last key-frame before pos.
     */
    virtual int Seek(framepos_t pos) 					=0;
    virtual int SeekTime(double pos)					=0;
    virtual framepos_t SeekToKeyFrame(framepos_t pos) 			=0;
    virtual double SeekTimeToKeyFrame(double pos)			=0;
    virtual int SkipFrame()						=0;
    virtual int SkipTo(double pos)					=0;
    /**
     * Time for frame - ERR -> current time
     */
    virtual double GetTime(framepos_t frame = ERR) const		=0;
    /**
     * Current frame position in stream.
     */
    virtual framepos_t GetPos() const					=0;

    virtual bool IsStreaming() const					=0;
    /**
     * Initialize decoder.
     */
    virtual int StartStreaming(const char* privname = 0)		=0;
    /**
     * Close decoder.
     */
    virtual int StopStreaming() 					=0;

    /*** For audio streams: ***/

    /** retrieve pointer to video decoder - use with caution!!! */
    virtual IAudioDecoder* GetAudioDecoder() const			=0;
    /**
     * Reads and decompresses variable number of frames into
     * user-supplied buffer.
     */
    virtual int ReadFrames(void* buffer, uint_t bufsize, uint_t samples,
			   uint_t& samples_read, uint_t& bytes_read)	=0;


    /*** For video streams  ***/

    /** retrieve pointer to video decoder - use with caution!!! */
    virtual IVideoDecoder* GetVideoDecoder() const			=0;
    /**
     * Returns current output format. Call after StartStreaming.
     */
    virtual uint_t GetOutputFormat(void* format = 0, uint_t size = 0) const	=0;
    /**
     * Returns flags associated with current frame. You'll need this
     * function if you use IVideoDecoder for decompression. Then,
     * pass resulting value to decoder together with frame data.
     */
    virtual int GetFrameFlags(int* flags) const				=0;
    /**
     * Can be calculated from video format as width*height*bpp/8 usually
     */
    virtual uint_t GetFrameSize() const					=0;

    /**
     * Flip image upside/down in decoder
     */
    virtual int SetDirection(bool direction) 				=0;

    /**
     * Specifies desired output format. Call before StartStreaming.
     * Only valid for video streams. Not implemented yet.
     */
    virtual int SetOutputFormat(void* bi, uint_t size)			=0;

    /**
     * Reads one frame, decompresses it into internal buffer
     * and increases position counter.
     */
    virtual int ReadFrame(bool render = true)				=0;
    /**
     * Returns pointer to internal frame buffer.
     * It will call ReadFrame internaly when readFrame == true.
     * when you no longer need this frame - call CImage::Release()
     */
    virtual CImage* GetFrame(bool readFrame = false)			=0;

    /**
     * Directly reads data from file. Incompatible with streaming.
     */
    virtual int ReadDirect(void* buffer, uint_t bufsize, uint_t samples,
			   uint_t& samples_read, uint_t& bytes_read,
			   int* flags = 0)				=0;

    /**
     * Returns approximate size of internal chunk cache. 1 is 'cache full'
     * and 0 is 'cache empty'. When cache size is too low, it may be a
     * good idea to wait for some time until it refills.
     */
    virtual double CacheSize() const					=0;


    /**
     * private mainly for aviplay usage
     */
    virtual int SetBuffering(uint_t maxsz = 1, IImageAllocator* ia = 0) =0;
    /**
     * retrieve information about buffers - max size and current status
     */
    virtual uint_t GetBuffering(uint_t* bufsz = 0) const		=0;

#ifdef AVM_COMPATIBLE
    /**
     * backward compatible inline declaration - do not use in new programs
     */

    /**
     * \deprecated
     * For audio stream returns audio format information. Stores main
     * format info in wf - usually WAVEFORMATEX structure - and pointer
     * to complete format in ext if ext is nonzero and if its size is
     * more than 18=sizeof(WAVEFORMATEX).
     * If *ext!=NULL on return, you should free it with   delete
     * when it's not needed anymore.
     */
    uint_t GetAudioFormatInfo(void* wf, char** ext) const
    {
	uint_t sz = GetAudioFormat();
	char* b = new char[sz];
	GetAudioFormat(b, sz);
	if (wf)
	{
	    for (unsigned i = 0; i < sz && i < sizeof(WAVEFORMATEX); i++)
		((char*)wf)[i] = b[i];
	}
	if (ext)
	{
	    if (*ext)
	    {
		for (unsigned i = 0; i < sz; i++)
		    (*ext)[i] = b[i];
                delete[] b;
	    }
	    else
		*ext = b;
	}
	else
            delete[] b;

        return 0;
    }
    /** \deprecated */
    uint_t GetVideoFormatInfo(void* bi, uint_t size) const { return GetVideoFormat(bi, size); }
    /** \deprecated */
    IVideoDecoder* GetDecoder() const { return GetVideoDecoder(); };
#endif
};

class IReadFile
{
public:
    enum {
	// currently these names aren't final
        // DO NOT USE if you want to stay compatible for NOW !

	// ignore indexes in files (i.e. if avi index gives false info)
	IGNORE_INDEX = 1,
	// ignore more header informations
	// (might help with some broken files)
	IGNORE_HEADER = 2,

	// do not create precaching thread
	NO_THREAD = (1 << 31)
    };
    virtual ~IReadFile();
    virtual uint_t StreamCount() 					=0;
    virtual uint_t VideoStreamCount() 					=0;
    virtual uint_t AudioStreamCount() 					=0;
    /**
     * May return something not completely correct for ASF file,
     * because it is not always possible to convert ASF to AVI.
     * For MMS URL returns structure with many invalid fields.
     * If you want to retrieve AVI header - pass approriate structure size
     * as it's rather internal you shouldn't need to use this function
     */
    virtual uint_t GetHeader(void* pheader, uint_t size)		=0;
    virtual IReadStream* GetStream(uint_t stream_id,
				   IStream::StreamType type)		=0;
    /**
     * With redirector you could only get list of URLs.
     */
    virtual bool GetURLs(avm::vector<avm::string>& urls)		=0;
    /**
     * See doc/README-DEVEL
     *   For local files IsOpened() will return true immediately after opening.
     */
    virtual bool IsOpened()    						=0;
    virtual bool IsValid()  	       					=0;
    virtual bool IsRedirector()	  					=0;
};

/**
 * Interface for storing any data into a stream
 */
class IWriteStream : public IStream
{
public:
    virtual int AddChunk(const void* chunk, uint_t size, int flags = 0) =0;
    virtual uint_t GetLength() const					=0;
    virtual StreamType GetType() const 					=0;
};

/**
 * Interface for video compressed stream
 */
class IVideoWriteStream
{
public:
    // pData will be filled with pointer to encoded data
    virtual int AddFrame(CImage* chunk, uint_t* pSize=0, int* pKeyframe=0, char** const pData = 0) =0;
    virtual const CodecInfo& GetCodecInfo() const			=0;
    virtual uint_t GetLength() const					=0;
    virtual int Start()							=0;
    virtual int Stop()							=0;

#ifdef AVM_COMPATIBLE
    //0..10000
    // use codecs atribute!
    virtual int SetQuality(int quality)		{ return -1; }
    virtual int SetKeyFrame(int frequency)	{ return -1; }
#endif
};

/**
 * Interface for audio compressed stream
 */
class IAudioWriteStream
{
public:
    virtual int AddData(void* data, uint_t size)			=0;
    virtual const CodecInfo& GetCodecInfo() const			=0;
    virtual uint_t GetLength() const					=0;
    //???virtual int SetAudioPreload(int preloadtime = 500.0, int rate = 100, bool bytime = true);
    virtual int Start()							=0;
    virtual int Stop()							=0;

#ifdef AVM_COMPATIBLE
    //0..10
    virtual int SetQuality(int quality)		{ return -1; }
#endif

};

class IWriteFile
{
public:
    virtual ~IWriteFile();
    // DO NOT delete these returned stream!
    virtual IVideoWriteStream* AddVideoStream(fourcc_t fourcc,
					      const BITMAPINFOHEADER* srchdr,
					      int frame_rate, int flags = 0) =0;
    virtual IVideoWriteStream* AddVideoStream(const CodecInfo& ci,
					      const BITMAPINFOHEADER* srchdr,
					      int frame_rate, int flags = 0) =0;
    virtual IVideoWriteStream* AddVideoStream(const VideoEncoderInfo* vi,
					      int frame_rate, int flags = 0) =0;
    virtual IAudioWriteStream* AddAudioStream(fourcc_t fourcc,
					      const WAVEFORMATEX* format,
					      int bitrate, int flags = 0) =0;
    virtual IAudioWriteStream* AddAudioStream(const CodecInfo& ci,
					      const WAVEFORMATEX* format,
					      int bitrate, int flags = 0) =0;
    virtual IWriteStream* AddStream(IStream::StreamType type,
				    const void* format,
				    uint_t format_size,
				    fourcc_t handler,
				    int frame_rate,
				    uint_t samplesize = 0,
				    int quality = 0,
				    int flags  = 0)			=0;
    // make stream with same parameters as given stream
    // useful for making copy of the stream
    virtual IWriteStream* AddStream(IReadStream* pCopyStream)		=0;
    virtual const char* GetFileName() const				=0;
    virtual int64_t GetFileSize() const					=0;
    virtual int Reserve(uint_t size)					=0;
    virtual int WriteChunk(fourcc_t fourcc, void* data, uint_t size)	=0;

    // close current file and open new avi file with same parameters
    virtual int Segment()                                               =0;

    // waits till the next keyframe and then segments
    virtual void SegmentAtKeyframe() =0;
    virtual void setSegmentName(avm::string new_name) =0;

};


/**
 * Open stream from given URL
 */
IReadFile* CreateReadFile(const char* url, unsigned int flags = 0);
/**
 * Create writable stream
 *
 */
IWriteFile* CreateWriteFile(const char* filename,
			    int64_t flimit = 0, // limit for segmentation
			    IStream::StreamFormat fmt = IStream::Avi,
			    int flags = 0,
			    int mask = 00666    // mask for file opennig
			   );

AVM_END_NAMESPACE;


//////////////////////////////////////
// backward compatibility
// - so far still use old API calls!!!
//////////////////////////////////////

#ifdef AVM_COMPATIBLE

typedef avm::IStream AviStream;
typedef avm::IReadStream IAviReadStream;

typedef avm::IReadFile IAviReadFile;
typedef avm::IWriteFile IAviWriteFile;
typedef avm::IWriteFile ISegWriteFile;
typedef avm::IWriteFile IAviSegWriteFile;
typedef avm::IWriteStream IAviWriteStream;
typedef avm::IAudioWriteStream IAviAudioWriteStream;
typedef avm::IVideoWriteStream IAviVideoWriteStream;

static inline IAviReadFile* CreateIAviReadFile(const char* url)
{
    return avm::CreateReadFile(url, 0);
}

static inline IAviWriteFile* CreateIAviWriteFile(const char* url, int flags = 0, int mask = 00666)
{
    return avm::CreateWriteFile(url, 0, avm::IStream::Avi, flags, mask);
}

static inline IAviSegWriteFile* CreateSegmentedFile(const char* url, uint_t flimit = 0x7F000000,
				      int flags = 0, int mask = 00666)
{
    return avm::CreateWriteFile(url, flimit, avm::IStream::Avi, flags, mask);
}

#endif // AVM_COMPATIBLE

#endif // AVIFILE_AVIFILE_H
