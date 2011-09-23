/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVM_FOURCC_H
#define AVM_FOURCC_H

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 ) \
  ( (long)(unsigned char)(ch0) | ( (long)(unsigned char)(ch1) << 8 ) | \
  ( (long)(unsigned char)(ch2) << 16 ) | ( (long)(unsigned char)(ch3) << 24 ) )
#endif /* mmioFOURCC */

/* On2 Truemotion VP3.x */
#define RIFFINFO_VP31	mmioFOURCC('V', 'P', '3', '1')
#define RIFFINFO_vp31	mmioFOURCC('v', 'p', '3', '1')
#define RIFFINFO_VP30	mmioFOURCC('V', 'P', '3', '0')
#define RIFFINFO_vp30	mmioFOURCC('v', 'p', '3', '0')

/* OpenDivX */
#define RIFFINFO_MP4S	mmioFOURCC('M', 'P', '4', 'S')
#define RIFFINFO_mp4s	mmioFOURCC('m', 'p', '4', 's')
#define RIFFINFO_DIVX	mmioFOURCC('D', 'I', 'V', 'X')
#define RIFFINFO_divx	mmioFOURCC('d', 'i', 'v', 'x')
#define RIFFINFO_DX50	mmioFOURCC('D', 'X', '5', '0')
#define RIFFINFO_dx50	mmioFOURCC('d', 'x', '5', '0')
#define RIFFINFO_DIV1	mmioFOURCC('D', 'I', 'V', '1')
#define RIFFINFO_div1	mmioFOURCC('d', 'i', 'v', '1')

/* DivX codecs */
#define RIFFINFO_DIV2	mmioFOURCC('D', 'I', 'V', '2')
#define RIFFINFO_div2	mmioFOURCC('d', 'i', 'v', '2')
#define RIFFINFO_DIV3	mmioFOURCC('D', 'I', 'V', '3')
#define RIFFINFO_div3	mmioFOURCC('d', 'i', 'v', '3')
#define RIFFINFO_DIV4	mmioFOURCC('D', 'I', 'V', '4')
#define RIFFINFO_div4	mmioFOURCC('d', 'i', 'v', '4')
#define RIFFINFO_DIV5	mmioFOURCC('D', 'I', 'V', '5')
#define RIFFINFO_div5	mmioFOURCC('d', 'i', 'v', '5')
#define RIFFINFO_DIV6	mmioFOURCC('D', 'I', 'V', '6')
#define RIFFINFO_div6	mmioFOURCC('d', 'i', 'v', '6')
#define RIFFINFO_MP41	mmioFOURCC('M', 'P', '4', '1')
#define RIFFINFO_mp41	mmioFOURCC('m', 'p', '4', '1')
#define RIFFINFO_MP43	mmioFOURCC('M', 'P', '4', '3')
#define RIFFINFO_mp43	mmioFOURCC('m', 'p', '4', '3')

/* old MS Mpeg-4 codecs */
#define RIFFINFO_MP42	mmioFOURCC('M', 'P', '4', '2')
#define RIFFINFO_mp42	mmioFOURCC('m', 'p', '4', '2')
#define RIFFINFO_MPG3	mmioFOURCC('M', 'P', 'G', '3')
#define RIFFINFO_mpg3	mmioFOURCC('m', 'p', 'g', '3')
#define RIFFINFO_MPG4	mmioFOURCC('M', 'P', 'G', '4')
#define RIFFINFO_mpg4	mmioFOURCC('m', 'p', 'g', '4')

/* Windows Media codecs */
#define RIFFINFO_WMV1	mmioFOURCC('W', 'M', 'V', '1')
#define RIFFINFO_wmv1	mmioFOURCC('w', 'm', 'v', '1')
#define RIFFINFO_WMV2	mmioFOURCC('W', 'M', 'V', '2')
#define RIFFINFO_wmv2	mmioFOURCC('w', 'm', 'v', '2')
#define RIFFINFO_WMV3	mmioFOURCC('W', 'M', 'V', '3')
#define RIFFINFO_wmv3	mmioFOURCC('w', 'm', 'v', '3')
#define RIFFINFO_MWV1	mmioFOURCC('M', 'W', 'V', '1')

/* Angel codecs */
#define RIFFINFO_AP41	mmioFOURCC('A', 'P', '4', '1')
#define RIFFINFO_ap41	mmioFOURCC('a', 'p', '4', '1')
#define RIFFINFO_AP42	mmioFOURCC('A', 'P', '4', '2')
#define RIFFINFO_ap42	mmioFOURCC('a', 'p', '4', '2')

