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
   <name>general.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <descr>
   General file header to be used by Ivideo source code. $/
   Implements some usefull global functions and defines some MACROS. $/
   Define Ivideo's typical structures and types.
   </descr>
//</head>
**/

#ifndef GENERAL_IVIDEO_H
#define GENERAL_IVIDEO_H

#define __CONTEXT(X)
//FOURCC macro

#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                  (((DWORD)(ch4) & 0xFF00) << 8) |    \
                  (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                  (((DWORD)(ch4) & 0xFF000000) >> 24))

#include <AFXWIN.H>

// ICF2 Includes 

#include <icf2/general.h>
#include <icf2/sockIO.hh>
#include <icf2/notify.hh>
#include <icf2/stdtask.hh>
#include <icf2/dictionary.hh>
#include <icf2/lockSupport.hh>
#include <icf2/item.hh>

// Direct Show Includes

#include <Dshow.h>
//#include <sbe.h>
#include <Aviriff.h>
#include <Uuids.h>
#include <qedit.h>
#include <io.h>
#include <FCNTL.H>
#include <Streams.h>
#include <Amfilter.h>
//#include "namedguid.h"



// Windows includes

#include <windows.h>
#include <stdio.h>

//Isabel Video Defines
#define NONE 0

//Channel Kind
#define REC_CHANNEL  0
#define PLAY_CHANNEL 1

//Source Kind
#define CAM_SOURCE  0
#define TEST_SOURCE 1
#define FILE_SOURCE 2
#define SHARED_DISPLAY 3

//Default Window Size
#define DEFAULT_WINDOW_TOP     100
#define DEFAULT_WINDOW_LEFT    100
#define DEFAULT_WINDOW_WIDTH   352
#define DEFAULT_WINDOW_HEIGTH  288

//Default Capture Definition
#define DEFAULT_CAPTURE_WIDTH  352
#define DEFAULT_CAPTURE_HEIGTH 288

//Default Shared Display Definition
#define DEFAULT_SHARED_HEIGTH 600
#define DEFAULT_SHARED_WIDTH  800
#define DEFAULT_SHARED_DISPLAY_FR 333333*5
#define MAX_SAMPLE_DATA 1024*768*24/8 //Max sample size of the  Output Pin (in bytes)

//Default Codec
//#define DEFAULT_CODEC    L"ffvfw MPEG-4 Codec"
//#define DEFAULT_CODEC_STR "ffvfw MPEG-4 Codec"
#define DEFAULT_CODEC    L"ffdshow video encoder"
#define DEFAULT_CODEC_STR "ffdshow video encoder"

#ifdef __ARES
#define WINDOW_OWNER "Ares"
#else
#define WINDOW_OWNER "ISABEL"
#endif


//Default Test File
#define DEFAULT_TEST_FILE L"testSource.avi"
#define DEFAULT_TEST_FILE_STR "testSource.avi"

//Default BW Control
#define DEFAULT_CHANNEL_BW 750 //750 Kbps
#define DEFAULT_BW_PERIOD  0

//Test Modes
#define DBG_SCHED 0
#define DBG_SAVE_FRAMES 0

#define SUPPORTED_CODECS "{MPEG4,MPEG1,H263}"
//PayLoads
#define __MPEG4 96
#define __MPEG1	32
#define __H263	34 

//Default fragmenter param
#define DEFAULT_MTU_SIZE 900
#define DEFAULT_PAYLOAD_HEAD 0

//Frame Buffer Defaults
#define DEFAULT_FRAME_SIZE 7
#define MAX_SEQ_NUM 2^16
#define MAX_SEQ_DIFF 0x0fff

//RTP
#define VIDEO_TIMESTAMP_UNIT   1.0/90000.0
#define VIDEO_TIMESTAMP_INV    90000.0
#define VIDEO_TIMES_MILLI_UNIT   1.0/90000.0
#define VIDEO_TIMES_MILLI_INV    90000.0
#define SAMPLE_BUFF_SIZE 100
#define VIDEO_TIME_DELAY 0.3
#define VIDEO_TIMEOUT 0.5*90000.0
#define VIDEO_TIMEOUT_SEC 0.5


