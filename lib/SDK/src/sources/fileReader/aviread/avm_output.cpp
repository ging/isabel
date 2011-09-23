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
#include <string.h>
#include <stdio.h>
#include <string>

#include "avm_output.h"
#include "avm_map.h"
#include "avm_cpuinfo.h"

// ALL STATIC OBJECTS are defined HERE
// NOTE: LDT is at the end of this file
avm::AvmOutput avm::out;
//avm::CPU_Info freq; // will be initialized first thing

AVM_BEGIN_NAMESPACE;

static int init_static_called = 0;
static void init_static()
{
    if (!init_static_called)
    {
		//out.init();
		//freq.Init();
        init_static_called++;
    }
}

class AvmOutput::AvmOutputPrivate
{
public: 
    class Less
    {
    public:
	bool operator()(const char* p1, const char* p2) const;
    };
    class Equal
    {
    public:
	bool operator()(const char* p1, const char* p2) const;
	bool operator()(const char* p1, int p2) const;
    };
    std::string m_sString;
    std::string m_sCurrentMode;
    char tmps[1024];
    PthreadMutex m_Mutex;
    int m_iDebugLevel;
    void* internal;
    avm::avm_map<const char*, int, Less, Equal> m_sMap;
};

AvmOutput::~AvmOutput()
{
    delete priv;
}

void AvmOutput::setDebugLevel(const char* mode, int level)
{
    if (!priv) resetDebugLevels(level);
    if (mode)
	*priv->m_sMap.find_insert(mode) = level;
}

// referenced internaly
void AvmOutput::vwrite(const char* format, va_list va)
{
    vsnprintf(priv->tmps, sizeof(priv->tmps)-1, format, va);
    priv->m_sString += priv->tmps;
    flush();
}

void AvmOutput::vwrite(const char* mode, const char* format, va_list va)
{
    if (!priv) resetDebugLevels();
    Locker locker(priv->m_Mutex);
    priv->m_sCurrentMode = mode;
    priv->m_iDebugLevel = 0;
    vwrite(format, va);
}

void AvmOutput::vwrite(const char* mode, int debuglevel, const char* format, va_list va)
{
    if (!priv) resetDebugLevels();
    Locker locker(priv->m_Mutex);
    priv->m_sCurrentMode = mode;
    priv->m_iDebugLevel = debuglevel;
    vwrite(format, va);
}
/*
void AvmOutput::write(const char* format, ...)
{
//    if(m_iDebugLevel<=m_iWriterDebugLevel)
//	return;
    m_iDebugLevel=0;
    m_sCurrentMode="";
    va_list va;
    va_start(va, format);
    vwrite(format, va);
    va_end(va);
    if((m_sString.size()>80) || (m_sString.find('\n')!=string::npos))
	flush();
}
*/
void AvmOutput::write(const char* mode, const char* format, ...)
{
//    if(m_iDebugLevel<=m_iWriterDebugLevel)
//	return;
    va_list va;
    if (!priv) resetDebugLevels();
    Locker locker(priv->m_Mutex);
//    string sOldMode=m_sCurrentMode;
    priv->m_sCurrentMode = mode;
    priv->m_iDebugLevel = 0;
    va_start(va, format);
    vwrite(format, va);
    va_end(va);
//    m_sCurrentMode=sOldMode;
}
/*
void AvmOutput::write(int debuglevel, const char* format, ...)
{
//    if(m_iDebugLevel<=debuglevel)
//	return;
    m_sCurrentMode="";
    m_iDebugLevel=debuglevel;
    va_list va;
    va_start(va, format);
    vwrite(format, va);
    va_end(va);
    if((m_sString.size()>80) || (m_sString.find('\n')!=string::npos))
	flush();
}
*/
void AvmOutput::write(const char* mode, int debuglevel, const char* format, ...)
{
//    if(m_iDebugLevel<=debuglevel)
//	return;
    va_list va;
    if (!priv) resetDebugLevels();
    Locker locker(priv->m_Mutex);
//    string sOldMode=m_sCurrentMode;
    priv->m_sCurrentMode = mode;
    priv->m_iDebugLevel = debuglevel;
    va_start(va, format);
    vwrite(format, va);
    va_end(va);
//    m_sCurrentMode=sOldMode;
}

void AvmOutput::resetDebugLevels(int level)
{
    int ri = 0;
    if (!priv)
    {
	priv = new AvmOutput::AvmOutputPrivate();
        ri++;
    }
    for(avm_map<const char*, int, AvmOutput::AvmOutputPrivate::Less, AvmOutput::AvmOutputPrivate::Equal>::const_iterator i = priv->m_sMap.begin(); i != priv->m_sMap.end(); i++)
	i->value=level;

    if (ri)
	init_static();
}

void AvmOutput::flush()
{
    if (!priv->m_sString.size())
	return;
    int p = *priv->m_sMap.find_default(priv->m_sCurrentMode.c_str());
    if (p >= priv->m_iDebugLevel)
	printf("<%s> : %s", priv->m_sCurrentMode.c_str(), priv->m_sString.c_str());
    priv->m_sString.erase();
}

bool AvmOutput::AvmOutputPrivate::Less::operator()(const char* p1, const char* p2) const
{
    if(!p1)
	return true;
    if(!p2)
	return false;
    return strcmp(p1, p2);
}

bool AvmOutput::AvmOutputPrivate::Equal::operator()(const char* p1, const char* p2) const
{
    //printf("P1: %p %s\n", p1, p1); printf("P2: %p %s\n", p2, p2); printf("done\n");
    if (!p1)
	return (!p2);
    if (!p2)
	return false;
    return strcmp(p1, p2);
}

bool AvmOutput::AvmOutputPrivate::Equal::operator()(const char* p1, int p2) const
{
    assert(!p2);
    if (!p1)
	return true;
    return (p1[0]==0);
}

AVM_END_NAMESPACE

extern "C" void avm_printf(const char* mode, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    avm::out.vwrite(mode, format, va);
    va_end(va);
}

extern "C" void avm_dprintf(const char* mode, int debuglevel, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    avm::out.vwrite(mode, debuglevel, format, va);
    va_end(va);
}
