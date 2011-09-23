/*************************************************************************/
/*  			FUNCCF.H					 */
/* Fichero en el que estan definidas todas la s funciones que se emplean */
/*************************************************************************/

 
/************************************************************/
/*	Implementacion de las funciones del bloque 1L       */


/*	SUBTRA								*/

short subtra (short xL_a, short sL_a);

/* QUANTL								*/
/***************************************************************************/
/*                                                                         */
/*	Funcion: cuant6()                                                  */
/*                                                                         */
/* 	Cuantificador de 30 niveles para entradas en coma fija. Es         */
/*  adaptativo al disponer de un factor de escala, que va variando con la  */
/*  entrada, y se multiplica por los niveles de decision de un             */
/*  cuantificador fijo no uniforme, para obtener los niveles finales.      */
/*	Tanto el factor de escala como los niveles del cuantificador fijo  */
/*  se introducen como variables globales.                                 */
/*	El codigo de salida es el siguiente:                               */
/*                                                                         */
/*      Nivel	    Intervalos positivos	    Intervalos negativos   */
/*                                                                         */
/*      1		111111                          111101             */
/*      2		111110				111100             */
/*  	3		011111				111011             */
/*  	4		011110                          111010             */
/*  	5		011101                          111001             */
/*  	6		011100                          111000             */
/*  	7		011011                          110111             */
/*  	8		011010                          110110             */
/*  	9		011001                          110101             */
/*     10		011000                          110100             */
/*     11		010111                          110011             */
/*     12		010110                          110010             */
/*     13		010101                          110001             */
/*     14		010100                          110000             */
/*     15		010011                          101111             */
/*     16		010010                          101110             */
/*     17		010001                          101101             */
/*     18		010000                          101100             */
/*     19		001111                          101011             */
/*     20		001110                          101010             */
/*     21		001101                          101001             */
/*     22		001100                          101000             */
/*     23		001011                          100111             */
/*     24		001010                          100110             */
/*     25		001001                          100101             */
/*     26		001000                          100100             */
/*     27		000111                          100011             */
/*     28		000110                          100010             */
/*     29		000101                          100001             */
/*     30		000100                          100000             */
/*                                                                         */
/***************************************************************************/

unsigned char cuantl( short eL_a, short detL_a);

/* La salida va a ser iL, la palabra codificada de 6 bits en un short	*/
/* eL_a es el error entre la senial de entrada y la senial predicha a 	*/
/* cuantificar. */



/**************************************************************************/
/*				INVQAL 					*/

short invqal (short iL_a, short detL_a);

/* La salida va a ser dlt, que es la senial diferencia cuantizada para el */
/* predictor adaptativo con delay =0 */
/* Las entradas son la palabra codificada iL, y el factor de escala retrasado*/
/* Utiliza las tablas donde define iL4 y qq4 globales */
/* Tablas 14/g722 y 17/g722*/

/************************************************************************/
/*	Implementacion de las funciones del bloque 3L			*/

/************************************************************************/
/*	LOGSCL								*/

short logscl (short iL_a, short nbL_a);

/* La salida es nbpL, el factor de escala logaritmico.*/
/* Las entradas son la palabra codificada y el factor de escala logaritmico*/
/* retrasado */
/* Utiliza las tablas globales de wL  e iL4 */



/****************************************************************/
/*	DELAYA							*/

/*short delaya (short *x);*/

/****************************************************************/
/*	DELAYL							*/

/*short delayl (short *x);*/

/****************************************************************/
/*	SCALEL							*/

/* Como entrada tiene el factor de escala logaritmico retrasado */
/* La salida es el factor de escala logaritmico actualizado */
/* Utiliza la tabla iLB */

short scalel(short nbpL_a);

/*short pasa_log_lin (short nbpL_a);*/


/************************************************************************/
/*		Implementacion de las funciones del bloque 4L		*/
/************************************************************************/
	

