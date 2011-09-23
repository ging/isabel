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
#ifndef AVIFILE_INFOTYPES_H
#define AVIFILE_INFOTYPES_H

#include "StreamInfo.h"
#include "formats.h"

AVM_BEGIN_NAMESPACE;

/**
 * Base class for other infotype classes
 */
class BaseInfo
{
public:
    BaseInfo();
    BaseInfo(const char* _name, const char* _about);
    ~BaseInfo();

    const char* GetName() const { return name.c_str(); }
    const char* GetAbout() const { return about.c_str(); }
protected:
    /// \internal unique name.
    std::string name;
    /// \internal text description.
    std::string about;
};

/** Describes the attibutes of decoder or encoder. */
struct AttributeInfo : public BaseInfo
{
    enum Kind
    {
	/// integer either limited by i_min and i_max (if i_min < i_max), or unlimited.
	Integer,
        /// any string value, such as registration code.
	String,
        /// value can have one of several string values from 'options'.
	Select,
	/// floating point number limited by i_min and i_max (if i_min < i_max), or unlimited.
	Float
    };
    Kind kind;
    union {
	float f_min;
	int i_min;
    };
    union {
	float f_max;
	int i_max;
    };
    union {
	float f_default;
	int i_default;
    };
    avm::vector<std::string> options;
    AttributeInfo();
    /**
     * Constructor for Select-type attribute
     * \param name of the attribute
     * \param about the attribute
     * \param options for this attribute
     * \param defitem default item from the list
     */
    AttributeInfo(const char* name, const char* about, const char** options, int defitem = 0);
    /**
     * Generic constructor for Integer-type attribute
     * \param name of the attribute
     * \param about the attribute
     * \param kind sets type of this attribute
     * \param minval set lower bound for the Integer-type attribute
     * \param maxval set upper bound for the Integer-type attribute
     *               is maxval < minval value is unlimited
     * \param defval use as default value for this attribute
     */
    AttributeInfo(const char* name, const char* about = 0, Kind kind = Integer, int minval = 0, int maxval = -1, int defval = -1);
    /**
     * double type constructor
     */
    AttributeInfo(const char* name, const char* about, float defval, float minval = 0, float maxval = -1);
    /** \internal */
    ~AttributeInfo();

    float GetDefaultFloat() const { return f_default; }
    int GetDefault() const { return i_default; }
    /**
     * \retval minimum for integer attribute */
    Kind GetKind() const { return kind; }
    float GetMinFloat() const { return f_min; }
    float GetMaxFloat() const { return f_max; }
    int GetMin() const { return i_min; }
    int GetMax() const { return i_max; }
    const avm::vector<std::string>& GetOptions() const { return options; }
    // the following names might change - do not use for
    bool IsAttr(const char* attribute) const;
    bool IsValid(int value) const
    {
	switch (GetKind()) {
	case Integer:
	case Select:
	    if (GetMin() < GetMax()
		&& (value < GetMin() || value > GetMax()))
		return false;
            break;
	case Float:
	    if (GetMinFloat() < GetMaxFloat()
		&& (value < GetMinFloat() || value > GetMaxFloat()))
		return false;
            break;
	default:
	    break;
	}
        return true;
    }
    bool IsValid(float value) const
    {
	switch (GetKind()) {
	case Integer:
	case Select:
	    if (GetMin() < GetMax()
		&& (value < GetMin() || value > GetMax()))
		return false;
            break;
	case Float:
	    if (GetMinFloat() < GetMaxFloat()
		&& (value < GetMinFloat() || value > GetMaxFloat()))
		return false;
            break;
	default:
	    break;
	}
        return true;
    }
};


/**
 * Structure describes audio/video codec, registered in the system.
 */
struct PluginPrivate;
struct CodecInfo : public BaseInfo
{
//
// Fields that loader expects to be filled by plugin
//
    /// Default fourcc for this codec.
    fourcc_t fourcc;
    /// Fourcc's handled by this codec.
    avm::vector<fourcc_t> fourcc_array;

    enum Kind
    {
	Source = 0,
	Plugin,
	Win32,
	Win32Ex,
	DShow_Dec,
        DMO
    };
    enum Media { Audio, Video };
    enum Direction { Encode = 1, Decode = 2, Both = 3 };

    /**
     * private name of the codec
     * globally unique! i.e. there should be at most one CodecInfo in audio
     * and one CodecInfo in video for a given private name on any system
     * assigned to the codec by avifile ( or mplayer ) developers ;^)
     */
    std::string privatename;

