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
#include <stdarg.h>
#include <string.h>
#include <stdio.h> //vsnprintf
#include <stdlib.h> //free

#include "avm_except.h"
#include "utils.h"
#include "avm_output.h"

BaseError::~BaseError()
{
    if (module) free(module);
    if (description) free(description);
    if (severity) free(severity);
}

BaseError::BaseError()
    :file(0), module(0), description(0), severity(0), line(0)
{
}

BaseError::BaseError(const char* sev, const char* mod, const char* f, int l, const char* desc)
    :file(f), line(l)
{
    module = (char*) malloc(strlen(mod) + 1);
    if (!module) exit(-1);
    strcpy(module, mod);
    description = (char*) malloc(strlen(desc) + 256);
    if (!description) exit(-1);
    description[0] = 0;
    severity = (char*) malloc(strlen(sev) + 1);
    if (!severity) exit(-1);
    strcpy(severity, sev);
}

BaseError::BaseError(const BaseError& f)
    :module(0), description(0), severity(0)
{
    operator=(f);
}

BaseError& BaseError::operator=(const BaseError& f)
{
    file = f.file;
    line = f.line;
    if (module) free(module);
    if (description) free(description);
    if (severity) free(severity);

    module = (char*) malloc(strlen(f.module) + 1);
    if (!module) exit(-1);
    strcpy(module, f.module);
    description = (char*) malloc(strlen(f.description) + 1);
    if (!description) exit(-1);
    strcpy(description, f.description);
    severity = (char*) malloc(strlen(f.severity) + 1);
    if (!severity) exit(-1);
    strcpy(severity, f.severity);

    return *this;
}

void BaseError::Print()
{
    avm_printf("exception", "%s: %s: %s\n", module, severity, description);
}

void BaseError::PrintAll()
{
    char bf[256];
    int p = 0;
    if (file && strlen(file) < 230)
	p = sprintf(bf, " at %s", file);
    if (line) 
	p += sprintf(bf + p, ": %d", line);
    avm_printf("exception", "%s: %s: %s%s\n",
	      module, severity, description, bf);
}

const char* BaseError::GetModule() const
{
    return module;
}

const char* BaseError::GetDesc() const
{
    return description;
}

FatalError::FatalError(const char* mod, const char* f, int l, const char* desc,...)
    :BaseError("FATAL", mod, f, l, desc)
{
    va_list va;
    va_start(va, desc);
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    vsnprintf(description, strlen(desc)+255, desc, va);    
#elif defined(__BUILD_FOR_WINXP)
	_vsnprintf(description, strlen(desc)+255, desc, va);    
#endif
    va_end(va);
}

GenError::GenError(const char* mod, const char* f, int l, const char* desc,...)
    :BaseError("WARNING", mod, f, l, desc)
{
    va_list va;
    va_start(va, desc);
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    vsnprintf(description, strlen(desc)+255, desc, va);    
#elif defined(__BUILD_FOR_WINXP)
	_vsnprintf(description, strlen(desc)+255, desc, va);    
#endif
    va_end(va);
}