/************************************************************************/
/*			PARREC						*/

short parrec (short dLt_a, short szL_a);

/* La salida es pLt, la senial reconstruida parcial (partial rec)*/
/* Las entradas son la senial diferencia cuantizada y la senial 
predicha de los ceros*/


/************************************************************************/
/*			RECONS						*/

short recons (short sL_a, short dLt_a);

/* La salida es la senial reconstruida para el predictor*/
/* Las entradas son la senial predicha y la diferencia cuantizada*/


/************************************************************************/
/*			PREDIC						*/

short predic (short spL_a, short szL_a);

/* La salida es la senial predicha */
/* Las entradas son las contribuciones a la senial predicha de los polos y 
ceros */


/************************************************************************/
/*			FILTEP						*/

short filtep (short *reconsLt_a, short *ppolosL_a);

/* La salida es la contribucion del predictor de los polos, spL, S -1...-14*/
/* Las entradas son los retrasos de la senial reconstruida y los polos retrasados*/

/************************************************************************/
/*			FILTEZ						*/

short filtez (short *diferLt_a, short *pcerosL_a);

/* La salida es la contribucion del predictor de los ceros, szL, S -1...-14*/
/* Las entradas son los retrasos de la diferencia de la senial cuantizada y 
los ceros retrasados*/


/************************************************************************/
/*			UPPOL1						*/

short uppol1 (short *ppolosL_a, short *ppolospL_a, short *parcialLt_a);

/* La salida es la actualizacion del primer coeficiente del predictor*/
/* Las entradas necesarias son AL1, APL2, PLT, PLT1*/


/************************************************************************/
/*			UPPOL2						*/

short uppol2 (short *ppolosL_a, short *parcialLt_a);

/* La salida es la actualizacion del segundo coeficiente del predictor*/
/* Las entradas necesarias son AL1, AL2, PLT, PLT1, PLT2*/


/************************************************************************/
/*			UPZERO						*/

void upzero (short *diferLt_a, short *pcerosL_a, short *pcerospL_a);

/* La salida es en realidad short *pcerospL_a, los ceros actualizados*/
/* Las entradas son las diferencias de senial cuantizadas y los ceros 
anteriores*/

/************************************************************************/
/*	Implementacion de las funciones del bloque 6L			*/

short limit(short YL_a);

/************************************************************************/
/*	Implementacion de las funciones del bloque 1H			*/


/************************************************************************/
/*	SUBTRA								*/

/* Utilizar la implementacion para la parte baja con las 
entradas xH y sH*/

/************************************************************************/
/*			QUANTH						*/

unsigned char quanth (short eH_a, short detH_a);


/****************************************************************************/
/*			BLOQUE 2H					    */
/****************************************************************************/
/*		INVQAH							*/		

short  invqah(short iH_a, short detH_a);

/****************************************************************************/
/*			BLOQUE 3H					*/
/****************************************************************************/
/*		LOGSCH							*/

short logsch (short iH_a, short nbH_a);


/****************************************************************************/
/*		SCALEH							*/

short scaleh (short nbpH_a);


/*short pasa_log_linh (short nbpH_a);*/


/****************************************************************************/
/*		DELAYH							*/
/* No la escribo porque despues no la voy a usar*/

/****************************************************************************/
/*		BLOQUE 4H						*/
/****************************************************************************/
/* Todas las funciones de este bloque son las mismas que las del bloque 4L	*/
/*		DELAYA																*/
/*		PARREC																*/
/*		RECONS																*/
/*		UPZERO																*/
/*		UPPOL2																*/
/*		UPPOL1																*/
/*		FILTEZ																*/
/*		FILTEP																*/
/*		PREDIC																*/
/* Solo hay que poner los parametros adecuados								*/
/****************************************************************************/
/*			BLOQUE 5L					*/
/****************************************************************************/

/*			INVQBL						*/

short invqbl (unsigned char iLr_a, short detL_a, int mode_a);

