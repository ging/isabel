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
/*
        hpscanpbm
        User-level SCSI HP Scanjet driver
        Written by David Etherton, etherton@netcom.com
        HPUX Port by John Fehr, fehr@ninja.aes.mb.doe.ca
	Little hacks by Steve Enns  ennss@nhrisv.nhrc.sk.doe.ca
	Copyright 1995, David Etherton
        Released under the terms of the GPL.
        *NO WARRANTY*

        Thanks to Les Johnson (les@ulysses.homer.att.com) for
        suggesting some Linux documentation clarifications.

        THIS IS ALPHA SOFTWARE.  IT WORKS FINE FOR ME.
        IT MIGHT NOT WORK FOR YOU.  You'll need to be
        familiar with rebuilding kernels to take
        advantage of this program (under older versions
	of Linux at least)

	Version 0.3alpha: dpi, save/restore tweaks, documentation
		update for newer linux kernels.

        Version 0.2alpha: HPUX port, better documentation, more
                scanner adjustments.

        Version 0.1alpha: initial release
*/

/*
        Build with cc hpscanpbm.c -o hpscanpbm

        Use "hpscanpbm -help" for instructions.  Creates portable pitmap
        file as output.  Can easily be used in a pipeline or with a
        "real" scanning application.  To use from an application,
        fork, redirect stdout and stderr, then exec this program with the
        -quiet switch.  On termination, if you got anything from
        stderr, issue it as a diagnostic; otherwise, interpret
        stdout as a PBM file.

*** Linux Notes ***

        THIS IS NOT A DEVICE DRIVER.  Your Scanjet must be connected
        to a SCSI adapter that is supported by Linux.  While the chipsets
        used on the boards supplied with the IIc and IIcx (NCR 53C400 and
        53C800) do seem to have some support under Linux, these exact
        board configurations are not supported yet.  I've had good luck
        with a Future Domain TMC-850.  Performance of this 8bit card
        isn't amazing but it definitely works.

        This program uses the generic SCSI interface.  Therefore, you 
        must have generic scsi enabled in your kernel, and you should
        have a series of generic SCSI devices set up:
        
                /dev/sga        major 21, minor 0 (mknod /dev/sga c 21 0)
                /dev/sgb        major 21, minor 1 (mknod /dev/sgb c 21 1)
                :               :

        SCSI generic devices should be character devices, not block
        devices.  Furthermore, even devices that are already assigned
        to other names (like /dev/sda, /dev/sr0) also get a generic
        entry point.  This means that you need at least as many scsi
        generic device entries as you have scsi devices.

       	If you're paranoid, clear all permissions on all generic devices 
       	except the scanner.  Figuring out in advance which generic device 
       	ends up being your scanner can get a little tricky.  If you only
        have one SCSI bus, they're allocated in order of SCSI id.
        In my case, I've got two SCSI controllers, one for my
        internal stuff and one for my scanner; I've got three devices
        on the first bus so my scanner ends up being on /dev/sgd.
        (I've got the wrong kind of cable to hook everything onto
        a single bus).  Unless the program is given the "-quiet" or
        "-dev" options, it will tell you where it found the scanner.
        The scanner's device must have both read and write permission.

        If you're running anthing older than about 1.1.74, you'll probably 
	also need to patch /usr/src/linux/drivers/scsi/scsi.c
        near line 370.  In a switch statement containing the lines:
        (the first line is a bit different as of kernel 1.1.73)

                switch (type = scsi_result[0])
                {
                      case TYPE_TAPE :
                      case TYPE_DISK :
                      case TYPE_MOD :
                        ** add this: **
                      case 3 :
                        SDpnt->writeable = 1;
                        break;

        you'll need to add "case 3:" as indicated above; you'll also need
        to add the HP scanner to the scsi blacklist near the top of
        the file:

        static struct blist blacklist[] =
        {
                :       :         :
                : lots of entries :
                {"HP", "C1750A", "3226"},
                {"HP", "C2500A", "????"},
		{"HP", "C2570A", "3406"},
                {NULL, NULL, NULL}};
        }

        I've tested this under 1.1.49, 1.1.72, 1.1.73, and 1.1.94.
	That shouldn't matter too much, I hope.  

        The program is smart enough to autoprobe for your Scanjet
        and won't try, for example, to read your disk drive.

	Generic SCSI got broken in the late 1.1.70's and didn't
	get fixed again until the late .80's.  All of the patches
	listed above are in the current kernel (1.1.94) *except*
	for the "C2570A" blacklist entry which is for the Scanjet 3p.

*** HPUX Notes ***

        On the 700 series, use the following command:
        mknod /dev/scanner c 105 0x201<N>00 (where N is the scsi #)
        You might also want to do a 'chmod 666 /dev/scanner' to allow 
        everyone to access the scanner.

	Compile:   cc +O3 -Aa -D_HPUX_SOURCE hpscanpbm.c

        (Please direct HPUX questions to John)

*** Steve Enns/Synergrafix Consulting notes ***

	Code changes marked with **ENNS**

	I have added the following options:

	-xres, -yres   to specify the desired width,height of the
		scanned image in pixels.  xdpi,ydpi  will be adjusted
		to get the desired number of pixels for a given scan
		size.
	-s filename, -l filename   to save/load a file containing
		the other optional parameters.  These options are
		sensitive to option order on the command line - 
		i.e. specify -l first if you want to override some 
		settings from the file, and specify -s last if you
		want to save all the options from the current command
		line to the parameter file.  

	e.g.
	
	hpscanpbm -xres 1280 -yres 1280 -cm -width 12 -height 12 -format color -s cdcover.hps > sample.ppm

	will scan an image and save the parameters to  cdcover.hps  which can
	be used later as:

	hpscanpbm -l cdcover.hps > sample2.ppm

	suitable for scanning audio CD covers ;-)

*/


