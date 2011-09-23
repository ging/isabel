/*************************************************************************/
/********** Fecha: 13 de abril de 1999 ***********************************/
/********** FICHERO TODO.C  **********************************************/
/* Funciones:

	RESETEO		Hace un reset al valor adecuado de las variables del
				codificador y decodificador  segun este definido en la
				norma.
	LEO_IHILR	Extrae de la palabra que se transmite por el canal la
				informacion de las palabras codificadas alta y baja. 
				Realiza	la demultiplexacion.
	SACO_RHL	Saca en un long los dos resultados de la codificacion-
				decodificacion. Daba problemas (funciona?), asi que se
				sustituyo por un array.

					NO SE USA.

	DECOD		Esta funcion realiza la decodificacion de una muestra.
	CODIF		Esta funcion realiza la codificacion de una muestra.
*/

/*************************************************************************/
/* Autora: Anabel Glez- Tablas						 */
/*************************************************************************/


/*#define	SINTESIS	1*/



#include <stdio.h>
#include <stdlib.h>
/*#include <conio.h>*/


#include <math.h>

#include "vardec.h"

/* definicion de todas las variables */
#include "funccf.h"
/* funciones definidas en la norma, exceptuando las de los filtros */
#include "decod.h"

G722StateDecoder *G722StateDecoderNew() {
   int i;
   G722StateDecoder *state;
   
   if ( !(state = (G722StateDecoder *)malloc(sizeof(G722StateDecoder))) ) {
      fprintf(stderr, "G722StateDecoder::malloc\n");
      return NULL;
   }
   for ( i = 0 ; i < 24 ; i++ ) {
      state -> buffQmfRx[i] = 0;
   }
   state -> wp_buffQmfRx = 0;
   state -> rss = 0;
   state -> mode = 0;
   state -> decodiH = 0;
   state -> decodiLr = 0;
   state -> decodsL = 0;
   state -> decodsH = 0;
   state -> decoddL = 0;
   state -> decodspL = 0;
   state -> decodszL = 0;
   state -> decodspH = 0;
   state -> decodszH = 0;
   for ( i = 0 ; i < 2 ; i++ ) {
      state -> decoddetL[i] = 32;
      state -> decoddetH[i] = 8;
      state -> decodnbL[i]=0;
      state -> decodnbH[i]=0;
      state -> decodapL[i]=0;
      state -> decodapH[i]=0;
      state -> decodaL[i]=0;
      state -> decodaH[i]=0; 
   }
   state -> decodf_esc_linL = state -> decoddetL;
   state -> decodf_esc_linH = state -> decoddetH;
   state -> decodf_esc_logL = state -> decodnbL;
   state -> decodf_esc_logH = state -> decodnbH;
   state -> decodppolosL = state -> decodapL;
   state -> decodppolospL = state -> decodaL;
   state -> decodppolosH = state -> decodapH;
   state -> decodppolospH = state -> decodaH; 
   for ( i = 0 ; i < 6 ; i++ ) {
      state -> decodbpL[i] = 0;
      state -> decodbpH[i] = 0;
      state -> decodbL[i] = 0;
      state -> decodbH[i] = 0;
   }
   state -> decodpcerosL = state -> decodbpL;
   state -> decodpcerospL = state -> decodbL;
   state -> decodpcerosH = state -> decodbpH;
   state -> decodpcerospH = state -> decodbH;
   for ( i = 0 ; i < 7 ; i++ ) {
      state -> decoddLt[i] = 0;
      state -> decoddH[i] = 0;
   }
   for ( i = 0 ; i < 3 ; i++ ) {
      state -> decodrLt[i] = 0;
      state -> decodrH[i] = 0;
      state -> decodpLt[i] = 0;
      state -> decodpH[i] = 0;
   }
   state -> decoddiferLt = state ->decoddLt; 
   state -> decoddiferHt = state ->decoddH; 
   state -> decodreconsLt = state ->decodrLt; 
   state -> decodreconsHt = state -> decodrH; 
   state -> decodparcialLt = state -> decodpLt; 
   state -> decodparcialH = state -> decodpH; 
   return state;
}

