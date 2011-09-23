/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: cod.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codec.hh"
#include "config.hh"
#include "tablas.hh"

//#define DEBUG 1


/**************************************************************************
* Entradas:
*        session: datos que se conservan de la anterior imagen codificada
*        img_nueva: imagen tipo yuv que se quiere codificar
* Salidas:
*        img_H263: imagen tipo H263 codificada
*        Si algo va mal, devuelve NULL
* Descripcion:
*        Es la funcion principal del codificador.
*        Dada una imagen yuv la codifica en una H263.
*        Para ello, primero codifica la cabecera y luego codifica los GOBs.
*        Solo se aceptan imagenes con uno de los cinco formatos:
*      sQCIF, QCIF, CIF, 4CIF, 16CIF
***************************************************************************/
H263EncodedImage_t *
Cod::cod_pic (H263EncodeSession_t *session, yuvImage_t *img_nueva, int mode, float quality)
{
    unsigned width = img_nueva->w;
    unsigned height= img_nueva->h;

    //
    // Si no es uno de los 5 formatos CIF, no se codifica -> return NULL
    //
    if ( (width==128) && (height==96) )
        cab.formato=SQCIF;
    else if ( (width==176) && (height==144) )
        cab.formato=QCIF;
    else if ( (width==352) && (height==288) )
        cab.formato=CIF;
    else if ( (width==704) && (height==576) )
        cab.formato=CIF4;
    else if ( (width==1408) && (height==1152) )
        cab.formato=CIF16;
    else {
        cab.formato=OTRO;
    }


    //
    // Inicializacion
    //
    cab.x=0;
    cab.y=0;

    // Vamos a codificar con vectores de movimiento nulos
    cab.mvx=0;
    cab.mvy=0;

    cab.width=width;
    cab.height=height;

    //
    // Reserva de memoria para la imagen codificada
    //
    H263EncodedImage_t *img_H263 = new H263EncodedImage_t;
    img_H263->buffer = new u8[width * height*3];
    img_H263->w = width;
    img_H263->h = height;
    img_H263->GOBsPopped = 0;
    img_H263->MBsPopped = 0;
    img_H263->mode = (u8)mode;
    img_H263 ->GOBp = NULL;
    img_H263 ->GOBq = NULL;
    img_H263 ->MBp = NULL;
    img_H263 ->MBq = NULL;

    // Inicializacion del escritor
    escritor.reset (img_H263->buffer);

    // Decision del modo INTRA/INTER
//  session->contador++;
    if ((session->contador++) >= MAX_INTER_SEGUIDAS) {
        cab.INTER=0;  //INTRA
        session->contador=0;
    } else {
        cab.INTER=1;
    }
    elQuant= (int)((-10*quality+1033)/33);
//  elQuant = (session->contador%25)+3;
//fprintf(stderr, "QUANT=%d\n", elQuant);

    //
    // Codificacion de la cabecera de la imagen
    //
    cod_cab (cab, img_H263);

    //
    // Codificacion de todos los GOBs
    //

    // Calculo del numero de GOBs segun el formato
    int nGOBs= 0;   // remove warning from compiler
    switch (cab.formato) {
    case SQCIF:
        nGOBs=6;
        break;
    case QCIF:
        nGOBs=9;
        break;
    case CIF:
    case CIF4:
    case CIF16:
        nGOBs=18;
        break;
    case OTRO:
        {
            /////////////////////
            int k = 0;
            if ((cab.height >=4) && (cab.height <=400)) {
                k = 1;
            } else
            if ((cab.height >=404) && (cab.height <=800)) {
                k = 2;
            } else
            if ((cab.height >=804) && (cab.height <=1152)) {
                k = 4;
            }
            nGOBs = cab.height/(k*16);
            cab.nGOBs = nGOBs;
            if (cab.height % (k*16)) {
                fprintf(stderr, "error: Height is not divisible by 16\n");
            }
            /////////////////////
        }
    }
    //No se codifica la cabecera en ninguno

    //fprintf(stderr, "bloques= %d\n", nGOBs);

    img_H263 -> nGOBs = nGOBs;
    img_H263 -> GOBp = new unsigned int[nGOBs];
    img_H263 -> GOBq = new unsigned int[nGOBs];

    for (int i=0; i<nGOBs; i++) {
        cab.n_gob=i;
        if (gob.cod_gob(escritor,
                        session->img_cache,
                        img_nueva, cab,
                        img_H263
                       )
           ) {
            delete (img_H263);
            fprintf(stderr, "Error en cod_pic 1\n");
            return NULL;
        } else{
            img_H263 -> GOBp[i] = escritor.get_nBytes();
            img_H263 -> GOBq[i] = escritor.get_quedan();
        }
    }

    //
    // Se guarda esta imagen para usarla luego como comparacion en INTER
    //
    actualiza_imagen (session->img_cache, img_nueva);


    //No ponemos EndOfSequence


    //
    // PSTUF, para que el siguiente PSC este alineado
    //
    escritor.alinea();

    img_H263->numBytes = escritor.get_nBytes();

    return img_H263;
}