#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _HPUX_SOURCE
#include <sys/scsi.h>
#include <fcntl.h>
#elif defined Solaris
#include <sys/scsi/generic/commands.h>
#include <sys/scsi/impl/uscsi.h>
#else
#include "/usr/src/linux/include/scsi/sg.h"
#endif

/* this might eventually go into a header file */
#define THRESHOLDED     0
#define DITHERED        1
#define GREYSCALE       2
#define TRUECOLOR       3

/**ENNS  added xres,yres **/

typedef struct {
    int xdpi, ydpi;
	int xres, yres; /* in pixels */
    int x, y, width,height; /* in decipoints, 1/720in */
    int format;
    double brightness, contrast;
}       ScanSettings;

/* default is 100dpi binarized 8.5x11 image */
ScanSettings settings = { 100, 100, 0, 0, 0, 0, 6120, 7920, THRESHOLDED, 0.0, 0.0 };

int min(int a,int b) { return a<b? a : b; }

static int quiet;

#if 0
static void dump(void *buffer,int count)
{
    unsigned char *c = buffer;
    int i,j;
    for (i=0; i<count; i+=16) {
        printf("%04x: ",i);
        for (j=i; j<i+16; j++)
            if (j < count)
                printf("%02x ",c[j]);
            else
                printf("   ");
        for (j=i; j<i+16; j++)
            if (j < count)
                putchar(isprint(c[j])?c[j]:'.');
            else
                putchar(' ');
        putchar('\n');
    }
}
#endif

#ifdef Linux    /* Linux version */
        
int sg_xfer(int fid,unsigned char *src,int len,unsigned char *dest,
            int reply_len)
{
    int count, result;
    static struct sg_header hdr;
    static char buf[4096 + sizeof(struct sg_header)];
    hdr.pack_len = sizeof(hdr) + len;
    hdr.reply_len = sizeof(hdr) + reply_len;
    memcpy(buf, &hdr, sizeof(hdr));
    memcpy(buf + sizeof(hdr), src, len);
    if (write(fid, buf, hdr.pack_len) == -1) {
        perror("sg_xfer(write)");
        return -1;
    }
    if ((count = read(fid,buf,hdr.reply_len)) == -1) {
        perror("sg_xfer(read)");
        return -1;
    }
    result = ((struct sg_header*)buf)->result;
    if (result) {
        fprintf(stderr,"sg_xfer: result is %d\n",result);
    }
    if (count > sizeof(struct sg_header)) {
        count -= sizeof(struct sg_header);
        memcpy(dest,buf + sizeof(hdr),count);
    }
    else
        count = 0;
    return count;
}

