#ifndef VARDEC_H
#define VARDEC_H

   /* Variables para calcRxQmf() */
 
   /*short buffQmfRx[24];*/
typedef struct {
   short buffQmfRx[24];
   short wp_buffQmfRx; /* puntero de escritura */
 
   /****************************************************************/
   /*			DEFCF.H					*/
   /* Fichero que comprende las definiciones de las variables 	*/
   /* globales y de las tablas que va a usar el CODEC G722		*/
   /* Estan en coma fija, cada una con el formato que viene 	*/
   /* en la norma							*/
   /****************************************************************/
 
   /*short  decodxL, decodxH;*/
   /*15-bit quantized input signal; seniales de entrada al codificador*/
   /* SS-2...-15*/
 
   int rss; /* flag de reset */
 
   int mode; /* flag de modo*/
 
   unsigned char  /*decodiL,*/ decodiH;
   /* 6-bit ADPCM codeword, 2-bit ...;*/
 
   unsigned char decodiLr;
   /* 6-bit ADPCM codeword received*/
 
   short decodsL, decodsH;
   /* predictor output value; S-1...-15*/
 
   /*short decodeL, decodeH;*/
   /* difference signal; S-1...-15*/
 
   short decoddL;
   /* quantized difference signal for decoder output*/ 
 
   short decodspL, decodszL;	
   /*Contribucion de los polos y los ceros a la senial predicha L, S -1...-15*/
 
   short decodspH, decodszH;
   /*Contribucion de los polos y los ceros a la senial predicha H, S-1...-15*/
 
   short  decoddetL[2];
   short  decoddetH[2];
   /*inicializamos al valor que le corresponde al reset*/
   /* detL[1]=detL delayed quantizer scale factor; S-4...-18;
   quantizer scale factor detL[0]=deLp*/
 
   short *decodf_esc_linL;
   short *decodf_esc_linH;
 
   short decodnbL[2];
   /*inicializamos al valor que le corresponde al reset*/
   /* S 3... -11, delayed logarithmic scale quantizer factor,nbL[1]=nbL;
   logarithmic scale quantizer factor, nbL[0]=nbpL*/
 
   short *decodf_esc_logL;
   /* Puntero al array del factor de escala logaritmico L*/
 
   short  decodnbH[2];
   /*inicializamos al valor que le corresponde al reset*/
   /*S3...-11, delayed logarithmic scale quantizer factor, nbH[1]=nbH;
   logarithmic scale quantizer factor, nbH[0]=nbpH*/
 
   short *decodf_esc_logH;
   /*puntero al array anterior de factor de escala logaritmico H*/
 
   short decodapL[2];
   short decodapH[2];
   /*polos del predictor bajo y alto*/
   /* apL1=apL[0]...; S 0...-14*/
 
   short decodaL[2];
   short decodaH[2]; 
   /*polos retrasados del predictor*/
   /* S 0...-14 */
 
   short *decodppolosL;
   short *decodppolospL;
   short *decodppolosH;
   short *decodppolospH; 
   /* punteros a los anteriores */
 
   short decodbpL[6];
   short decodbpH[6];
   /*ceros del predictor bajo y alto*/
   /* S 0... -14*/
 
   short decodbL[6];
   short decodbH[6];
   /*ceros retrasados*/
   /*S0...-14*/
 
   short *decodpcerosL;
   short *decodpcerospL;
   short *decodpcerosH;
   short *decodpcerospH;
   /*punteros a los anteriores*/
 
   short decoddLt[7];
   /* senial diferencia cuantizada L para el predictor adaptativo, S -1...-15*/
   /* Almacena el valor actual mas siete retrasos*/
 
   short *decoddiferLt; 
   /*puntero al anterior*/
 
   short decoddH[7];
   /* senial diferencia cuantizada L para el predictor adaptativo, S -1...-15*/
   /* Almacena el valor actual mas siete retrasos*/
 
   short *decoddiferHt; 
   /*puntero al anterior*/
 
   short decodrLt[3];
   /* senial reconstruida para el predictor adaptativo, S -1...-15, delay 0, 1 y 2*/
 
   short *decodreconsLt; 
   /*puntero al anterior*/
 
   short decodrH[3];
   /* senial reconstruida para el predictor adaptativo, S -1...-15, delay 0, 1 y 2*/
 
   short *decodreconsHt; 
   /*puntero al anterior*/
 
   short decodpLt[3];
   /*senial reconstruida parcial,S -1...-15, delay 0, 1 y 2*/
 
   short *decodparcialLt; 
   /*puntero al anterior*/
 
   short decodpH[3];
   /*senial reconstruida parcial,S -1...-15, delay 0, 1 y 2*/
 
   short *decodparcialH; 
   /*puntero al anterior*/
} G722StateDecoder;

G722StateDecoder *G722StateDecoderNew();

#endif
