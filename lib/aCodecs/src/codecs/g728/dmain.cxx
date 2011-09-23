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

#include "state.h"
#include "common.h"
#include "prototyp.h"

#ifdef MAIN

#ifdef NOPF
static int postfiltering_p=0;
#else
static int postfiltering_p=1;
#endif

static void usage(char *name);

int main(int argc, char *argv[])
{
    if(argc!=3) usage(argv[0]);

    init_output(argv[1], argv[2]);

    init_decoder();
    decoder();

    close_output();

    return 0;
}

static void usage(char *name)
{
    fprintf(stderr, "Usage: %s <index-file> <audio-file>\n", name);
    exit(1);
}

#endif /*MAIN*/

void init_decoder(void *ldst_decoder, int postfilter) {
    LDCELP_STATE *ldst;
    
    ldst=(LDCELP_STATE *)ldst_decoder;
    memset(ldst, 0, sizeof(LDCELP_STATE));

    LDST(vx)=0;
    LDST(ffase)=1;
    /*ZARR(LDST(thequeue));*/

    LDST(sf_coeff_index) = 1;
    LDST(sf_coeff_obsolete_p) = 0;
    LDST(sf_coeff)=&LDST(sf_vcoeff)[0][0];
    sf_coeff_next[0] = 1.0;
    init_bsf_adapter(LDST(sf_coeff));

    init_gain_adapter(LDST(gp_coeff));
    init_gain_buf(ldst);

    init_postfilter();
    LDST(postfilter)=postfilter;
}

#ifdef MAIN

void decoder(void)
{
    int  ix;
    real *output;

    while((ix=get_index())!=-1)
    {
	output=decode_vector(ix);
        if(LDST(postfiltering)_p)
            output=postfilt(output);
	adapt_decoder();
        LDST(vx)=add(LDST(vx),1);
        NEXT_FFASE;

	write_sound_buffer(IDIM, output);
    }
}

#endif /*MAIN*/

void decode_vector(LDCELP_STATE *ldst, int ix, real *output)
{
    real gain;

    /* Las siguientes variables no son de estado */

    static real
        zero_response[IDIM],
        cb_vec[IDIM],
        qspeech[IDIM];

    UPDATE(sf_coeff);
    zresp(ldst, zero_response);

    gain = predict_gain(ldst);
    cb_excitation(ix, cb_vec);
    sig_scale(gain, cb_vec, qspeech);

    update_gain(ldst, qspeech, LDST(log_gains)+add(LDST(vx),3)/IDIM);
    mem_update(ldst, qspeech, LDST(synspeech)+LDST(vx));

    RCOPY(LDST(synspeech)+LDST(vx), output, IDIM);
}

void postfilt(LDCELP_STATE *ldst, real *input, real *output)
{
    /* Las siguientes variables no son de estado */

    static real qs[NFRSZ];

    inv_filter(ldst, input);
    FFASE(1)
        compute_sh_coeff(ldst);
    FFASE(3) 
    {
        CIRCOPY(qs, LDST(synspeech), add(LDST(vx),1), NFRSZ, NFRSZ);
        psf_adapter(qs);
    }
    postfilter(input, output);
}

void adapt_decoder(LDCELP_STATE *ldst)
{
    /*static real synth[NFRSZ], lg[NUPDATE];*/

    FFASE(1)
    {
	/*CIRCOPY(lg, LDST(log_gains), LDST(vx)/IDIM, NUPDATE, NUPDATE);*/
	gain_adapter(ldst, LDST(log_gains), LDST(gp_coeff));
    }
    FFASE(2)
        LDST(sf_coeff_obsolete_p) = 1;
    FFASE(4)
    {     
	/*CIRCOPY(synth, LDST(synspeech), LDST(vx), NFRSZ, NFRSZ);*/
	bsf_adapter(ldst, LDST(synspeech), sf_coeff_next);
    }
}
