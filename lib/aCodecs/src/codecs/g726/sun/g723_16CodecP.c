/*
 * g723_16CodecP.h -- libg723_16 wrapper, g723_16 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_16CodecP.c	5.1

 * $au$ : djr@dit.upm.es

 */



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <memory.h>


#include "general.h"
#include "g723_16CodecP.h"







PUBLIC
i32
g723_16InitP( void)
{
	return 0;	/* OK */
}


PUBLIC
i32
g723_16DoneP( void)
{
	return 0;	/* OK */
}

PUBLIC
i32
g723_16NewCtxP( void **ptr)
{
	g723_16CodCtx_t *ctx = malloc( sizeof( g723_16CodCtx_t));

	if( !ctx)
		return -1;

        g72x_init_state( ctx);
	*ptr = ctx;

	return 0;
}

PUBLIC
i32
g723_16DelCtxP( g723_16CodCtx_t *ctx)
{
	if( ctx) {
                free( ctx);
		return 0;
	}
	return -1;
}

PUBLIC
i32
g723_16EncodeP( g723_16CodCtx_t *ctx, u8 *outBuf, i16 *inBuf, u32 inSamples)
{
	u32 cnt, cnt2;
	u32 code, code2;

	/* Codificar el buffer y devolver numero de bytes */
	for( cnt = 0; cnt < (inSamples/4); cnt++) {
                code2 = 0;
                for( cnt2 = 0; cnt2 < 4; cnt2++) {
                    code = g723_16_encoder( inBuf[cnt*4+cnt2],
                                            AUDIO_ENCODING_LINEAR,
                                            ctx);
                    code2 |= (( code & 0X03 ) << ( cnt2*2));
                }
                outBuf[cnt] = code2 & 0xff;
	}
	return cnt;
}

PUBLIC
i32
g723_16DecodeP( g723_16CodCtx_t *ctx, i16 *outBuf, u8 *inBuf, u32 inBytes)
{
	u32 cnt, cnt2;
	u32 code, code2;

	for( cnt = 0; cnt < (inBytes); cnt++) {
        code2 = inBuf[cnt];
        for( cnt2 = 0; cnt2 < 4; cnt2++) {
           code = g723_16_decoder( code2 & 0x03,
                                   AUDIO_ENCODING_LINEAR,
                                   ctx);
           code2 >>= 2;
           outBuf[cnt*4+cnt2] = code;
        }
	}
	return cnt*4;
}
