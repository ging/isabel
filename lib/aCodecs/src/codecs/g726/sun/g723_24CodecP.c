/*
 * g723_24CodecP.h -- libg723_24 wrapper, g723_24 codec
 *

 * $ww$ : edit with ts= 4

 * $id$ : dit.upm.es -- @(#)g723_24CodecP.c	5.1

 * $au$ : djr@dit.upm.es

 */



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <memory.h>


#include "general.h"
#include "g723_24CodecP.h"







PUBLIC
i32
g723_24InitP( void)
{
	return 0;	/* OK */
}


PUBLIC
i32
g723_24DoneP( void)
{
	return 0;	/* OK */
}

PUBLIC
i32
g723_24NewCtxP( void **ptr)
{
	g723_24CodCtx_t *ctx = malloc( sizeof( g723_24CodCtx_t));

	if( !ctx)
		return -1;

        g72x_init_state( ctx);
	*ptr = ctx;

	return 0;
}

PUBLIC
i32
g723_24DelCtxP( g723_24CodCtx_t *ctx)
{
	if( ctx) {
                free( ctx);
		return 0;
	}
	return -1;
}

PUBLIC
i32
g723_24EncodeP( g723_24CodCtx_t *ctx, u8 *outBuf, i16 *inBuf, u32 inSamples)
{
	u32 cnt, cnt2;
	u32 code, code2;

	/* Codificar el buffer y devolver numero de bytes */
	for( cnt = 0; cnt < (inSamples/8); cnt++) {
                code2 = 0;
                for( cnt2 = 0; cnt2 < 8; cnt2++) {
                    code = g723_24_encoder( inBuf[cnt*8+cnt2],
                                            AUDIO_ENCODING_LINEAR,
                                            ctx);
                    code2 |= (( code & 0X07 ) << ( cnt2*3));
                }
                outBuf[cnt*3] = code2 & 0xff;
                code2 >>= 8;
                outBuf[cnt*3+1] = code2 & 0xff;
                code2 >>= 8;
                outBuf[cnt*3+2] = code2 & 0xff;
	}
	return 3*cnt;
}

PUBLIC
i32
g723_24DecodeP( g723_24CodCtx_t *ctx, i16 *outBuf, u8 *inBuf, u32 inBytes)
{
	u32 cnt, cnt2;
	u32 code, code2;

	for( cnt = 0; cnt < (inBytes/3); cnt++) {
        code2 = inBuf[cnt*3];
        code2 |= inBuf[cnt*3+1] << 8;
        code2 |= inBuf[cnt*3+2] << 16;
        for( cnt2 = 0; cnt2 < 8; cnt2++) {
           code = g723_24_decoder( code2 & 0x07,
                                   AUDIO_ENCODING_LINEAR,
                                   ctx);
           code2 >>= 3;
           outBuf[cnt*8+cnt2] = code;
        }
	}
	return cnt*8;
}