#elif defined _HPUX_SOURCE   /* HPUX version */

int sg_xfer(int fid,unsigned char *src,int len,unsigned char *dest,
            int reply_len,int flags)
{
    int count, result;
    struct sctl_io hdr;
    static char buf[2048];
    memset(&hdr,0,sizeof(struct sctl_io));
    memcpy(hdr.cdb,src,len);
    hdr.flags=flags;
    hdr.cdb_length=len;
    hdr.data=dest;
    hdr.data_length=reply_len;
    hdr.max_msecs=10000;    /* ten seconds for command */
    if (ioctl(fid,SIOC_IO,&hdr) == -1)
    {
        perror("sg_xfer(ioctl)");
        return -1;
    }
    result=hdr.cdb_status;
    count=hdr.data_xfer;

    if (result) {
        fprintf(stderr,"sg_xfer: result is %d\n",result);
    }
    return count;
}
#endif

int inquire_device(int fid,char *mfg,char *model)
{
#ifndef Solaris
    static unsigned char inquiry[] = { 0x12,0,0,0,255,0 };
    unsigned char reply[256];
#ifdef Linux
    if (sg_xfer(fid,inquiry,sizeof(inquiry),reply,sizeof(reply)) < 16)
        return -1;
#elif defined _HPUX_SOURCE
    if (sg_xfer(fid,inquiry,sizeof(inquiry),reply,sizeof(reply),SCTL_READ) < 16)
        return -1;
#endif
    memcpy(mfg,reply + 8, 8);
    memcpy(model,reply + 16, 16);
    return reply[0];
#else
    strcpy(mfg,"HP");
    strcpy(model,"C2502A");
    return 3;
#endif
}

#ifndef Solaris
int write_data(int fid,char *data,int len)
{
    char write_6[4096];
    memset(write_6,0,6);
    write_6[0] = 0xa;
    write_6[2] = len >> 16;
    write_6[3] = len >> 8;
    write_6[4] = len;
    memcpy(write_6 + 6, data, len);
#ifdef Linux
    if (sg_xfer(fid,write_6,6 + len,0,0) < 0)
#elif defined _HPUX_SOURCE
        if (sg_xfer(fid,write_6,6,write_6 + 6,sizeof(write_6) - 6,0) < 0)
#endif
            return -1;
    return 0;
}

int read_data(int fid,char *dest,int len)
{
    static unsigned char read_6[6] = { 8,0,0,0,0,0 };
    read_6[2] = len >> 16;
    read_6[3] = len >> 8;
    read_6[4] = len;
#ifdef Linux
    if ((len=sg_xfer(fid,read_6,6,dest,len)) < 0)
#elif defined _HPUX_SOURCE
        if ((len=sg_xfer(fid,read_6,6,dest,len,SCTL_READ))<0)
#endif
            return -1;
    /* dump(dest,len); */
    return len;
}
#else