/* other codecs	*/
/* Intel video */
#define RIFFINFO_IV31	mmioFOURCC('I', 'V', '3', '1')
#define RIFFINFO_iv31	mmioFOURCC('i', 'v', '3', '1')
#define RIFFINFO_IV32	mmioFOURCC('I', 'V', '3', '2')
#define RIFFINFO_iv32	mmioFOURCC('i', 'v', '3', '2')
#define RIFFINFO_IV41	mmioFOURCC('I', 'V', '4', '1')     /* Indeo Interactive */
#define RIFFINFO_iv41	mmioFOURCC('i', 'v', '4', '1')
#define RIFFINFO_IV50	mmioFOURCC('I', 'V', '5', '0')
#define RIFFINFO_iv50	mmioFOURCC('i', 'v', '5', '0')
#define RIFFINFO_H263	mmioFOURCC('H', '2', '6', '3')
#define RIFFINFO_h263	mmioFOURCC('h', '2', '6', '3')
#define RIFFINFO_I263	mmioFOURCC('I', '2', '6', '3')
#define RIFFINFO_i263	mmioFOURCC('i', '2', '6', '3')
#define RIFFINFO_U263	mmioFOURCC('U', '2', '6', '3')
#define RIFFINFO_u263	mmioFOURCC('u', '2', '6', '3')

#define RIFFINFO_MJPG	mmioFOURCC('M', 'J', 'P', 'G')     /* Motion JPEG */
#define RIFFINFO_mjpg	mmioFOURCC('m', 'j', 'p', 'g')

#define RIFFINFO_HFYU	mmioFOURCC('H', 'F', 'Y', 'U')

#define RIFFINFO_CVID	mmioFOURCC('C', 'V', 'I', 'D')     /* Cinepak (Radius) */
#define RIFFINFO_cvid	mmioFOURCC('c', 'v', 'i', 'd')     /* Cinepak (Radius) */

/* Sony Digital Video */
#define RIFFINFO_DVSD	mmioFOURCC('D', 'V', 'S', 'D')
/* standard DV format */
#define RIFFINFO_dvsd	mmioFOURCC('d', 'v', 's', 'd')
/* High Definition DV */
#define RIFFINFO_dvhd	mmioFOURCC('d', 'v', 'h', 'd')
/* Long Play DV */
#define RIFFINFO_dvsl	mmioFOURCC('d', 'v', 's', 'l')
/*
 * Note: DVSD is incorrect FOURCC - will not be accepted by codec - but
 * some files contains this fcc - avifile fallbacks to use dvsd
 */

/* Ati codecs */
#define RIFFINFO_VCR2	mmioFOURCC('V', 'C', 'R', '2')
#define RIFFINFO_VCR1	mmioFOURCC('V', 'C', 'R', '1')
#define RIFFINFO_VYUY	mmioFOURCC('V', 'Y', 'U', 'Y')
#define RIFFINFO_YVU9	mmioFOURCC('I', 'Y', 'U', '9')

/* Asus codecs */
#define RIFFINFO_ASV1	mmioFOURCC('A', 'S', 'V', '1')
#define RIFFINFO_ASV2	mmioFOURCC('A', 'S', 'V', '2')

/* let's give some FCC to Xvid */
#define RIFFINFO_XVID   mmioFOURCC('X', 'V', 'I', 'D')
#define RIFFINFO_xvid   mmioFOURCC('x', 'v', 'i', 'd')
#define RIFFINFO_XviD   mmioFOURCC('X', 'v', 'i', 'D')

/* Microsoft video */
#define RIFFINFO_CRAM	mmioFOURCC('C', 'R', 'A', 'M')
#define RIFFINFO_cram	mmioFOURCC('c', 'r', 'a', 'm')
#define RIFFINFO_MSVC	mmioFOURCC('M', 'S', 'V', 'C')

#define RIFFINFO_MSZH	mmioFOURCC('M', 'S', 'Z', 'H')

#define RIFFINFO_ZLIB	mmioFOURCC('Z', 'L', 'I', 'B')

#define RIFFINFO_TM20	mmioFOURCC('T', 'M', '2', '0')

#define RIFFINFO_M261	mmioFOURCC('M', '2', '6', '1')
#define RIFFINFO_m261	mmioFOURCC('m', '2', '6', '1')

#define RIFFINFO_TSCC	mmioFOURCC('T', 'S', 'C', 'C')
#define RIFFINFO_tscc	mmioFOURCC('t', 's', 'c', 'c')

#define RIFFINFO_PIM1	mmioFOURCC('P', 'I', 'M', '1')

#define RIFFINFO_YUV 	mmioFOURCC('Y', 'U', 'V', ' ')

/* YUNV V422 */
#define RIFFINFO_YUY2	mmioFOURCC('Y', 'U', 'Y', '2')

