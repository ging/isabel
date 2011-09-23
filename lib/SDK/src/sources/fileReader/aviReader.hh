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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/sockIO.hh>
#include <icf2/notify.hh>
#include <icf2/lockSupport.hh>

#include <vUtils/image.h>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>

#include "aviread/AviReadHandler.h"
#include "aviread/AviReadStream.h"

#include <vCodecs/codecs.h>
#include <aCodecs/codecs.h>
#include <aCodecs/soundutils.h>

using namespace avm;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

class aviReader_t: public virtual source_t, public virtual lockedItem_t
{
private:
    dgramSocket_t *m_pSocket;
    avm::AviReadHandler *m_hAviFile;
    avm::AviReadStream  *m_hVideo;
    avm::AviReadStream  *m_hAudio;
    int m_ProcessedFrames, m_TotalFrames;
    int m_VideoID;
    int m_AudioID;
    u8 *m_Buff;
    int m_iBuffSize;
    int m_iH, m_iW;
    vDecoder_t *m_hVideoDec;
    aDecoder_t *m_hAudioDec;
    int m_Rate;
    static const int OUTPUT_SAMPLE_RATE = 48000;
    SoundUtils::Resampler m_Resampler;
    char m_szFileName[MAX_PATH];
    u32 m_Time;
    u32 m_TimePerFrame;

    sourceDescriptor_ref theSourceDescriptor;

    bool openAviFile(const char *fileName);

public:

    aviReader_t(sourceDescriptor_ref desc, const char *fileName);

    virtual ~aviReader_t(void);

    image_t *getImage(void);

    bool   setFrameRate(double fps);
    double getFrameRate(void);

    bool setGrabSize(unsigned width, unsigned height);
    void getGrabSize(unsigned *width, unsigned *height);

    const char *getInputPort(void);

    bool setGrabFormat(u32 fcc);
    u32  getGrabFormat(void);

    sourceDescriptor_ref getDescriptor(void) const
    {
        return theSourceDescriptor;
    }
};

