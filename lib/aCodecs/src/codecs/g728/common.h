#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include "parm.h"

#define real double

#define sig_scale(SCALE,A,B) \
  { int i; for(i=0; i<IDIM; i++) (B)[i]=SCALE*(A)[i]; }

#define sub_sig(A,B,C) \
  { int i; for(i=0; i<IDIM; i++) (C)[i]=(A)[i]-(B)[i]; }

#define REG(X)

#define EPSILON       1.0e-35
#define REALZEROP(x)  ((x<EPSILON)&&(x>-EPSILON))

#define RCOPY(X,Y,N)  memcpy(Y,X,(N)*sizeof(real))

#define CLIPP(X,LOW,HIGH)  ((X)<(LOW)?(LOW):(X)>(HIGH)?(HIGH):(X))

/*#define ZARR(A) \
  { int i; for(i=sizeof(A)/sizeof(A[0])-1; i>=0; i--) A[i]=0.0; }*/

/* Update obsoleted atomic array */

#if 0
#define UPDATE(NAME) \
  { int i; \
    if(NAME##_obsolete_p) \
      for(i=sizeof(NAME)/sizeof(NAME[0])-1; i>=0; i--) \
        NAME[i]=NAME##_next[i]; \
    NAME##_obsolete_p=0; }
#elif 0
#define UPDATE(NAME,N) \
  { if(LDST(NAME##_obsolete_p)) RCOPY(LDST(NAME##_next),LDST(NAME),N); \
    LDST(NAME##_obsolete_p)=0; }
#else
#define UPDATE(NAME) \
  if(LDST(NAME##_obsolete_p)) \
  { LDST(NAME)=NAME##_next; \
    LDST(NAME##_obsolete_p)=0; \
    LDST(NAME##_index)^=1; }
#endif

/* Copy L words to X from circular buffer CIRC ending at offset EOS. 
   CL is the size of circular buffer CIRC */

#define CIRCOPY(X,CIRC,EOS,L,CL) \
  { int i,i1,i2,lx=0; \
    if((EOS)>=(L)) { i1=(EOS)-(L); i2=(CL); } \
    else           { i1=0;         i2=(CL)+(EOS)-(L); } \
    for(i=i2; i<(CL);  i++) X[lx++]=CIRC[i]; \
    for(i=i1; i<(EOS); i++) X[lx++]=CIRC[i]; }

#if 0
#define RSHIFT(START,LENGTH,SHIFT) \
  { int i; \
    real *from = &(START)[LENGTH-1]; \
    real *to = &(START)[LENGTH+SHIFT-1]; \
    for(i=0; i<LENGTH; i++) *to-- = *from--; }
#else
#define RSHIFT(START,LENGTH,SHIFT) \
    memmove((START)+(SHIFT),START,(LENGTH)*sizeof(real))
#endif

#define VPROD(X,Y,Z,N) \
  { int i; \
    const real *xp=X, *yp=Y; \
    real xi=*xp++, yi=*yp++; \
    real zi, *zp=Z; \
    for(i=1; i<N; i++) \
      { zi=xi*yi; xi=*xp++; yi=*yp++; *zp++=zi; } \
    zi=xi*yi; *zp=zi; }

#define add(k,n) (((k)+(n)*IDIM)%NFRSZ)

#define FFASE(N) if(LDST(ffase)==N)
#define NEXT_FFASE(ldst) \
  { (ldst) -> vx=add((ldst) -> vx,1); \
    (ldst) -> ffase=((ldst) -> ffase==NUPDATE?1:(ldst) -> ffase+1); }

#endif /*COMMON_H*/
