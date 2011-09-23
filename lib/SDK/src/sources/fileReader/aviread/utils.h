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
#ifndef AVIFILE_UTILS_H
#define AVIFILE_UTILS_H

#include "avm_default.h"
#include "formats.h"

/**
 *  Utility functions.
 *    plain C functions are here
 */

/**
 *  These two functions return number of ticks ( processor cycles )
 *   since power-on. Return value of first one wraps around each
 *   few seconds ( depends on processor ), but calculations with
 *   32-bit 'int' are a bit faster than with 64-bit 'int64_t'.
 *  They can be used for very precise and fast measurement of time
 *   intervals - call to localcount() takes less than 0.2us on 500
 *   MHz processor, and its resolution is measured in nanoseconds.
 *   On systems with properly configured Linux kernel they can be
 *   used as 3-4 times faster alternative to gettimeofday()
 *   because they do not require a trip to kernel and back.
 *   On other systems they will probably be even more efficient.
 *  For processors without time-stamp counter feature ( a few
 *   old Pentium-compatible models by Cyrix and maybe others )
 *   these functions are emulated using gettimeofday().
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern uint_t (*localcount)(void);
extern int64_t (*longcount)(void);


/*
 * function to retrieve/store data in machine independent format
 * - right now dump implementation - as we could check functionality
 */
static inline uint16_t avm_get_le16(const void* p) {
#ifdef WORDS_BIGENDIAN
    const uint8_t* c = (const uint8_t*) p;
    return c[0] | c[1] << 8;
#else
    return *(const uint16_t*)p;
#endif
}

static inline uint8_t* avm_set_le16(void* p, uint16_t v) {
    uint8_t* b = (uint8_t*) p;
#ifdef WORDS_BIGENDIAN
    b[0] = v & 0xff;
    b[1] = (v >> 8) & 0xff;
#else
    *(uint16_t*)p = v;
#endif
    return b;
}

static inline uint32_t avm_get_le32(const void* p) {
#ifdef WORDS_BIGENDIAN
    const uint8_t* c = (const uint8_t*) p;
    return c[0] | c[1] << 8 | c[2] << 16 | c[3] << 24;
#else
    return *(const uint32_t*)p;
#endif
}

static inline uint8_t* avm_set_le32(void* p, uint32_t v) {
    uint8_t* b = (uint8_t*) p;
#ifdef WORDS_BIGENDIAN
    b[0] = v & 0xff;
    b[1] = (v >> 8) & 0xff;
    b[2] = (v >> 16) & 0xff;
    b[3] = (v >> 24) & 0xff;
#else
    *(uint32_t*)p = v;
#endif
    return b;
}

static inline uint64_t avm_get_le64(const void* p) {
#ifdef WORDS_BIGENDIAN
    const uint8_t* c = (const uint8_t*) p;
    return avm_get_le32(c) | (((uint64_t)avm_get_le32(c + 4)) << 32);
#else
    return *(const uint64_t*)p;
#endif
}

static inline uint16_t avm_get_be16(const void* p) {
#ifdef WORDS_BIGENDIAN
    return *(const uint16_t*)p;
#else
    const uint8_t* c = (const uint8_t*) p;
    return c[0] << 8 | c[1];
#endif
}

static inline uint8_t* avm_set_be16(void* p, uint16_t v) {
    uint8_t* b = (uint8_t*) p;
    b[0] = (v >> 8) & 0xff;
    b[1] = v & 0xff;
    return b;
}

static inline uint32_t avm_get_be32(const void* p) {
#ifdef WORDS_BIGENDIAN
    return *(const uint32_t*)p;
#else
    const uint8_t* c = (const uint8_t*) p;
    return c[0] << 24 | c[1] << 16 | c[2] << 8 | c[3];
#endif
}

static inline uint8_t* avm_set_be32(void* p, uint32_t v) {
    uint8_t* b = (uint8_t*) p;
    b[0] = (v >> 24) & 0xff;
    b[1] = (v >> 16) & 0xff;
    b[2] = (v >> 8) & 0xff;
    b[3] = v & 0xff;
    return b;
}

static inline uint64_t avm_get_be64(const void* p) {
#ifdef WORDS_BIGENDIAN
    return *(const uint64_t*)p;
#else
    const uint8_t* c = (const uint8_t*) p;
    return ((uint64_t) avm_get_be32(c)) << 32 | avm_get_le32(c + 4);
#endif
}

static inline int avm_img_is_rgb(fourcc_t fmt)
{
    return ((fmt & 0xffffff00) == IMG_FMT_RGB);
}
static inline int avm_img_is_bgr(fourcc_t fmt)
{
    return ((fmt & 0xffffff00) == IMG_FMT_BGR);
}
static inline int avm_img_get_depth(fourcc_t fmt)
{
    return fmt & 0xff;
}

WAVEFORMATEX* avm_get_leWAVEFORMATEX(WAVEFORMATEX* wf);
BITMAPINFOHEADER* avm_get_leBITMAPINFOHEADER(BITMAPINFOHEADER* bi);

const char* avm_fcc_name(fourcc_t fcc);
const char* avm_wave_format_name(short fmt); /* not sure - might be special type */
const char* avm_img_format_name(fourcc_t fmt);
char* avm_wave_format(char* buf, uint_t s, const WAVEFORMATEX* wf);

/* wrapping functions */
/* A thread-save usec sleep */
int avm_usleep(unsigned long);

/* some system doesn't have setenv/unsetenv */
int avm_setenv(const char *name, const char *value, int overwrite);
void avm_unsetenv(const char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AVIFILE_UTILS_H */
