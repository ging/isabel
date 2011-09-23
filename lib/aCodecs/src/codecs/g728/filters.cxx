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

#include "parm.h"
#include "state.h"
#include "common.h"
#include "prototyp.h"

static void sf_zresp(LDCELP_STATE *ldst,real output[]);
static void pwf_zresp(LDCELP_STATE *ldst,real input[], real output[]);
static real log_rms(real input[]);

static real DOTPROD(real X[], real Y[], int N)
{
  int i;
  real sum=0.0;

  for(i=0; i<N; i++) sum+=X[i]*Y[i];

  return sum;
}

/********************************* Perceptual Weighting Filter **/

void pwfilter2(LDCELP_STATE *ldst,real input[], real output[])
{
    int k;
    real out;

    RSHIFT(LDST(firmem), LPCW, IDIM);
    for(k=0; k<IDIM; k++)
	LDST(firmem)[k] = input[IDIM-1-k];
    RSHIFT(LDST(iirmem), LPCW, IDIM);

    for (k=0; k<IDIM; k++) {
	out = LDST(firmem)[IDIM-1-k];  /* pwf_z_coeff[0] is always 1.0 */
	out += DOTPROD(LDST(firmem)+IDIM-k, LDST(pwf_z_coeff)+1, LPCW);
	out -= DOTPROD(LDST(iirmem)+IDIM-k, LDST(pwf_p_coeff)+1, LPCW);
	LDST(iirmem)[IDIM-1-k] = out;
	output[k] = out;
    }
}

/* Synthesis and Perceptual Weighting Filter */

void sf_zresp(LDCELP_STATE *ldst,real output[])
{
  int k,j;

  for(j=LPC-1; j>=0; j--)
      LDST(statelpc)[j+IDIM] = LDST(statelpc)[j];

  for(k=0; k<IDIM; k++)
  {
      real out = 0.0, sj, aj;

      sj = LDST(statelpc)[LPC+IDIM-k-1];
      aj = LDST(sf_coeff)[LPC];
      for (j=LPC-2; j>=1; j--) {
	  out -= sj*aj;
	  sj = LDST(statelpc)[IDIM-k+j];
	  aj = LDST(sf_coeff)[j+1];
      }
      output[k] = out - sj*aj-LDST(statelpc)[IDIM-k] * LDST(sf_coeff)[1];
      LDST(statelpc)[IDIM-1-k] = output[k];
  }
}

void
pwf_zresp(LDCELP_STATE *ldst,real input[], real output[]) {
   int j,k;
   real tmp;

   for (k=0; k<IDIM; k++) {
   	tmp = input[k];
   	for (j=LPCW-1; j>=1; j--) {
   	   input[k] += LDST(zirwfir)[j] * LDST(pwf_z_coeff)[j+1];
   	   LDST(zirwfir)[j] = LDST(zirwfir)[j-1];
   	}
	input[k] += LDST(zirwfir)[0] * LDST(pwf_z_coeff)[1];
   	LDST(zirwfir)[0] = tmp;
   	for (j=LPCW-1; j>=1; j--) {
   	    input[k] -= LDST(zirwiir)[j] * LDST(pwf_p_coeff)[j+1];
   	    LDST(zirwiir)[j] = LDST(zirwiir)[j-1];
   	}
   	output[k] = input[k] - LDST(zirwiir)[0] * LDST(pwf_p_coeff)[1];
   	LDST(zirwiir)[0] = output[k];
   }
}

void zresp(LDCELP_STATE *ldst,real output[])
{
    real temp[IDIM];

    sf_zresp(ldst, temp);
    pwf_zresp(ldst, temp, output);
}

void mem_update(LDCELP_STATE *ldst,real input[], real output[])
{
    int i,k;
    real temp[IDIM], a0, a1, a2;
    real *t2 = LDST(zirwfir);

    t2[0] = temp[0] = input[0];
    for (k=1; k<IDIM; k++) {
	a0 = input[k];
	a1 = a2 = 0.0;
	for (i=k; i>= 1; i--) {
	    t2[i] = t2[i-1];
	    temp[i] = temp[i-1];
	    a0 -=   LDST(sf_coeff)[i] * t2[i];
	    a1 += LDST(pwf_z_coeff)[i] * t2[i];
	    a2 -= LDST(pwf_p_coeff)[i] * temp[i];
	}
	t2[0] = a0;
	temp[0] = a0+a1+a2;
    }
    for (k=0; k<IDIM; k++) {
   	LDST(statelpc)[k] += t2[k];
   	if (LDST(statelpc)[k] > MAX)
	    LDST(statelpc)[k] = MAX;
        else if (LDST(statelpc)[k] < MIN)
	    LDST(statelpc)[k] = MIN;
        LDST(zirwiir)[k] += temp[k];
    }
    for (i=0; i<LPCW; i++)
   	LDST(zirwfir)[i] = LDST(statelpc)[i];
    for (k=0; k<IDIM; k++)
	output[k] = LDST(statelpc)[IDIM-1-k];
}

/*********************************************** The Gain Predictor */

#include <math.h>

#define LOG10(X) log10(X)
#define EXP10(X) pow(10,X)

static real log_rms(real input[])
{
    int k;
    real etrms=0.0;

    for(k=0; k<IDIM; k++)
	etrms += input[k]*input[k];
    etrms /= IDIM;
    if (etrms<1.0)
	etrms = 1.0;
    return etrms = 10.0*log10(etrms);
}

real predict_gain(LDCELP_STATE *ldst)
{
  int i;
  real new_gain = GOFF;
  real temp;

  for (i=1;i<=LPCLG;i++)
  {
      temp = LDST(gp_coeff)[i] * LDST(gain_input)[LPCLG-i];
      new_gain -= temp;
  }
  if (new_gain <  0.0) new_gain = 0.0;
  if (new_gain > 60.0) new_gain = 60.0;
  new_gain = pow((double)10,(double)0.05*new_gain);
  return new_gain;
}

void update_gain(LDCELP_STATE *ldst,real input[], real *lgp)
{
    int i;

    *lgp = log_rms(input) - GOFF;
    for (i=0; i<LPCLG-1; i++)
      LDST(gain_input)[i] = LDST(gain_input)[i+1];
    LDST(gain_input)[LPCLG-1] = *lgp;
}

void init_gain_buf(LDCELP_STATE *ldst)
{
  int i;

  for(i=0;i<LPCLG;i++) LDST(gain_input)[i] = -GOFF;
  for(i=0;i<NUPDATE;i++) LDST(log_gains)[i] = -GOFF;
}
