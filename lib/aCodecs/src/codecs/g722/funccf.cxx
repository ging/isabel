/*************************************************************************/
/*  			FUNCCF.C					*/
/* Fichero en el que estan definidas todas las funciones que se emplean 
en la implementación de la norma G722				*/
/*************************************************************************/


#include <math.h>
#include "funccf.h"
#include "opercf.h"
/*#include "convert.h"*/

#include "tabla.h"

/*extern int rss;*/


/************************************************************************/
/*	Implementacion de las funciones del bloque 1L			*/
/*************************************************************************/


/*************************************************************************/
/*			SUBTRA						*/

short subtra (short xL_a, short sL_a)
{
	return _sub (xL_a, sL_a);
}

/*************************************************************************/
/*			 QUANTL						*/
/***************************************************************************/
/*                                                                         */
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

unsigned char cuantl( short eL_a, short detL_a)

/* La salida va a ser iL, la palabra codificada de 6 bits en un short	*/
/* eL_a es el error entre la senial de entrada y la senial predicha a 	*/
/* cuantificar. */

{

	/*
	Para determinar el nivel del cuantificador en el que se encuentra la
  muestra de entrada, se emplea un algoritmo de busqueda binaria. Necesito
  estas dos variables de tipo 'unsigned char'.
	*/
	unsigned char	inferior, mitad, superior;

	/*  Valor auxiliar para efectuar la comparacion.*/
	short	compara;

	int sil; /* signo de eL_a*/
	short wd; /* magnitude de eL_a*/


	/*  Comenzamos la busqueda con las siguientes asignaciones.*/
	inferior = 0;
	mitad = 14;
	superior = 29;

	sil=eL_a>>15; /*extraemos el signo*/

	if (sil==0) {
		wd=eL_a;}
	else {
		wd=((32767-eL_a)&32767);}; 
		/* aqui no podemos emplear _sub porque no queremos que este 
		controlado el overflow */
	
	/*
	Si debemos codificar un valor superior al de sobrecarga, ya le
	asignamos el nivel correspondiente a este ultimo.
	*/
	compara = _gmpy((short)((*( q6 + superior))<<3), detL_a);
	if ( ( wd < compara) && ( wd > -compara))
	{
		do	{
			/*
			El nivel mitad se multiplica por el factor de escala, para hacer
			la comparacion.
			*/
			compara = _gmpy((short)((*( q6 + mitad))<<3), detL_a);
			if ( ( wd >= compara) || ( wd <= -compara))
			{
				inferior = mitad;
			}
			else	superior = mitad;
			/*
			Se debe interrumpir la busqueda cuando el nivel inferior y el
			superior sean consecutivos.
			*/
			/*Cambio realizado en el ultimo momento: 8:9:99 */
			/*mitad = (unsigned char)floor(( inferior + superior) * 0.5);*/
			mitad = (unsigned char) ((inferior + superior)>>1);
			/*A
			floor(x) mayor entero no mayor que x, como double
			A*/
		}  while ( mitad != inferior);
	}
	else	{
		inferior = superior;
	}
	/*
	Debemos sumar 1 a 'inferior' para obtener el nivel correcto de
	intervalo.
	*/
	inferior++;
	/*  La funcion devuelve un valor codificado con 6 bits.*/

	if ( sil!=0)
	{
		if ( inferior > 2)
		{
			return	34 - inferior;
		}
		else	return 	64 - inferior;
	}
	else
	{
		return  62 - inferior;
	}
}

/**************************************************************************/
/*			INVQAL 						*/

short invqal (short iL_a, short detL_a)

/* La salida va a ser dlt, que es la senial diferencia cuantizada para el */
/* predictor adaptativo con delay =0 */
/* Las entradas son la palabra codificada iL, y el factor de escala retrasado*/
/* Utiliza las tablas donde define iL4 y qq4 globales */
/* Tablas 14/g722 y 17/g722*/
{
	unsigned char ril;
	short wd1;

	ril=(iL_a>>2);

	wd1= (qq4[iL4[ril]+1]<<3);
	if ((ril>0) && (ril<8))
		return -(_gmpy(detL_a, wd1));/*sil=-1;*/
	else
		return (_gmpy(detL_a, wd1));/*sil=0;*/
	
}

	

