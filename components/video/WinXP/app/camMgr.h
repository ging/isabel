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
   <name>camMgr.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_CAM_MGR_H
#define _IVIDEO_CAM_MGR_H

#include <icf2/vector.hh>

#include "general.h"


/**
 <function> 
   <name>CamListInit()</name>
   <descr>
   Initializes CamList with the (CAM) 
   sources installed in the system. 
   </descr>  
 </function>
**/
void camListInit(void);

// Is there any freeSources() ??
/**
 <function> 
   <name>freeSources()</name>
   <descr>
   Returns a boolean telling if there are any   
   free sources in camList, if freesources() returns false     
   a new source will be loaded for that channel: by default   
   this source is a TEST source.  
   </descr>  
 </function>
**/
bool freeSources(void);

/**
 <class> 
   <name>CamInfo_t</name> 
   <descr>
   This class represent a single CAM source. CamInfo_t acts as 
   superclass of fileSource_t and sharedDisplay_t witch        
   represents a MEDIA, SHARED or TEST source.$/				    							
 															    
   CamList is a dictionary_t of cam sources. When a channel    
   looks for a source, camList has a pointer to each source	
   and tells the channel if the source is being used by other	
   channel.	
 </descr>
**/

#define STR_AUX_SIZE 4096

class camInfo_t: public item_t
{

private:
	
	static char Str[STR_AUX_SIZE];

	char camName[100];
	char supportedFormats[100];
   	int  ID;
	bool free;
	sourceKind_e Kind;
	ql_t<AM_MEDIA_TYPE *> formats;
    char supportedRes[1024];


public:
	
	IBaseFilter * pSource;
	IPin *  output;
	
	sourceKind_e getKind(void);
	HRESULT      setKind(sourceKind_e);

	bool	getFree(void);
	void    setFree(bool);
	
	double  getCurTime(void){return 0;};
	void    setCurTime(double=0.0){;};

	char *  getCamName(void);
	void    setCamName(char *);
	
	char *  getSupportedFormats(void);
	void    setSupportedFormats(char *);

    char *  getSupportedRes(void);
    void    setSupportedRes(char *);

	ql_t<AM_MEDIA_TYPE *> getFormatList(void);
	//char *  getExtendedInfoSupportedFormats(void);

	int     getID();
	void    setID(int);

	virtual void setRate(int rate){ return; }
	virtual bool setSharedRect(RECT newRect){ return false; }
	virtual HRESULT RunSource(void){return 0;}

	camInfo_t(void);
	virtual ~camInfo_t(void);


	HRESULT initSource(void);
	HRESULT initFormats(void);
    HRESULT initRes(void);
	char *list_source_formats(void);
};
//</class>
	
// CamArray
extern vector_t<camInfo_t*> camArray;
#endif