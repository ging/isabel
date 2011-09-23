
#include <stdio.h>
#include <stdlib.h>

#include <vCodecs/general.h>

#if 0
// smooth and reduce
int smoothRedX24 (u8 *, u8 *, int, int, int, int);
int smoothRedY24 (u8 *, u8 *, int, int, int, int);
int smoothRedXY24(u8 *, u8 *, int, int, int, int);
#endif

// smooth and enlarge
int smoothIncXY24(u8 *, u8 *, int, int, int, int);

void
smoothRGB24(u8 *srcBuf, u8 *dstBuf, int sW, int sH, int dW, int dH)
{
    // apply a smoothing resize to RGB 24 image

    if (dW < sW && dH < sH)
        return; // smoothRedXY24(srcBuf, dstBuf, sW, sH, dW, dH);
    else if (dW < sW && dH >= sH)
        return; // smoothRedX24(srcBuf, dstBuf, sW, sH, dW, dH);
    else if (dW >= sW && dH < sH)
        return; // smoothRedX24(srcBuf, dstBuf, sW, sH, dW, dH);
    else
        smoothIncXY24(srcBuf, dstBuf, sW, sH, dW, dH);
}

int
smoothIncXY24(u8 *srcBuf, u8 *dstBuf, int sW, int sH, int dW, int dH)
{
  // dw >= sw && dH >= sH

  u8  *pp;
  int  *cxtab, *pxtab;
  int   y1Off, cyOff;
  int   ex, ey, cx, cy, px, py, apx, apy, x1, y1;
  int   cA, cB, cC, cD;
  int   pA, pB, pC, pD;

  cA = cB = cC = cD = 0;
  pp = dstBuf;

    /* cx,cy = original pixel in srcBuf.  px,py = relative position
       of pixel ex,ey inside of cx,cy as percentages +-50%, +-50%.
       0,0 = middle of pixel */

    /* we can save a lot of time by precomputing cxtab[] and pxtab[], both
       dwide arrays of ints that contain values for the equations:
         cx = (ex * swide) / dwide;
         px = ((ex * swide * 100) / dwide) - (cx * 100) - 50; */

    cxtab = (int *) malloc(dW * sizeof(int));
    if (!cxtab) { return 1; }

    pxtab = (int *) malloc(dW * sizeof(int));
    if (!pxtab) { free(cxtab);  return 1; }

    for (ex=0; ex<dW; ex++) {
      cxtab[ex] = (ex * sW) / dW;
      pxtab[ex] = (((ex * sW)* 100) / dW)
            - (cxtab[ex] * 100) - 50;
    }

    for (ey= 0; ey < dH; ey++)
    {
      u8 *pptr, rA, gA, bA, rB, gB, bB, rC, gC, bC, rD, gD, bD;

      cy = (ey * sH) / dH;
      py = (((ey * sH) * 100) / dH) - (cy * 100) - 50;
      if (py < 0) { y1 = cy-1;  if (y1 < 0) y1= 0; }
      else { y1 = cy+1;  if (y1 > sH - 1) y1= sH - 1; }

      cyOff = cy * sW * 3;    /* current line */
      y1Off = y1 * sW * 3;    /* up or down one line, depending */

      for (ex= 0; ex < dW; ex++)
      {
        rA = rB = rC = rD = gA = gB = gC = gD = bA = bB = bC = bD = 0;

        cx = cxtab[ex];
        px = pxtab[ex];

        if (px<0) { x1 = cx-1;  if (x1<0) x1=0; }
        else { x1 = cx+1;  if (x1>sW-1) x1=sW-1; }

        pptr = srcBuf + y1Off + x1*3;   /* corner pixel */
        rA = *pptr++;  gA = *pptr++;  bA = *pptr++;

        pptr = srcBuf + y1Off + cx*3;   /* up/down center pixel */
        rB = *pptr++;  gB = *pptr++;  bB = *pptr++;

        pptr = srcBuf + cyOff + x1*3;   /* left/right center pixel */
        rC = *pptr++;  gC = *pptr++;  bC = *pptr++;

        pptr = srcBuf + cyOff + cx*3;   /* center pixel */
        rD = *pptr++;  gD = *pptr++;  bD = *pptr++;

        /* compute weighting factors */
        apx = abs(px);  apy = abs(py);
        pA = (apx * apy) / 100;
        pB = (apy * (100 - apx)) / 100;
        pC = (apx * (100 - apy)) / 100;
        pD = 100 - (pA + pB + pC);

        *pp++ = ((int) (pA * rA))/100 + ((int) (pB * rB))/100 +
                ((int) (pC * rC))/100 + ((int) (pD * rD))/100;

        *pp++ = ((int) (pA * gA))/100 + ((int) (pB * gB))/100 +
                ((int) (pC * gC))/100 + ((int) (pD * gD))/100;

        *pp++ = ((int) (pA * bA))/100 + ((int) (pB * bB))/100 +
                ((int) (pC * bC))/100 + ((int) (pD * bD))/100;
          }
        }

    free(cxtab);
    free(pxtab);
}