/************************************************************************/
/*	Implementacion de las funciones del bloque 3L			*/

/************************************************************************/
/*		LOGSCL							*/

short logscl (short iL_a, short nbL_a)

/* La salida es nbpL, el factor de escala logaritmico.*/
/* Las entradas son la palabra codificada y el factor de escala logaritmico*/
/* retrasado */
/* Utiliza las tablas globales de wL  e iL4 */

{
	short aux;
	short wd;
	unsigned char riL=iL_a>>2;
	/* esta bien, rellena con ceros por ser unsigned*/
	
	/**nbLp=nbL[0];*/ /* inicializo el puntero */
	
	/**nbLp++=*nbLp;*/ /* actualizo el valor del factor de escala retrasado*/

	/*calculo el nuevo valor del factor de escala*/
	wd= _gmpy (nbL_a,32512); /* 32512 es una constante 127/128*/
	
	aux= _add(wd, wL[iL4[riL]+1]);

	/**nbLp = _add(wd, wL[iL4[riL]]);*/

	if (aux<0)
		return 0; /*limite inferior de 0*/
	else	{if (aux>18432)
				return 18432;/* limite superior de 9*/
			else 
			{return aux;}
			};
}

/****************************************************************/
/*			DELAYA					*/

/*short delaya (short *x)
{
	if (rss==0)
		return *(x+1);
	else return 0;
} */

/****************************************************************/
/*			DELAYL					*/

/*short delayl (short *x)
{
	if (rss==0)
		return *(x+1);
	else return 32;
} */

/****************************************************************/
/*			SCALEL					*/

/* Como entrada tiene el factor de escala logaritmico retrasado */
/* La salida es el factor de escala logaritmico actualizado */
/* Utiliza la tabla iLB */

short scalel(short nbpL_a)
{ 
	short wd1, wd2, wd3;
	
	wd1= ((nbpL_a>>6)&31);
	wd2= nbpL_a>>11;

	wd3= iLB [wd1]>>(8-wd2);

	if (wd2==9)
	{
		return 32767;
	}
	else
	{
		return wd3<<3;
	};
}

/*short pasa_log_lin (short nbpL_a)
{
	short aux=(nbpL_a>>9)&63;
	return tablaloginv[aux];
} */


/************************************************************************/
/*		Implementacion de las funciones del bloque 4L		*/
/************************************************************************/
	

/************************************************************************/
/*			PARREC						*/

short parrec (short dLt_a, short szL_a)

/* La salida es pLt, la senial reconstruida parcial (partial rec)*/
/* Las entradas son la senial diferencia cuantizada y la senial 
predicha de los ceros*/

{
	return _add(dLt_a, szL_a);
}

/************************************************************************/
/*			RECONS						*/

short recons (short sL_a, short dLt_a)

/* La salida es la senial reconstruida para el predictor*/
/* Las entradas son la senial predicha y la diferencia cuantizada*/

{
	return _add(sL_a, dLt_a);
}

/************************************************************************/
/*			PREDIC						*/

short predic (short spL_a, short szL_a)

/* La salida es la senial predicha */
/* Las entradas son las contribuciones a la senial predicha de los polos y 
ceros */

{
	return _add(spL_a, szL_a);
}

/************************************************************************/
/*			FILTEP						*/

short filtep (short *reconsLt_a, short *ppolosL_a)

/* La salida es la contribucion del predictor de los polos, spL, S -1...-14*/
/* Las entradas son los retrasos de la senial reconstruida y los polos retrasados*/

{
	short wd1, wd2;

	wd1= _add(*(reconsLt_a+1),*(reconsLt_a+1));
	wd1= _gmpy(wd1, *ppolosL_a);
	
	wd2= _add(*(reconsLt_a+2),*(reconsLt_a+2));
	wd2= _gmpy(wd2, *(ppolosL_a+1));
	
	return _add(wd1, wd2);
}

/************************************************************************/
/*			FILTEZ						*/

short filtez (short *diferLt_a, short *pcerosL_a)

/* La salida es la contribucion del predictor de los ceros, szL, S -1...-14*/
/* Las entradas son los retrasos de la diferencia de la senial cuantizada y 
los ceros retrasados*/