/*********************************************************************
* Entradas:
*        cab: datos de la imagen (inter/intra, cuantificador...)
* Salidas:
* Descripcion:
*        Codifica la cabecera de la imagen.
*        No se usa ninguno de los modos opcionales
**********************************************************************/
void
Cod::cod_cab (CAB &cab, H263EncodedImage_t *img_H263)
{
    u32 campo;

    // ----------- PSC  ---------------------------
    escritor.escribe_bits (0x20, 22);
#ifdef DEBUG
    fprintf(stderr, "PSC escrita\n");
#endif

    // ----------- TR -----------------------------
    campo=0;
    escritor.escribe_bits(campo, 8);
#ifdef DEBUG
    fprintf(stderr, "TR=%i\n", campo);
#endif

    // ----------- PTYPE --------------------------
    // Los dos primeros bits 1 0 en H263
    escritor.escribe_bits (0x2, 2);
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Split Screen=%i\n", campo);
#endif
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Document Camera=%i\n", campo);
#endif
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Freeze Picture=%i\n", campo);
#endif
    switch (cab.formato) {
    case SQCIF:
        campo=0x1;
        img_H263 -> source = SQCIF;
        break;
    case QCIF:
        campo=0x2;
        img_H263 -> source = QCIF;
        break;
    case CIF:
        campo=0x3;
        img_H263 -> source = CIF;
        break;
    case CIF4:
        campo=0x4;
        img_H263 -> source = CIF4;
        break;
    case CIF16:
        campo=0x5;
        img_H263 -> source = CIF16;
        break;
    case OTRO:
        campo=0x7;
        img_H263 -> source = OTRO;
        break;
    }
    escritor.escribe_bits(campo, 3);
#ifdef DEBUG
    fprintf(stderr, "Source Format=%i\n", campo);
#endif

    if (img_H263 -> source != OTRO) {
        cod_cab_noplus (cab, img_H263);
    } else {
        cod_cab_plus (cab, img_H263);
    }
}


/*********************************************************************
* Entradas:
*        cab: datos de la imagen (inter/intra, cuantificador...)
* Salidas:
* Descripcion:
*        Codifica la cabecera de la imagen con formato normalizado.
*        No se usa ninguno de los modos opcionales
**********************************************************************/
void
Cod::cod_cab_noplus (CAB &cab, H263EncodedImage_t *img_H263)
{
    u32 campo;

    img_H263 -> inter = cab.INTER;

    escritor.escribe_bits(cab.INTER, 1);
#ifdef DEBUG
    fprintf(stderr, "INTER?=%i\n", cab.INTER);
#endif

    //
    // Los 4 modos opcionales. No se usa ninguno
    //
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Unrestricted Motion Vector=%i\n", campo);
#endif
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Syntax Arithmetic=%i\n", campo);
#endif
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Advanced Prediction=%i\n", campo);
#endif
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "PB-frames=%i\n", campo);
#endif

    // -----------  PQUANT ----------------------

    img_H263 -> quant = elQuant;

    cab.quant=elQuant;
    escritor.escribe_bits(cab.quant, 5);