#define RIFFINFO_YV12	mmioFOURCC('Y', 'V', '1', '2')      /* Planar mode: Y + V + U (3 planes) */
/* just swaped V U planes */
#define RIFFINFO_I420	mmioFOURCC('I', '4', '2', '0')      /* Planar mode: Y + U + V (3 planes) */
/* same as I420 */
#define RIFFINFO_IYUV	mmioFOURCC('I', 'Y', 'U', 'V')      /* Planar mode: Y + U + V (3 planes) */
#define RIFFINFO_UYVY	mmioFOURCC('U', 'Y', 'V', 'Y')      /* Packed mode: U0+Y0+V0+Y1 (1 plane) */
#define RIFFINFO_YVYU	mmioFOURCC('Y', 'V', 'Y', 'U')      /* Packed mode: Y0+V0+Y1+U0 (1 plane) */
#define RIFFINFO_Y800	mmioFOURCC('Y', '8', '0', '0')      /* 8bit Y plane for monochrome images */


/* INFO LIST CHUNKS (from the Multimedia Programmer's Reference plus new ones) */
#define RIFFINFO_IARL	mmioFOURCC ('I', 'A', 'R', 'L')     /* Archival location */
#define RIFFINFO_IART	mmioFOURCC ('I', 'A', 'R', 'T')     /* Artist */
#define RIFFINFO_ICMS	mmioFOURCC ('I', 'C', 'M', 'S')     /* Commissioned */
#define RIFFINFO_ICMT	mmioFOURCC ('I', 'C', 'M', 'T')     /* Comments	*/
#define RIFFINFO_ICOP	mmioFOURCC ('I', 'C', 'O', 'P')     /* Copyright */
#define RIFFINFO_ICRD	mmioFOURCC ('I', 'C', 'R', 'D')     /* Creation date of subject	*/
#define RIFFINFO_ICRP	mmioFOURCC ('I', 'C', 'R', 'P')     /* Cropped */
#define RIFFINFO_IDIM	mmioFOURCC ('I', 'D', 'I', 'M')     /* Dimensions */
#define RIFFINFO_IDPI	mmioFOURCC ('I', 'D', 'P', 'I')     /* Dots per inch */
#define RIFFINFO_IENG	mmioFOURCC ('I', 'E', 'N', 'G')     /* Engineer	*/
#define RIFFINFO_IGNR	mmioFOURCC ('I', 'G', 'N', 'R')     /* Genre */
#define RIFFINFO_IKEY	mmioFOURCC ('I', 'K', 'E', 'Y')     /* Keywords	*/
#define RIFFINFO_ILGT	mmioFOURCC ('I', 'L', 'G', 'T')     /* Lightness settings */
#define RIFFINFO_IMED	mmioFOURCC ('I', 'M', 'E', 'D')     /* Medium */
#define RIFFINFO_INAM	mmioFOURCC ('I', 'N', 'A', 'M')     /* Name of subject */
#define RIFFINFO_IPLT	mmioFOURCC ('I', 'P', 'L', 'T')     /* Palette Settings. No. of colors requested */
#define RIFFINFO_IPRD	mmioFOURCC ('I', 'P', 'R', 'D')     /* Product */
#define RIFFINFO_ISBJ	mmioFOURCC ('I', 'S', 'B', 'J')     /* Subject description */
#define RIFFINFO_ISFT	mmioFOURCC ('I', 'S', 'F', 'T')     /* Software. Name of package used to create file */
#define RIFFINFO_ISHP	mmioFOURCC ('I', 'S', 'H', 'P')     /* Sharpness */
#define RIFFINFO_ISRC	mmioFOURCC ('I', 'S', 'R', 'C')     /* Source */
#define RIFFINFO_ISRF	mmioFOURCC ('I', 'S', 'R', 'F')     /* Source Form. ie slide, paper */
#define RIFFINFO_ITCH	mmioFOURCC ('I', 'T', 'C', 'H')     /* Technician who digitized the subject. */

/* New INFO Chunks as of August 30, 1993: */
#define RIFFINFO_ISMP	mmioFOURCC ('I', 'S', 'M', 'P')     /* SMPTE time code */
/* ISMP: SMPTE time code of digitization start point expressed as a NULL terminated
   text string "HH:MM:SS:FF". If performing MCI capture in AVICAP, this
   chunk will be automatically set based on the MCI start time.
*/
#define RIFFINFO_IDIT	mmioFOURCC ('I', 'D', 'I', 'T')     /* Digitization Time */
/* IDIT: "Digitization Time" Specifies the time and date that the digitization
   commenced. The digitization time is contained in an ASCII string which
   contains exactly 26 characters and is in the format
   "Wed Jan 02 02:03:55 1990\n\0".
   The ctime(), asctime(), functions can be used to create strings
   in this format. This chunk is automatically added to the capture
   file based on the current system time at the moment capture is initiated.
*/

/*Template line for new additions*/
/*#define RIFFINFO_I	  mmioFOURCC ('I', '', '', '')	      */

