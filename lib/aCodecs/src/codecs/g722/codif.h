/********** Fecha: 19 de febrero de 1999 ******/
/* Version en coma flotante del decodificador pero preparado para pasar 
el test en configuracion 2 de la ITU*/

#ifndef CODIF_H
#define CODIF_H

#include "varcod.h"

/*void reseteo ();*/

/*void leo_iHiLr (unsigned char in_rx, unsigned char *iH_a,unsigned char *iLr_a);*/

/*long saco_RHL(short rH_a, short rL_a);*/

/*void decod(unsigned char palabra, short *psalest_a);*/

void calcTxQmf(G722StateEncoder *se,short *xL, short *xH, short x1, short x2);

unsigned char codif(G722StateEncoder *se, short xL_a, short xH_a);

#endif