#ifdef DEBUG
    fprintf(stderr, "PQUANT=%i\n", cab.quant);
#endif

    // -----------  CPM ----------------------
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "CPM=%i\n", campo);
#endif


    // Como no hay CPM, no hay PSBI


    // Como no PB_FRAME, no TRB ni DBQUANT


    // -----------  PEI  --------------------
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "PEI=%i\n", campo);
#endif

    // Como no hay PEI, no hay PSPARE
}


/*********************************************************************
* Entradas:
*        cab: datos de la imagen (inter/intra, cuantificador...)
* Salidas:
* Descripcion:
*        Codifica la cabecera de la imagen con formato no normalizado.
*        No se usa ninguno de los modos opcionales
**********************************************************************/
void
Cod::cod_cab_plus (CAB &cab, H263EncodedImage_t *img_H263)
{
    u32 campo;

    // -----------  UFEP  --------------------
    campo = 0x01;
    escritor.escribe_bits (campo, 3);
#ifdef DEBUG
    fprintf(stderr, "UFEP=%i\n", campo);
#endif

    // -----------  OPPTYPE  --------------------
    campo = 0x06;
    escritor.escribe_bits (campo, 3);
#ifdef DEBUG
    fprintf(stderr, "Source Format=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Picture Clock Format=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Unrestricted Motion Vector=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Syntax-based Arithmetic Coding=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Advanced Prediction=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Advanced Intra Coding=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Deblocking Filter=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Slice Structured=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Reference Picture Selection=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Independent Segment Decoding=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Alternative Inter VLC=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Modified Quantization=%i\n", campo);
#endif
    campo = 0x01;
    escritor.escribe_bits (campo, 1);
    campo = 0x0;
    escritor.escribe_bits (campo, 3);

    // -----------  MPPTYPE  --------------------

    img_H263 -> inter = cab.INTER;
    campo = cab.INTER;
    escritor.escribe_bits (campo, 3);
#ifdef DEBUG
    fprintf(stderr, "INTER?=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Reference Picture Resampling=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Reduced-resolution Update=%i\n", campo);
#endif
    campo = 0x01;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "Rounding Type=%i\n", campo);
#endif
    campo = 0x0;
    escritor.escribe_bits (campo, 2);
    campo = 0x01;
    escritor.escribe_bits (campo, 1);

    // -----------  CPM  --------------------

    campo = 0x0;
    escritor.escribe_bits (campo, 1);
#ifdef DEBUG
    fprintf(stderr, "CPM=%i\n", campo);
#endif

    // -----------  CPFMT  --------------------

    campo = 0x02;
    escritor.escribe_bits (campo, 4);
    campo = (cab.width/4)-1;
    escritor.escribe_bits (campo, 9);

#ifdef DEBUG
    fprintf(stderr, "PWI=%i\n", campo);
#endif
    campo = 0x01;
    escritor.escribe_bits (campo, 1);
    campo = (cab.height/4);

    escritor.escribe_bits (campo, 9);
#ifdef DEBUG
    fprintf(stderr, "PHI=%i\n", campo);
#endif

    //No EPAR, CPCFC, ETR, UUI, SSS, ELNUM, RLNUM, RPSMF, TRPI, TRP, BCI, capa de BCM, RPRP

    // -----------  PQUANT ----------------------

    img_H263 -> quant = elQuant;

    cab.quant=elQuant;

    escritor.escribe_bits(cab.quant, 5);
#ifdef DEBUG
    fprintf(stderr, "PQUANT=%i\n", cab.quant);
