/****************************************************************/
/* Fichero que simula las operaciones que se utilizan en el G722*/
/* (pag 33 del G722) */
/* OPERCF.H*/
/* Fecha: 28 de octubre de 1998*/


/*#include <stdio.h>*/
/*#include "convert.h"*/

#ifndef OPERCF_H
#define OPERCF_H

short _add(short a, short b);
/* esta seria la suma que hay que hacer par el g722*/

short _sub(short a, short b);

short _mpy(short a, short b);
/* Esta seria la multiplicacion del c206*/

short _gmpy(short a, short b);
/* Esta la que dice el g722 que hay que implementar*/

short _gmpy_fe(short a, short b);

#endif
