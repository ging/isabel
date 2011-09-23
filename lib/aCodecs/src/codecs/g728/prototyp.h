#ifndef PROTOTYP_H
#define PROTOTYP_H

#include "common.h"
#include "state.h"

/* cmain.c */
void init_encoder(void *);
void encoder(void);
int  encode_vector(LDCELP_STATE *ldst, real *input);
void adapt_encoder(LDCELP_STATE *ldst);

/* dmain.c */
void init_decoder(void *, int);
void decoder(void);
void decode_vector(LDCELP_STATE *ldst,int, real *);
void postfilt(LDCELP_STATE *ldst,real *, real *);
void adapt_decoder(LDCELP_STATE *ldst);

/* adapters.c */
void pwf_adapter(LDCELP_STATE *ldst,real input[], real z_out[], real p_out[]);
void bsf_adapter(LDCELP_STATE *ldst,real input[], real p_out[]);
void gain_adapter(LDCELP_STATE *ldst,real log_gain[], real coeff[]);
void init_pwf_adapter(real z_co[], real p_co[]);
void init_bsf_adapter(real co[]);
void init_gain_adapter(real coeff[]);

/* codebook.c */
void iresp_vcalc(real sf_co[], real pwf_z_co[], real pwf_p_co[], real h[]);
void shape_conv(real h[], real shen[]);
void trev_conv(real h[], real target[], real pn[]);
void cb_excitation(int ix, real v[]);
int  cb_index(LDCELP_STATE *ldst,real pn[]);

/* filters.c */
void pwfilter2(LDCELP_STATE *ldst,real input[], real output[]);
void zresp(LDCELP_STATE *ldst,real output[]);
void mem_update(LDCELP_STATE *ldst,real input[], real output[]);
real predict_gain(LDCELP_STATE *ldst);
void update_gain(LDCELP_STATE *ldst,real input[], real *lgp);
void init_gain_buf(LDCELP_STATE *ldst);

/* postfil.c */
void inv_filter(LDCELP_STATE *ldst,real input[]);
void postfilter(real input[], real output[]);
void psf_adapter(real frame[]);
void compute_sh_coeff(LDCELP_STATE *ldst);
void init_postfilter(void);

/* io-sparc.c */
void init_input(char *iname, char *oxname);
void init_output(char *ixname, char *oname);
int  read_sound_buffer(int n, short *buffer);
int  write_sound_buffer(int n, short *buffer);
void put_index(int ix);
int  get_index(void);
void close_input(void);
void close_output(void);

#endif /*PROTOTYP_H*/
