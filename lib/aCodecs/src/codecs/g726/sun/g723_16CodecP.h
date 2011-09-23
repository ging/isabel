/*
 * g723_16CodecP.h -- libg723_16 wrapper, g723_16 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_16CodecP.h	5.2

 * $au$ : mpetit@dit.upm.es

 */



#ifndef	__G723_16CODECP_H__
#define	__G723_16CODECP_H__

#define G723_16CODNUMSAMPLES	4
#define G723_16CODNUMBYTES	1

#include "g72x.h"


typedef struct g72x_state g723_16CodCtx_t;

/*
typedef struct g723_16CodCtx_t g723_16CodCtx_t;
*/


EXTERNAL CDECL i32 g723_16InitP( void);
EXTERNAL CDECL i32 g723_16DoneP( void);


EXTERNAL CDECL i32 g723_16NewCtxP( void **);
EXTERNAL CDECL i32 g723_16DelCtxP( g723_16CodCtx_t *);

EXTERNAL CDECL i32 g723_16EncodeP( g723_16CodCtx_t *, u8 *outBuf, i16 *inBuf, u32 inSamples);
EXTERNAL CDECL i32 g723_16DecodeP( g723_16CodCtx_t *, i16 *outBuf, u8 *inBuf, u32 inBytes);

#endif