int
write_data( int fd, char *data, int len)
{
    struct uscsi_cmd scmd;
    char buf[255];
    memset( &scmd, 0, sizeof( scmd));
    memset( buf, 0, sizeof(buf));
    buf[0] = SCMD_WRITE;
    buf[2] = (len >> 16) & 0xff;
    buf[3] = (len >> 8) & 0xff;
    buf[4] = (len >> 0) & 0xff;

    scmd.uscsi_flags = USCSI_WRITE | USCSI_DIAGNOSE | USCSI_SILENT;
    scmd.uscsi_timeout = 100;
    scmd.uscsi_bufaddr = data;
    scmd.uscsi_buflen = len;
    scmd.uscsi_cdb = buf;
    scmd.uscsi_cdblen = CDB_GROUP0;
    ioctl( fd, USCSICMD, &scmd);
    return 0;
}
int
read_data( int fd, char *data, int len)
{
    struct uscsi_cmd scmd;
    char buf[255];
    memset( &scmd, 0, sizeof( scmd));
    memset( buf, 0, sizeof(buf));
    buf[0] = SCMD_READ;
    buf[2] = (len >> 16) & 0xff;
    buf[3] = (len >> 8) & 0xff;
    buf[4] = (len >> 0) & 0xff;

    scmd.uscsi_flags = USCSI_READ | USCSI_DIAGNOSE | USCSI_SILENT;
    scmd.uscsi_timeout = 100;
    scmd.uscsi_bufaddr = data;
    scmd.uscsi_buflen = len;
    scmd.uscsi_cdb = buf;
    scmd.uscsi_cdblen = CDB_GROUP0;
    ioctl( fd, USCSICMD, &scmd);
    return scmd.uscsi_buflen;
}

#endif

int write_string(int fid,char *format,...)
{
    va_list args;
    char buf[1024];
    va_start(args, format);
    vsprintf(buf,format,args);
    write_data(fid,buf,strlen(buf));
    return 0;
}

int inquire(int fid,char *inq)
{
    char reply[128];
    write_string(fid,inq);
    if (read_data(fid,reply,sizeof(reply))) {
        /* dump(reply,128); */
        /* this should be less haphazard */
        return atoi(strchr(reply,'d') + 1);
    }
    else
        return -1;
}

int norm8(double value)
{
	if (value < -1.0)
		value = -1.0;
	else if (value > 1.0)
		value = 1.0;
	return (int)(value * 127.0);
}

void do_scan(char *dest,int fid,ScanSettings* ss)
{
    char buf[4000];
    int len, mode;
    FILE *f = dest? fopen(dest,"w") : stdout;
    int pixels_per_line, bytes_per_line, lines, pixels;

    /* reset scanner; returns all paramters to defaults */
    write_string(fid,"\033E");

    /* set resolution, in DPI */
    write_string(fid,"\033*a%dR",ss->xdpi);
    write_string(fid,"\033*a%dS",ss->ydpi);

    /* set scan extents, in 1/720'ths of an inch */
    write_string(fid,"\033*a%dX",ss->x);
    write_string(fid,"\033*a%dY",ss->y);
    write_string(fid,"\033*a%dP",ss->width);
    write_string(fid,"\033*a%dQ",ss->height);

    /* (the original scanjet only allows -1, 0, or 1) */
    write_string(fid,"\033*a%dL",norm8(ss->brightness));
    write_string(fid,"\033*a%dK",norm8(ss->contrast));

    /* greyscale and color modes seems to need to have their
       outputs inverted; thresholded and dithered modes
       do not (the default) */
    if (ss->format > DITHERED)
        write_string(fid,"\033*a1I");
    mode = 4;
    /* data format is thresholded by default; pixel format
       is 8 pixels/byte */
    if (ss->format == DITHERED) {
        /* data format: dithered */
        write_string(fid,"\033*a3T");
    }
    else if (ss->format == GREYSCALE) {
        /* data format is greyscale, pixel format is
           one pixel/byte */
        write_string(fid,"\033*a4T");
        write_string(fid,"\033*a8G");
        mode = 5;
    }
    else if (ss->format == TRUECOLOR) {
        /* data format is 24bit color, pixel format
           is one pixel/three bytes */
        write_string(fid,"\033*a5T");
        write_string(fid,"\033*a24G");
        mode = 6;
    }
    /* inquire resulting size of image after setting it up */
    pixels_per_line = inquire(fid,"\033*s1024E");
    bytes_per_line = inquire(fid,"\033*s1025E");
    lines = inquire(fid,"\033*s1026E");

    if (!quiet) {
        fprintf(stderr,"%d pixels per line, %d bytes, %d lines high\n",
                pixels_per_line, bytes_per_line, lines);
		fprintf(stderr,"xdpi %d, ydpi %d\n",ss->xdpi,ss->ydpi);
    }
#if 0
    fprintf(f,"P%d\n# created by hpscanpbm\n%d %d\n",
            mode,pixels_per_line,lines);
#else
    fprintf(f,"P%d %d %d ",
            mode,pixels_per_line,lines);
#endif
    if (mode > 4)
        fprintf(f,"255\n");
    pixels = bytes_per_line * lines;

    write_string(fid,"\033*f0S");           /* begin scan! */

    /* consume all data */
    while (pixels && (len = read_data(fid,buf,min(sizeof(buf),pixels)))) {
        pixels -= len;
        fwrite(buf,len,1,f);
    }
    if (dest)
        fclose(f);
}

