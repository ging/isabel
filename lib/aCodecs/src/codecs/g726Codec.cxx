#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "codec.h"

#include "g726Codec.h"

#include "g726/g726_xx.h"
#include "g726/g72x.h"

int pack(unsigned char *packBuff, unsigned char *unPackBuff, int nUnpackedBytes, int bitsPerUnpacked);
int unpack(unsigned char *packBuff, unsigned char *unPackBuff, int nUnpackedBytes, int bitsPerUnpacked);

int g726DeleteCoder(void *context);
int g726DeleteDecoder(void *context);

//////////////////////////////////////////////
////////        16 Kbtis            //////////
//////////////////////////////////////////////
int g726_16Encode(void *context,
                  unsigned char *inBuff,
                  int nSamples,
                  unsigned char *outBuff
                 );
int g726_16Decode(void *context,
                  unsigned char *inBuff,
                  int nBytes,
                  unsigned char *outBuff
                 );
//////////////////////////////////////////////
////////        24 Kbtis            //////////
//////////////////////////////////////////////
int g726_24Encode(void *context,
                  unsigned char *inBuff,
                  int nSamples,
                  unsigned char *outBuff
                 );
int g726_24Decode(void *context,
                  unsigned char *inBuff,
                  int nBytes,
                  unsigned char *outBuff
                 );

//////////////////////////////////////////////
////////        32 Kbtis            //////////
//////////////////////////////////////////////
int g726_32Encode(void *context,
                  unsigned char *inBuff,
                  int nSamples,
                  unsigned char *outBuff
                 );
int g726_32Decode(void *context,
                  unsigned char *inBuff,
                  int nBytes,
                  unsigned char *outBuff
                 );

//////////////////////////////////////////////
////////        40 Kbtis            //////////
//////////////////////////////////////////////
int g726_40Encode(void *context,
                  unsigned char *inBuff,
                  int nSamples,
                  unsigned char *outBuff
                 );
int g726_40Decode(void *context,
                  unsigned char *inBuff,
                  int nBytes,
                  unsigned char *outBuff
                 );


aCoder_t *
g726NewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));
   
    if ( ! coder )
    {
        perror("g726NewCoder::malloc");
        return NULL;
    }

    g72x_state *state= (g72x_state *)malloc(sizeof(g72x_state));

    if ( ! state )
    {
        free(coder);
        perror("g726NewCoder::malloc(2)");
        return NULL;
    }

    g72x_init_state(state);

    coder->Delete = g726DeleteCoder;

    coder->format = fmt;
    coder->sampleRate = sampleRate;
    coder->bandwidth = bandwidth;

    switch (fmt)
    {
#if 0
    case G726_XX_FORMAT:
        coder->Encode = g726_16Encode;

        coder->ratio = 1.0/8.0;
        coder->invRatio = 8;
        coder->bytesPerSample = 2;
        coder->minSamples = 4;

        break;
#endif
    case G726_8_FORMAT:
    case G726_16_FORMAT:
        coder->Encode = g726_24Encode;

        coder->ratio = 3.0/16.0;
        coder->invRatio = 5; // Realmente 5.3
        coder->bytesPerSample = 2;
        coder->minSamples = 8;

        break;
#if 0
    case G726_XX_FORMAT:
        coder->Encode = g726_32Encode;

        coder->ratio = 1.0/4.0;
        coder->invRatio = 4;
        coder->bytesPerSample = 2;
        coder->minSamples = 2;

        break;
    case G726_XX_FORMAT:
        coder->Encode = g726_40Encode;

        coder->ratio = 5.0/16.0;
        coder->invRatio = 3; // Realmente 3.2
        coder->bytesPerSample = 2;
        coder->minSamples = 8;

        break;
#endif
    default:
        perror("G726NewCoder: cannot handle format format\n");
        free(state);
        free(coder);

        return NULL;
    }

    coder->state = state;

    return coder;
}

int
g726DeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    g72x_state *state= (g72x_state *)coder->state;

    free(state);
    free(coder);

    return 1;
}

