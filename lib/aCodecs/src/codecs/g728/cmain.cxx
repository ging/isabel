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
#include <string.h>

#include "parm.h"
#include "state.h"
#include "common.h"
#include "prototyp.h"

#ifdef MAIN

static void usage(char *name);

int main(int argc, char *argv[])
{
    if(argc!=3) usage(argv[0]);

    init_input(argv[1], argv[2]);

    init_encoder();
    encoder();

    close_input();

    return 0;
}

static void usage(char *name)
{
    fprintf(stderr, "Usage: %s <audio-file> <index-file>\n", name);
    exit(1);
}

#endif /*MAIN*/

void init_encoder(void *ldst_encoder) {
    LDCELP_STATE *ldst;
    
    ldst=(LDCELP_STATE *)ldst_encoder;
    memset(ldst, 0, sizeof(LDCELP_STATE));

    LDST(vx)=0;
    LDST(ffase)=1;
    /*ZARR(LDST(thequeue));*/

    init_pwf_adapter(LDST(pwf_z_coeff), LDST(pwf_p_coeff));

    LDST(sf_coeff_index) = 1;
    LDST(sf_coeff_obsolete_p) = 0;
    LDST(sf_coeff)=&LDST(sf_vcoeff)[0][0];
    sf_coeff_next[0] = 1.0;
    init_bsf_adapter(LDST(sf_coeff));

    init_gain_adapter(LDST(gp_coeff));
    init_gain_buf(ldst);

    /*ZARR(LDST(imp_resp));*/
    LDST(imp_resp)[0] = 1.0;
    shape_conv(LDST(imp_resp), LDST(shape_energy));
}

#ifdef MAIN

void encoder(void)
{
    int  ix;
    real input[IDIM];

    while(read_sound_buffer(IDIM, input)>0) 
    {
	ix=encode_vector(input);
	adapt_encoder();
	LDST(vx)=add(LDST(vx),1);
        NEXT_FFASE;

        put_index(ix);
    }
}

#endif /*MAIN*/

int encode_vector(LDCELP_STATE *ldst, real *input) {
    int ix;
    real gain;

    /* Las siguientes variables no son de estado */

    static real
	zero_response[IDIM],
	weighted_speech[IDIM],
	qspeech[IDIM],
	target[IDIM],
	normtarg[IDIM],
	cb_vec[IDIM],
	pn[IDIM];

    pwfilter2(ldst, input, weighted_speech);

    UPDATE(sf_coeff);
    zresp(ldst, zero_response);

    sub_sig(weighted_speech, zero_response, target);

    gain = predict_gain(ldst);
    sig_scale(1.0/gain, target, normtarg);

    trev_conv(LDST(imp_resp), normtarg, pn);

    ix = cb_index(ldst, pn);

    cb_excitation(ix, cb_vec);
    sig_scale(gain, cb_vec, qspeech);

    RCOPY(input, LDST(thequeue)+add(LDST(vx),2), IDIM);
    update_gain(ldst, qspeech, LDST(log_gains)+add(LDST(vx),3)/IDIM);
    mem_update(ldst, qspeech, LDST(synspeech)+LDST(vx));

    return ix;
}

void adapt_encoder(LDCELP_STATE *ldst) {
    /*static real input[NFRSZ], synth[NFRSZ], lg[NUPDATE];*/

    /* Gain coeff update before second vector of frame */
    FFASE(1)
    {
	/*CIRCOPY(lg, LDST(log_gains), LDST(vx)/IDIM, NUPDATE, NUPDATE);*/
	gain_adapter(ldst, LDST(log_gains), LDST(gp_coeff));
    }

    FFASE(2)
    {
	/*CIRCOPY(input, LDST(thequeue), LDST(vx), NFRSZ, NFRSZ);*/
	pwf_adapter(ldst, LDST(thequeue), LDST(pwf_z_coeff), LDST(pwf_p_coeff));
	
        LDST(sf_coeff_obsolete_p) = 1;
	iresp_vcalc(sf_coeff_next, LDST(pwf_z_coeff), LDST(pwf_p_coeff), LDST(imp_resp));
	shape_conv(LDST(imp_resp), LDST(shape_energy));
    }

    /* Backward syn. filter coeff update. Occurs after full frame (before
       first vector) but not used until the third vector of the frame */
    FFASE(4)
    {
        /*CIRCOPY(synth, LDST(synspeech), LDST(vx), NFRSZ, NFRSZ);*/
        bsf_adapter(ldst, LDST(synspeech), sf_coeff_next);
    }
}
