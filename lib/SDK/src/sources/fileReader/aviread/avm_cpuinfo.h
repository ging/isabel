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
#ifndef AVIFILE_AVM_CPUINFO_H
#define AVIFILE_AVM_CPUINFO_H

#include "avm_default.h"

AVM_BEGIN_NAMESPACE;

class CPU_Info
{
    double freq;
    bool have_tsc;
    bool have_mmx;
    bool have_mmxext;
    bool have_sse;
public:
    CPU_Info() {}
    void Init();
    /**
     * Returns nonzero if the processor supports MMX instruction set.
     */
    bool HaveMMX() const {return have_mmx;}
    /**
     * Returns nonzero if the processor supports extended integer MMX instruction set
     ( Pentium-III, AMD Athlon and compatibles )
     */
    bool HaveMMXEXT() const {return have_mmxext;}
    /**
     * Returns nonzero if the processor supports 'SSE' floating-point SIMD instruction set
     ( Pentium-III and compatibles )
     */
    bool HaveSSE() const {return have_sse;}
    /**
     * Returns nonzero if the processor has time-stamp counter feature.
     */
    bool HaveTSC() const {return have_tsc;}
    /**
     * Returns processor frequency in kHz.
     */
    operator double() const {return freq;}
};

AVM_END_NAMESPACE;

extern avm::CPU_Info freq;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline int avm_is_mmx_state()
{
#ifdef ARCH_X86
    if (freq.HaveMMX())
    {
	unsigned short tagword = 0;
	char b[28];
	__asm__ __volatile__ ("fnstenv (%0)\n\t" : : "r"(&b));
	tagword = *(unsigned short*) (b + 8);
	return (tagword != 0xffff);
    }
#endif
    return 0;
}

/**
 *  Returns duration of time interval between two timestamps, received
 *  with longcount().
 */
static inline float to_float(int64_t tend, int64_t tbegin)
{
    return float((tend - tbegin) / (double)freq / 1000.);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // AVIFILE_AVM_CPUINFO_H