/*************************************************************************/
/*	                      void reseteo ()                                 */
/*************************************************************************/
/* Realiza un reset al valor apropiado segun la norma. En la norma hay una
variable global 'rss'= flag de reset, dependiendo de su valor se resetean
las variables PARA CADA MUESTRA (cada muestra que se transmite por el canal
lleva asociado un bit de reset). Aqui hemos optado por realizar una rutina
suponiendo que dentro de la trama de voz no vamos a encontrar ningun reset.
La rutina se utiliza para inicializar el codec reseteandolo 16 veces
(equivalente a recibir 16 muestras con el bit de reset activado).
	ENTRADAS:	  ninguna
	SALIDAS:	  ninguna
	AFECTA A:	  coddiferLt, coddiferHt, decoddiferLt, decoddiferHt
				  codpcerosL, codpcerosH, decodpderosL, decodpcerosH
				  codppolosL, codppolosH, decodppolosL, decodppolosH
				  codreconsLt, codreconsHt, decodreconsLt, decodreconsHt
				  codf_esc_logL, codf_esc_linL
				  codf_esc_logH, codf_esc_linH
				  decodf_esc_logL, decodf_esc_linL
				  decodf_esc_logH, decodf_esc_linH
				  codparcialLt, codparcialH, decodparcialLt, decodparcialH
*/
/***************************************************************************/

/*void reseteo ()
{
	int i;

	for (i=1; i<7; i++)
	{
		*(coddiferLt+i)=0;
		*(coddiferHt)=0;
		*(decoddiferLt+i)=0;
		*(decoddiferHt)=0;

	}
	for (i=0;i<6;i++)
	{
		*(codpcerosL+i)=0;
		*(codpcerosH+i)=0;
		*(decodpcerosL+i)=0;
		*(decodpcerosH+i)=0;
	}
	for (i=0;i<2;i++)
	{
		*(codppolosL+i)=0;
		*(codppolosH+i)=0;
		*(decodppolosL+i)=0;
		*(decodppolosH+i)=0;
	}
	for (i=1; i<3; i++)
	{
		*(codreconsLt+i)=0;
		*(codreconsHt+i)=0;
		*(decodreconsLt+i)=0;
		*(decodreconsHt+i)=0;
	}
	*(codf_esc_logL)=0;
	*(codf_esc_linL+1)=32;
	*(codf_esc_logH)=0;
	*(codf_esc_linH+1)=8;
	*(decodf_esc_logL)=0;
	*(decodf_esc_linL+1)=32;
	*(decodf_esc_logH)=0;
	*(decodf_esc_linH+1)=8;
	for (i=1; i<3; i++)
	{
		*(codparcialLt+i)=0;
		*(codparcialH+i)=0;
		*(decodparcialLt+i)=0;
		*(decodparcialH+i)=0;

	}

}
*/

/*************************************************************************/
/*     			void leo_iHiLr
      (unsigned char in_rx, unsigned char *iH_a,unsigned char *iLr_a)    */
/*************************************************************************/
/* Demultiplexa la palabra transmitida por el canal en las palabras baja y
alta.
	ENTRADAS:	(unsigned char) in_rx.
				Formato: IH2 IH1 IL6 IL5 IL4 IL3 IL2 IL1
				Son las palabras codificadas alta y baja.

	SALIDAS: 	(unsigned char) iH_a.
				Formato: 0 0 0 0 0 0 IH2 IH1
				Palabra alta codificada.

			(unsigned char) iL_a.
				Formato: 0 0 IL6 IL5 IL4 IL3 IL2 IL1
				Palabra baja codificada.
*/
/*************************************************************************/

void leo_iHiLr (unsigned char in_rx, unsigned char *iH_a,unsigned char *iLr_a)
/* extraigo las palabras alta y baja codificadas*/
{
	/* en el demultiplexado es donde se tiene que extraer la informacion de
	reset si la muestra la lleva. Actualmente las muestras no llevan reset */
	/*	*rss_a= in_rx&1;*/

	*iLr_a=((in_rx)&63); /*palabra baja */
	*iH_a=(in_rx>>6);		/*palabra alta */
}

