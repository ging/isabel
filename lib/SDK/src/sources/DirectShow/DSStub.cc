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

#include "DSgrabber.hh"
#include <stdlib.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <vUtils/image.h>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include <vector>
#include <map>

#define DLM_NAME "DSGrabber"

using namespace std;

class CDSGrabberFactory: public sourceFactory_t
{
private:
	map<CDSGrabberDesc,source_ref> m_DescList;//lista de descriptores
    static sourceFactory_ref m_pFactory;

    CDSGrabberFactory(void) {
		CoInitialize(NULL);
		//Primero obtenemos la lista de camaras del sistema.
		NOTIFY("registerDSGrabber:: Getting DirectShow devices...\n");
		vector<CDSGrabberDesc> devdesc_list = CDSGrabber::GetDeviceList();
		for (vector<CDSGrabberDesc>::iterator iter = devdesc_list.begin();
			 iter != devdesc_list.end();++iter)
		{
			NOTIFY("registerDSGrabber:: %s\n",iter->getID());
			//Creamos los descriptores del mapa
			m_DescList[*iter] = NULL;
		}
    }

    ~CDSGrabberFactory(void) {
		m_DescList.clear();
    }

    bool ready(void) {
		return (m_DescList.size() > 0);
    }

public:
    static sourceFactory_ref createFactory(void) {
        if(!m_pFactory.isValid())
		{
			CDSGrabberFactory *f = new CDSGrabberFactory();
			if(f && f->ready()) {
				m_pFactory = f;
			}
        }
        return m_pFactory;
    }

    source_ref createSource(sourceDescriptor_ref desc, const char * inPort = NULL) {
		sourceDescriptor_t * tmp = static_cast<sourceDescriptor_t*>(desc);
		CDSGrabberDesc * DSdesc = static_cast<CDSGrabberDesc *>(tmp);
		if (m_DescList[*DSdesc] == NULL)
		{
			m_DescList[*DSdesc] = new CDSGrabber(*DSdesc);
		}
		return m_DescList[*DSdesc];
    }
    friend class smartReference_t<CDSGrabberFactory>;
};
sourceFactory_ref CDSGrabberFactory::m_pFactory;

int  registerDSGrabber(void)
{
	NOTIFY("registerDSGrabber:: starting up\n");
	//
    // start registration
    //
	sourceFactory_ref    fact= CDSGrabberFactory::createFactory();
    if ( ! fact.isValid())
    {
        NOTIFY("registerDSGrabber:: ---Cannot build synthetic factory\n");
        NOTIFY("registerDSGrabber:: ---Bailing out\n");

        return -1;
    }
	std::vector<CDSGrabberDesc> device_list = CDSGrabber::GetDeviceList();
	for (std::vector<CDSGrabberDesc>::iterator iter = device_list.begin(); 
		 iter != device_list.end(); ++iter)
	{
		sourceDescriptor_ref desc = new CDSGrabberDesc(*iter);
		if (registerSourceFactory(desc, fact))
		{
			NOTIFY("registerDSGrabber:: +++Registered grabber '%s'\n",
				   desc->getID());
		}
		else
		{
			NOTIFY("registerDSGrabber:: ---Unable to register grabber '%s'\n",
				   desc->getID());
			NOTIFY("registerDSGrabber:: ---Bailing out\n");

			return -1;
		}
	}
    return 0;
}

void releaseDSGrabber(void)
{
    NOTIFY("registerDSGrabber:: shutting down\n");
}
