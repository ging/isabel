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
// $Id: returnCode.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "returnCode.h"

#include <icf2/notify.hh>

char * context = new char[1024];

#if 0//__BUILD_FOR_LINUX
ExceptionTracer::ExceptionTracer(void)
{
     void * array[25];
     int nSize = backtrace(array, 25);
     char ** symbols = backtrace_symbols(array, nSize);
     for (int i = 0; i < nSize; i++)
     {
         NOTIFY("%s\n",symbols[i]);
     }
     free(symbols);
}
void kill(int sig_num)
{
    NOTIFY("Exception raised [%d]\n",sig_num);
    NOTIFY("-----------STACK-----------\n\n"); 
    __GET_TRACE
    NOTIFY("---------------------------\n\n");
    exit(0);
}
#endif

msgCode_t msgCodeArray[] =

{
    //error messages
    {E_ERROR,                "ERROR: Unknown error"                  },
    {E_NOT_IMPLEMENTED,      "ERROR: Not implemented"                },
    {E_SESSION_EXISTS,       "ERROR: Session already exists"         },
    {E_SESSION_NOT_EXISTS,   "ERROR: Session doesn't exists"         },
    {E_PART_EXISTS,          "ERROR: Participant already exists"     },
    {E_PART_NOT_EXISTS,      "ERROR: Participant doesn't exists"     },
    {E_IP,                   "ERROR: Error working with IP address"  },
    {E_TARGET_EXISTS,        "ERROR: Target already exists"          },
    {E_TARGET_NOT_EXISTS,    "ERROR: Target doesn't exists"          },
    {E_SWITCHER_NOT_EXISTS,  "ERROR: Switcher doesn't exists"        },
    {E_TRANSCODER_NOT_EXISTS,"ERROR: Transcoder doesn't exists"      },
    {E_GRID_NOT_EXISTS,      "ERROR: Grid doesn't exists"            },
    {E_MIXER_NOT_EXISTS,     "ERROR: Mixer doesn't exists"           },
    {E_PROC_NOT_EXISTS,      "ERROR: Error processor doesn't exists" },
    {E_TRAFFIC_OVERLOAD,     "ERROR: Traffic overload"               },
    {E_NEXT_NOT_EXISTS,      "ERROR: Filter without next pointer"    },
    {E_FEC_OUT_OF_RANGE,     "ERROR: FEC set k out of range"         },
    {E_AUDIO_DECODE,         "ERROR: Decoding audio"                 },
    {E_AUDIO_CODE,           "ERROR: Coding audio"                   },
	{E_VIDEO_DECODE,         "ERROR: Decoding video"                 },
    {E_VIDEO_CODE,           "ERROR: Coding video"                   },
	{E_VIDEO_SCALE,          "ERROR: Scaling video"					 },
	{E_FRAGMENTER,           "ERROR: Fragmenting data"				 },
	{E_DEFRAGMENTER,		 "ERROR: Defragmenting data"			 },
	{E_SILENCE_LEVEL,        "ERROR: Silence level out of range"	 },
	{E_NAT_ERROR,			 "ERROR: Nat Participant is not yet configurated"	 },
	{E_NAT_PORT,			 "ERROR: Port not available for Nat Participants"	 },
    
	//succesfull messages
    {S_OK,                  "OK"                                    },
    
    //end of array
    {X_UNDEF,               NULL                                    }
};

const char * 
getMessageByCode(HRESULT hr)
{
    for(unsigned int i = 0; msgCodeArray[i].hr != X_UNDEF;i++)
    {
        if (hr == msgCodeArray[i].hr)
        {
            return msgCodeArray[i].msg;
        }
    }
    return NULL;
}

void 
errorCheck(int hr, const char *className)
{
    if (FAILED(hr))
    {
        NOTIFY("%s :: %s\n",className,getMessageByCode(hr));
    }
}

#ifdef WIN32
#define MQDLL_W_ERROR_TEXT  TEXT("MQutil.dll")

HRESULT 
GetErrorText(DWORD dwErrorCode,
             char  **ppErrorText,
             DWORD *pdwSize)
{
  HMODULE hMod = NULL;
  char * pMsgBuf = NULL;
  DWORD dwSize = 0;
  
  
  // Validate the input parameters
  if (ppErrorText == NULL || pdwSize == NULL)
  {
    return ERROR_INVALID_PARAMETER;
  }
  
  
  // Initialize the two OUT parameters
  *ppErrorText = NULL;
  *pdwSize = 0;
  
  if (HRESULT_FACILITY(dwErrorCode) == FACILITY_MSMQ)
  {
    // Load MQDLL_W_ERROR_TEXT DLL, i.e., MQutil.dll
    hMod = LoadLibrary(MQDLL_W_ERROR_TEXT);
    
    if (hMod)
    {
      
      // Use the FormatMessage API to translate the error code
      dwSize = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | 
                             FORMAT_MESSAGE_ALLOCATE_BUFFER |
                             FORMAT_MESSAGE_IGNORE_INSERTS,
                             hMod, dwErrorCode, 0, (LPTSTR)&pMsgBuf, 0, NULL);


      // Unload MQDLL_W_ERROR_TEXT DLL, i.e., MQutil.dll.
      FreeLibrary(hMod);
    

      // Return the description and size to the caller in the OUT parameters.
      if (dwSize)
      {
        *pdwSize = dwSize;
        *ppErrorText = (TCHAR*)pMsgBuf;
        return S_OK;
      }
    }

  
    // Return the error code.
    return GetLastError();
  }
  // Retrieve the Win32 error message.
  dwSize = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, dwErrorCode, 0, (LPTSTR) &pMsgBuf, 0, NULL);
  // Return the description and size to the caller in the OUT parameters.
  if (dwSize)
  {
      *pdwSize = dwSize;
      *ppErrorText = (TCHAR*)pMsgBuf;
      return S_OK;
  }
  // Return the error code.
  return GetLastError();
}
#ifdef __FINAL
int 
Eval_Exception (int code,EXCEPTION_POINTERS * exInfo,char * name = NULL)
{
    char * message;
    unsigned long size;
    GetErrorText(SCODE_CODE(code),&message,&size);
    NOTIFY("%s :: Exception(0x%x) :: %s\n",name,code,message);
    while(exInfo->ExceptionRecord)
    {
        NOTIFY("Exception Info :: \n"
               "\t- ExceptionCode    : 0x%x\n"
               "\t- ExceptionFlags   : %d\n"
               "\t- ExceptionAddress : 0x%x\n"
               "\t- Read/Write error : %s(0x%x)\n",
               exInfo->ExceptionRecord->ExceptionCode,
               exInfo->ExceptionRecord->ExceptionFlags,
               exInfo->ExceptionRecord->ExceptionAddress,
               exInfo->ExceptionRecord->ExceptionInformation[0]?"YES":"NO",
               exInfo->ExceptionRecord->ExceptionInformation[0]?exInfo->ExceptionRecord->ExceptionInformation[1]:0);
        exInfo->ExceptionRecord = exInfo->ExceptionRecord->ExceptionRecord;
    }
    abort();
    return code;
}
#endif
#endif