#define RIFFINFO_MPG1	mmioFOURCC ('M', 'P', 'G', '1')
#define RIFFINFO_MPG2	mmioFOURCC ('M', 'P', 'G', '2')

/* backward compatibile defines  - RIFFINFO should be now prefered name */
#define fccMP4S		RIFFINFO_MP4S
#define fccmp4s		RIFFINFO_mp4s
#define fccDIVX		RIFFINFO_DIVX
#define fccdivx		RIFFINFO_divx
#define fccDX50		RIFFINFO_DX50
#define fccdx50		RIFFINFO_dx50
#define fccDIV1		RIFFINFO_DIV1
#define fccdiv1		RIFFINFO_div1
#define fccDIV2		RIFFINFO_DIV2
#define fccdiv2		RIFFINFO_div2
#define fccDIV3		RIFFINFO_DIV3
#define fccdiv3		RIFFINFO_div3
#define fccDIV4		RIFFINFO_DIV4
#define fccdiv4		RIFFINFO_div4
#define fccDIV5		RIFFINFO_DIV5
#define fccdiv5		RIFFINFO_div5
#define fccDIV6		RIFFINFO_DIV6
#define fccdiv6		RIFFINFO_div6
#define fccMP41		RIFFINFO_MP41
#define fccmp41		RIFFINFO_mp41
#define fccMP43		RIFFINFO_MP43
#define fccmp43		RIFFINFO_mp43
#define fccMP42		RIFFINFO_MP42
#define fccmp42		RIFFINFO_mp42
#define fccMPG4		RIFFINFO_MPG4
#define fccmpg4		RIFFINFO_mpg4
#define fccWMV1		RIFFINFO_WMV1
#define fccwmv1		RIFFINFO_wmv1
#define fccWMV2		RIFFINFO_WMV2
#define fccwmv2		RIFFINFO_wmv2
#define fccMWV1		RIFFINFO_MWV1
#define fccAP41		RIFFINFO_AP41
#define fccap41		RIFFINFO_ap41
#define fccAP42		RIFFINFO_AP42
#define fccap42		RIFFINFO_ap42
#define fccIV31		RIFFINFO_IV31
#define fcciv31		RIFFINFO_iv31
#define fccIV32		RIFFINFO_IV32
#define fcciv32		RIFFINFO_iv32
#define fccIV41		RIFFINFO_IV41
#define fcciv41		RIFFINFO_iv41
#define fccIV50		RIFFINFO_IV50
#define fcciv50		RIFFINFO_iv50
#define fccH263		RIFFINFO_H263
#define fcch263		RIFFINFO_h263
#define fccI263		RIFFINFO_I263
#define fcci263		RIFFINFO_i263
#define fccU263		RIFFINFO_U263
#define fccu263		RIFFINFO_u263
#define fccMJPG		RIFFINFO_MJPG
#define fccmjpg		RIFFINFO_mjpg
#define fccHFYU		RIFFINFO_HFYU
#define fccXVID		RIFFINFO_XVID
#define fccCVID		RIFFINFO_CVID
#define fcccvid		RIFFINFO_cvid
#define fccdvsd		RIFFINFO_dvsd
#define fccdvsl		RIFFINFO_dvsl
#define fccdvhd		RIFFINFO_dvhd
#define fccDVSD		RIFFINFO_DVSD
#define fccVCR2		RIFFINFO_VCR2
#define fccVCR1		RIFFINFO_VCR1
#define fccVYUY		RIFFINFO_VYUY
#define fccYVU9		RIFFINFO_YVU9
#define fccASV1		RIFFINFO_ASV1
#define fccASV2		RIFFINFO_ASV2
#define fcccram		RIFFINFO_cram
#define fccCRAM		RIFFINFO_CRAM
#define fccMSVC		RIFFINFO_MSVC
#define fccMSZH		RIFFINFO_MSZH
#define fccZLIB		RIFFINFO_ZLIB
#define fccTM20		RIFFINFO_TM20
#define fccVP31		RIFFINFO_VP31
#define fccvp31		RIFFINFO_vp31
#define fccVP30		RIFFINFO_VP30
#define fccvp30		RIFFINFO_vp30
#define fccM261		RIFFINFO_M261
#define fccm261		RIFFINFO_m261
#define fccTSCC		RIFFINFO_TSCC
#define fcctscc		RIFFINFO_tscc
#define fccPIM1		RIFFINFO_PIM1
#define fccYUV		RIFFINFO_YUV
#define fccYUY2		RIFFINFO_YUY2
#define fccYV12		RIFFINFO_YV12
#define fccI420		RIFFINFO_I420
#define fccIYUV		RIFFINFO_IYUV
#define fccUYVY		RIFFINFO_UYVY
#define fccYVYU		RIFFINFO_YVYU
#define fccY800		RIFFINFO_Y800

/* interleaved - iavs */

#endif // AVM_FOURCC_H
