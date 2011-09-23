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
// $Id: returnCode.h 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_RETURN_CODES_H_
#define _MCU_RETURN_CODES_H_

#include <icf2/general.h>


#ifdef _FINAL

#define __TRY __try
#define __EXCEPT() __except(Eval_Exception(GetExceptionCode(),\
                             GetExceptionInformation())){}
#define __CONTEXT(x) //context_t tmp(x);
#else
#define __TRY
#define __EXCEPT()
#define __CONTEXT(x)
#endif
#define MQDLL_W_ERROR_TEXT  TEXT("MQutil.dll")

HRESULT GetErrorText(DWORD dwErrorCode,
                     TCHAR **ppErrorText,
                     DWORD *pdwSize);
int  Eval_Exception (int,EXCEPTION_POINTERS *);

#endif