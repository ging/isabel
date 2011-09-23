/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header$ */

#include "gsm.h"

#	include	<stdlib.h>

void gsm_destroy (gsm S)
{
	if (S) free((char *)S);
}
