/////////////////////////////////////////////////////////////////////////
//
// $Id: test.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <vCodecs/codecs.h>
#include <vCodecs/payloads.h>
#include <vCodecs/vUtils.h>
#include <vCodecs/colorspace.h>

int BPP=1; //bytes per pixel
int XDIM=320;
int YDIM=240;

#define CODEC H264_PT

static int write_pnm(char *filename, unsigned char *image)
{
    FILE * f;
    char head[256];

    f = fopen(filename, "wb");
    if ( f == NULL) {
        return -1;
    }

    // convert YUV to RGB
    int size= XDIM * YDIM;
    u8 *picRaw= (u8*)malloc(size * 3);

    colorspace->YUV420PtoRGB24(picRaw, image, XDIM, YDIM);

    sprintf(head, "P6\n %d %d\n255\n", XDIM, YDIM);

    fwrite(head, 1, strlen(head), f);

    fwrite(picRaw, 1, size * 3, f);

    fclose(f);

    return 0;
}

static int write_tga(char *filename, unsigned char *image)
{
    FILE * f;
    char hdr[18];

    f = fopen(filename, "wb");
    if ( f == NULL) {
        return -1;
    }

    hdr[0]  = 0; /* ID length */
    hdr[1]  = 0; /* Color map type */
    hdr[2]  = (BPP>1)?2:3; /* Uncompressed true color (2) or greymap (3) */
    hdr[3]  = 0; /* Color map specification (not used) */
    hdr[4]  = 0; /* Color map specification (not used) */
    hdr[5]  = 0; /* Color map specification (not used) */
    hdr[6]  = 0; /* Color map specification (not used) */
    hdr[7]  = 0; /* Color map specification (not used) */
    hdr[8]  = 0; /* LSB X origin */
    hdr[9]  = 0; /* MSB X origin */
    hdr[10] = 0; /* LSB Y origin */
    hdr[11] = 0; /* MSB Y origin */
    hdr[12] = (XDIM>>0)&0xff; /* LSB Width */
    hdr[13] = (XDIM>>8)&0xff; /* MSB Width */
    if (BPP > 1) {
        hdr[14] = (YDIM>>0)&0xff; /* LSB Height */
        hdr[15] = (YDIM>>8)&0xff; /* MSB Height */
    } else {
        hdr[14] = ((YDIM*3)>>1)&0xff; /* LSB Height */
        hdr[15] = ((YDIM*3)>>9)&0xff; /* MSB Height */
    }
    hdr[16] = BPP*8;
    hdr[17] = 0x00 | (1<<5) /* Up to down */ | (0<<4); /* Image descriptor */

    /* Write header */
    fwrite(hdr, 1, sizeof(hdr), f);

    /* write first plane */
    fwrite(image, 1, XDIM*YDIM*BPP, f);

    /* Write Y and V planes for YUV formats */
    if (BPP == 1) {
        int i;

        /* Write the two chrominance planes */
        for (i=0; i<YDIM/2; i++) {
                fwrite(image+XDIM*YDIM + i*XDIM/2, 1, XDIM/2, f);
                fwrite(image+5*XDIM*YDIM/4 + i*XDIM/2, 1, XDIM/2, f);
        }
    }

    /* Close the file */
    fclose(f);

    return(0);
}

int main(int argc, char *argv[])
{
    //vCodecs test
    vCodecInit();
    printf("vCodec initialized\n");

    //get a codec
    vCoderArgs_t params;
    params.bitRate  = 1000;
    params.frameRate= 30;
    params.height   = YDIM;
    params.width    = XDIM;
    params.format   = I420P_FORMAT;
    params.quality  = 0;

    printf("Creating coder 0x%x, %d bps, %d fps, %dx%d input= 0x%x\n",
           CODEC,
           params.bitRate, params.frameRate,
           params.width, params.height,
           params.format
          );

    vCoder_t *coder = vGetCoderByPT(CODEC)->newCoder(&params);

    //get a decoder
    vDecoderArgs_t params2;
    params2.format = I420P_FORMAT;
    params2.height = 0;
    params2.width  = 0;

    printf("Creating decoder 0x%x, input= 0x%x\n", CODEC, params.format);

    vDecoder_t * decoder = vGetDecoderByPT(CODEC)->newDecoder(&params2);

    int i, size, x, y;
    unsigned char *outbuf, *picture_buf,*decoded_buf;

    printf("Video encoding\n");

    /* alloc image and output buffer */
    size = params.width*params.height;
    int total_size = size*3;

    outbuf = new unsigned char[total_size];      // coded frame
    picture_buf = new unsigned char[total_size]; // original frame
    decoded_buf = new unsigned char[total_size]; // decodec frame

    unsigned char * data = picture_buf;
    int linesize = params.width*3;
    timeval time1,time2;
    gettimeofday(&time1,NULL);

    /* encode 1 second of video */
    for(i=0;i<24*5;i++)
    {
        memset(outbuf,0,total_size);
        memset(picture_buf,0,total_size);
        memset(decoded_buf,0,total_size);

        /* prepare a dummy image */
        for(y=0;y<params.height;y++)
        {
            for(x=0;x<linesize/2;x+=3)
            {
                int pos0 = y * linesize + x;

                if (y<(i*5)%240)
                {
                    data[pos0]   = 0x00;
                    data[pos0+1] = 0x00;
                    data[pos0+2] = 0xff;
                } else if (y<(i*10)%240) {
                    data[pos0]   = 0x00;
                    data[pos0+1] = 0xff;
                    data[pos0+2] = 0x00;
                } else {
                    data[pos0]   = 0xff;
                    data[pos0+1] = 0x00;
                    data[pos0+2] = 0x00;
                }
            }
        }

        int size = coder->params.width*coder->params.height;

        unsigned char * yuv = new unsigned char[size*3/2];

        colorspace->BGR24toYUV420P(yuv,
                                   picture_buf,
                                   coder->params.width,
                                   coder->params.height
                                  );

        /* encode the image */
        int n1 = vEncode(coder, yuv, size*3/2, outbuf, total_size);
        printf("encoding frame %3d (size=%5d)\n", i, n1);

        char filename[128];
        sprintf(filename,"orig-%03d.tga",i);
        write_tga(filename, yuv);

        /*decode image */
        int n2 = vDecode(decoder,outbuf,n1,decoded_buf,total_size);
        printf("decoding frame %3d (size=%5d)\n", i, n2);
        sprintf(filename,"dest-%03d.tga",i);
        write_tga(filename, decoded_buf);
        delete yuv;
    }

    printf("End encoding \n");

    gettimeofday(&time2,NULL);
    delete picture_buf;
    delete outbuf;
    printf("elapsed time = %ld ms\n",
           (time2.tv_sec*1000 + time2.tv_usec/1000)
           - (time1.tv_sec*1000 + time1.tv_usec/1000)
          );
    printf("Closing codecs \n");

    //delete coder/decoder
    vDeleteCoder(coder);
    vDeleteDecoder(decoder);

    printf("Codecs deleted\n");

    return 0;
}

