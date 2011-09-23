/*---------------------------------------------------
 *  state.h
 *  Estado del coder-encoder ldcelp ITU G.728.
 *  Angel Fdez. Herrero. Octubre-2000.
 *--------------------------------------------------*/

#ifndef STATE_H
#define STATE_H

#include "parm.h"
#include "common.h"

typedef struct {
/* cmain.c */
	int vx;
	int ffase;
	int postfilter;
	real thequeue[NFRSZ];
	real synspeech[NFRSZ];    /* Synthesized Speech */
	real log_gains[NUPDATE];  /* Logarithm of Gains */

/* adapters.c */
	real pwf_old_input[LPCW+NFRSZ+NONRW];
	real pwf_rec[LPCW+1];	  /* Recursive Part */
	real bsf_old_input[LPC+NFRSZ+NONR];
	real bsf_rec[LPC+1];
	real g_old_input[LPCLG+NUPDATE+NONRLG];
	real g_rec[LPCLG+1];      /* Recursive part for Hybrid Window */
	real a10[11];
	real k10;

/* codebook.c */
	real shape_energy[NCWD];
	real imp_resp[IDIM];

/* filters.c */
	real firmem[LPCW+IDIM];
	real iirmem[LPCW+IDIM];
	real statelpc[LPC+IDIM];
	real zirwfir[LPCW];
	real zirwiir[LPCW];
	real gain_input[LPCLG];

/* global.c */
	int sf_coeff_index;
	int sf_coeff_obsolete_p;
	real *sf_coeff;
	real sf_vcoeff[2][LPC+1];
	real gp_coeff[LPCLG+1];
	real pwf_z_coeff[LPCW+1];
	real pwf_p_coeff[LPCW+1];
} LDCELP_STATE;

//extern LDCELP_STATE *ldst;

#define LDST(name) (ldst->name)
#define sf_coeff_next (&LDST(sf_vcoeff)[LDST(sf_coeff_index)][0])

#endif /*STATE_H*/