#endif

    // -----------  PEI  --------------------
    campo=0x0;
    escritor.escribe_bits(campo, 1);
#ifdef DEBUG
    fprintf(stderr, "PEI=%i\n", campo);
#endif

    // Como no hay PEI, no hay PSPARE
}


/**********************************************************************
* Entradas:
*        img_nueva
* Salidas:
*        img
* Descripcion:
*        Copia el contenido de 'img_nueva' en 'img'
************************************************************************/
void
Cod::actualiza_imagen (yuvImage_t *img, yuvImage_t *img_nueva)
{
    int size= img_nueva->w * img_nueva->h;

    memcpy (img->lum,   img_nueva->lum,   size);
    memcpy (img->cromU, img_nueva->cromU, size/4);
    memcpy (img->cromV, img_nueva->cromV, size/4);
}



/****************************************************************************

 ------------- IMPLEMENTACION DE LOS METODOS DE LA CLASE: ------------------
 ------------- GOB                                        ------------------

****************************************************************************/

/************************************************************************
* Entradas:
*        escritor: se encarga de ir escribiendo la cadena de bits
*        img_cache: anterior imagen que se codifico (para codificar solo
*          la diferencia en modo INTER)
*        img_nueva: imagen tipo yuv que se quiere codificar
*        cab: datos de la imagen (inter/intra, (x,y) actual...)
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
* Descripcion:
*        Codifica un GOB de la imagen.
*        Para ello, codifica los macrobloques que lo componen.
*        Actualiza la posicion actual (x,y)
**************************************************************************/
int
GOB::cod_gob (Escritor &escritor,
              yuvImage_t *img_cache,
              yuvImage_t *img_nueva,
              CAB &cab,
              H263EncodedImage_t *img_H263
             )
{
#ifdef DEBUG
    fprintf(stderr, "Comienza GOB layer\n");
#endif

    //
    // Codificacion de los macrobloques
    //

    // Calculo del numero de macrobloques por cada GOB
    int nMacro;
    switch (cab.formato) {
    case SQCIF:
        nMacro=8;
        break;
    case QCIF:
        nMacro=11;
        break;
    case CIF:
        nMacro=22;
        break;
    case CIF4:
        nMacro=88; //88
        break;
    case CIF16:
        nMacro=352; //352
        break;
    case OTRO :
        nMacro = (cab.width/16)*((cab.height/cab.nGOBs)/16);
        break;
    }
    //fprintf(stderr, "Macrobloques= %d\n", nMacro);

    if ( (img_H263 -> MBp == NULL) && (img_H263 -> MBq == NULL) ) {
        img_H263 -> nMBTotal = (img_H263 -> nGOBs) * nMacro;
        img_H263 -> nMBs = nMacro;
        img_H263 -> MBp = new unsigned int[img_H263 -> nGOBs * nMacro];
        img_H263 -> MBq = new unsigned int[img_H263 -> nGOBs * nMacro];
    }

    for (int i=0; i<nMacro; i++) {
        cab.n_macro=i;
        if ( macroblock.cod_macroblock (escritor, img_cache, img_nueva, cab) ) {
            fprintf(stderr, "Error en cod_gob 1\n");
            return 1;
        } else{
            img_H263 -> MBp[(cab.n_gob * nMacro) + cab.n_macro] = escritor.get_nBytes();
            img_H263 -> MBq[(cab.n_gob * nMacro) + cab.n_macro] = escritor.get_quedan();
        }
        cab.x+=16;
        if (cab.x == cab.width) {
            cab.x=0;
            cab.y+=16;
        }
    }

    return 0;
}




/****************************************************************************

 ------------- IMPLEMENTACION DE LOS METODOS DE LA CLASE: ------------------
 ------------- MACROblock                                 ------------------

****************************************************************************/


