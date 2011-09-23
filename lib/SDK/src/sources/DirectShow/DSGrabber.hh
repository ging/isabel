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
// $Id: v4lGrabber.hh 9025 2006-09-08 11:50:20Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __DS_grabber_hh__
#define __DS_grabber_hh__

#include <string.h>

#include <Isabel_SDK/sourceDefinition.hh>
#include <Isabel_SDK/sourceFactory.hh>

#include <DShow.h>

#include <vector>
#include <map>
#include <string>


#include "DSGrabberDesc.hh"
#include "SampleGrabber.hh"

class CDSGrabber: public source_t
{
private:    
    u32 m_eFormat;
    CDSGrabberDesc m_Desc;

    //Graph filters
    IGraphBuilder *m_pGraph;
    IMediaControl *m_pControl;
    IMediaEvent   *m_pEvent;
    IBaseFilter   *m_pCam;
    IBaseFilter   *m_pRender;
    //The sample grabber
    ISampleGrabber    *m_pGrabber;
    CSampleGrabberCB  *m_pGrabberCB;

    //mapa de formatos
    std::map<u32,GUID> m_mFormats;
    HRESULT Unload(void);
    //Atención : Una vez se deja de usar el vector hay que liberar 
    //todos los punteros llamando a DeleteMediaType()
    std::vector<AM_MEDIA_TYPE*> m_vSupportedFormats;
    std::vector<AM_MEDIA_TYPE*> GetSupportedFormats(void);
    //Atención : Una vez se deja de usar el puntero, hay que liberarlo con CoTaskMemFree() 
    AM_MEDIA_TYPE* GetCurrentFormat(void);

    bool  SetGrabSizeAndFormat(unsigned width, unsigned height,u32 fId);

public:

    CDSGrabber(CDSGrabberDesc desc);
    ~CDSGrabber();

    //source_t implementation
    virtual image_t *getImage(void);


    bool  setGrabSize(unsigned width, unsigned height);
    void  getGrabSize(unsigned *width, unsigned *height);

    bool   setGrabFormat(u32 fId);
    u32    getGrabFormat(void);

    bool   setFrameRate(double fps);
    double getFrameRate(void);

    sourceDescriptor_ref getDescriptor(void) const;

    const char *className(void) const { return "CDSGrabber"; }
    
    static std::vector<CDSGrabberDesc> GetDeviceList(void);
};

#endif
