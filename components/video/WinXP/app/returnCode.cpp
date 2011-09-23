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
// $Id: returnCode.cpp 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "returnCode.h"
#include <icf2/notify.hh>

HRESULT 
GetErrorText(DWORD dwErrorCode,
             TCHAR **ppErrorText,
             DWORD *pdwSize)
{
  HMODULE hMod = NULL;
  TCHAR * pMsgBuf = NULL;
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

int 
Eval_Exception (int code,EXCEPTION_POINTERS * exInfo,char * name = NULL)
{
    char * message;
    unsigned long size;
    GetErrorText(SCODE_CODE(code),&message,&size);
    setNotifyFile("exception.log");
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
    exit(-1);
    return code;
}