/************************************************************************
* Entradas:
*        escritor: se encarga de ir escribiendo la cadena de bits
*        img_cache: anterior imagen que se codifico (para codificar solo
*          la diferencia en modo INTER)
*        img_nueva: imagen tipo yuv que se quiere codificar
*        cab: datos de la imagen (inter/intra, (x,y) actual...)
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
* Descripcion:
*        Codifica un macrobloque de la imagen.
*        1. Obtiene las 6 ristras de coeficientes de los 6 bloques. Va
*     actualizando la posicion actual (x,y)
*        2. Como ya sabe los bloques que tienen AC no nulos, codifica
*     la cabecera del macrobloque
*        3. Codifica los simbolos TCOEF de los 6 bloques
**************************************************************************/
int
MACROblock::cod_macroblock (Escritor &escritor,
                            yuvImage_t *img_cache,
                            yuvImage_t *img_nueva,
                            CAB &cab
                           )
{
    int i;
    int n_AC[6];  // Num de coef AC no nulos en cada bloque
    i16 ristra[6][64];


    //
    //decidir MB intra/inter  en INTER-Pictures
    //
    if (cab.INTER) {
        int x,y;
        int sad=0;
        for (x=0; x<16; x++) {
            for (y=0; y<16; y++) {
                sad += abs (img_nueva->lum[(cab.y+y)*cab.width + cab.x+x] -
                            img_cache->lum[(cab.y+y)*cab.width + cab.x+x] );
            }
        }

        if (sad>SAD) {
#ifdef DEBUG
            fprintf(stderr, "mb INTRA\n");
#endif
            cab.INTERmacro=0;
        }
        else {
#ifdef DEBUG
            fprintf(stderr, "mb INTER\n");
#endif
            cab.INTERmacro=1;
        }
    } else { // En INTRA-Pictures, el macrobloque siempre sera INTRA
        cab.INTERmacro=0;
    }


    cab.CBPY=0;
    cab.CBPC=0;


    //
    // LUMINANCIA
    // **********
    // Obtiene las 4 ristras de coeficientes ya cuantificados
    // Comprueba si hay coeficientes AC no nulos y rellena cab.CBPY
    //
    n_AC[0]= lum.escribe_ristra (img_cache->lum,
                                 img_nueva->lum,
                                 ristra[0],
                                 cab
                                );
    if ( n_AC[0] != 0 ) {
        cab.CBPY |= 0x8;
    }
    cab.x+=8;

    n_AC[1]= lum.escribe_ristra (img_cache->lum,
                                 img_nueva->lum,
                                 ristra[1],
                                 cab
                                );
    if ( n_AC[1] != 0 ) {
        cab.CBPY |= 0x4;
    }
    cab.x-=8;
    cab.y+=8;

    n_AC[2]= lum.escribe_ristra (img_cache->lum,
                                 img_nueva->lum,
                                 ristra[2],
                                 cab
                                );
    if ( n_AC[2] != 0 ) {
        cab.CBPY |= 0x2;
    }
    cab.x+=8;

    n_AC[3]= lum.escribe_ristra (img_cache->lum,
                                 img_nueva->lum,
                                 ristra[3],
                                 cab
                                );
    if ( n_AC[3] != 0 ) {
        cab.CBPY |= 0x1;
    }
    cab.x-=8;
    cab.y-=8;



    //
    // CROMINANCIA
    // ***********
    // Obtiene las 2 ristras de coeficientes ya cuantificados
    // Comprueba si hay coeficientes AC no nulos y rellena cab.CBPC
    //
    n_AC[4]= crom.escribe_ristra (img_cache->cromU,
                                  img_nueva->cromU,
                                  ristra[4],
                                  cab
                                 );
    if ( n_AC[4] != 0 ) {
      cab.CBPC |= 0x2;
    }

    n_AC[5]= crom.escribe_ristra (img_cache->cromV,
                                  img_nueva->cromV,
                                  ristra[5],
                                  cab
                                 );
    if ( n_AC[5] != 0 ) {
      cab.CBPC |= 0x1;
    }


    //
    // Codifica la cabecera del macrobloque
    //
    if ( cod_cab (escritor, cab) ) {
        fprintf(stderr, "Error en cod_macroblock 1\n");
        return 1;
    }

    // En modo INTER, si no hay AC no nulos no se transmite nada mas
    if ( (cab.INTERmacro) && (cab.CBPY==0) && (cab.CBPC==0) ) {
        return 0;
    }

    //
    // Codifica los 6 bloques
    //
    // A partir de las ristras, los VLC TCOEFF
    for (i=0; i<4; i++) {
        if ( lum.cod_bloque (escritor, ristra[i], n_AC[i], cab) ) {
            fprintf(stderr, "Error en cod_macroblock 2\n");
            return 1;
        }
    }
    for (i=0; i<2; i++) {
        if ( crom.cod_bloque (escritor, ristra[4+i], n_AC[4+i], cab) ) {
            fprintf(stderr, "Error en cod_macroblock 3\n");
            return 1;
        }
    }

    return 0;
}




