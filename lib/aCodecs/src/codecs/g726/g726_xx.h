#ifndef __G726_XX_H__
#define __G726_XX_H__

#include "g72x.h"

int g726_16_encoder(	int		sl,g72x_state *state_ptr);
int g726_16_decoder(	int		i,	g72x_state *state_ptr);
int g726_24_encoder(	int		sl,g72x_state *state_ptr);
int g726_24_decoder(	int		i,	g72x_state *state_ptr);
int g726_32_encoder(	int		sl,g72x_state *state_ptr);
int g726_32_decoder(	int		i,	g72x_state *state_ptr);
int g726_40_encoder(	int		sl,g72x_state *state_ptr);
int g726_40_decoder(	int		i,	g72x_state *state_ptr);

#endif