    /**
     * Source - structure describes codec that is implemented as
     * a part of library. All other values in this field are ignored
     * by the library and can be used within plugins.
     */
    Kind kind;
    Media media;
    Direction direction;

    /*
     * These fields contain custom data that may be needed by plugin.
     * They will be ignored by loader.
     */
    GUID guid;		// CLSID ( only needed by DirectShow codecs + Vorbis ).
    std::string dll;	// Win32 DLL/ffmpeg name.

    avm::vector<AttributeInfo> decoder_info; // acceptable attrs for decoder.
    avm::vector<AttributeInfo> encoder_info; // acceptable attrs for encoder.

    CodecInfo();
    ~CodecInfo();
    CodecInfo(const fourcc_t* array, const char* info, const char* path,
	      const char* about, Kind _kind, const char* privname, Media _media = Video,
	      Direction _direction = Both, const GUID* id = 0,
	      const avm::vector<AttributeInfo>& ei = avm::vector<AttributeInfo>(),
	      const avm::vector<AttributeInfo>& di = avm::vector<AttributeInfo>());
    CodecInfo(const CodecInfo& ci);
    CodecInfo& operator=(const CodecInfo&);
    bool operator==(const CodecInfo& v) const { return (this==&v); }

    const char* GetPrivateName() const { return privatename.c_str(); }
    /**
     * Tries to find given attribute
     * \param attr  name of the searched attribute
     * \param dir   seach in either decoder or encoder set of attributes or both
     * \retval Attribute
     */
    const AttributeInfo* FindAttribute(const char* attr, Direction dir = Both) const;
//
// Searches for codec matching given fourcc, returns NULL if there's no match.
//
    static const CodecInfo* match(fourcc_t codec, Media media=Video, const CodecInfo* start=0, Direction direction=Both);
    static const CodecInfo* match(Media media, const char* privname);

    static const fourcc_t ANY = 0x414e5920; // as any type
    // use this method to receive all CodecInfos for given media,
    // direction and fourcc
    static void Get(avm::vector<const CodecInfo*>&, Media media = Video,
		    Direction direction = Decode, fourcc_t fcc = ANY);
    //
    // These two fields will be filled by plugin loader.
    //
    std::string modulename;
    mutable PluginPrivate* handle;
};

struct VideoEncoderInfo
{
    fourcc_t compressor;// codec's FourCC
    std::string cname;  // more precise specification of the codec
    BITMAPINFOHEADER header;

#ifdef AVM_COMPATIBLE
    int quality;        // these are ignored
    int keyfreq;        // use only codec properties
#endif
};

/**
 * Interface for runtime configuration of selected Integer attributes
 */
class IRtConfig
{
public:
    /**
     * Returns vector with list of supported attributes
     */
    virtual const avm::vector<AttributeInfo>& GetAttrs() const  =0;
    /**
     * Get value for value for a given attribute name
     * Returns 0 for success or -1 when something fails
     */
    virtual int GetValue(const char* attr, int* value) const	=0;
    /**
     * Sets value for a given attribute name
     * Returns 0 - success   -1 failure
     */
    virtual int SetValue(const char* attr, int value)		=0;

#ifdef AVM_COMPATIBLE
    int GetValue(const char* a, int& v) const { return GetValue(a, &v); }
#endif
};

struct Mp3AudioInfo
{
    enum MPEG_MODE { MPEG1 = 0, MPEG2, ERR, MPEG2_5 } mode;
    enum STEREO_MODE { STEREO = 0, JSTEREO, DUAL_CHANNEL, MODE_MONO } stereo_mode;
    uint_t xing;
    uint_t layer;
    uint_t bitrate;
    uint_t start_offset;
    uint_t sample_rate;
    uint_t samples_per_frame;
    uint_t num_channels;
    uint_t frame_size;
    uint_t header;

    Mp3AudioInfo();
    int Init(const char* buf, int fast = 0);
    int GetBitrate();
    int GetFrameSize();
    void PrintHeader();
};

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE
typedef avm::AttributeInfo AttributeInfo;
typedef avm::CodecInfo CodecInfo;
typedef avm::VideoEncoderInfo VideoEncoderInfo;
#endif

// use CodecInfo::Get() instead
// usage of these two vectors is obsoleted
extern avm::vector<avm::CodecInfo> video_codecs;
extern avm::vector<avm::CodecInfo> audio_codecs;

#endif // AVIFILE_INFOTYPES_H