/***********************************************************************
* Entradas:
*        escritor: se encarga de ir escribiendo la cadena de bits
*        cab: datos de la imagen (CBPC, CBPY...)
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
* Descripcion:
*        Codifica la cabecera del macrobloque
*        En imagenes INTER, el vector de movimiento siempre sera cero
************************************************************************/
int
MACROblock::cod_cab (Escritor &escritor, CAB &cab)
{
    Inf_MCBPC inf_MCBPC;
    Tabla_MCBPC_for_I tabla_MCBPC_for_I;
    Tabla_MCBPC_for_P tabla_MCBPC_for_P;

    Inf_CBPY inf_CBPY;
    Tabla_CBPY tabla_CBPY;

#ifdef DEBUG
    fprintf(stderr, "Comienza Macro Block layer\n");
#endif

    if (cab.INTER) {
        // *************************
        // INTER-Picture
        // *************************

        // Rellenar COD
        if ( (cab.INTERmacro) && (cab.CBPY==0) && (cab.CBPC==0) ) {
            // COD=1 (si no hay ningun AC no nulo, no se codifica nada mas)
            escritor.escribe_bits (1, 1);
            return 0;
        } else {
            // COD=0
            escritor.escribe_bits (0,1);

            if (cab.INTERmacro) {
                //
                // Macrobloque tipo INTER
                //
                // --- MCBPC
                inf_MCBPC.MBtype='0';
                inf_MCBPC.CBPC=cab.CBPC;
#ifdef DEBUG
                fprintf(stderr, "MBType=0\n");
                fprintf(stderr, "CBPC=%i\n", cab.CBPC);
#endif
                if ( tabla_MCBPC_for_P.escribir_info (escritor, inf_MCBPC) ) {
                    fprintf(stderr, "Error en cod_cab 1\n");
                    return 1;
                }

                // --- CBPY
                inf_CBPY.CBPY_P=cab.CBPY;
#ifdef DEBUG
                fprintf(stderr, "CBPY=%i\n", cab.CBPY);
#endif
                if ( tabla_CBPY.escribir_info_for_P (escritor, inf_CBPY) ) {
                    fprintf(stderr, "Error en cod_cab 2\n");
                    return 1;
                }

                // --- MVDx=0
                escritor.escribe_bits (1, 1);
                // --- MVDy=0
                escritor.escribe_bits (1, 1);
            } else {
                //
                // Macrobloque tipo INTRA (pero en INTER-Picture)
                //
                // --- MCBPC
                inf_MCBPC.MBtype='3';
                inf_MCBPC.CBPC=cab.CBPC;
#ifdef DEBUG
                fprintf(stderr, "MBType=3\n");
                fprintf(stderr, "CBPC=%i\n", cab.CBPC);
#endif
                if ( tabla_MCBPC_for_P.escribir_info (escritor, inf_MCBPC) ) {
                    fprintf(stderr, "Error en cod_cab 3\n");
                    return 1;
                }

                // --- CBPY
                inf_CBPY.CBPY_I=cab.CBPY;
#ifdef DEBUG
                fprintf(stderr, "CBPY=%i\n", cab.CBPY);
#endif
                if ( tabla_CBPY.escribir_info_for_I (escritor, inf_CBPY) ) {
                    fprintf(stderr, "Error en cod_cab 4\n");
                    return 1;
                }
            }
        }
    } else {
        // *************************
        // INTRA-Picture
        // *************************

        // --- MCBPC
        inf_MCBPC.MBtype='3';
        inf_MCBPC.CBPC=cab.CBPC;
#ifdef DEBUG
        fprintf(stderr, "MBType=3\n");
        fprintf(stderr, "CBPC=%i\n", cab.CBPC);
#endif
        if ( tabla_MCBPC_for_I.escribir_info (escritor, inf_MCBPC) ) {
            fprintf(stderr, "Error en cod_cab 5\n");
            return 1;
        }

        // --- CBPY
        inf_CBPY.CBPY_I=cab.CBPY;
#ifdef DEBUG
        fprintf(stderr, "CBPY=%i\n", cab.CBPY);
#endif
        if ( tabla_CBPY.escribir_info_for_I (escritor, inf_CBPY) ) {
            fprintf(stderr, "Error en cod_cab 6\n");
            return 1;
        }
    }

    return 0;
}





