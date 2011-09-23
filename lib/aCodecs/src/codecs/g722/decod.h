/********** Fecha: 19 de febrero de 1999 ******/
/* Version en coma flotante del decodificador pero preparado para pasar 
el test en configuracion 2 de la ITU*/

#ifndef DECOD_H
#define DECOD_H

#include "vardec.h"
/*void reseteo ();*/

void leo_iHiLr(unsigned char in_rx, unsigned char *iH_a,unsigned char *iLr_a);

/*long saco_RHL(short rH_a, short rL_a);*/

void calcRxQmf(G722StateDecoder *sd, short xL, short xH, short *x1,short *x2);

void decod(G722StateDecoder *sd, unsigned char palabra, short *psalest_a, int mode);

/*unsigned char codif(short xL_a, short xH_a);*/

#endif
