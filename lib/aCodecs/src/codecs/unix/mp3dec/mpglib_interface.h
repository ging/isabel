#ifndef MPGLIB_INTERFACE_H
#define MPGLIB_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <lame/lame.h>

#undef REAL_IS_FLOAT
#define REAL_IS_FLOAT

#ifdef REAL_IS_FLOAT
#  define real float
#elif defined(REAL_IS_LONG_DOUBLE)
#  define real long double
#else
#  define real double
#endif

#define         SBLIMIT                 32
#define         SSLIMIT                 18

#define MAXFRAMESIZE 2880

struct frame {
    int stereo;
    int jsbound;
    int single;
    int lsf;
    int mpeg25;
    int header_change;
    int lay;
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize; /* computed framesize */

	/* AF: ADDED FOR LAYER1/LAYER2 */
    int II_sblimit;
    struct al_table2 const *alloc;
	int down_sample_sblimit;
	int	down_sample;


};

struct gr_info_s {
      int scfsi;
      unsigned part2_3_length;
      unsigned big_values;
      unsigned scalefac_compress;
      unsigned block_type;
      unsigned mixed_block_flag;
      unsigned table_select[3];
      unsigned subblock_gain[3];
      unsigned maxband[3];
      unsigned maxbandl;
      unsigned maxb;
      unsigned region1start;
      unsigned region2start;
      unsigned preflag;
      unsigned scalefac_scale;
      unsigned count1table_select;
      real *full_gain[3];
      real *pow2gain;
};

struct III_sideinfo
{
  unsigned main_data_begin;
  unsigned private_bits;
  struct {
    struct gr_info_s gr[2];
  } ch[2];
};


#ifndef plotting_data_defined
#define plotting_data_defined
struct plotting_data;
typedef struct plotting_data plotting_data;
#endif


#ifndef NOANALYSIS
extern plotting_data *mpg123_pinfo;
#endif

struct buf {
        unsigned char *pnt;
        long size;
        long pos;
        struct buf *next;
        struct buf *prev;
};

struct framebuf {
        struct buf *buf;
        long pos;
        struct frame *next;
        struct frame *prev;
};

typedef struct mpstr_tag {
        struct buf *head,*tail;
        int vbr_header;               /* 1 if valid Xing vbr header detected */
        int num_frames;               /* set if vbr header present */
        int enc_delay;                /* set if vbr header present */
        int enc_padding;              /* set if vbr header present */
        int header_parsed;
        int side_parsed;
        int data_parsed;
        int free_format;             /* 1 = free format frame */
        int old_free_format;        /* 1 = last frame was free format */
        int bsize;
        int framesize;
        int ssize;
        int dsize;
        int fsizeold;
        int fsizeold_nopadding;
        struct frame fr;
        unsigned char bsspace[2][MAXFRAMESIZE+512]; /* MAXFRAMESIZE */
        real hybrid_block[2][2][SBLIMIT*SSLIMIT];
        int hybrid_blc[2];
        unsigned long header;
        int bsnum;
        real synth_buffs[2][2][0x110];
        int  synth_bo;
        int  sync_bitstream;

    int bitindex;
    unsigned char* wordpointer;
} MPSTR, *PMPSTR;



#define MP3_ERR -1
#define MP3_OK  0
#define MP3_NEED_MORE 1






/*********************************************************************
 *
 * decoding
 *
 * a simple interface to mpglib, part of mpg123, is also included if
 * libmp3lame is compiled with HAVE_MPGLIB
 *
 *********************************************************************/

/* required call to initialize decoder
 * NOTE: the decoder should not be used when encoding is performed
 * with decoding on the fly */
int mylame_decode_init(MPSTR * mp);

/*********************************************************************
 * input 1 mp3 frame, output (maybe) pcm data.
 *
 *  nout = lame_decode(mp3buf,len,pcm_l,pcm_r);
 *
 * input:
 *    len          :  number of bytes of mp3 data in mp3buf
 *    mp3buf[len]  :  mp3 data to be decoded
 *
 * output:
 *    nout:  -1    : decoding error
 *            0    : need more data before we can complete the decode
 *           >0    : returned 'nout' samples worth of data in pcm_l,pcm_r
 *    pcm_l[nout]  : left channel data
 *    pcm_r[nout]  : right channel data
 *
 *********************************************************************/
int mylame_decode(
        MPSTR * mp,
        unsigned char *  mp3buf,
        int              len,
        short            pcm_l[],
        short            pcm_r[] );

/* same as lame_decode, and also returns mp3 header data */
int mylame_decode_headers(
        MPSTR * mp,
        unsigned char*   mp3buf,
        int              len,
        short            pcm_l[],
        short            pcm_r[],
        mp3data_struct*  mp3data );

/* same as lame_decode, but returns at most one frame */
int mylame_decode1(
        MPSTR * mp,
        unsigned char*  mp3buf,
        int             len,
        short           pcm_l[],
        short           pcm_r[] );

/* same as lame_decode1, but returns at most one frame and mp3 header data */
int mylame_decode1_headers(
        MPSTR * mp,
        unsigned char*   mp3buf,
        int              len,
        short            pcm_l[],
        short            pcm_r[],
        mp3data_struct*  mp3data );

/* same as lame_decode1_headers, but also returns enc_delay and enc_padding
   from VBR Info tag, (-1 if no info tag was found) */
int mylame_decode1_headersB(
        MPSTR * mp,
        unsigned char*   mp3buf,
        int              len,
        short            pcm_l[],
        short            pcm_r[],
        mp3data_struct*  mp3data,
        int              *enc_delay,
        int              *enc_padding );


/* cleanup call to exit decoder  */
int mylame_decode_exit(MPSTR * mp);


#if defined(__cplusplus)
}
#endif
#endif /* LAME_LAME_H */

