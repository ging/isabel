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
#ifndef AVIFILE_CONFIGFILE_H
#define AVIFILE_CONFIGFILE_H

#include "avm_default.h"

/**
 * This namespace provides means to conveniently store and retrieve app-specific
 * data from the registry ( file ~/.avm/ ).
 *
 * Function names are self-explanatory. Do not include spaces in first two
 * arguments to avoid collisions. 'def_value' arguments correspond to
 * values which will be returned if needed record is not found in the
 * registry or if registry access fails for some reason.
 */

/* declaration */

AVM_BEGIN_NAMESPACE;


// change default config directory name .avm/default
void* RegInit(const char* regname, const char* dirname = ".avm");
void RegSave();
int RegWriteInt(const char* appname, const char* valname, int value);
int RegReadInt(const char* appname, const char* valname, int def_value);
int RegWriteFloat(const char* appname, const char* valname, float value);
float RegReadFloat(const char* appname, const char* valname, float def_value);
int RegWriteString(const char* appname, const char* valname, const char* value);
const char* RegReadString(const char* appname, const char* valname, const char* def_value);

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE
#define REGISTRY_BEGIN_NAMESPACE    namespace Registry {
#define REGISTRY_END_NAMESPACE      }

REGISTRY_BEGIN_NAMESPACE;

/* backward declaration */

static inline void* Init(const char* regname, const char* dirname = ".avm")
{ return avm::RegInit(regname, dirname); }
static inline int WriteInt(const char* appname, const char* valname, int value)
{ return avm::RegWriteInt(appname, valname, value); }
static inline int ReadInt(const char* appname, const char* valname, int def_value)
{ return avm::RegReadInt(appname, valname, def_value); }
static inline int WriteFloat(const char* appname, const char* valname, float value)
{ return avm::RegWriteFloat(appname, valname, value); }
static inline float ReadFloat(const char* appname, const char* valname, float def_value)
{ return avm::RegReadFloat(appname, valname, def_value); }
static inline int WriteString(const char* appname, const char* valname, const char* value)
{ return avm::RegWriteString(appname, valname, value); }
static inline const char* ReadString(const char* appname, const char* valname, const char* def_value)
{ return avm::RegReadString(appname, valname, def_value); }

REGISTRY_END_NAMESPACE;
#endif // AVM_COMPATIBLE
//
// For internal usage by avifile applications
// do not try to use elsewhere!
//
// used by defining DECLARE_REGISTRY_SHORTCUT
// before inclusion of this file
#ifdef DECLARE_REGISTRY_SHORTCUT

extern const char* g_pcProgramName;

static inline float RF(const char* name, float def)
{ return avm::RegReadFloat(g_pcProgramName, name, def); }
static inline int RI(const char* name, int def)
{ return avm::RegReadInt(g_pcProgramName, name, def); }
static inline const char* RS(const char* name, const char* def)
{ return avm::RegReadString(g_pcProgramName, name, def); }
static inline int WF(const char* name, int def)
{ return avm::RegWriteFloat(g_pcProgramName, name, def); }
static inline int WI(const char* name, int def)
{ return avm::RegWriteInt(g_pcProgramName, name, def); }
static inline int WS(const char* name, const char* def)
{ return avm::RegWriteString(g_pcProgramName, name, def); }

#ifdef QSTRING_H

static inline int WI(const char* name, const QString& def)
{ return avm::RegWriteInt(g_pcProgramName, name, def.toInt()); }
static inline int WS(const char* name, const QString& def)
{ return avm::RegWriteString(g_pcProgramName, name, def.ascii()); }

#endif // QSTRING_H

#endif // DECLARE_REGISTRY_SHORTCUT

#endif // AVIFILE_CONFIGFILE_H