#if 0
int
smoothRedX24(u8 *srcBuf, u8 *dstBuf, int sW, int sH, int dW, int dH)
{
  u8 *cptr, *cptr1;
  int  i, j;
  int  *lbufR, *lbufG, *lbufB;
  int  pixR, pixG, pixB;
  int  pcnt0, pcnt1, lastpix, pixcnt, thisline, ypcnt;
  int  *pixarr, *paptr;

  /* returns '0' if okay, '1' if failed (malloc) */

  /* for case where pic8 is shrunk horizontally and stretched vertically
     maps pic8 into an dwide * dhigh 24-bit picture.  Only works correctly
     when swide>=dwide and shigh<=dhigh */


  /* malloc some arrays */
  lbufR  = (int *) calloc((size_t) sW,   sizeof(int));
  lbufG  = (int *) calloc((size_t) sW,   sizeof(int));
  lbufB  = (int *) calloc((size_t) sW,   sizeof(int));
  pixarr = (int *) calloc((size_t) sW+1, sizeof(int));

  if (!lbufR || !lbufG || !lbufB || !pixarr) {
    if (lbufR)  free(lbufR);
    if (lbufG)  free(lbufG);
    if (lbufB)  free(lbufB);
    if (pixarr) free(pixarr);
    return 1;
  }

  for (j=0; j<=sW; j++)
    pixarr[j] = (j*dW + (15*sW)/16) / sW;

  cptr = srcBuf;  cptr1 = cptr + sW * 3;

  for (i=0; i<dH; i++) {
    ypcnt = (((i*sH)<<6) / dH) - 32;
    if (ypcnt<0) ypcnt = 0;

    pcnt1 = ypcnt & 0x3f;                     /* 64ths of NEXT line to use */
    pcnt0 = 64 - pcnt1;                       /* 64ths of THIS line to use */

    thisline = ypcnt>>6;

    cptr  = pic824 + thisline * sW * 3;
    if (thisline+1 < sH) cptr1 = cptr + sW * 3;
    else cptr1 = cptr;

    for (j=0; j<sW; j++) {
      lbufR[j] = ((int) ((*cptr++ * pcnt0) + (*cptr1++ * pcnt1))) >> 6;
      lbufG[j] = ((int) ((*cptr++ * pcnt0) + (*cptr1++ * pcnt1))) >> 6;
      lbufB[j] = ((int) ((*cptr++ * pcnt0) + (*cptr1++ * pcnt1))) >> 6;
    }

    pixR = pixG = pixB = pixcnt = lastpix = 0;

    for (j=0, paptr=pixarr; j<=sW; j++,paptr++) {
      if (*paptr != lastpix) {   /* write a pixel to pic24 */
    if (!pixcnt) pixcnt = 1;    /* this NEVER happens:  quiets compilers */
    *pic24++ = pixR / pixcnt;
    *pic24++ = pixG / pixcnt;
    *pic24++ = pixB / pixcnt;
    lastpix = *paptr;
    pixR = pixG = pixB = pixcnt = 0;
      }

      if (j<sW) {
    pixR += lbufR[j];
    pixG += lbufG[j];
    pixB += lbufB[j];
    pixcnt++;
      }
    }
  }

  free(lbufR);  free(lbufG);  free(lbufB);  free(pixarr);
  return 0;
}


