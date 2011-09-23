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

#include "data.h"
#include "state.h"
#include "common.h"
#include "prototyp.h"

/* Impulse Response Vector Calculator */

void iresp_vcalc(real sf_co[], 
		 real pwf_z_co[], real pwf_p_co[], 
		 real h[])
{
    /* Las siguientes variables no son de estado */

    static real temp[IDIM];
    static real rc[IDIM];

    real a0,a1,a2;
    int i,k;

    temp[0] = rc[0] = 1.0;
    for (k=1; k<IDIM; k++) {
   	a0=a1=a2=0.0;
   	for (i=k; i>=1; i--) {
	    temp[i] = temp[i-1];
	    rc[i] = rc[i-1];
	    a0 -= sf_co[i]   * temp[i];
	    a1 += pwf_z_co[i] * temp[i];
	    a2 -= pwf_p_co[i] * rc[i];
  	}
   	temp[0] = a0;
   	rc[0] = a0+a1+a2;
    }
    for (k=0; k<IDIM; k++)
	h[k] = rc[IDIM-1-k];
}

/* Cb_shape Codevector Convolution Module and Energy Table Calculator */
/* The output is energy table */

void
shape_conv(real h[], real shen[])
{
    int j;
    real h0 = h[0], h1 = h[1], h2 = h[2], h3 = h[3], h4 = h[4], tmp;

    for (j=0; j<NCWD; j++)
    {
	real energy=0;

	tmp = h0*cb_shape[j][0];
	energy += tmp*tmp;

	tmp = h0*cb_shape[j][1] + h1*cb_shape[j][0];
	energy += tmp*tmp;

	tmp = h0*cb_shape[j][2] + h1*cb_shape[j][1] + h2*cb_shape[j][0];
	energy += tmp*tmp;

	tmp = h0*cb_shape[j][3] + h1*cb_shape[j][2] + h2*cb_shape[j][1] +
	      h3*cb_shape[j][0];
	energy += tmp*tmp;

	tmp = h0*cb_shape[j][4] + h1*cb_shape[j][3] + h2*cb_shape[j][2] +
	      h3*cb_shape[j][1] + h4*cb_shape[j][0];
	energy += tmp*tmp;

	shen[j] = energy;
    }
}

/* Time Reversed Convolution Module -- Block 13 */

void
trev_conv(real h[], real target[], real pn[])
{
    int j, k;

    for (k=0; k<IDIM; k++) {
	real tmp=0.0;

   	for (j=k; j<IDIM; j++)
	    tmp += target[j]*h[j-k];
	pn[k] = tmp;
    }
}

/* Error Calculator and Best Codebook Index Selector */
/* Blocks 17 and 18 */

void
cb_excitation(int ix, real v[])
{
    int i,
	sx = ix>>3,
	gx = ix&7;
    real gain = cb_gain[gx];

    for(i=0; i<IDIM; i++)
	v[i] = cb_shape[sx][i] * gain;
}

#define GTINC(A,B,X) if(A>B)X++

int
cb_index(LDCELP_STATE *ldst,real pn[])
{
    real d, distm = BIG;
    int
	j,
	is=0,	/* best shape index */
	ig=0,	/* best gain index */
	idxg,	/* current gain index */
	ichan;	/* resulting combined index */
    real *shape_ptr = (real *) cb_shape;
    real *sher_ptr = LDST(shape_energy);
    real pcor, b0, b1, b2;
    real *pb = pn;
    real g2, gsq;
    /*register*/ real
	cgm0 REG(r0) = cb_gain_mid_0,
	cgm1 REG(r1) = cb_gain_mid_1,
	x REG(r2),
	cgm2 = cb_gain_mid_2,
	energy REG(r4), y REG(r5),
	cor REG(r8) = 0,
	t REG(r12);
    /*register*/ int minus5 REG(m0) = -5;
    

    for (j=0; j<NCWD; j++) {
	cor = 0.0;
	energy = *sher_ptr++;

	b0 = cgm0 * energy; x=*shape_ptr++; y=*pb++;
	            t=x*y;  x=*shape_ptr++; y=*pb++;
	cor+=t;     t=x*y;  x=*shape_ptr++; y=*pb++;
	cor+=t;     t=x*y;  x=*shape_ptr++; y=*pb++;
	cor+=t;     t=x*y;  x=*shape_ptr++; y=*pb++;
	cor+=t;     t=x*y;
	cor+=t;     b1 = cgm1 * energy;
	
	pb += minus5;
	b2 = cgm2 * energy;
	idxg = 0;
	pcor = cor;
	if (cor < 0.0) {
	    pcor = -cor;
	    idxg += 4;
	}
	GTINC(pcor, b0, idxg);
	GTINC(pcor, b1, idxg);
	GTINC(pcor, b2, idxg);
	
	g2 = cb_gain2[idxg];
	gsq = cb_gain_sq[idxg];
	d = gsq * energy - g2 * cor;
	
	if (d < distm) {
	    ig = idxg;
	    is = j;
	    distm = d;
	}
    }
    return ichan = (is<<3)+ig;
}
