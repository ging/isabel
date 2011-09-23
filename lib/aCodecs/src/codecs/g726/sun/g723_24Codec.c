/*
 * g723_24Codec.c -- G723_24 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_24Codec.c	5.1

 * $au$ : djr@dit.upm.es

 */



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "general.h"
#include "audioCodec.h"
#include "g723_24Codec.h"
#include "g723_24CodecP.h"







FORWARD i32 g723_24Init( void);
FORWARD i32 g723_24Done( void);


FORWARD codCtx_t *g723_24NewCtx( u32 codFunction);
FORWARD void g723_24DelCtx( codCtx_t *);


FORWARD i32 g723_24Encode( codCtx_t *, u8 *outBuf, u32 outBytes, i16 *inBuf, u32 inBytes);
FORWARD i32 g723_24Decode( codCtx_t *, i16 *outBuf, u32 outBytes, u8 *inBuf, u32 inBytes);


PUBLIC audioCodec_t g723_24Codec = {

	G723_24CODNUMSAMPLES,
	G723_24CODNUMBYTES,
	(((G723_24CODNUMSAMPLES*2)/G723_24CODNUMBYTES)+1),

	g723_24Init,
	g723_24Done,

	g723_24NewCtx,
	g723_24DelCtx,

	g723_24Encode,
	g723_24Decode
};





PRIVATE
i32
g723_24Init( void)
{
	if( g723_24InitP() < 0)
		abort();
	return 0;
}

PRIVATE
i32
g723_24Done( void)
{
	if( g723_24DoneP() < 0)
		abort();
	return 0;
}


PRIVATE
codCtx_t *
g723_24NewCtx( u32 codFunction)
{
	codCtx_t *retVal = (codCtx_t *) malloc( sizeof(codCtx_t));

	assert( retVal);

	retVal->codType= CODEC_G723_24;
	retVal->codFunction= codFunction;

	if( g723_24NewCtxP( &retVal->codOpaque) < 0)
		abort();

	return retVal;
}


PRIVATE
void
g723_24DelCtx( codCtx_t *ctx)
{
	if( ctx) {
		if( ctx->codType != CODEC_G723_24) {
			abort();
		}

		if( g723_24DelCtxP( ctx->codOpaque) < 0)
			abort();

		free( ctx);
	}
}


PRIVATE
i32
g723_24Encode( codCtx_t *ctx, u8 *outBuf, u32 outBytes, i16 *inBuf, u32 inSamples)
{
	if( !ctx)
		abort();

	if( ctx->codType != CODEC_G723_24)
		abort();

	if( ctx->codFunction != CODEC_ENCODE)
		abort();

	if( inSamples%g723_24Codec.codNumSamples != 0) {
		fprintf( stderr, "g723_24Codec: invalid number of samples, truncating\n");
		inSamples = ( inSamples/g723_24Codec.codNumSamples)* g723_24Codec.codNumSamples;
	}

	if( (inSamples*g723_24Codec.codNumBytes) > (outBytes*g723_24Codec.codNumSamples))
		abort();

	return g723_24EncodeP( (g723_24CodCtx_t *)ctx->codOpaque, outBuf, inBuf, inSamples); 
}

PRIVATE
i32
g723_24Decode( codCtx_t *ctx, i16 *outBuf, u32 outSamples, u8 *inBuf, u32 inBytes)
{
	if( !ctx)
		abort();

	if( ctx->codType != CODEC_G723_24)
		abort();

	if( ctx->codFunction != CODEC_DECODE)
		abort();


	if( (inBytes*g723_24Codec.codNumSamples) > ( outSamples*g723_24Codec.codNumBytes))
		abort();

	if( inBytes%g723_24Codec.codNumBytes != 0) {
		fprintf( stderr, "g723_24Codec: Invalid number of bytes, truncating\n");
		inBytes = ( inBytes/g723_24Codec.codNumBytes)*g723_24Codec.codNumBytes;
	}

	return g723_24DecodeP( (g723_24CodCtx_t *)ctx->codOpaque, outBuf, inBuf, inBytes); 
}