aDecoder_t *
g726NewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));
   
    if ( ! decoder )
    {
        perror("g726NewDecoder::malloc");
        return NULL;
    }

    g72x_state *state= (g72x_state *)malloc(sizeof(g72x_state));

    if ( ! state )
    {
        free(decoder);
        perror("g726NewDecoder::malloc(2)");
        return NULL;
    }

    g72x_init_state(state);

    decoder->Delete = g726DeleteDecoder;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;

    switch (fmt)
    {
#if 0
    case G726_XX_FORMAT:
        decoder->Decode = g726_16Decode;

        decoder->bytesPerSample = 2;

        break;
#endif
    case G726_8_FORMAT:
    case G726_16_FORMAT:
        decoder->Decode = g726_24Decode;

        decoder->bytesPerSample = 2;

        break;
#if 0
    case G726_XX_FORMAT:
        decoder->Decode = g726_32Decode;

        decoder->bytesPerSample = 2;

        break;
    case G726_XX_FORMAT:
        decoder->Decode = g726_40Decode;

        decoder->bytesPerSample = 2;

        break;
#endif
    default:
        perror("G726NewDecoder: cannot handle format format\n");
        free(state);
        free(decoder);

        return NULL;
    }

    decoder->state = state;

    return decoder;
}

int
g726DeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    g72x_state *state= (g72x_state *)decoder->state;

    free(state);
    free(decoder);

    return 1;
}

int
g726_16Encode(void *context,
              unsigned char *inBuff,
              int nSamples,
              unsigned char *outBuff
             )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    g72x_state *state= (g72x_state *)coder->state;

    short int *buffer= (short int *)inBuff;

    int nframes = nSamples / coder->bytesPerSample;

    /* Codificar el buffer y devolver numero de bytes */
    for (int i = 0; i < nSamples / 4; i++)
    {
        unsigned int cnt2, code2;
      
        code2 = 0;
        for (cnt2 = 0; cnt2 < 4; cnt2++)
        {
            unsigned int code;
         
            code = g726_16_encoder(buffer[i*4+cnt2], state);
            code2 |= (( code & 0X03 ) << ( cnt2*2));
        }
        outBuff[i] = code2 & 0xFF;
    }

    return nSamples * coder->bytesPerSample / coder->invRatio;
}

int
g726_16Decode(void *context,
              unsigned char *inBuff,
              int nBytes,
              unsigned char *outBuff
             )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    g72x_state *state= (g72x_state *)decoder->state;

    short int *buffer= (short int *)outBuff;

    for (int i = 0; i < nBytes ; i++)
    {
        unsigned int code2, cnt2;
      
        code2 = inBuff[i];
        for ( cnt2 = 0; cnt2 < 4; cnt2++)
        {
            unsigned int code;
         
            code = g726_16_decoder(code2 & 0x03, state);
            code2 >>= 2;
            buffer[i*4+cnt2] = code;
        }
    }

    return nBytes * 8;
}


int
g726_24Encode(void *context,
              unsigned char *inBuff,
              int nSamples,
              unsigned char *outBuff
             )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    g72x_state *state= (g72x_state *)coder->state;
 
    short int *buffer= (short int *)inBuff;

    int nframes = nSamples / coder->bytesPerSample;

    /* Codificar el buffer y devolver numero de bytes */
    for (int i = 0; i < nSamples / 8; i++)
    {
        unsigned int cnt2, code2;
      
        code2 = 0;
        for (cnt2 = 0; cnt2 < 8; cnt2++)
        {
            unsigned int code;
         
            code = g726_24_encoder(buffer[i*8+cnt2], state);
            code2 |= (( code & 0X07 ) << ( cnt2*3));
        }
        outBuff[i*3] = code2 & 0xFF;
        outBuff[i*3+1] = (code2>>8) & 0xFF;
        outBuff[i*3+2] = (code2>>16) & 0xFF;
    }

    return (nSamples/8)*3;
}


int
g726_24Decode(void *context,
              unsigned char *inBuff,
              int nBytes,
              unsigned char *outBuff
             )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    g72x_state *state= (g72x_state *)decoder->state;

    short int *buffer= (short int *)outBuff;

    for (int i = 0; i < (nBytes/3) ; i++)
    {
        unsigned int code2, cnt2;
      
        code2 = inBuff[i*3];
        code2 |= inBuff[i*3+1]<<8;
        code2 |= inBuff[i*3+2]<<16;
        for ( cnt2 = 0; cnt2 < 8; cnt2++)
        {
            unsigned int code;
         
            code = g726_24_decoder(code2 & 0x07, state);
            code2 >>= 3;
            buffer[i*8+cnt2] = code;
        }
    }

    return nBytes * 16 / 3;
}

