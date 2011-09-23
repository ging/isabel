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
   This converts a P4-type PBM data-stream to GIF.
   
   rd@redleaf.bbs.no  (c) 2.3.96

   
   Patched by Santiago Pavon to solve problem parsing the pbm header.
   */


#include <stdio.h>
#include <stdlib.h>

/* lzw compression constants */

#define bits 2                 /* 2  bits for each pixel. black/white=2 */
#define firstsize bits+1       /* 3  size of first code */
#define clearcode 1<<bits      /* 4  value of code for clearing tables */
#define endcode (clearcode)+1  /* 5  value of code for ending bitstream */
#define mincode (endcode)+1    /* 6  min code value to use for compress */
#define maxcode (clearcode)<<1 /* 8  I'm not using this, whatever it's for */

/* compression vars */

int child[4096], sib[4096], shade[4096];
unsigned char buff[255];
int nextcode, codesize=firstsize, color, parent, son, bro, buffpos;

/* gif format vars */

int xres, yres;
int xpad;
int xsize;

void readpbmhead();
void writegifhead();
void lzwcompstream();
void writegiftail();
void clearall();
int  getcolor();
void buffercode();
int  calccs();
void writebuff();

int
main(int argc, char *argv[]) 
{
    readpbmhead();   /* get resolution and filetype. abort if wrong filetype */
    writegifhead();  /* write gif head */
    lzwcompstream(); /* read, lzw-compress, then write bitstream */
    writegiftail();  /* write gif trailer */

    return 0; // to shut lint off
}


void
readpbmhead() /* get resolution and filetype. abort if wrong filetype */
{
    char sig[2];
#if 0
    scanf("%2s %d %d", sig, &xres, &yres);
#else
    scanf("%2s\n", sig);
    if (!(sig[0]=='P' && sig[1]=='4')) {
        fprintf(stderr,"Sorry, can only handle P4-type PBM data.\n"); 
        exit(-1);
    }
    while (scanf("%d %d\n", &xres, &yres) == 0) {
       while (getchar()!='\n');
    }
#endif
    if( xres % 32 ) {
        xpad = ( xres + 32)/32;
        xpad *= 32;
        xpad -= xres;
    } else {
        xpad = 0;
    }
    xsize = xres + xpad;
    /* fprintf( stderr, "xres %d yres %d xpad %d xsize %d\n", xres, yres, xpad, xsize); */
}


void
writegifhead() /* write gif head */
{
    /* header */

    printf("GIF87a");

  /* logical screen descriptor */

    putchar(xres);putchar(xres>>8); /* xres */
    putchar(yres);putchar(yres>>8); /* yres */
    putchar(0xf0);                  /* misc */
    putchar(0x01);                  /* background color index */
    putchar(0x00);                  /* pixel aspect ratio not given */

    /* global color-table */

    putchar(0xff);putchar(0xff);putchar(0xff); /* white */
    putchar(0x00);putchar(0x00);putchar(0x00); /* black */

  /* image descriptor */

    putchar(0x2c);                  /* signature */
    putchar(0x00);putchar(0x00);    /* left position */
    putchar(0x00);putchar(0x00);    /* top position */
    putchar(xres);putchar(xres>>8); /* xres */
    putchar(yres);putchar(yres>>8); /* yres */
    putchar(0x00);                  /* misc */
}


void
writegiftail() /* write gif trailer */
{
    putchar (0x3b); /* end stream code */
}


void
lzwcompstream() /* read, lzw-compress, then write bitstream */

{
    putchar(bits);
    clearall();

    color=getcolor();
    parent=color;
    while((color=getcolor())!=EOF)
    {
        son=child[parent];
        if (son>0)
        {
            if (shade[son]==color) parent=son;
            else 
            {
                bro=son;
                do
                {
                    if (sib[bro]>0)
                    {
                        bro=sib[bro];
                        if (shade[bro]==color)
                        {
                            parent=bro;
                            break;
                        }}
                    else
                    {
                        sib[bro]=nextcode;
                        shade[nextcode]=color;
                        buffercode(parent);
                        parent=color;
                        nextcode++;
                        codesize=calccs(nextcode);
                        break;
                    }
                }
                while (1);
            }}
        else 
        {
            child[parent]=nextcode;
            shade[nextcode]=color;
            buffercode(parent);
            parent=color;
            nextcode++;
            codesize=calccs(nextcode);
        }
        if (nextcode==4096) clearall();
    }    
    buffercode(endcode);
    codesize=7; buffercode(0); /* flush last bits, if any, from wrkint to buff*/
    if (buffpos) writebuff();  /* write final part of buffer */
    putchar(0);                /* write block terminator */
} 


void
clearall() /* write clearcode and clear all vars/compression tables */
{
    int i;
    buffercode(clearcode);
    for (i=0; i<4096; i++) child[i]=sib[i]=shade[i]=0;
    nextcode=mincode; codesize=firstsize;
    color=parent=son=bro=0;
}


int
calccs(int code) /* return size of code */
{
    int len=1;
    code--;
    while (code>>=1) len++;
    return len;
}


int
getcolor() /* get value of next color (0 or 1) */
{
    static int wrkint;
    static unsigned char wrkpos = 0;

    static int filepos = 0;
    int tmp;

    if( filepos && !(filepos %xres)) {
        /* some bits are there for padding, actually xpad bits */
        /* it can't be 8, so we won't be dropping a full wrkpos */
        /* fprintf( stderr, "filepos %d wrkpos %d ", filepos, wrkpos ); */
        wrkpos >>= xpad;
    }
    filepos++;
    if (!wrkpos) 
    {
        if ((wrkint=getchar())==EOF) {
            /* fprintf( stderr, "\nreturning eof filepos %d\n", filepos); */
            return EOF;
        } else
            wrkpos=128;
    }
    tmp = (wrkint & wrkpos) ? 1 : 0;
    wrkpos >>= 1;
    return tmp;
}      


void
buffercode(int code) /* buffer bit-pattern of codesize length */
{
    static int wrkint, wrkpos;
    wrkint|=(code<<wrkpos);
    wrkpos+=codesize;
    while (wrkpos>7)
    {
        buff[buffpos]=wrkint;
        buffpos++;
        wrkpos-=8;
        wrkint>>=8;
        if (buffpos==255) writebuff();
    }
}


void
writebuff() /* write buffer with counter to compressed stream */
{
    int i;
    putchar(buffpos);
    for (i=0; i<buffpos; i++) putchar(buff[i]);
    buffpos=0;
}








