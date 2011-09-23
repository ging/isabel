#ifndef VARCOD_H
#define VARCOD_H

/* Variables para calcTxQmf() */

/*short buffQmfTx[24];*/
typedef struct {
   short int buffQmfTx[24];
   short wp_buffQmfTx; /* puntero de escritura */

/****************************************************************/
/*			VARCOD.H				*/
/* Fichero que comprende las definiciones de las variables 	*/
/* que se usan en el codificador				*/
/* Estan en coma fija, cada una con el formato que viene 	*/
/* en la norma.							*/
/****************************************************************/

/*short codxL, codxH;*/

/*15-bit quantized input signal; seniales de entrada al codificador*/
/* SS-2...-15*/
  
/* PABLO F-C */  
/* int rss; */ /* flag de reset */
/* int mode; */ /* flag de modo*/

   unsigned char codiL, codiH;

/* 6-bit ADPCM codeword, 2-bit ...;*/

/*unsigned char codiLr;*/

/* 6-bit ADPCM codeword received*/

   short codsL, codsH;

/* predictor output value; S-1...-15*/

   short codeL, codeH;

/* difference signal; S-1...-15*/

/*short coddL;*/

/* quantized difference signal for decoder output*/ 

   short codspL, codszL;	
/*Contribucion de los polos y los ceros a la senial predicha L, S -1...-15*/

   short codspH, codszH;
/*Contribucion de los polos y los ceros a la senial predicha H, S-1...-15*/

   short coddetL[2];
   short coddetH[2];
/*inicializamos a los valores en el caso de reset*/
/* detL[1]=detL delayed quantizer scale factor; S-4...-18;
quantizer scale factor detL[0]=deLp*/

   short *codf_esc_linL;
   short *codf_esc_linH;
/*punteros a los arrays anteriores*/

   short codnbL[2];
/*inicializamos a los valores en el caso de reset*/
/* S 3... -11, delayed logarithmic scale quantizer factor,nbL[1]=nbL;
logarithmic scale quantizer factor, nbL[0]=nbpL*/

   short *codf_esc_logL;
/* Puntero al array del factor de escala logaritmico L*/

   short codnbH[2];
/*S3...-11, delayed logarithmic scale quantizer factor, nbH[1]=nbH;
logarithmic scale quantizer factor, nbH[0]=nbpH*/

   short *codf_esc_logH;
/*puntero al array anterior de factor de escala logaritmico H*/

   short codapL[2];
   short codapH[2];
/*polos del predictor bajo y alto*/
/* apL1=apL[0]...; S 0...-14*/

   short codaL[2];
   short codaH[2];
/*polos retrasados del predictor*/
/* S 0...-14 */

   short *codppolosL;
   short *codppolospL;
   short *codppolosH;
   short *codppolospH; 
/* punteros a los anteriores */

   short codbpL[6];
   short codbpH[6];
/*ceros del predictor bajo y alto*/
/* S 0... -14*/

   short codbL[6];
   short codbH[6];
/*ceros retrasados*/
/*S0...-14*/

   short *codpcerosL;
   short *codpcerospL;
   short *codpcerosH;
   short *codpcerospH;
/*punteros a los anteriores*/

   short coddLt[7];
/* senial diferencia cuantizada L para el predictor adaptativo, S -1...-15*/
/* Almacena el valor actual mas siete retrasos*/

   short *coddiferLt; 
/*puntero al anterior*/

   short coddH[7];
/* senial diferencia cuantizada L para el predictor adaptativo, S -1...-15*/
/* Almacena el valor actual mas siete retrasos*/

   short *coddiferHt; 
/*puntero al anterior*/

   short codrLt[3];
/* senial reconstruida para el predictor adaptativo, S -1...-15, delay 0, 1 y 2*/

   short *codreconsLt; 
/*puntero al anterior*/

   short codrH[3];
/* senial reconstruida para el predictor adaptativo, S -1...-15, delay 0, 1 y 2*/

   short *codreconsHt; 
/*puntero al anterior*/

   short codpLt[3];
/*senial reconstruida parcial,S -1...-15, delay 0, 1 y 2*/

   short *codparcialLt; 
/*puntero al anterior*/

   short codpH[3];
/*senial reconstruida parcial,S -1...-15, delay 0, 1 y 2*/

   short *codparcialH; 
/*puntero al anterior*/
} G722StateEncoder;

G722StateEncoder *G722StateEncoderNew();


#endif