/*************************************************************************/
/*************************************************************************/
/*                         NO SE USA                                     */

/*long saco_RHL(short rH_a, short rL_a)

{
	return ((long)(rH_a))<<16+(long)(rL_a);

}
*/


/*************************************************************************/
/*        void decod(unsigned char palabra, short *psalest_a)            */
/*************************************************************************/
/* Realiza la decodificacion de una muestra.
	ENTRADAS:	(unsigned char) palabra
				Formato:Formato: IH2 IH1 IL6 IL5 IL4 IL3 IL2 IL1
				Son las palabras codificadas alta y baja.

	SALIDAS:	(short *) psalest_a
				Puntero a un array de dos posiciones en donde se van a
				guardar la decodificacion para las subbandas baja y alta.
				* (psalest) <> decodificacion subbanda alta
				* (psalest+1) <> decodificacion subbanda baja

	AFECTA A:	Todas las variables definidas por la norma para la
				decodificacion, exceptuando el reset.

	UTILIZA:       leo_iHiLr (definida arriba)
				Todas las funciones definidas por la norma para la
				decodificacion, exceptuando delaya, delayl, delayh.
*/
/*************************************************************************/

void decod(G722StateDecoder *sd, unsigned char palabra, short *psalest_a, int mode_a)
{

	/* Codigo subbanda inferior y superior, respectivamente.*/
	unsigned char	codigoL, codigoH;

	/* salidas baja y alta del codificador - decodificador encadenado */
	short entrada_auxiliar_L, entrada_auxiliar_H;

	/*punteros para actualizar los arrays*/
	short *aux1, *aux0;

	int i;

	sd -> mode=mode_a;

	/***********************************************************************/
	/**********************     DEMULTIPLEXADO        **********************/
	/* extraigo de la palabra transmitida la información de las	palabras alta
	y baja con leo_iHiLr */
	/***********************************************************************/

	/* demultiplexado */
	leo_iHiLr (palabra, &sd -> decodiH, &sd -> decodiLr);

	/* pasamos los valores de las variables globales decodiH y decodiLr
	a las variables codigoL y codigoH por claridad */

	codigoL=sd -> decodiLr;
	codigoH=sd -> decodiH;

	/***********************************************************************/
	/**************    DECODIFICACION MUESTRA SUBBANDA INFERIOR   **********/
	/***********************************************************************/

	/***********************************************************************/
	/*************** CUANTIFICACION INVERSA	DEPENDIENTE DE MODO	********/
	/* Se hace pasar 'codigoL' por un cuantificador inverso de 6 bits,de 5
	o de	4 (según el modo) para obtener 'error', diferencia entre la senial
	original	y la prediccion */
	/**********************************************************************/


	/* se elije el modo tres, es decir, se toman sólo cuatro bits de
	los seis transmitidos para la parte baja. Equivalente a insertar
	dos bits de información adicional */

	/*mode =3;*/
	

	/* cuantificación inversa dependiendo del modo */

	sd -> decoddL=invqbl(codigoL, *(sd -> decodf_esc_linL), sd -> mode);


	/**********************************************************************/
	/** CALCULO DE LA PREDICCION DE LA MUESTRA Y DE LA RECONSTRUCCION *****/
	/**********************************************************************/

	/* esta parte del reset se ha quitado porque suponemos que no va a haber reset
	en mitad del fichero de voz */

	/* reseteamos */
	/*
	if (rss==1){
		for (i=1; i<7; i++)
			*(diferLt+i)=0;
		for (i=0;i<6;i++)
			*(pcerosL+i)=0;
		for (i=0;i<2;i++)
			*(ppolosL+i)=0;
		for (i=1; i<3; i++)
			*(reconsLt+i)=0;
	}
	else;
	*/

	/* Obtengo la prediccion de la muestra */

	sd -> decodszL=filtez(sd -> decoddiferLt, sd -> decodpcerosL);
	/* obtengo la salida de la prediccion	de los ceros*/
	sd -> decodspL=filtep(sd -> decodreconsLt, sd -> decodppolosL);
	/*obtengo la salida de la predicion de los polos*/
	sd -> decodsL=predic(sd -> decodspL, sd -> decodszL);
	/* sumo ambas contribuciones obteniendo la predicción de la muestra */


	/* este recons (en la siguiente linea de codigo), parte del bloque 5,
	'reconstructed signal calculator', tiene que ir aqui porque tiene que
	ser despues de haber calculado la prediccion; se tiene que calcular
	con dL y no con dLt=*diferLt. Esto es porque para calcular la salida
	del decodificador se emplean todos los bits de informacion valida
	--> error<>dL; sin embargo para 'uso interno' del decodificador solo
	se emplean los cuatro bits, igual que el codificador --> error<>dLt*/

	entrada_auxiliar_L=recons(sd -> decodsL,sd -> decoddL);
	/* reconstruimos la señal a partir de la predicción y de la diferencia*/

	/* ESTA VA A SER LA SALIDA DE LA PARTE BAJA DEL DECODIFICADOR */
	/*	:::				ENTRADA_AUXILIAR_L			*/

	entrada_auxiliar_L=limit(entrada_auxiliar_L);
	/* limitamos su valor */

	/**********************************************************************/
	/************ CUANTIFICACION INVERSA PARA RECONSTRUCCION	**************/
	/* Volvemos a cuantificar pero esta vez con cuatro bits unicamente, tal
	como hace el codificador */
	/**********************************************************************/

	*(sd -> decoddiferLt)=invqal(codigoL, *(sd -> decodf_esc_linL));


	/**********************************************************************/
	/******************* FACTOR DE ESCALA *********************************/
	/* Adapto el factor de escala del cuantificador para la siguiente muestra
	de entrada. */
	/**********************************************************************/

	/*reseteo; ahora está en la rutina de reseteo */
	/*if (rss==1)
		*(f_esc_logL)=0;
	else ;*/	/*delaya, reseteo si es el caso*/


	*(sd -> decodf_esc_logL+1)= logscl(codigoL, *(sd -> decodf_esc_logL));
	/* adapto el factor de escala logaritmico*/
	*(sd -> decodf_esc_logL)=*(sd -> decodf_esc_logL+1);
	/* lo actualizo en su array*/

	*(sd -> decodf_esc_linL+1)=scalel(*(sd -> decodf_esc_logL));
	/* hallo la actualizacion del fac. esc. lineal desde el nuevo f.esc.log.*/

	/*reseteo ...*/
	/*if (rss==1)
		*(f_esc_linL+1)=32;
	else;*/ /*delayl*/

	*(sd -> decodf_esc_linL)=*(sd -> decodf_esc_linL+1);
	/* actualizo el array*/

	/**********************************************************************/
	/******************* ADAPTACION DEL PREDICTOR *************************/
	/* Recalculo los polos y ceros del predictor */
	/**********************************************************************/

	/* Preparo la senial 'poloadL' que debe usar el adaptador del
	predictor.*/

	upzero( sd -> decoddiferLt, sd -> decodpcerosL, sd -> decodpcerospL);
	/*adapto los ceros del predictor*/

	*(sd -> decodparcialLt)=parrec(*(sd -> decoddiferLt), sd -> decodszL);
	/*hallo la senial reconstruida parcial*/

	/* reseteo...*/
	/*reseteo el array parcialLt, si es el caso*/
	/*if (rss==1)
		for (i=1; i<3; i++)
			*(parcialLt+i)=0;
	else;*/

	*(sd -> decodreconsLt)=recons(sd -> decodsL, *(sd -> decoddiferLt));
	/*hallo la senial reconstruida*/

	/* Hallo los nuevos polos*/
	*(sd -> decodppolospL+1)=uppol2(sd -> decodppolosL, sd -> decodparcialLt);
	*(sd -> decodppolospL)=uppol1(sd -> decodppolosL, sd -> decodppolospL, sd -> decodparcialLt);


	/**********************************************************************/
	/* Actualizamos (retrasamos) el array rLt y pLt y dLt antes de coger
	la siguiente muestra, y los ceros y polos*/
	/**********************************************************************/

	aux1 = sd -> decoddiferLt + 6;
	aux0 = sd -> decoddiferLt + 5;
	for ( i = 0; i < 6; i++)
	{
		*aux1-- = *aux0--;
		*(sd -> decodpcerosL+i)=*(sd -> decodpcerospL+i);
	};

	aux1 = sd -> decodparcialLt + 2;
	aux0 = sd -> decodparcialLt + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
		*(sd -> decodppolosL+i)=*(sd -> decodppolospL+i);
	};

	aux1 = sd -> decodreconsLt + 2;
	aux0 = sd -> decodreconsLt + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
	};

	/**********************************************************************/
	/**************** Fin decodificacion banda baja ***********************/
	/**********************************************************************/


	/**********************************************************************/
	/**********   DECODIFICACION MUESTRA SUBBANDA SUPERIOR       **********/
	/**********************************************************************/

	/***********************************************************************/
	/*************** CUANTIFICACION INVERSA	******************************/
	/* Se hace pasar 'codigoH' por un cuantificador inverso de 2 bits, para
	obtener 'error', diferencia entre la senial original y la prediccion.*/
	/***********************************************************************/

	*(sd -> decoddiferHt)=invqah(codigoH, *(sd -> decodf_esc_linH));


	/**********************************************************************/
	/** CALCULO DE LA PREDICCION DE LA MUESTRA Y DE LA RECONSTRUCCION *****/
	/**********************************************************************/

	/* En la rutina de reseteo.... */
	/*reseteamos*/
	/*if (rss==1){
		for (i=1; i<7; i++)
			*(diferHt+i)=0;
		for (i=0;i<6;i++)
			*(pcerosH+i)=0;
		for (i=0;i<2;i++)
			*(ppolosH+i)=0;
		for (i=1; i<3; i++)
			*(reconsHt+i)=0;
	}
	else;*/

	/*calculo la prediccion de la muestra */

	sd -> decodszH=filtez(sd -> decoddiferHt, sd -> decodpcerosH);
	/* obtengo la salida de la prediccion	de los ceros*/
	sd -> decodspH=filtep(sd -> decodreconsHt, sd -> decodppolosH);
	/*obtengo la salida de la predicion de los polos*/
	sd -> decodsH=predic(sd -> decodspH, sd -> decodszH);
	/* sumo ambas contribuciones*/



	entrada_auxiliar_H=recons(sd -> decodsH,*(sd -> decoddiferHt));
	/* Reconstruyo la salida de la parte alta.*/

	/* ESTA ES LA SALIDA DEL DECODIFICADOR DE LA BANDA ALTA */
	/* ::::			ENTRADA_AUXILIAR_H						*/

	entrada_auxiliar_H=limit(entrada_auxiliar_H);
	/* limito su valor*/


	/**********************************************************************/
	/****************** FACTOR DE ESCALA **********************************/
	/* Adapto el factor de escala de cuantificacion para la siguiente muestra
	de entrada.*/
	/**********************************************************************/

	/* reseteo...*/
	/*if (rss==1)
		*(f_esc_logH)=0;
	else ;*/	/*delaya, reseteo si es el caso*/

	*(sd -> decodf_esc_logH+1)= logsch(codigoH, *(sd -> decodf_esc_logH));
	/* adapto el factor de escala logaritmico*/
	*(sd -> decodf_esc_logH)=*(sd -> decodf_esc_logH+1);
	/* lo actualizo en su array*/

	*(sd -> decodf_esc_linH+1)=scaleh(*(sd -> decodf_esc_logH));
	/* hallo la actualizacion del fac. esc. lineal desde el nuevo f.esc.log.*/

	/* reseteo...*/
	/*if (rss==1)
		*(f_esc_linH+1)=8;
	else;*/ /*delayh*/

	*(sd -> decodf_esc_linH)=*(sd -> decodf_esc_linH+1);
	/* actualizo el array*/

	/**********************************************************************/
	/****************** ADAPTACION DEL PREDICTOR **************************/
	/**********************************************************************/

	/* Preparo la senial 'poloadL' que debe usar el adaptador del predictor*/

	upzero( sd -> decoddiferHt, sd -> decodpcerosH, sd -> decodpcerospH);
	/*adapto los ceros del predictor*/

	*(sd -> decodparcialH)=parrec(*(sd -> decoddiferHt), sd -> decodszH);
	/*hallo la senial reconstruida parcial*/

	/* reseteo...*/
	/*reseteo el array parcialLt, si es el caso*/
	/*if (rss==1)
		for (i=1; i<3; i++)
			*(parcialH+i)=0;
	else;*/

	*(sd -> decodreconsHt)=recons(sd -> decodsH, *(sd -> decoddiferHt));
	/*hallo la senial reconstruida*/

	/* Hallo los nuevos polos*/
	*(sd -> decodppolospH+1)=uppol2(sd -> decodppolosH, sd -> decodparcialH);
	*(sd -> decodppolospH)=uppol1(sd -> decodppolosH, sd -> decodppolospH, sd -> decodparcialH);

	/***********************************************************************/
	/*Actualizamos (retrasamos) el array rHt y pHt y dHt antes de coger la
	siguiente muestra, y los ceros y polos*/
	/***********************************************************************/

	aux1 = sd -> decoddiferHt + 6;
	aux0 = sd -> decoddiferHt + 5;
	for ( i = 0; i < 6; i++)
	{
		*aux1-- = *aux0--;
		*(sd -> decodpcerosH+i)=*(sd -> decodpcerospH+i);
	};

	aux1 = sd -> decodparcialH + 2;
	aux0 = sd -> decodparcialH + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
		*(sd -> decodppolosH+i)=*(sd -> decodppolospH+i);
	};

	aux1 = sd -> decodreconsHt + 2;
	aux0 = sd -> decodreconsHt + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
	};


	/**********************************************************************/
	/*********** Fin de decodificación de la banda superior ***************/
	/**********************************************************************/

	/**********************************************************************/
	/* Ponemos las salidas en el array destinado a ello*/
	/**********************************************************************/

	*psalest_a=entrada_auxiliar_H;
	*(psalest_a+1)=entrada_auxiliar_L;

}