/****************************************************************************

 ------------- IMPLEMENTACION DE LOS METODOS DE LA CLASE: ------------------
 ------------- Block                                      ------------------

****************************************************************************/


void ChenDct (short *x, short *y);


/***************************************************************************
* Entradas:
*        cache: componente (Y, Cb o Cr) de la ultima imagen que se codifico
*      (para codificar solo la diferencia en modo INTER)
*        nueva: componente (Y, Cb o Cr) de la imagen que se quiere codificar
*        cab: datos de la imagen (inter/intra, (x,y) actual...)
* Salidas:
*        Devuelve el numero de coeficientes AC no nulos.
*        ristra: los 64 coeficientes cuantificados del bloque en una fila
* Descripcion:
*        Obtiene la ristra de 64 coeficientes del bloque
*        1. Busca en la imagen el bloque de 8x8 pixels situados en la
*      posicion actual (x,y), que obtiene de 'cab'
*        2. Le aplica la transformada DCT y cuantifica los coeficientes
*        3. Recorre en zigzag la matriz y obtiene la ristra
***************************************************************************/
int
Block::escribe_ristra (u8 *cache, u8*nueva,  i16 *ristra, CAB &cab)
{
    int cont;
    int primer_AC;

    i16 mat[8][8];
    i16 mat_dif[8][8];
    i16 mat_coef[8][8];


    // Meter bloque de pixels de imagen actual en mat
    coge_bloque (nueva, mat, cab);

    if (cab.INTERmacro) {
        //
        // Macrobloque tipo INTER
        //

        // Meter bloque de pixels que habia en imagen anterior en mat_dif
        coge_bloque (cache, mat_dif, cab);

        // Deja en mat = nueva - anterior
        resta_mat (mat, mat_dif);
    }

    // Transformada + Zigzag
    ChenDct (mat[0], mat_coef[0]);
    mat2ristra (mat_coef, ristra);


    //
    // Cuantificacion INTRADC, solo en macrobloques INTRA
    //
    if (!cab.INTERmacro) {
        ristra[0] /= 8;
        if (ristra[0] == 0) ristra[0]=1; // valor prohibido
        if (ristra[0] > 254) ristra[0]=254;
        if (ristra[0] == 128) ristra[0]=255; // valor prohibido
#ifdef DEBUG
        fprintf(stderr, "INTRADC=%i\n", ristra[0]);
#endif
    }


    //
    // Cuantificacion AC
    //
    if (cab.INTERmacro) {
        // En bloques INTER, los 64 coeficientes son AC
        primer_AC=0;
    } else {
        // En bloques INTRA, 1 DC y 63 AC
        primer_AC=1;
    }

    for (int a=primer_AC; a<64; a++) {
        ristra[a] /= (2*cab.quant);
        if (ristra[a] < (-127)) ristra[a]=(-127);
        if (ristra[a] > 127) ristra[a]=127;
    }


    //
    //Contar el numero de coeficientes AC no nulos
    //
    cont=0;
    for (int i=primer_AC; i<64; i++) {
        if ( ristra[i] != 0 ) {
            cont++;
        }
    }

    return cont;
}