int
g726_32Encode(void *context,
              unsigned char *inBuff,
              int nSamples,
              unsigned char *outBuff
             )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    g72x_state *state= (g72x_state *)coder->state;

    short int *buffer= (short int *)inBuff;

    int nframes = nSamples / coder->bytesPerSample;

    /* Codificar el buffer y devolver numero de bytes */
    for (int i = 0; i < nSamples / 2; i++)
    {
        unsigned int cnt2, code2;
      
        code2 = 0;
        for (cnt2 = 0; cnt2 < 2; cnt2++)
        {
            unsigned int code;

            code = g726_32_encoder(buffer[i*2+cnt2], state);
            code2 |= (( code & 0X0F ) << ( cnt2*4));
        }
        outBuff[i] = code2 & 0xFF;
    }

    return nSamples * coder->bytesPerSample / coder->invRatio;
}

int
g726_32Decode(void *context,
              unsigned char *inBuff,
              int nBytes,
              unsigned char *outBuff
             )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    g72x_state *state= (g72x_state *)decoder->state;

    short int *buffer= (short int *)outBuff;

    for (int i = 0; i < nBytes ; i++)
    {
        unsigned int code2, cnt2;
      
        code2 = inBuff[i];
        for ( cnt2 = 0; cnt2 < 2; cnt2++)
        {
            unsigned int code;
         
            code = g726_32_decoder(code2 & 0x0F, state);
            code2 >>= 4;
            buffer[i*2+cnt2] = code;
        }
    }

    return nBytes * 4;
}

int
g726_40Encode(void *context,
              unsigned char *inBuff,
              int nSamples,
              unsigned char *outBuff
             )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    g72x_state *state= (g72x_state *)coder->state;

    short int *buffer= (short int *)inBuff;

    int nframes = nSamples / coder->bytesPerSample;
   
    for (int i = 0; i < nSamples / 8; i++)
    {
        unsigned int cnt2;
        unsigned char code[8];   
        memset(code, 0, 8);
      
        for (cnt2 = 0 ; cnt2 < 8 ; cnt2++)
        {
            code[cnt2] = g726_40_encoder(buffer[i*8+cnt2], state);
        }
        pack(outBuff+i*5, code, 8, 5);
    }

    return (nSamples/8)*5;
}

int
g726_40Decode(void *context,
              unsigned char *inBuff,
              int nBytes,
              unsigned char *outBuff
             )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    g72x_state *state= (g72x_state *)decoder->state;

    short int *buffer= (short int *)outBuff;

    for (int i = 0; i < nBytes/5 ; i++)
    {
        unsigned int cnt2;
        unsigned char code2[8];
      
        memset(code2, 0, 8);
        unpack(inBuff+i*5, code2, 8, 5);
        for ( cnt2 = 0; cnt2 < 8; cnt2++)
        {
            unsigned int code;
         
            code = g726_40_decoder(code2[cnt2], state);
            buffer[i*8+cnt2] = code;
        }
    }

    return (nBytes/5*8) * decoder->bytesPerSample; 
}

//////////////////////////////////////////////////
/////////     UTILS        ///////////////////////
//////////////////////////////////////////////////

/* Tomado de rat */
int
pack(unsigned char *packBuff,
     unsigned char *unPackBuff,
     int nUnpackedBytes,
     int bitsPerUnpacked
    )
{
    int i, bits, x;
   
    memset(packBuff, 0, nUnpackedBytes*bitsPerUnpacked/8);
    i = bits = x = 0;
    for ( i = 0 ; i < nUnpackedBytes ; i++ )
    {
        packBuff[x] |= (unPackBuff[i]<<bits) & 0xFF;
        bits += bitsPerUnpacked;
        if (bits > 8)
        {
            bits &= 0x07;
            x++;
            packBuff[x] |= unPackBuff[i] >> (bitsPerUnpacked - bits);
        }
    }

    return nUnpackedBytes * bitsPerUnpacked / 8;
}

int
unpack(unsigned char *packBuff,
       unsigned char *unPackBuff,
       int nUnpackedBytes,
       int bitsPerUnpacked
      )
{
    int i, bits, x;
    unsigned char mask;
   
    i = bits = x = 0;
    mask = 0;
    while ( i < bitsPerUnpacked )
    {
        mask |= 1<<i;
        i++;
    }
    for ( i = 0 ; i < nUnpackedBytes ; i++ )
    {
        unPackBuff[i] |= (packBuff[x]>>bits) & mask;
        bits += bitsPerUnpacked;
        if (bits > 8)
        {
            bits &= 0x07;
            x++;
            unPackBuff[i] |= packBuff[x] << (bitsPerUnpacked - bits);
            unPackBuff[i] &= mask;
        }
    }

    return nUnpackedBytes * bitsPerUnpacked / 8;
}