int is_hp_scanner(char *devname)
{
    int fd = open(devname, O_RDWR);
    if (fd != -1) {
        char mfr[8], model[16];
        int type = inquire_device(fd,mfr,model);
        if (type == 3 && !strncmp(mfr,"HP",2))
            return fd;
        close(fd);
    }
    return -1;
}

int find_hp_scanner(void)
{
    char buf[16];
    int i, fd;
#ifdef _HPUX_SOURCE
    sprintf(buf,"/dev/scanner");
    if ((fd = is_hp_scanner(buf)) != -1) {
        if (!quiet)
            fprintf(stderr,
                    "HP Scanjet found on '%s'\n",buf);
        return fd;
    }
#elif defined Linux
    for (i='a'; i <= 'z'; i++) {
        sprintf(buf,"/dev/sg%c",i);
        if ((fd = is_hp_scanner(buf)) != -1) {
            if (!quiet)
                fprintf(stderr,
                        "HP Scanjet found on '%s'\n",buf);
            return fd;
        }
    }
#elif defined Solaris
    sprintf(buf,"/dev/isasg");
    if ((fd = is_hp_scanner(buf)) != -1) {
        if (!quiet)
            fprintf(stderr,
                    "HP Scanjet found on '%s'\n",buf);
        return fd;
    }
#endif
    return -1;
}
/** ENNS  Added loadsettings and savesettings **/

int loadsettings(char *filename) {
    FILE *fin;
    if ((fin=fopen(filename,"rb"))==NULL) {
        fprintf(stderr,"Can't open settings file %s for read.\n",filename);
        exit(1);
    }
    if (fread(&settings,sizeof(settings),1,fin)!=1) {
        fprintf(stderr,"Can't read settings file %s\n",filename);
        exit(1);
    }
    return 0;
}


int savesettings(char *filename) {
    FILE *fin;
    if ((fin=fopen(filename,"wb"))==NULL) {
        fprintf(stderr,"Can't open settings file %s for write.\n",filename);
        exit(1);
    }
    if (fwrite(&settings,sizeof(settings),1,fin)!=1) {
        fprintf(stderr,"Can't write settings file %s\n",filename);
        exit(1);
    }
    return 0;
}


#define nextargi (--argc,atoi(*++argv))
#define nextargf (--argc,atof(*++argv))
#define nextargs (--argc,*++argv)

/** ENNS  Added xres,yres,l,s options **/