/*===========================================================*
 *	Default Window Properties:								 *
 *===========================================================*

 	Here we describe wich kind of window we want to see
 	in our video:
 
 	WS_BORDER...........Creates a window that has a border. 
 	WS_CAPTION..........Creates a window that has a title bar (implies the WS_BORDER style). 
 						Cannot be used with the WS_DLGFRAME style. 
 	WS_CHILD............Creates a child window. Cannot be used with the WS_POPUP style. 
 	WS_CHILDWINDOW......Same as the WS_CHILD style. 
 	WS_CLIPCHILDREN.....Excludes the area occupied by child windows when you draw within the parent window. 
 						Used when you create the parent window. 
  	WS_CLIPSIBLINGS.....Clips child windows relative to each other; 
 						that is, when a particular child window receives a paint message, 
 						the WS_CLIPSIBLINGS style clips all other overlapped child windows 
 						out of the region of the child window to be updated. 
 						(If WS_CLIPSIBLINGS is not given and child windows overlap, 
 						when you draw within the client area of a child window, 
						it is possible to draw within the client area of a neighboring child window.) 
						For use with the WS_CHILD style only. 
	WS_DISABLED.........Creates a window that is initially disabled. 
	WS_DLGFRAME.........Creates a window with a double border but no title. 
	WS_GROUP............Specifies the first control of a group of controls in which the user
						can move from one control to the next with the arrow keys. 
						All controls defined with the WS_GROUP style FALSE after the first control 
						belong to the same group. 
						The next control with the WS_GROUP style starts the next group 
						(that is, one group ends where the next begins). 
	WS_HSCROLL..........Creates a window that has a horizontal scroll bar. 
	WS_ICONIC...........Creates a window that is initially minimized. Same as the WS_MINIMIZE style. 
	WS_MAXIMIZE.........Creates a window of maximum size. 
	WS_MAXIMIZEBOX......Creates a window that has a Maximize button. 
	WS_MINIMIZE.........Creates a window that is initially minimized. For use with the WS_OVERLAPPED style only. 
	WS_MINIMIZEBOX......Creates a window that has a Minimize button. 
	WS_OVERLAPPED.......Creates an overlapped window. An overlapped window usually has a caption and a border. 
	WS_OVERLAPPEDWINDOW.Creates an overlapped window with the WS_OVERLAPPED, WS_CAPTION, 
						WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, and WS_MAXIMIZEBOX styles. 
	WS_POPUP............Creates a pop-up window. Cannot be used with the WS_CHILD style. 
	WS_POPUPWINDOW......Creates a pop-up window with the WS_BORDER, WS_POPUP, and WS_SYSMENU styles. 
						The WS_CAPTION style must be combined with the WS_POPUPWINDOW style 
						to make the Control menu visible. 
	WS_SIZEBOX..........Creates a window that has a sizing border. Same as the WS_THICKFRAME style. 
	WS_SYSMENU..........Creates a window that has a Control-menu box in its title bar. 
						Used only for windows with title bars. 
	WS_TABSTOP..........Specifies one of any number of controls through which the user can move 
						by using the TAB key. 
						The TAB key moves the user to the next control specified by the WS_TABSTOP style. 
	WS_THICKFRAME.......Creates a window with a thick frame that can be used to size the window. 
	WS_TILED............Creates an overlapped window. An overlapped window has a title bar and a border. 
						Same as the WS_OVERLAPPED style. 
	WS_TILEDWINDOW......Creates an overlapped window with the WS_OVERLAPPED, 
						WS_CAPTION, WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, and WS_MAXIMIZEBOX styles. 
						Same as the WS_OVERLAPPEDWINDOW style. 
	WS_VISIBLE..........Creates a window that is initially visible. 
	WS_VSCROLL..........Creates a window that has a vertical scroll bar. 
*/
//Wanted properties
#define DEFAULT_WINDOW_PROPS WS_CHILD
//NON Wanted properties
#define DEFAULT_WINDOW_NON_PROPS WS_SYSMENU - WS_SIZEBOX

