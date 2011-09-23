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



#include <stdio.h>
#include <stdlib.h>
/*#include <conio.h>*/

#include <math.h>
/*#include "cambio.h"*/
/*#include "convert.h"*/

#include "varcod.h"

/* definicion de todas las variables */
#include "funccf.h"
/* funciones definidas en la norma, exceptuando las de los filtros */
#include "codif.h"


G722StateEncoder *G722StateEncoderNew() {
   int i;
   G722StateEncoder *state;
   
   if ( !(state = (G722StateEncoder *)malloc(sizeof(G722StateEncoder))) ) {
      fprintf(stderr, "G722StateEncoder::malloc\n");
      return NULL;
   }
   for ( i = 0 ; i < 24 ; i++ ) {
      state -> buffQmfTx[i] = 0;
   }
   state -> wp_buffQmfTx = 0;
   state -> codiL = 0;
   state -> codiH = 0;
   state -> codsL = 0;
   state -> codsH = 0;
   state -> codeL = 0;
   state -> codeH = 0;
   state -> codspL = 0;
   state -> codspH = 0;
   state -> codszL = 0;
   state -> codszH = 0;

   for ( i = 0 ; i < 2 ; i++ ) {
      state -> coddetL[i] = 32;
      state -> coddetH[i] = 8;
      state -> codnbL[i]=0;
      state -> codnbH[i]=0;
      state -> codapL[i]=0;
      state -> codapH[i]=0;
      state -> codaL[i]=0;
      state -> codaH[i]=0; 
   }
   state -> codf_esc_linL = state -> coddetL;
   state -> codf_esc_linH = state -> coddetH;
   state -> codf_esc_logL = state -> codnbL;
   state -> codf_esc_logH = state -> codnbH;
   state -> codppolosL = state -> codapL;
   state -> codppolospL = state -> codaL;
   state -> codppolosH = state -> codapH;
   state -> codppolospH = state -> codaH; 
   for ( i = 0 ; i < 6 ; i++ ) {
      state -> codbpL[i] = 0;
      state -> codbpH[i] = 0;
      state -> codbL[i] = 0;
      state -> codbH[i] = 0;
   }
   state -> codpcerosL = state -> codbpL;
   state -> codpcerospL = state -> codbL;
   state -> codpcerosH = state -> codbpH;
   state -> codpcerospH = state -> codbH;
   for ( i = 0 ; i < 7 ; i++ ) {
      state -> coddLt[i] = 0;
      state -> coddH[i] = 0;
   }
   for ( i = 0 ; i < 3 ; i++ ) {
      state -> codrLt[i] = 0;
      state -> codrH[i] = 0;
      state -> codpLt[i] = 0;
      state -> codpH[i] = 0;
   }
   state -> coddiferLt = state ->coddLt; 
   state -> coddiferHt = state ->coddH; 
   state -> codreconsLt = state ->codrLt; 
   state -> codreconsHt = state -> codrH; 
   state -> codparcialLt = state -> codpLt; 
   state -> codparcialH = state -> codpH; 
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
/*            unsigned char codif(short xL_a, short xH_a)                */
/*************************************************************************/
/* Realiza la codificacion de una muestra, una vez se han separado la banda
baja y alta.
		
	ENTRADAS:	(short) xL_a. Es la parte correspondiente a la subbanda baja
				tal como se define xL en la norma.

			(short) xH_a. Es la parte correspondiente a la subbanda alta
				tal como se define xH en la norma.

	SALIDAS:	(unsigned char). Es la codificación de las dos subbandas.
				Formato: IH2 IH1 IL6 IL5 IL4 IL3 IL2 IL1
				Son las palabras codificadas alta y baja.

	AFECTA A:	Todas las variables definidas por la norma para la 
				codificacion, exceptuando el reset.

	UTILIZA:	Todas las funciones definidas por la norma para la
				codificacion, exceptuando delaya, delayl, delayh.
 
*/
/*************************************************************************/
unsigned char codif(G722StateEncoder *se, short xL_a, short xH_a)
{

	/*punteros para actualizar los arrays*/
	short *aux1, *aux0;
	int i;

	/***********************************************************************/
	/**********     CODIFICACION MUESTRA SUBBANDA INFERIOR        **********/
	/***********************************************************************/

	/************************************************************************/
	/*		PREPARACION DE LAS MUESTRAS PARA LA CONF. 1 		*/
	/*infa (*aux_entrada_s, &xL, &xH, &rss);*/
	/* infa es la funcion que extrae la señal de reset, cambia a 15 bits
	significativos X 1...15, y obtiene xL y xH*/

	/************************************************************************/
	/***************  CALCULO DE LA PREDICCION	****************************/
	/*reseteamos*/
	/*if (rss==1){
		for (i=1; i<7; i++)
		*(diferLt+i)=0;
		for (i=0;i<6;i++)
			*(pcerosL+i)=0;
		for (i=0;i<2;i++)
			*(ppolosL+i)=0;
		for (i=1; i<3; i++)
			*(reconsLt+i)=0;
	}
	else;*/


	se -> codszL=filtez(se -> coddiferLt, se ->codpcerosL);
	/* obtengo la salida de la prediccion	de los ceros*/
	se -> codspL=filtep(se -> codreconsLt, se -> codppolosL);
	/*obtengo la salida de la predicion de	los polos*/
	se -> codsL=predic(se -> codspL, se -> codszL);
	/* sumo ambas contribuciones*/

	/************************************************************************/
	/********************** ERROR EN LA PREDICCION **************************/
	/* Calculo el error de prediccion */
	/************************************************************************/

	se -> codeL= subtra (xL_a, se -> codsL);

	/************************************************************************/
	/************ CUANTIFICACION DEL ERROR EN LA PREDICCION *****************/
	/************************************************************************/

	se -> codiL= cuantl (se -> codeL,*(se -> codf_esc_linL));/* cuantifico el error de prediccion*/


	/************************************************************************/
	/******************** SENIAL DIFERENCIA CUANTIZADA **********************/
	/* hallo la senial diferencia	cuantizada-inversa */
	/************************************************************************/

	*(se -> coddiferLt)= invqal(se -> codiL, *(se -> codf_esc_linL));

	/************************************************************************/
	/********************* ADAPTACION FACTOR DE ESCALA	**********************/
	/* Adapto el factor de escala del cuantificador para la siguiente muestra
	de entrada.*/
	/************************************************************************/

	/* en la rutina de reseteo...*/
	/*if (rss==1)
		*(f_esc_logL)=0;
	else ;*/	/*delaya, reseteo si es el caso*/

	*(se -> codf_esc_logL+1)= logscl(se -> codiL, *(se -> codf_esc_logL));
	/* adapto el factor de escala logaritmico*/
	*(se -> codf_esc_logL)=*(se -> codf_esc_logL+1);
	/* lo actualizo en su array*/

	*(se -> codf_esc_linL+1)=scalel(*(se -> codf_esc_logL));
	/* hallo la actualizacion del fac. esc. lineal desde el nuevo f.esc.log.*/

	/* en reseteo...*/
	/*if (rss==1)
		*(f_esc_linL+1)=32;
	else;*/ /*delayl*/

	*(se -> codf_esc_linL)=*(se -> codf_esc_linL+1);
	/* actualizo el array*/

	/*puedo actualizar (adelantar) aqui los arrays de los factores de escala,
	porque en la adaptacion del predictor no se usan*/


	/************************************************************************/
	/************************ ADAPTACION DEL PREDICTOR	**********************/
	/*	Preparo la senial 'poloadL' que debe usar el adaptador del predictor.*/
	/************************************************************************/

	upzero( se -> coddiferLt, se -> codpcerosL, se -> codpcerospL);
	/*adapto los ceros del predictor*/

	*(se -> codparcialLt)=parrec(*(se -> coddiferLt), se -> codszL);
	/*hallo la senial reconstruida parcial*/

	/* en la rutina de reseteo...*/
	/*reseteo el array parcialLt, si es el caso*/
	/*if (rss==1)
		for (i=1; i<3; i++)
			*(parcialLt+i)=0;
	else;*/

	*(se -> codreconsLt)=recons(se -> codsL, *(se -> coddiferLt));
	/*hallo la senial reconstruida*/

	/* Hallo los nuevos polos*/
	*(se -> codppolospL+1)=uppol2(se -> codppolosL, se -> codparcialLt);
	*(se -> codppolospL)=uppol1(se -> codppolosL, se -> codppolospL, se -> codparcialLt);


	/************************************************************************/
	/*Actualizamos (retrasamos) el array rLt y pLt y dLt antes de coger la
	siguiente muestra, y los ceros y polos*/
	/************************************************************************/

	aux1 = se -> coddiferLt + 6;
	aux0 = se -> coddiferLt + 5;
	for ( i = 0; i < 6; i++)
	{
		*aux1-- = *aux0--;
		*(se -> codpcerosL+i)=*(se -> codpcerospL+i);
	};

	aux1 = se -> codparcialLt + 2;
	aux0 = se -> codparcialLt + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
		*(se -> codppolosL+i)=*(se -> codppolospL+i);
	};

	aux1 = se -> codreconsLt + 2;
	aux0 = se -> codreconsLt + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
	};

	/************************************************************************/
	/**********         CODIFICACION MUESTRA SUBBANDA SUPERIOR     **********/
	/************************************************************************/

	/************************************************************************/
	/*********************** PREDICCION DE LA SENIAL ************************/
	/************************************************************************/

	/* en la reutina reseteo...*/
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


	se -> codszH=filtez(se -> coddiferHt, se -> codpcerosH);
	/* obtengo la salida de la prediccion de los ceros*/
	se -> codspH=filtep(se -> codreconsHt, se -> codppolosH);
	/*obtengo la salida de la predicion de	los polos*/
	se -> codsH=predic(se -> codspH, se -> codszH);
	/* sumo ambas contribuciones*/


	/************************************************************************/
	/****************** ERROR EN LA PREDICCION ******************************/
	/************************************************************************/

	se -> codeH = subtra(xH_a, se -> codsH);

	/************************************************************************/
	/************ CUANTIFICACION DEL ERROR EN LA PREDICCION *****************/
	/************************************************************************/

	se -> codiH= quanth(se -> codeH, *(se -> codf_esc_linH));
	/* cuantifico el error de prediccion*/

	/************************************************************************/
	/******************** SENIAL DIFERENCIA CUANTIZADA **********************/
	/* hallo la senial diferencia	cuantizada-inversa */
	/************************************************************************/

	*(se -> coddiferHt)=invqah(se -> codiH, *(se -> codf_esc_linH));

	/***********************************************************************/
	/********************* ADAPTACION FACTOR DE ESCALA	*********************/
	/*	Adapto el factor de escala de cuantificacion para la siguiente muestra
	de entrada */
	/***********************************************************************/


	/* en la rutina de reseteo...*/
	/*if (rss==1)
		*(f_esc_logH)=0;
	else ;*/	/*delaya, reseteo si es el caso*/

	*(se -> codf_esc_logH+1)= logsch(se -> codiH, *(se -> codf_esc_logH));
	/* adapto el factor de escala logaritmico*/
	*(se -> codf_esc_logH)=*(se -> codf_esc_logH+1);
	/* lo actualizo en su array*/

	*(se -> codf_esc_linH+1)=scaleh(*(se -> codf_esc_logH));
	/* hallo la actualizacion del fac. esc. lineal desde el nuevo f.esc.log.*/

	/* en la rutina de reseteo...*/
	/*if (rss==1)
		*(f_esc_linH+1)=8;
	else;*/ /*delayl*/

	*(se -> codf_esc_linH)=*(se -> codf_esc_linH+1);
	/* actualizo el array*/


	/************************************************************************/
	/******************* ADAPTACION DEL PREDICTOR ***************************/
	/*	Preparo la senial 'poloadL' que debe usar el adaptador del predictor.*/
	/************************************************************************/

	upzero( se -> coddiferHt, se -> codpcerosH, se -> codpcerospH);
	/*adapto los ceros del predictor*/

	*(se -> codparcialH)=parrec(*(se -> coddiferHt), se -> codszH);
	/*hallo la senial reconstruida parcial*/

	/* en la rutina de reseteo...*/
	/*reseteo el array parcialLt, si es el caso*/
	/*if (rss==1)
		for (i=1; i<3; i++)
			*(parcialH+i)=0;
	else;*/

	*(se -> codreconsHt)=recons(se -> codsH, *(se -> coddiferHt));
	/*hallo la senial reconstruida*/

	/* Hallo los nuevos polos*/
	*(se -> codppolospH+1)=uppol2(se -> codppolosH, se -> codparcialH);
	*(se -> codppolospH)=uppol1(se -> codppolosH, se -> codppolospH, se -> codparcialH);


	/************************************************************************/
	/*Actualizamos (retrasamos) el array rLt y pLt y dLt antes de coger la
	siguiente muestra, y los ceros y polos*/
	/************************************************************************/

	aux1 = se -> coddiferHt + 6;
	aux0 = se -> coddiferHt + 5;
	for ( i = 0; i < 6; i++)
	{
		*aux1-- = *aux0--;
		*(se -> codpcerosH+i)=*(se -> codpcerospH+i);
	};

	aux1 = se -> codparcialH + 2;
	aux0 = se -> codparcialH + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
		*(se -> codppolosH+i)=*(se -> codppolospH+i);
	};

	aux1 = se -> codreconsHt + 2;
	aux0 = se -> codreconsHt + 1;
	for ( i = 0; i < 2; i++)
	{
		*aux1-- = *aux0--;
	};


	/************************************************************************/
	/********************         MULTIPLEXADO          *********************/
	/************************************************************************/


	return (se -> codiL+ (se -> codiH<<6));
}

