/*
 * g723_40CodecP.h -- libg723_40 wrapper, g723_40 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_40CodecP.h	5.2

 * $au$ : mpetit@dit.upm.es

 */



#ifndef	__G723_40CODECP_H__
#define	__G723_40CODECP_H__

#define G723_40CODNUMSAMPLES	3
#define G723_40CODNUMBYTES	2

#include "g72x.h"


typedef struct g72x_state g723_40CodCtx_t;

/*
typedef struct g723_40CodCtx_t g723_40CodCtx_t;
*/


EXTERNAL CDECL i32 g723_40InitP( void);
EXTERNAL CDECL i32 g723_40DoneP( void);


EXTERNAL CDECL i32 g723_40NewCtxP( void **);
EXTERNAL CDECL i32 g723_40DelCtxP( g723_40CodCtx_t *);

EXTERNAL CDECL i32 g723_40EncodeP( g723_40CodCtx_t *, u8 *outBuf, i16 *inBuf, u32 inSamples);
EXTERNAL CDECL i32 g723_40DecodeP( g723_40CodCtx_t *, i16 *outBuf, u8 *inBuf, u32 inBytes);

#endif
