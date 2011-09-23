/*
 * g723_24CodecP.h -- libg723_24 wrapper, g723_24 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_24CodecP.h	5.2

 * $au$ : mpetit@dit.upm.es

 */



#ifndef	__G723_24CODECP_H__
#define	__G723_24CODECP_H__

#define G723_24CODNUMSAMPLES	8
#define G723_24CODNUMBYTES	3

#include "g72x.h"


typedef struct g72x_state g723_24CodCtx_t;

/*
typedef struct g723_24CodCtx_t g723_24CodCtx_t;
*/


EXTERNAL CDECL i32 g723_24InitP( void);
EXTERNAL CDECL i32 g723_24DoneP( void);


EXTERNAL CDECL i32 g723_24NewCtxP( void **);
EXTERNAL CDECL i32 g723_24DelCtxP( g723_24CodCtx_t *);

EXTERNAL CDECL i32 g723_24EncodeP( g723_24CodCtx_t *, u8 *outBuf, i16 *inBuf, u32 inSamples);
EXTERNAL CDECL i32 g723_24DecodeP( g723_24CodCtx_t *, i16 *outBuf, u8 *inBuf, u32 inBytes);

#endif
