/**************************************************************************/
/* Fichero que implementa las operaciones que se definen en la norma G722 */
/* (pag 33 del G722)                                                      */
/********************** OPERCF.C ******************************************/
/* Fecha: 28 de octubre de 1998                                           */
/* Autora: Anabel Glez-Tablas						  */
/**************************************************************************/



#include "opercf.h"

/**************************************************************************/
/* short _add(short a, short b)
  Esta función realiza la suma de dos enteros tipo short limitando su valor 
  al rango (-32768, +32767).

  ENTRADAS:	short a, b

  SALIDAS:	short														  */
/**************************************************************************/
short _add(short a, short b)
/* esta seria la suma que hay que hacer para el g722*/
{
	const short vmin=-1<<15 /*-32768, valor max. segun G722*/;
	const short vmax=-(vmin +1) /*32767*/;

	long r= (long)a +(long)b;

	if (r<vmin) r=vmin;
	if (r>vmax) r=vmax;

	return (short)r;
}

/**************************************************************************/
/* short _sub(short a, short b)
  Esta funcion realiza la resta de dos enteros tipo short limitando su valor 
  al rango (-32768, +32767).

  ENTRADAS:	short a, b

  SALIDAS:	short														  */
/**************************************************************************/
short _sub(short a, short b)
/* esta seria la resta que hay que hacer par el g722*/
{
	const short vmin=-1<<15 /*-32768, valor max. segun G722*/;
	const short vmax=-(vmin +1) /*32767*/;

	long r= (long)a -(long)b;

	if (r<vmin) r=vmin;
	if (r>vmax) r=vmax;

	return (short)r;
}

/**************************************************************************/
/*					NO SE USA			  */
/**************************************************************************/
/*short _mpy(short a, short b)*/
/* Esta seria la multiplicacion del c206*/
/* Cuando se desborda es desastroso: por ejemplo 2567*2567=-29647*/
/* Si el primer bit es uno lo toma como un numero negativo.*/
/* No se si habria que limitarlo o dejarlo asi*/
/*{
	long r =(long)a * (long)b;
	return (short)r;

} */

/**************************************************************************/
/* short _gmpy(short a, short b)
  Esta funcion realiza la multiplicion  de dos enteros tipo short tal como 
  define la norma G722. Se transforman a long antes de operar con ellos, y 
  posteriormente se corren 15 bits para devolverles su qn original. Al ser 
  una operacion definida asi, se supone que la suma de los qn de los factores
  debe ser inicialmente 30 (para que tenga sentido la operacion). Solo se 
  utiliza cuando la norma hace referencia a esta operacion, asi que su uso 
  es correcto.

  ENTRADAS:	short a, b

  SALIDAS:	short														  */
/**************************************************************************/
short _gmpy(short a, short b)
/* Esta la que dice el g722 que hay que implementar*/
{
	long r=(long)a*(long)b;
	r>>=15;
	/* como es un long (signed), hay extension de signo*/
	return (short)r;
}

/**************************************************************************/
/* short _gmpy_fe(short a, short b)      NO SE USA			  */
/**************************************************************************/
/*short _gmpy_fe(short a, short b)*/
/*esta es una multiplicacion que me he inventado para probar lo del
factor de escala 6 en un formato qn 13 en vez de qn 18*/
/*{
	long r=(long)a*(long)b;
	r>>=12;
*/
	/* como es un long (signed), hay extension de signo*/
	/*return (short)r;
} */
