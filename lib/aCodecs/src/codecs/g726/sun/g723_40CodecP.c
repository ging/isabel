/*
 * g723_40CodecP.h -- libg723_40 wrapper, g723_40 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_40CodecP.c	5.1

 * $au$ : djr@dit.upm.es

 */



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <memory.h>


#include "general.h"
#include "g723_40CodecP.h"







PUBLIC
i32
g723_40InitP( void)
{
	return 0;	/* OK */
}


PUBLIC
i32
g723_40DoneP( void)
{
	return 0;	/* OK */
}

PUBLIC
i32
g723_40NewCtxP( void **ptr)
{
	g723_40CodCtx_t *ctx = malloc( sizeof( g723_40CodCtx_t));

	if( !ctx)
		return -1;

        g72x_init_state( ctx);
	*ptr = ctx;

	return 0;
}

PUBLIC
i32
g723_40DelCtxP( g723_40CodCtx_t *ctx)
{
	if( ctx) {
                free( ctx);
		return 0;
	}
	return -1;
}

PUBLIC
i32
g723_40EncodeP( g723_40CodCtx_t *ctx, u8 *outBuf, i16 *inBuf, u32 inSamples)
{
	u32 cnt, cnt2;
	u32 code, code2;

	/* Codificar el buffer y devolver numero de bytes */
	for( cnt = 0; cnt < (inSamples/3); cnt++) {
                code2 = 0;
                for( cnt2 = 0; cnt2 < 3; cnt2++) {
                    code = g723_40_encoder( inBuf[cnt*3+cnt2],
                                            AUDIO_ENCODING_LINEAR,
                                            ctx);
                    code2 |= (( code & 0x1f ) << ( cnt2*5));
                }
                outBuf[cnt*2] = code2 & 0xff;
                code2 >>= 8;
                outBuf[cnt*2+1] = code2 & 0xff;
	}
	return 2*cnt;
}

PUBLIC
i32
g723_40DecodeP( g723_40CodCtx_t *ctx, i16 *outBuf, u8 *inBuf, u32 inBytes)
{
	u32 cnt, cnt2;
	u32 code, code2;

	for( cnt = 0; cnt < (inBytes/2); cnt++) {
        code2 = inBuf[cnt*2];
        code2 |= inBuf[cnt*2+1] << 8;
        for( cnt2 = 0; cnt2 < 3; cnt2++) {
           code = g723_40_decoder( code2 & 0x1f,
                                   AUDIO_ENCODING_LINEAR,
                                   ctx);
           code2 >>= 5;
           outBuf[cnt*3+cnt2] = code;
        }
	}
	return cnt*3;
}
