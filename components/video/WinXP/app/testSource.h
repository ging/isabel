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
/**
 <head> 
   <name>testSource.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_TEST_SOURCE_H
#define _IVIDEO_TEST_SOURCE_H

#include "camMgr.h"
#include "channel.h"


/**
 <class> 
   <name>fileSource_t</name> 
   <descr>
   This class represent a single TEST or MEDIA source,    
   inherits from camInfo_t and adds some special methods       
   used by media files.	
   </descr>
**/

class fileSource_t:public camInfo_t

{
private:
	
	double curTime;

public:
	
	double  getCurTime(void);
	void setCurTime(double=0.0);

	IFileSourceFilter * pConfig;
	fileSource_t(char * = NULL);
	virtual ~fileSource_t(void);
	
		
};

static camInfo_t*
createFileSource(char *fileName = NULL)
{
	camInfo_t *camInfo= new fileSource_t(fileName);
		
	int newId= 0;
	if (camArray.size() > 0)
	{
		newId= camArray.elementAt(camArray.size()-1)->getID();
	}

	camInfo->setID(newId);
	camInfo->setFree(true);
	camInfo->setKind(fileName != NULL ? MEDIA : TEST);
	camInfo->initFormats();

	camArray.add(camInfo);

	return camInfo;
}

//</class>

#endif
	