/*void saveToFile(short x, FILE *f) {

	fwrite( &x, sizeof(x), 1, f );
}*/


void calcTxQmf(G722StateEncoder *se, short *xL, short *xH, short x1, short x2) {
   const short qmfCoeffs[]={1,-11,-11,53,12,-156,32,362,-210,-805,951,3876,3876,951,-805,-210,362,32,-156,12,53,-11,-11,1};

	/* x1 y x2 son dos muestras consecutivas de la entrada */
	/* xL y xH son punteros a los valores de las salidas */

	long xA=0,xB=0,aux1,aux2; /* supongo long de 32 bit */

	short i;

	/* introducimos las nuevas muestras en el buffer */
	/* y avanzamos puntero */
   (se -> buffQmfTx)[(se -> wp_buffQmfTx)++]=x1;
	(se -> wp_buffQmfTx) %= 24;

	(se -> buffQmfTx)[(se -> wp_buffQmfTx)++]=x2;
	(se -> wp_buffQmfTx) %= 24;

	/* PFC */
	/*saveToFile(x1,fich_x1);
	saveToFile(x2,fich_x2);*/

	/* el puntero queda sobre la proxima posicion a escribir */
	/* que es tambien la muestra mas antigua */

	/* calculamos el qmf de tx*/

	for ( i=0 ; i< 24 ; ) {

		xB+=(long)(se -> buffQmfTx)[(se -> wp_buffQmfTx)++]*qmfCoeffs[i++];
		(se -> wp_buffQmfTx) %= 24;

		xA+=(long)(se -> buffQmfTx)[(se -> wp_buffQmfTx)++]*qmfCoeffs[i++];
		(se -> wp_buffQmfTx) %= 24;
	}

	/* PFC */
	/*saveToFile((short)(xB/8192),fich_xB);
	saveToFile((short)(xA/8192),fich_xA);*/

	aux1=(xA+xB)/8192;
	aux2=(xA-xB)/8192;

	/*saveToFile((short)(aux1),fich_aux1);
	saveToFile((short)(aux2),fich_aux2);*/

	aux1 = (aux1>16383)?16383:aux1;
	aux1 = (aux1<-16384)?-16384:aux1;
	aux2 = (aux2>16383)?16383:aux2;
	aux2 = (aux2<-16384)?-16384:aux2;

	*xL=(short)aux1;
	*xH=(short)aux2;
}           

