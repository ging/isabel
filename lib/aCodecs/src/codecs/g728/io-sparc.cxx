/*************************************************************************/
/*                                                                       */
/*                            LD-CELP  G.728                             */
/*                                                                       */
/*    Low-Delay Code Excitation Linear Prediction speech compression.    */
/*                                                                       */
/*    Code edited by Michael Concannon.                                  */
/*    Based on code written by Alex Zatsman, Analog Devices 1993         */
/*                                                                       */
/*    Reestructuracion: Angel Fdez. Herrero. Octubre-2000                */
/*									 */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "prototyp.h"

/*const real rscale=0.125;  * scaling factor for input */

#ifdef CODER

static FILE *ifd;   /* input file */
static FILE *oxfd;  /* output file (codebook indices) */

static char *ifname;
static char *oxfname;

void init_input(char *iname, char *oxname)
{
    ifname=iname;
    oxfname=oxname;

    ifd=fopen(ifname, "rb");
    if(ifd==NULL)
    {
	fprintf(stdout, "Can't open \"%s\"\n", ifname);
	exit(1);
    }

    oxfd=fopen(oxfname, "wb");
    if(oxfd==NULL)
    {
	fprintf(stdout, "Can't open \"%s\"\n", oxfname);
	exit(1);
    }
}

void close_input(void)
{
    if(fclose(ifd))
    {
	fprintf(stdout, "Can't close \"%s\"\n", ifname);
	exit(1);
    }

    if(fclose(oxfd))
    {
	fprintf(stdout, "Can't close \"%s\"\n", oxfname);
	exit(1);
    }
}

void put_index(int ix)
{
    short sx=ix;

    char b[2];

    *(short *)b=sx;
#ifdef ENDIAN
    { char t; t=b[0]; b[0]=b[1]; b[1]=t; }
#endif
    if(fwrite((void *)b, 1, 2, oxfd)!=2)
    {
	fprintf(stdout, "Can't write to \"%s\"\n", oxfname);
	exit(1);
    }
}

#if 1
int read_sound_buffer(int n, short *buffer)
{
    int i, c=0;
    char b[2];

    for(i=0; i<n; i++)
    {
      if(fread((void *)b, 1, 2, ifd)!=2) break;

#ifdef ENDIAN
      { char t; t=b[0]; b[0]=b[1]; b[1]=t; }
#endif
      buffer[c++]=*(short *)b;
    }
    return c;
}
#else
int read_sound_buffer(int n, real *buffer)
{
    int i, c=0;
    char b[2];

    for(i=0; i<n; i++)
    {
      if(fread((void *)b, 1, 2, ifd)!=2) break;

#ifdef ENDIAN
      { char t; t=b[0]; b[0]=b[1]; b[1]=t; }
#endif
      {
	short s=*(short *)b;
        buffer[c++]=rscale*(real)s;
      }
    }
    return c;
}
#endif

#endif /*CODER*/

#ifdef DECODER

static FILE *ixfd;  /* input file (codebook indices) */
static FILE *ofd;   /* output file */

static char *ixfname;
static char *ofname;

void init_output(char *ixname, char *oname)
{
    ixfname=ixname;
    ofname=oname;

    ofd=fopen(ofname, "wb");
    if(ofd==NULL)
    {
	fprintf(stdout, "Can't open \"%s\"\n", ofname);
	exit(1);
    }
#ifndef TLOOP
    ixfd=fopen(ixfname, "rb");
    if(ixfd==NULL)
    {
	fprintf(stdout, "Can't open \"%s\"\n", ixfname);
	exit(1);
    }
#endif
}

void close_output(void)
{
#ifndef TLOOP
    if(fclose(ixfd))
    {
	fprintf(stdout, "Can't close \"%s\"\n", ixfname);
	exit(1);
    }
#endif
    if(fclose(ofd))
    {
	fprintf(stdout, "Can't close \"%s\"\n", ofname);
	exit(1);
    }
}

int get_index(void)
{
    short sx;
    char b[2];

    if(fread((void *)b, 1, 2, ixfd)!=2) return -1;
#ifdef ENDIAN
    { char t; t=b[0]; b[0]=b[1]; b[1]=t; }
#endif
    sx=*(short *)b;

    return (int)sx;
}

#if 1
int write_sound_buffer(int n, short *buffer)
{
    int i;
    char b[2];
	    
    for(i=0; i<n; i++)
    {
	*(short *)b=buffer[i];
#ifdef ENDIAN
	{ char t; t=b[0]; b[0]=b[1]; b[1]=t; }
#endif
	if(fwrite((void *)b, 1, 2, ofd)!=2)
	{
	  fprintf(stdout, "Can't write to \"%s\"\n", ofname);
	  exit(1);
	}
    }
    return n;
}
#else
int write_sound_buffer(int n, real *buffer)
{
    int i;
    char b[2];
	    
    for(i=0; i<n; i++)
    {
        real xx=buffer[i]/rscale;

        if(xx>0.0)
            if(xx>32767.0) xx=32767.0;
            else xx+=0.5;
        else
            if(xx<-32768.0) xx=-32768.0;
            else xx-=0.5;

	*(short *)b=(short)xx;
#ifdef ENDIAN
	{ char t; t=b[0]; b[0]=b[1]; b[1]=t; }
#endif
	if(fwrite((void *)b, 1, 2, ofd)!=2)
	{
	  fprintf(stdout, "Can't write to \"%s\"\n", ofname);
	  exit(1);
	}
    }
    return n;
}
#endif

#endif /*DECODER*/