{
	short suma=0;
	short aux;
	int i;

	for (i=1; i<7; i++)
	{
		aux= _add(*(diferLt_a+i), *(diferLt_a+i));
		aux= _gmpy(aux, *(pcerosL_a+i-1));
		suma=_add(suma, aux);
	}

	return suma;
}
	


/************************************************************************/
/*			UPPOL1						*/

short uppol1 (short *ppolosL_a, short *ppolospL_a, short *parcialLt_a)

/* La salida es la actualizacion del primer coeficiente del predictor*/
/* Las entradas necesarias son AL1, APL2, PLT, PLT1*/

{
	short wd1, wd2;
	int sg0= (*parcialLt_a>>15);
	int sg1= (*(parcialLt_a+1)>>15);

	if (sg0==sg1)
		wd1=192;
	else
		wd1=-192;

	wd2=_gmpy(32640, *ppolosL_a);
	wd1=_add(wd1,wd2); /*esto seria ya APL1*/

	wd2=_sub(15360,*(ppolospL_a+1)); /* para los limites de APL1*/

	if ((wd1<=wd2)&&(wd1>=-wd2))
		return wd1;
	else
		if (wd1>wd2)
			return wd2;
		else /* (wd1<-wd2)*/
			return -wd2;
}


/************************************************************************/
/*			UPPOL2						*/

short uppol2 (short *ppolosL_a, short *parcialLt_a)

/* La salida es la actualizacion del segundo coeficiente del predictor*/
/* Las entradas necesarias son AL1, AL2, PLT, PLT1, PLT2*/

{
	short wd1, wd2;

	int sg0= (*parcialLt_a>>15);
	int sg1= (*(parcialLt_a+1)>>15);
	int sg2= (*(parcialLt_a+2)>>15);

	wd1=_add(*ppolosL_a, *ppolosL_a);

	if (sg0==sg1)
		wd1=-wd1;
	else; /*(sg0!=sg1) wd1=wd1*/

	wd1=wd1>>7;

	if (sg0==sg2)
		wd2=128;
	else  /*sg0!=sg2*/
		wd2=-128;

	wd2=_add(wd2,wd1);

	wd1=_gmpy(32512, *(ppolosL_a+1));
	wd1=_add(wd2, wd1);

	if ((wd1<=12288)&&(wd1>=-12288))
		return wd1;
	else
		if (wd1>12288)
			return 12288;
		else /*(wd1<-12288)*/
			return -12288;
}

/************************************************************************/
/*			UPZERO						*/

void upzero (short *diferLt_a, short *pcerosL_a, short *pcerospL_a)

/* La salida es en realidad short *pcerospL_a, los ceros actualizados*/
/* Las entradas son las diferencias de senial cuantizadas y los ceros 
anteriores*/

{
	short wd1, wd2, wd3;
	int sg0, sgi, i;

	if (*diferLt_a==0)
		wd1=0;
	else
		wd1=128;
	sg0=(*diferLt_a>>15);

	for (i=1; i<7; i++)
	{
		sgi=(*(diferLt_a+i)>>15);
		if (sgi==sg0)
			wd2=wd1;
		else
			wd2=-wd1;
		wd3=_gmpy(*(pcerosL_a+i-1), 32640);
		*(pcerospL_a+i-1)=_add(wd2, wd3);
	}
}

/************************************************************************/
/*	Implementacion de las funciones del bloque 6L			*/
/*************************************************************************/

/*************************************************************************/
/*		LIMIT							*/

short limit(short YL_a)
{
	short aux;

	if ((YL_a<16383)&(YL_a>-16384))
		aux=YL_a;
	else
	{
		if (YL_a>16383)
			aux=16383;
		else 
			aux=-16384;
	};

	return aux;
}

/************************************************************************/
/*	Implementacion de las funciones del bloque 1H			*/


/************************************************************************/
/*			SUBTRA						*/

/* Utilizar la implementacion para la parte baja con las 
entradas xH y sH*/

/************************************************************************/
/*			QUANTH						*/

unsigned char quanth(short eH_a, short detH_a)