void main(int argc,char **argv)
{
    double posfactor = 720.0;
    char *devname = (char*)0, *outname = (char*)0;
    int fd;

    while (--argc && **++argv=='-') {
        if (!strcmp(argv[0],"-l") || !strcmp(argv[0],"-load"))
            loadsettings(nextargs);
        else if (!strcmp(argv[0],"-dpi"))
            settings.xdpi = settings.ydpi = nextargi;
        else if (!strcmp(argv[0],"-xdpi"))
            settings.xdpi = nextargi;
        else if (!strcmp(argv[0],"-ydpi"))
            settings.ydpi = nextargi;
        else if (!strcmp(argv[0],"-xres"))
            settings.xres = nextargi;
        else if (!strcmp(argv[0],"-yres"))
            settings.yres = nextargi;
        else if (!strcmp(argv[0],"-in")) 
            posfactor = 720.0;
        else if (!strcmp(argv[0],"-cm")) 
            posfactor = 720.0 / 2.54;
        else if (!strcmp(argv[0],"-mm")) 
            posfactor = 720.0 / 25.4; 
        else if (!strcmp(argv[0],"-x"))
            settings.x = posfactor * nextargf;
        else if (!strcmp(argv[0],"-y"))
            settings.y = posfactor * nextargf;
        else if (!strcmp(argv[0],"-width"))
            settings.width = posfactor * nextargf;
        else if (!strcmp(argv[0],"-height"))
            settings.height = posfactor * nextargf;
        else if (!strcmp(argv[0],"-bright"))
            settings.brightness = nextargf;
        else if (!strcmp(argv[0],"-cont"))
            settings.contrast = nextargf;
        else if (!strcmp(argv[0],"-dev"))
            devname = nextargs;
        else if (!strcmp(argv[0],"-o") || !strcmp(argv[0],"-out"))
            outname = nextargs;
        else if (!strcmp(argv[0],"-q") || !strcmp(argv[0],"-quiet"))
            quiet = 1;
        else if (!strcmp(argv[0],"-format")) {
            char *mn = nextargs;
            if (!strcmp(mn,"thresholded"))
                settings.format = THRESHOLDED;
            else if (!strcmp(mn,"dithered"))
                settings.format = DITHERED;
            else if (!strcmp(mn,"greyscale") || 
                     !strcmp(mn,"grayscale"))
                settings.format = GREYSCALE;
            else if (!strcmp(mn,"color"))
                settings.format = TRUECOLOR;
            else {
                fprintf(stderr,"invalid mode; ");
                goto oops;
            }
        }
        else if (!strcmp(argv[0],"-s") || !strcmp(argv[0],"-save")) 
            savesettings(nextargs);
        else {
            if (strcmp(argv[0],"-help"))
                fprintf(stderr,"unknown option '%s'; ",argv[0]);	oops:
                fprintf(stderr,"valid options are:\n"
                        "-dpi    sets x & y scan resolution in dots per inch\n"
                        "-xdpi   sets x scan resolution only (default 100)\n"
                        "-ydpi   sets y scan resolution only (default 100)\n"
                        "-xres   sets desired number of x-pixels on output image\n"
                        "-yres   sets desired number of y-pixels on output image\n"
                        "-in     specify scan region in inches (default)\n"
                        "-cm     specify scan region in centimeters\n"
                        "-mm     specify scan region in millimeters\n"
                        "-x      specify upper left x of scan region (default 0.0)\n"
                        "-y      specify upper left y of scan region (default 0.0)\n"
                        "-width  specify width of scan region (default 8.5in)\n"
                        "-height specify height of scan region (default 11.0in)\n"
                        "-bright specify brightness adjustment (-1.0 to 1.0, default 0.0)\n"
                        "-cont   specify contrast adjustment (-1.0 to 1.0, default 0.0)\n"
                        "-dev    scsi generic device of scanner (default autoprobe)\n"
                        "-o,-out output file (default stdout)\n"
                        "-quiet  turn off informational messages\n"
                        "-load   read parameter settings from file\n"
                        "-save   save current parameter settings to file\n"
                        "-format pixel format: thresholded, dithered, greyscale, color\n"
                    );
                exit(1);
        }
    }
    if (argc)
        goto oops;
    if (!devname) {
        fd = find_hp_scanner();
        if (fd == -1) {
            fprintf(stderr,"No HP Scanjet found.\n");
            exit(2);
        }
    }
    else if ((fd = is_hp_scanner(devname)) == -1) {
        fprintf(stderr,"'%s' is not an HP Scanjet.\n",devname);
        exit(2);
    }

    /** ENNS  hack dpi for desired xres,yres */

    if (settings.xres != 0) 
        settings.xdpi=(settings.xres/(settings.width/720.0)+0.5);
    if (settings.yres != 0)
        settings.ydpi=(settings.yres/(settings.height/720.0)+0.5);

    do_scan(outname,fd,&settings);
    close(fd);
}

        
