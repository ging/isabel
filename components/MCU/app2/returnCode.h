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
/////////////////////////////////////////////////////////////////////////
//
// $Id: returnCode.h 8170 2006-03-10 13:47:35Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_RETURN_CODES_H_
#define _MCU_RETURN_CODES_H_

#include <icf2/general.h>
#include "general.h"

extern char * context;

#ifdef __BUILD_FOR_LINUX
#include <execinfo.h>
#include <signal.h>
#endif

/*
#ifdef __MANDRAKE
#include <execinfo.h>
#else
#include <cc++/exception.h>
#endif
class ExceptionTracer
{
public:
     ExceptionTracer();
};
#define __GET_TRACE ExceptionTracer e;
void kill(int sig_num);

#define __TRY try
#define __EXCEPT() catch(...){kill(10);}
#define __CONTEXT(x) sprintf(context,x);

#endif

#ifdef WIN32
#ifdef _FINAL
#define __TRY __try
#define __EXCEPT() __except(Eval_Exception(GetExceptionCode(),\
                             GetExceptionInformation(),\
                             (char*)context)){}
#define __CONTEXT(x) sprintf(context,x);
int  Eval_Exception (int,EXCEPTION_POINTERS *,char *);

#define MQDLL_W_ERROR_TEXT  TEXT("MQutil.dll")
HRESULT GetErrorText(DWORD dwErrorCode,
                     char  **ppErrorText,
                     DWORD *pdwSize);
*/
//#else
#define __TRY
#define __EXCEPT()
#define __CONTEXT(x)
//#endif
#define __GET_TRACE
//#endif
//MessageId  = X_UNDEF
//MessageStr = NULL
#define X_UNDEF                 (HRESULT)0xffffffff

//MessageId  = S_OK
//MessageStr = OK
#ifndef S_OK
#define S_OK                    (HRESULT)0x00000000
#endif

//MessageId  = E_ERROR
//MessageStr = ERROR: Unknown error
#define E_ERROR                 (HRESULT)0x80000000

//MessageId  = E_NOT_IMPLEMENTED
//MessageStr = ERROR: Not implemented
#define E_NOT_IMPLEMENTED       (HRESULT)0x80000001

//MessageId  = E_SESSION_EXISTS
//MessageStr = ERROR: Session already exists
#define E_SESSION_EXISTS        (HRESULT)0x80000002

//MessageId  = E_SESSION_NOT_EXISTS
//MessageStr = ERROR: Session doesn't exists
#define E_SESSION_NOT_EXISTS    (HRESULT)0x80000003

//MessageId  = E_PART_EXISTS
//MessageStr = ERROR: Participant already exists
#define E_PART_EXISTS           (HRESULT)0x80000004

//MessageId  = E_PART_NOT_EXISTS
//MessageStr = ERROR: Participant doesn't exists
#define E_PART_NOT_EXISTS       (HRESULT)0x80000005

//MessageId  = E_IP
//MessageStr = ERROR: Error working with IP address
#define E_IP                    (HRESULT)0x80000006

//MessageId  = E_TARGET_EXISTS
//MessageStr = ERROR: Error already exists
#define E_TARGET_EXISTS         (HRESULT)0x80000007

//MessageId  = E_TARGET_NOT_FOUND
//MessageStr = ERROR: Error target doesn't exists
#define E_TARGET_NOT_EXISTS     (HRESULT)0x80000008

//MessageId  = E_SWITCHER_NOT_EXISTS
//MessageStr = ERROR: Error switcher doesn't exists
#define E_SWITCHER_NOT_EXISTS   (HRESULT)0x80000009

//MessageId  = E_TRANSCODER_NOT_FOUND
//MessageStr = ERROR: Error transcoder doesn't exists
#define E_TRANSCODER_NOT_EXISTS (HRESULT)0x8000000a

//MessageId  = E_GRID_NOT_FOUND
//MessageStr = ERROR: Error grid doesn't exists
#define E_GRID_NOT_EXISTS       (HRESULT)0x8000000b

//MessageId  = E_MIXER_NOT_FOUND
//MessageStr = ERROR: Error mixer doesn't exists
#define E_MIXER_NOT_EXISTS      (HRESULT)0x8000000c

//MessageId  = E_PROC_NOT_EXISTS
//MessageStr = ERROR: Error processor doesn't exists
#define E_PROC_NOT_EXISTS       (HRESULT)0x8000000d

//MessageId  = E_TRAFFIC_OVERLOAD
//MessageStr = ERROR: Traffic overload
#define E_TRAFFIC_OVERLOAD      (HRESULT)0x8000000e

//MessageId  = E_NEXT_NOT_EXISTS
//MessageStr = ERROR: Filter without next pointer
#define E_NEXT_NOT_EXISTS       (HRESULT)0x8000000f

//MessageId  = E_FEC_OUT_OF_RANGE
//MessageStr = ERROR: FEC set k out of range
#define E_FEC_OUT_OF_RANGE      (HRESULT)0x80000010

//MessageId  = E_AUDIO_DECODE
//MessageStr = ERROR: Decoding audio
#define E_AUDIO_DECODE          (HRESULT)0x80000011

//MessageId  = E_AUDIO_CODE
//MessageStr = ERROR: Coding audio
#define E_AUDIO_CODE            (HRESULT)0x80000012

//MessageId  = E_VIDEO_DECODE
//MessageStr = ERROR: Decoding video
#define E_VIDEO_DECODE          (HRESULT)0x80000013

//MessageId  = E_VIDEO_CODE
//MessageStr = ERROR: Coding video
#define E_VIDEO_CODE            (HRESULT)0x80000014

//MessageId  = E_VIDEO_SCALE
//MessageStr = ERROR: Scaling video
#define E_VIDEO_SCALE           (HRESULT)0x80000015

//MessageId  = E_FRAGMENTER
//MessageStr = ERROR: Fragmenting data
#define E_FRAGMENTER            (HRESULT)0x80000016

//MessageId  = E_SILENCE_LEVEL
//MessageStr = ERROR: Silence level out of range
#define E_SILENCE_LEVEL         (HRESULT)0x80000018

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//
#ifndef SUCCEEDED
#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
#endif
//
// and the inverse
//
#ifndef FAILED
#define FAILED(Status) ((HRESULT)(Status)<0)
#endif

struct msgCode_t
{
    HRESULT hr;
    const char *msg;
};

extern msgCode_t msgCodeArray[];

const char * 
getMessageByCode(HRESULT);


void 
errorCheck(int hr, const char *className = NULL);

#endif