{
	short wd;
	unsigned char mih;
	int sih=eH_a>>15; /*signo de eH*/
	if (sih==0)		/*para hallar la magnitud de eH*/
		wd =eH_a;
	else
		wd=(32767-eH_a)&32767;

	if ((0<=wd)&&(wd<(_gmpy(4512, detH_a)))) /*para hallar el nivel de decision*/
		mih=1;
	else mih=0;

	/* se deberia usar la tabla 14/G722 y la 20/G722. Al ser muy pequenias se ha
	sustituido por una funciona*/

	if (sih|=0)
		return mih;
	else return (mih+2);
}



/****************************************************************************/
/*			BLOQUE 2H					*/
/****************************************************************************/

/*************************************************************************/
/*		INVQAH							*/		

short  invqah(short iH_a, short detH_a)
{
	short wd1, wd2;

	wd1=iH2[iH_a];/* tabla 21/G722*/
	wd2=qq2[wd1];/* usa la tabla 14/G722 modificada (<<3)*/
	
	if ((iH_a==0)||(iH_a==1)) /*signo del intervalo*/
	{
		return -(_gmpy(detH_a, wd2));/*signo =-1*/
	}
	else
	{
		return _gmpy(detH_a, wd2);/*signo =0*/
	}
}

/****************************************************************************/
/*			BLOQUE 3H					*/
/*************************************************************************/

/****************************************************************************/
/*		LOGSCH							*/

short logsch (short iH_a, short nbH_a)
{
	short wd1;
	wd1= _gmpy(32512, nbH_a);/* factor 127/128*/
	wd1= wd1+ wH[iH2[iH_a]];	/*suma de factor de correccion*/

	if (wd1<0)
		return 0; /*limite inferior de 0*/
	else
	{
		if (wd1>22528)
		{
			return 22528;/* limite superior de 11*/
		}
		else
		{
			return wd1;
		}
	};
}

/****************************************************************************/
/*		SCALEH							*/

short scaleh (short nbpH_a)
{	/* Utilizamos el segundo metodo*/

	short wd1, wd2, wd3;
	wd1=(nbpH_a>>6)&31;
	wd2=nbpH_a>>11;
	wd3=(iLB[wd1])>>(10-wd2);
	if (wd2==11)
	{
		return 32767;
	}
	else
	{
		return wd3<<3;
	};
}

/*short pasa_log_linh (short nbpH_a)
{
	short aux=(nbpH_a>>9)&63;
	return tablaloginvh[aux];
} */


/****************************************************************************/
/*		DELAYH							*/
/* No la escribo porque despues no la voy a usar*/

/***************************************************************************/
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

short invqbl (unsigned char iLr_a, short detL_a, int mode_a)
{
	unsigned char ril;
	int sil=0;
	short wd1=0;

	if (mode_a==1){
		ril=iLr_a;
		wd1=(qq6[iL6[ril]+1]<<3);
		if ((31<ril)&&(ril<62))
			{sil=0;}
		else
			{sil=1;};
	}
	if (mode_a==2){
		ril=(iLr_a>>1);
		wd1=(qq5[iL5[ril]+1]<<3);
		if ((16<ril)&&(ril<31))
			{sil=0;}
		else
			{sil=1;};
	}
	if (mode_a==3){
		ril=(iLr_a>>2);
		wd1=(qq4[iL4[ril]+1]<<3);
		if ((0<ril)&&(ril<8))
			{sil=1;}
		else
			{sil=0;};
	}

	/*switch(mode_a){
	case '1':
		ril=iLr_a;
		wd1=(qq6[iL6[ril]+1]<<3);
		if ((31<ril)&&(ril<62))
			{sil=0;}
		else
			{sil=1;};
		break;
	case '2':
		ril=(iLr_a>>1);
		wd1=(qq5[iL5[ril]+1]<<3);
		if ((16<ril)&&(ril<31))
			{sil=0;}
		else
			{sil=1;};
		break;
	case '3':
		ril=(iLr_a>>2);
		wd1=(qq4[iL4[ril]+1]<<3);
		if ((0<ril)&&(ril<8))
			{sil=1;}
		else
			{sil=0;};
		break;
	default:
		break;
	};*/

	if (sil==0){	/*signo positivo*/
		return _gmpy(wd1, detL_a);
	}
	else{  /*signo negativo*/
		return -_gmpy(wd1, detL_a);
	};
}

