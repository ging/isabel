/*
 * g723_40Codec.c -- G723_40 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_40Codec.c	5.1

 * $au$ : djr@dit.upm.es

 */



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "general.h"
#include "audioCodec.h"
#include "g723_40Codec.h"
#include "g723_40CodecP.h"







FORWARD i32 g723_40Init( void);
FORWARD i32 g723_40Done( void);


FORWARD codCtx_t *g723_40NewCtx( u32 codFunction);
FORWARD void g723_40DelCtx( codCtx_t *);


FORWARD i32 g723_40Encode( codCtx_t *, u8 *outBuf, u32 outBytes, i16 *inBuf, u32 inBytes);
FORWARD i32 g723_40Decode( codCtx_t *, i16 *outBuf, u32 outBytes, u8 *inBuf, u32 inBytes);


PUBLIC audioCodec_t g723_40Codec = {

	G723_40CODNUMSAMPLES,
	G723_40CODNUMBYTES,
	(((G723_40CODNUMSAMPLES*2)/G723_40CODNUMBYTES)+1),

	g723_40Init,
	g723_40Done,

	g723_40NewCtx,
	g723_40DelCtx,

	g723_40Encode,
	g723_40Decode
};





PRIVATE
i32
g723_40Init( void)
{
	if( g723_40InitP() < 0)
		abort();
	return 0;
}

PRIVATE
i32
g723_40Done( void)
{
	if( g723_40DoneP() < 0)
		abort();
	return 0;
}


PRIVATE
codCtx_t *
g723_40NewCtx( u32 codFunction)
{
	codCtx_t *retVal = (codCtx_t *) malloc( sizeof(codCtx_t));

	assert( retVal);

	retVal->codType= CODEC_G723_40;
	retVal->codFunction= codFunction;

	if( g723_40NewCtxP( &retVal->codOpaque) < 0)
		abort();

	return retVal;
}


PRIVATE
void
g723_40DelCtx( codCtx_t *ctx)
{
	if( ctx) {
		if( ctx->codType != CODEC_G723_40) {
			abort();
		}

		if( g723_40DelCtxP( ctx->codOpaque) < 0)
			abort();

		free( ctx);
	}
}


PRIVATE
i32
g723_40Encode( codCtx_t *ctx, u8 *outBuf, u32 outBytes, i16 *inBuf, u32 inSamples)
{
	if( !ctx)
		abort();

	if( ctx->codType != CODEC_G723_40)
		abort();

	if( ctx->codFunction != CODEC_ENCODE)
		abort();

	if( inSamples%g723_40Codec.codNumSamples != 0) {
		trCodec(( stderr, "g723_40Codec: invalid number of samples, truncating\n"));
		inSamples = ( inSamples/g723_40Codec.codNumSamples)* g723_40Codec.codNumSamples;
	}

	if( (inSamples*g723_40Codec.codNumBytes) > (outBytes*g723_40Codec.codNumSamples))
		abort();

	return g723_40EncodeP( (g723_40CodCtx_t *)ctx->codOpaque, outBuf, inBuf, inSamples); 
}

PRIVATE
i32
g723_40Decode( codCtx_t *ctx, i16 *outBuf, u32 outSamples, u8 *inBuf, u32 inBytes)
{
	if( !ctx)
		abort();

	if( ctx->codType != CODEC_G723_40)
		abort();

	if( ctx->codFunction != CODEC_DECODE)
		abort();


	if( (inBytes*g723_40Codec.codNumSamples) > ( outSamples*g723_40Codec.codNumBytes))
		abort();

	if( inBytes%g723_40Codec.codNumBytes != 0) {
		trCodec(( stderr, "g723_40Codec: Invalid number of bytes, truncating\n"));
		inBytes = ( inBytes/g723_40Codec.codNumBytes)*g723_40Codec.codNumBytes;
	}

	return g723_40DecodeP( (g723_40CodCtx_t *)ctx->codOpaque, outBuf, inBuf, inBytes); 
}
