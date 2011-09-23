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
#ifndef AVIFILE_IVIDEOENCODER_H
#define AVIFILE_IVIDEOENCODER_H

/********************************************************

	Video encoder interface
	Copyright 2000 Eugene Kuznetsov  (divx@euro.ru)

*********************************************************/

#include "infotypes.h"
#include "image.h"

AVM_BEGIN_NAMESPACE;

/**
 *
 *    Video encoder class.
 *
 *  Usage:
 *	
 *   Create encoder object. Optionally
 * set quality and key-frame frequency. Call Start().
 * Call GetOutputSize() to identify maximum possible
 * size of compressed frame. Allocate buffer of this size.
 * Pass it as 'dest' in calls to EncodeFrame(). At the end
 * call Stop(). Do not forget to delete the object.
 *
 *  Some codec DLLs have be controlled by special codec-specific
 * parameters. In Windows you can set them from codec configuration 
 * dialog, but since we are completely GUI-independent, we'll need
 * an internal method to set them. Currently it is done by calling
 * static func SetExtendedAttr(), which takes FOURCC of codec, 
 * name of parameter and its integer value. Complete list of
 * supported attributes can be retrieved as GetCodecInfo().encoder_info.
 * SetExtendedAttr() should be called before object creation,
 * because values for these attributes are typically stored in 
 * registry and read by codec during object creation.
 *
 */
class IVideoEncoder
{
public:
    enum CAPS
    {
	CAP_NONE = 0,
        // packed
	CAP_IYUV = 1,
	CAP_YUY2 = 2,
	CAP_UYVY = 4,
	CAP_YVYU = 8,
        // planar
	CAP_YV12 = 128,
	CAP_I420 = 256
    };

    IVideoEncoder(const CodecInfo& info);
    virtual ~IVideoEncoder();
    virtual int EncodeFrame(const CImage* src, void* dest, int* is_keyframe,
			    uint_t* size, int* lpckid = 0)	=0;
    virtual const CodecInfo& GetCodecInfo() const;
    virtual const BITMAPINFOHEADER& GetOutputFormat() const	=0;
    virtual int GetOutputSize() const				=0;

    virtual int Start()						=0;
    virtual int Stop()						=0;

    virtual float GetFps() const;
    virtual int SetFps(float fps);


#ifdef AVM_COMPATIBLE
    /* just for backward compatibility */
    int QueryOutputSize() const { return GetOutputSize(); };
    const BITMAPINFOHEADER& QueryOutputFormat() const { return GetOutputFormat(); }

    /* use Attribute stuff for these */
    int GetQuality() const { return -1; }
    int SetQuality(int quality) { return -1; }
    int GetKeyFrame() const { return -1; }
    int SetKeyFrame(int frequency) { return -1; }
#endif

protected:
    const CodecInfo& m_Info;
};

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE
typedef avm::IVideoEncoder IVideoEncoder;
#endif

#endif // AVIFILE_IVIDEOENCODER_H