/**************************************************************************
* Entradas:
*        escritor: se encarga de ir escribiendo la cadena de bits
*        ristra: los 64 coeficientes cuantificados del bloque en una fila
*        n_AC: numero de coeficientes AC no nulos
*        cab: datos de la imagen (inter/intra, (x,y) actual...)
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
* Descripcion:
*        A partir de la ristra de coeficientes cuantificados en 'ristra',
*     codifica los simbolos TCOEFF=(last, run, level).
***************************************************************************/
int
Block::cod_bloque(Escritor &escritor, i16 *ristra, int n_AC, CAB &cab)
{
    Inf_TCOEF inf;
    Tabla_TCOEF tabla;

    int cont=0;

    //
    // Codificacion de INTRADC (solo en macrobloques INTRA)
    //
    if (!cab.INTERmacro) {
        escritor.escribe_bits (ristra[cont++], 8);
    }

    //
    // Formacion y codificacion de simbolos TCOEFF
    //
    if (n_AC >= 1) {
        inf.last=0;
        while (n_AC > 1) {
            n_AC--;
            inf.run=0;
            while ( (inf.level=ristra[cont++]) == 0) {
                inf.run++;
            }
            if (inf.level < 0) {
                inf.level *= (-1);
                inf.sign = 1;
            } else {
                inf.sign = 0;
            }
            if ( tabla.escribir_info (escritor, inf) ) {
                fprintf(stderr, "Error en cod_bloque 1\n");
                return 1;
            }
        }

        inf.last=1;
        inf.run=0;
        while ( (inf.level=ristra[cont++]) == 0) {
            inf.run++;
        }
        if (inf.level < 0){
            inf.level *= (-1);
            inf.sign = 1;
        } else {
            inf.sign = 0;
        }
        if ( tabla.escribir_info (escritor, inf) ) {
            fprintf(stderr, "Error en cod_bloque 2\n");
            return 1;
        }
    }

    return 0;
}


/***********************************************************************
* Entradas:
*        matriz: de coeficientes de 8x8
* Salidas:
*        ristra: de 64 coeficientes en fila
* Descripcion:
*        Recorre la matriz en zigzag y la copia en una fila de 64.
************************************************************************/
void
Block::mat2ristra(i16 matriz [8][8], i16 *ristra)
{
    static short conv[8][8]= { {1 ,2 ,6 ,7 ,15,16,28,29},
                               {3 ,5 ,8 ,14,17,27,30,43},
                               {4 ,9 ,13,18,26,31,42,44},
                               {10,12,19,25,32,41,45,54},
                               {11,20,24,33,40,46,53,55},
                               {21,23,34,39,47,52,56,61},
                               {22,35,38,48,51,57,60,62},
                               {36,37,49,50,58,59,63,64}
                             };

    for (int x=0; x<8; x++) {
        for (int y=0; y<8; y++) {
            ristra [ conv[y][x]-1 ] = matriz [y][x] ;
        }
    }
}


/*********************************************************************
* Entradas:
*        mat1
*        mat2
* Salidas:
*        mat1 = mat1 - mat2
**********************************************************************/
void
Block::resta_mat (i16 mat1[8][8], i16 mat2[8][8])
{
    for (int x=0; x<8; x++) {
        for (int y=0; y<8; y++) {
            mat1[y][x] -= mat2[y][x];
        }
    }
}

