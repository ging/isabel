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
#ifndef AVIFILE_AVM_CREATORS_H
#define AVIFILE_AVM_CREATORS_H

#include "formats.h"
#include "infotypes.h"

AVM_BEGIN_NAMESPACE;

class IAudioEncoder;
class IAudioDecoder;
class IVideoEncoder;
class IVideoDecoder;

/**
 * Attempts to create decoder for specified format.
 * Returns valid interface on success, 0 on failure.
 */
IAudioDecoder* CreateDecoderAudio(const WAVEFORMATEX* format, const char* privcname = 0);

/**
 * Creates the encoder for specified format. Pass desired format id in
 * fourcc ( see lib/libwin32/loader/wine/mmreg.h
 * (you could be more specific with the codec's name)
 * or lib/audiodecoder/audiodecoder.cpp
 * for values ) and format of input data in fmt.
 * Returns interface on success, 0 on failure.
 */

IAudioEncoder* CreateEncoderAudio(const CodecInfo& ci, const WAVEFORMATEX* fmt);
IAudioEncoder* CreateEncoderAudio(fourcc_t compressor, const WAVEFORMATEX* fmt);

IVideoDecoder* CreateDecoderVideo(const BITMAPINFOHEADER& bh, int depth = 24, int flip = 0, const char* privcname = 0);

IVideoEncoder* CreateEncoderVideo(const CodecInfo& ci, const BITMAPINFOHEADER& bh);
IVideoEncoder* CreateEncoderVideo(fourcc_t compressor, const BITMAPINFOHEADER& bh, const char* cname = 0);
IVideoEncoder* CreateEncoderVideo(const VideoEncoderInfo& info);

void FreeDecoderAudio(IAudioDecoder* decoder);
void FreeEncoderAudio(IAudioEncoder* encoder);
void FreeDecoderVideo(IVideoDecoder* decoder);
void FreeEncoderVideo(IVideoEncoder* encoder);

float CodecGetAttr(const CodecInfo& info, const char* attribute, float* value);
int CodecSetAttr(const CodecInfo& info, const char* attribute, float value);
int CodecGetAttr(const CodecInfo& info, const char* attribute, int* value);
int CodecSetAttr(const CodecInfo& info, const char* attribute, int value);
int CodecGetAttr(const CodecInfo& info, const char* attribute, const char** value);
int CodecSetAttr(const CodecInfo& info, const char* attribute, const char* value);

/**
 * list of prefered codecs - delimited with commas  ","
 * e.g: DivX ;-) fast-motion,ATI VCR-1,
 *
 * spaces are important!
 * comma must not be in the codec's name!
 */
int SortVideoCodecs(const char* orderlist);
int SortAudioCodecs(const char* orderlist);

std::string CodecGetError();

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE

#define CREATORS_BEGIN_NAMESPACE    namespace Creators {
#define CREATORS_END_NAMESPACE      }

CREATORS_BEGIN_NAMESPACE;

static inline avm::IAudioDecoder* CreateAudioDecoder(const WAVEFORMATEX* format, const char* privcname = 0)
{
    return avm::CreateDecoderAudio(format, privcname);
}

/**
 * Creates the encoder for specified format. Pass desired format id in
 * fourcc ( see lib/libwin32/loader/wine/mmreg.h
 * (you could be more specific with the codec's name)
 * or lib/audiodecoder/audiodecoder.cpp
 * for values ) and format of input data in fmt.
 * Returns interface on success, 0 on failure.
 */
static inline avm::IAudioEncoder* CreateAudioEncoder(const CodecInfo& ci, const WAVEFORMATEX* fmt)
{
    return avm::CreateEncoderAudio(ci, fmt);
}
static inline avm::IAudioEncoder* CreateAudioEncoder(fourcc_t compressor, const WAVEFORMATEX* fmt)
{
    return avm::CreateEncoderAudio(compressor, fmt);
}
static inline avm::IVideoDecoder* CreateVideoDecoder(const BITMAPINFOHEADER& bh, int depth = 24, int flip = 0, const char* privcname = 0)
{
    return avm::CreateDecoderVideo(bh, depth, flip, privcname);
}
static inline avm::IVideoEncoder* CreateVideoEncoder(const CodecInfo& ci, const BITMAPINFOHEADER& bh)
{
    return avm::CreateEncoderVideo(ci, bh);
}
static inline avm::IVideoEncoder* CreateVideoEncoder(fourcc_t compressor, const BITMAPINFOHEADER& bh, const char* cname = 0)
{
    return avm::CreateEncoderVideo(compressor, bh, cname);
}
static inline avm::IVideoEncoder* CreateVideoEncoder(const VideoEncoderInfo& info)
{
    return avm::CreateEncoderVideo(info);
}
static inline void FreeDecoderAudio(avm::IAudioDecoder* decoder)
{
    return avm::FreeDecoderAudio(decoder);
}
static inline void FreeEncoderAudio(avm::IAudioEncoder* encoder)
{
    return avm::FreeEncoderAudio(encoder);
}
static inline void FreeVideoDecoder(avm::IVideoDecoder* decoder)
{
    return avm::FreeDecoderVideo(decoder);
}
static inline void FreeVideoEncoder(avm::IVideoEncoder* encoder)
{
    return avm::FreeEncoderVideo(encoder);
}

static inline int GetCodecAttr(const CodecInfo& info, const char* attribute, int& value)
{
    return avm::CodecGetAttr(info, attribute, &value);
}
static inline int SetCodecAttr(const CodecInfo& info, const char* attribute, int value)
{
    return avm::CodecSetAttr(info, attribute, value);
}
static inline int GetCodecAttr(const CodecInfo& info, const char* attribute, const char** value)
{
    return avm::CodecGetAttr(info, attribute, value);
}
static inline int SetCodecAttr(const CodecInfo& info, const char* attribute, const char* value)
{
    return avm::CodecSetAttr(info, attribute, value);
}

/**
 * list of prefered codecs - delimited with commas  ","
 * e.g: DivX ;-) fast-motion,ATI VCR-1,
 *
 * spaces are important!
 * comma must not be in the codec's name!
 */
static inline int SortVideoCodecs(const char* orderlist)
{
    return avm::SortVideoCodecs(orderlist);
}
static inline int SortAudioCodecs(const char* orderlist)
{
    return avm::SortAudioCodecs(orderlist);
}
static inline std::string GetError()
{
    return avm::CodecGetError();
}
CREATORS_END_NAMESPACE;

#endif

#endif //AVIFILE_AVM_CREATORS_H