void calcRxQmf(G722StateDecoder *sd, short xL, short xH, short *x1,short *x2) {
   const short qmfCoeffs[]={1,-11,-11,53,12,-156,32,362,-210,-805,951,3876,3876,951,-805,-210,362,32,-156,12,53,-11,-11,1};

	/* x1 y x2 son punteros con los que devolvemos las muestras de salida */
	/* xL y xH son los valores de las entradas */

	long aux1,aux2; /* necesito de 32 bit */
	short i;

	aux1=xL-xH; /* en la norma se llama recA */
	aux2=xL+xH; /* en la norma se llama recB */

	/* los introducimos en los buffers */
	/* y avanzamos puntero */

	sd -> buffQmfRx[sd -> wp_buffQmfRx++]=(short)aux2;  /* esto es recB */
	sd -> wp_buffQmfRx %= 24;

	sd -> buffQmfRx[sd -> wp_buffQmfRx++]=(short)aux1;  /* esto es recA */
	sd -> wp_buffQmfRx %= 24;

	/* calculamos el qmf */

	aux1=0;
	aux2=0;

	for (i=0; i<=23; ) {

		aux2+=(long)sd -> buffQmfRx[sd -> wp_buffQmfRx++]*qmfCoeffs[i++]; /* es el xout2 de la norma */
		sd -> wp_buffQmfRx %= 24;

		aux1+=(long)sd -> buffQmfRx[sd -> wp_buffQmfRx++]*qmfCoeffs[i++]; /* es el xout1 de la norma */
		sd -> wp_buffQmfRx %= 24;
	}

    /*AGTF Aqui hay que multiplicar por dos, esta en la norma*/
	/*aux1=aux1/8192;
	aux2=aux2/8192;*/
	aux1=aux1/4096;
	aux2=aux2/4096;

	aux1 = (aux1>16383)?16383:aux1;
	aux1 = (aux1<-16384)?-16384:aux1;
	aux2 = (aux2>16383)?16383:aux2;
	aux2 = (aux2<-16384)?-16384:aux2;

	*x1=(short)aux1;
	*x2=(short)aux2;
}           