int
smoothRedY(u8 *srcBuf, u8 *dstBuf, int sW, int sH, int dW, int dH)
{
  u8 *clptr, *cptr, *cptr1;
  int  i, j;
  int  *lbufR, *lbufG, *lbufB, *pct0, *pct1, *cxarr, *cxptr;
  int  lastline, thisline, linecnt;
  int  retval;


  /* returns '0' if okay, '1' if failed (malloc) */

  /* for case where pic8 is shrunk vertically and stretched horizontally
     maps pic8 into a dwide * dhigh 24-bit picture.  Only works correctly
     when swide<=dwide and shigh>=dhigh */

  retval = 0;   /* no probs, yet... */

  lbufR = lbufG = lbufB = pct0 = pct1 = cxarr = NULL;
  lbufR = (int *) calloc((size_t) dW, sizeof(int));
  lbufG = (int *) calloc((size_t) dW, sizeof(int));
  lbufB = (int *) calloc((size_t) dW, sizeof(int));
  pct0  = (int *) calloc((size_t) dW, sizeof(int));
  pct1  = (int *) calloc((size_t) dW, sizeof(int));
  cxarr = (int *) calloc((size_t) dW, sizeof(int));

  if (!lbufR || !lbufG || !lbufB || !pct0 || ! pct1 || !cxarr) {
    retval = 1;
    goto smyexit;
  }



  for (i=0; i<dW; i++) {                /* precompute some handy tables */
    int cx64;
    cx64 = (((i * sW) << 6) / dW) - 32;
    if (cx64<0) cx64 = 0;
    pct1[i] = cx64 & 0x3f;
    pct0[i] = 64 - pct1[i];
    cxarr[i] = cx64 >> 6;
  }


  lastline = linecnt = 0;

  for (i=0, clptr=pic824; i<=sH; i++, clptr+=sW*3) {
    thisline = (i * dH + (15*sH)/16) / sH;

    if (thisline != lastline) {  /* copy a line to pic24 */
      for (j=0; j<dW; j++) {
    *pic24++ = lbufR[j] / linecnt;
    *pic24++ = lbufG[j] / linecnt;
    *pic24++ = lbufB[j] / linecnt;
      }

      xvbzero( (char *) lbufR, dW * sizeof(int));  /* clear out line bufs */
      xvbzero( (char *) lbufG, dW * sizeof(int));
      xvbzero( (char *) lbufB, dW * sizeof(int));
      linecnt = 0;  lastline = thisline;
    }


    for (j=0, cxptr=cxarr; j<dW; j++, cxptr++) {
      cptr  = clptr + *cxptr * 3;
      if (*cxptr < sW-1) cptr1 = cptr + 1*3;
                       else cptr1 = cptr;

      lbufR[j] += ((int)((*cptr++ * pct0[j]) + (*cptr1++ * pct1[j]))) >> 6;
      lbufG[j] += ((int)((*cptr++ * pct0[j]) + (*cptr1++ * pct1[j]))) >> 6;
      lbufB[j] += ((int)((*cptr++ * pct0[j]) + (*cptr1++ * pct1[j]))) >> 6;
    }

    linecnt++;
  }


 smyexit:
  if (lbufR) free(lbufR);
  if (lbufG) free(lbufG);
  if (lbufB) free(lbufB);
  if (pct0)  free(pct0);
  if (pct1)  free(pct1);
  if (cxarr) free(cxarr);

  return retval;
}

int
smoothRedXY24(u8 *srcBuf, u8 *dstBuf, int sW, int sH, int dW, int dH)
{
  u8 *cptr;
  int  i,j;
  int  *lbufR, *lbufG, *lbufB;
  int  pixR, pixG, pixB;
  int  lastline, thisline, lastpix, linecnt, pixcnt;
  int  *pixarr, *paptr;


  /* returns '0' if okay, '1' if failed (malloc) */

  /* shrinks pic8 into a dwide * dhigh 24-bit picture.  Only works correctly
     when swide>=dwide and shigh>=dhigh (ie, the picture is shrunk on both
     axes) */


  /* malloc some arrays */
  lbufR  = (int *) calloc((size_t) sW,   sizeof(int));
  lbufG  = (int *) calloc((size_t) sW,   sizeof(int));
  lbufB  = (int *) calloc((size_t) sW,   sizeof(int));
  pixarr = (int *) calloc((size_t) sW+1, sizeof(int));
  if (!lbufR || !lbufG || !lbufB || !pixarr) {
    if (lbufR)  free(lbufR);
    if (lbufG)  free(lbufG);
    if (lbufB)  free(lbufB);
    if (pixarr) free(pixarr);
    return 1;
  }

  for (j=0; j<=sW; j++)
    pixarr[j] = (j*dW + (15*sW)/16) / sW;

  lastline = linecnt = pixR = pixG = pixB = 0;
  cptr = pic824;

  for (i=0; i<=sH; i++) {
    thisline = (i * dH + (15*sH)/16 ) / sH;

    if ((thisline != lastline)) {      /* copy a line to pic24 */
      pixR = pixG = pixB = pixcnt = lastpix = 0;

      for (j=0, paptr=pixarr; j<=sW; j++,paptr++) {
    if (*paptr != lastpix) {                 /* write a pixel to pic24 */
      if (!pixcnt) pixcnt = 1;    /* NEVER happens: quiets compilers */
      *pic24++ = (pixR/linecnt) / pixcnt;
      *pic24++ = (pixG/linecnt) / pixcnt;
      *pic24++ = (pixB/linecnt) / pixcnt;
      lastpix = *paptr;
      pixR = pixG = pixB = pixcnt = 0;
    }

    if (j<sW) {
      pixR += lbufR[j];
      pixG += lbufG[j];
      pixB += lbufB[j];
      pixcnt++;
    }
      }

      lastline = thisline;
      xvbzero( (char *) lbufR, sW * sizeof(int));  /* clear out line bufs */
      xvbzero( (char *) lbufG, sW * sizeof(int));
      xvbzero( (char *) lbufB, sW * sizeof(int));
      linecnt = 0;
    }

    if (i<sH) {
      for (j=0; j<sW; j++) {
        lbufR[j] += *cptr++;
        lbufG[j] += *cptr++;
        lbufB[j] += *cptr++;
      }

      linecnt++;
    }
  }

  free(lbufR);  free(lbufG);  free(lbufB);  free(pixarr);
  return 0;
}
#endif