//Program structures

class encodedImage_t
{
public:   

    u8         *buffer;
    unsigned    numBytes;
    unsigned    w;
    unsigned    h;
  
    ~encodedImage_t(void) 
	{ 
		buffer= NULL; 
	}
};

enum channelKind_e{
	REC=REC_CHANNEL,
	PLAY=PLAY_CHANNEL
};

enum sourceKind_e{
	CAM  = CAM_SOURCE,
	TEST = TEST_SOURCE,
	MEDIA = FILE_SOURCE,
	SHARED = SHARED_DISPLAY
};

struct windowInfo_t{
	long left;
	long top;
	long heigth;
	long width;
};



struct captureInfo_t{
	int heigth;
	int width;
};

// General Frame Struct, used by MPEG4

struct frame_t{
	int sequenceNumber;
	BYTE* data;
	long dataLen;
	unsigned long timestamp;
};

enum mySetOfMsgs_t {
     START = 0x8000
};

extern windowInfo_t DEFAULT_WINDOW_INFO;
extern sched_t *s; 
extern bool *SetAutoChannel;

/*===================================================================*
 *  Global Functions: The functions described bellow are developed   *
 *	to serve to the video module classes.							 *
 *																	 *
 *===================================================================*/

/**
 <function> 
   <name>ErrorCheck</name>
   <descr>Decodes error codes and prints the error description</descr>
 </function>
**/

 void errorCheck(int hr);

/**
 <function> 
   <name>GetUnconnectedPin</name>
   <descr>
	  Obtains from pFilter an UNCONNECTED  PIN
	  with PinDir direction and saves it in ppPin
   </descr>  
 </function>
**/

 HRESULT GetUnconnectedPin(
    IBaseFilter *pFilter,   // Pointer to the filter.
    PIN_DIRECTION PinDir,   // Direction of the pin to find.
    IPin **ppPin);           // Receives a pointer to the pin.
/**
 <function> 
   <name>GetPin</name>
   <descr>
   Obtains from pFilter a connected or not PIN with PinDir
   direction  and saves it in ppPin.
   </descr>  
 </function>
**/
 HRESULT GetPin(IBaseFilter *pFilter,
					  PIN_DIRECTION PinDir, 
					  IPin **ppPin);

/**
 <function> 
   <name>GetPin</name>
   <descr>
   Obtains from pFilter a connected PIN with PinDir
   direction and saves it in ppPin 
   </descr>  
 </function>
**/

 HRESULT GetConnectedPin(IBaseFilter *pFilter, 
							   PIN_DIRECTION PinDir, 
							   IPin **ppPin);
/**
 <function> 
   <name>lText</name>
   <descr>
   Gets from text the equivalent Wide Character String
   and saves it in wText, usefull for many DirectX functions
   input arguments
   </descr>  
 </function>
**/

 void lText(wchar_t * wtext,char * text);
 void mbText(char * text,wchar_t * wtext);
/**
 <function> 
   <name>enumVideoSources</name>
   <descr>
   Enumerates installed Video Sources in system,
   adds testSource and fileSource and creates a String with
   correct format.
   </descr>  
 </function>
**/

 void enumVideoSources(char *deviceArray);

/**
 <function> 
   <name>defaultWindowInit</name>
   <descr>
   Fills default window channel settings,
   changing default defines, default window struct will be changed	
   </descr>  
 </function>
**/

 void defaultWindowInit(void);
/**
 <function> 
   <name>initSource</name>
   <descr>
   Loads a source with specific name and saves it in
   pFilter. Returns error code if can't load specified source 	
   </descr>  
 </function>
**/

 HRESULT initSource(char * name,IBaseFilter ** pSource);
#endif
