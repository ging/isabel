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
// $Id: dec.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "codec.hh"
#include "tablas.hh"

#ifdef __SYMBIAN32__
#include "../../../include/vCodecs/vUtils.h"
#else
#include <vCodecs/vUtils.h>
#endif


//#define DEBUG 1


/**************************************************************************
* Entradas:
*        session: datos que se conservan de la anterior imagen decodificada
*        imgBuffer: buffer con imagen tipo H263 que se quiere decodificar
* Salidas:
*        img_yuv: imagen tipo yuv decodificada
*        Si algo va mal, devuelve NULL
* Descripcion:
*        Es la funcion principal del decodificador.
*        Dada una imagen H263 la decodifica en una yuv.
*        Para ello, primero decodifica la cabecera y luego decodifica
*      los GOBs.
*        No se aceptan imagenes con ninguno de los modos opcionales
*      de codificacion
***************************************************************************/
yuvImage_t *
Dec::dec_pic (H263DecodeSession_t *session, unsigned char *imgBuffer)
{
    u32 campo;

    //
    // Inicializacion
    //

    // Se inicializa el lector
    lector.reset (imgBuffer);

    int width = 0;
    int height= 0;

    cab.x=0;
    cab.y=0;

    //
    // Se decodifica la cabecera de la imagen
    // Si hay algun problema (algun modo opcional o mal formato
    // de la imagen) se devuelve NULL
    //
    if (dec_cab (cab)) {
        fprintf(stderr,"Error en dec_pic 1\n");
        return NULL;
    }

    if (!width)  width=cab.width;
    if (!height) height=cab.height;

    if ( (width == 0) || (height==0)) {
        fprintf(stderr,"Error en dec_pic OTRO\n");
        return NULL;
    }


    for (int kk1= 0 ; kk1 < 2 ; kk1++)
        for (int kk2= 0 ; kk2 < 4 ; kk2++)
            for (int kk3= 0 ; kk3 < 88 ; kk3++)
                cab.MV[kk1][kk2][kk3] = 0;

    //
    // Reserva de memoria para la imagen decodificada
    //
    yuvImage_t *img_nueva = new yuvImage_t;
    img_nueva->lum  = new u8[width * height];
    img_nueva->cromU= new u8[width * height/4];
    img_nueva->cromV= new u8[width * height/4];
    memset(img_nueva->lum,    16, width*height);
    memset(img_nueva->cromU, 128, width*height/4);
    memset(img_nueva->cromV, 128, width*height/4);
    img_nueva->w = width;
    img_nueva->h = height;

    //fprintf(stderr,"Imagen Inter=%d\n", cab.INTER);
    if ((cab.INTER) && (session->contador==0)) {
//      fprintf(stderr,"Esperaba una INTRA\n");
        return img_nueva;
    }
    session->contador++;

    //
    // Deodificacion de todos los GOBs
    //

    // Calculo del numero de GOBs segun el formato
    int nGOBs;
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
            int k = 0;
            if ((cab.height >=4) && (cab.height <=400)) {
                k = 1;
            } else if ((cab.height >=404) && (cab.height <=800)) {
                k = 2;
            } else if ((cab.height >=804) && (cab.height <=1152)) {
                k = 4;
            } else {
                fprintf(stderr, "La imagen es demasiado grande\n");
                delete (img_nueva);
                return NULL;
            }
            nGOBs = cab.height/(k*16);
            cab.nGOBs = nGOBs;
            if (cab.height % 16) {
                fprintf(stderr,"El alto no es multiplo de 16\n");
            }
        }
    }
    //fprintf(stderr,"bloquesDEC= %d -- cab.x: %d -- cab.y: %d\n", nGOBs, cab.x, cab.y);
    for (int i=0; i<nGOBs; i++) {
        cab.n_gob=i;
        if ( gob.dec_gob (lector, session->img_cache, img_nueva, cab) ) {
            delete (img_nueva);
            fprintf(stderr,"Error en dec_pic 2\n");
            return NULL;
        }
    }

//return img_nueva;
    //
    // Se guarda esta imagen para usarla luego como comparacion en INTER
    //
    actualiza_imagen (session->img_cache, img_nueva);

    //
    // ESTUF, para que EndOfSequence (si existe) este alineado
    //
    lector.alinea();

    //
    //Vemos si hay EndOfSequence
    //
    if ( (campo=lector.lee_bits(22)) != 0x3f ) {
#if DEBUG
        fprintf(stderr,"Error en dec_pic: No hay EOS\n");
#endif
        lector.devuelve_bits (22);
    } else {
#ifdef DEBUG
        fprintf(stderr,"EOS leida\n");
#endif

        //
        // PSTUF, para que el siguiente PSC este alineado
        //
        lector.alinea();
    }

    return img_nueva;
}

/*********************************************************************
* Entradas:
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
*        cab: datos de la imagen (inter/intra, cuantificador...)
* Descripcion:
*        Decodifica la cabecera de la imagen.
*        No se admite ninguno de los modos opcionales
**********************************************************************/
int
Dec::dec_cab (CAB &cab)
{
    u32 campo;

    // --------- PSC ---------------------------
    if ( (campo=lector.lee_bits(22)) != 0x20 ) {
        fprintf(stderr,"Error en dec_cab: PSC incorrecta\n");
        return 1;
    }
#ifdef DEBUG
    else {
        fprintf(stderr,"PSC leida\n");
    }
#endif


    // --------- TR ---------------------------
    campo=lector.lee_bits(8);
#ifdef DEBUG
    fprintf(stderr,"TR=%i\n", campo);
#endif


    // --------- PTYPE ---------------------------
    campo=lector.lee_bits(2);
    if (campo!=2) {
        fprintf(stderr,"Error en dec_cab: PTYPE incorrecto\n");
//      return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Split Screen=%i\n", campo);
#endif
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Document Camera=%i\n", campo);
#endif
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Freeze Picture=%i\n", campo);
#endif

    //
    // Formato de la imagen
    // Se comprueba que coincide con el taman~o de la imagen
    //
    campo=lector.lee_bits(3);
    switch (campo) {
    case 0x1:
        cab.formato=SQCIF;
        if ( (cab.width != 128) || (cab.height != 96) ) {
            //fprintf(stderr,"Error en dec_cab: Source Format SQCIF distinto de %dx%d\n",
            //        cab.width, cab.height);
            //return 1;
            cab.width = 128;
            cab.height= 96;
        }
        break;
    case 0x2:
        cab.formato=QCIF;
        /*if ( (cab.width != 176) || (cab.height != 144) ) {
            fprintf(stderr,"Error en dec_cab: Source Format QCIF distinto de %dx%d\n",
                    cab.width, cab.height);
            return 1;
        }*/
        cab.width = 176;
        cab.height= 144;
        break;
    case 0x3:
        cab.formato=CIF;
        /*if ( (cab.width != 352) || (cab.height != 288) ) {
            fprintf(stderr,"Error en dec_cab: Source Format CIF distinto de %dx%d\n",
                    cab.width, cab.height);
            return 1;
        }*/
        cab.width = 352;
        cab.height= 288;
        break;
    case 0x4:
        cab.formato=CIF4;
        /*if ( (cab.width != 704) || (cab.height != 576) ) {
            fprintf(stderr,"Error en dec_cab: Source Format CIF4 distinto de %dx%d\n",
                    cab.width, cab.height);
            return 1;
        }*/
        cab.width = 704;
        cab.height= 576;
        break;
    case 0x5:
        cab.formato=CIF16;
        /*if ( (cab.width != 1408) || (cab.height != 1152) ) {
            fprintf(stderr,"Error en dec_cab: Source Format CIF16 distinto de %dx%d\n",
                    cab.width, cab.height);
            return 1;
        }*/
        cab.width = 1408;
        cab.height= 1152;
        break;
    case 0x7:
        cab.formato=OTRO;
        cab.width = 1408;
        cab.height= 1152;
        break;
    default:
        fprintf(stderr,"Error en dec_cab: Source Format prohibido\n");
        return 1;
    }
#ifdef DEBUG
    fprintf(stderr,"Source Format=%i\n", campo);
#endif

    if (cab.formato != OTRO) {
        return dec_cab_noplus (cab);
    } else {
        return dec_cab_plus (cab);
    }
}


/*********************************************************************
* Entradas:
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
*        cab: datos de la imagen (inter/intra, cuantificador...)
* Descripcion:
*        Decodifica la cabecera de la imagen.
*        No se admite ninguno de los modos opcionales
**********************************************************************/
int
Dec::dec_cab_noplus (CAB &cab)
{
    u32 campo;

    cab.INTER=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"INTER?=%i\n", cab.INTER);
#endif

    //
    // Los 4 modos opcionales. No se admite ninguno
    //
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Unrestricted Motion Vector=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 4\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Syntax Arithmetic=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 5\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Advanced Prediction=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 6\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"PB-frames=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 7\n");
        return 1;
    }

    // --------- PQUANT ---------------------------
    cab.quant=lector.lee_bits(5);

#ifdef DEBUG
    fprintf(stderr,"PQUANT=%i\n", cab.quant);
#endif


    // --------- CPM ---------------------------
    // Este modo no se admite
    //
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"CPM=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 8\n");
        return 1;
    }


    // Como no PB_FRAME, no TRB ni DBQUANT


    // --------- PEI ---------------------------
    while ((campo=lector.lee_bits(1))) {
        // --- PSPARE
        // Se descarta
        campo=lector.lee_bits(8);
    }

    return 0;
}


/*********************************************************************
* Entradas:
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
*        cab: datos de la imagen (inter/intra, cuantificador...)
* Descripcion:
*        Decodifica la cabecera de la imagen.
*        No se admite ninguno de los modos opcionales
**********************************************************************/
int
Dec::dec_cab_plus (CAB &cab)
{
    u32 campo;

    // -----------  UFEP  --------------------
    campo=lector.lee_bits(3);
#ifdef DEBUG
    fprintf(stderr,"UFEP=%i\n", campo);
#endif
    if (!campo) {
        fprintf(stderr,"Error en dec_cab_plus 4\n");
        return 1;
    }

    // -----------  OPPTYPE  --------------------
    campo = lector.lee_bits(3);
#ifdef DEBUG
    fprintf(stderr,"Source Format=%i\n", campo);
#endif
    if (campo != 6) {
        fprintf(stderr,"Error en dec_cab_plus 5\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Picture Clock Format=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_plus 6\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Unrestricted Motion Vector=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_plus 7\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Syntax-based Arithmetic Coding=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_plus 8\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Advanced Prediction=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_plus 9\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Advanced Intra Coding=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_plus 10\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Deblocking Filter=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 11\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Slice Structured=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 12\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Reference Picture Selection=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 13\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Independent Segment Decoding=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 14\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Alternative Inter VLC=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 15\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Modified Quantization=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 16\n");
        return 1;
    }
    campo=lector.lee_bits(1);
    if (!campo) {
        fprintf(stderr,"Error en dec_cab_noplus 17\n");
        return 1;
    }
    campo=lector.lee_bits(3);
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 18\n");
        return 1;
    }

    // -----------  MPPTYPE  --------------------

    cab.INTER=lector.lee_bits(3);
#ifdef DEBUG
    fprintf(stderr,"INTER?=%i\n", cab.INTER);
#endif
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Reference Picture Resampling=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 19\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Reduced-resolution Update=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 20\n");
        return 1;
    }
    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"Rounding Type=%i\n", campo);
#endif
    if (!campo) {
        fprintf(stderr,"Error en dec_cab_noplus 21\n");
        return 1;
    }
    campo=lector.lee_bits(2);
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 22\n");
        return 1;
    }
    campo=lector.lee_bits(1);
    if (!campo) {
        fprintf(stderr,"Error en dec_cab_noplus 23\n");
        return 1;
    }

    // -----------  CPM  --------------------

    campo=lector.lee_bits(1);
#ifdef DEBUG
    fprintf(stderr,"CPM=%i\n", campo);
#endif
    if (campo) {
        fprintf(stderr,"Error en dec_cab_noplus 24\n");
        return 1;
    }

    // -----------  CPFMT  --------------------

    campo=lector.lee_bits(4);
#ifdef DEBUG
    fprintf(stderr,"PAR=%i\n", campo);
#endif
    campo=lector.lee_bits(9);
    cab.width=(campo+1)*4;

#ifdef DEBUG
    fprintf(stderr,"PWI=%i\n", campo);
#endif
    campo=lector.lee_bits(1);
    campo=lector.lee_bits(9);
    cab.height=(campo)*4;

#ifdef DEBUG
    fprintf(stderr,"PHI=%i\n", campo);
#endif

    //No EPAR, CPCFC, ETR, UUI, SSS, ELNUM, RLNUM, RPSMF, TRPI, TRP, BCI, capa de BCM, RPRP

    // -----------  PQUANT ----------------------

    campo=lector.lee_bits(5);
    cab.quant=campo;
#ifdef DEBUG
    fprintf(stderr,"PQUANT=%i\n", cab.quant);
#endif

    // -----------  PEI  --------------------

    while ((campo=lector.lee_bits(1))){
        // --- PSPARE
        // Se descarta
        campo=lector.lee_bits(8);
    }

    return 0;
}



/**********************************************************************
* Entradas:
*        img_nueva
* Salidas:
*        img_cache
* Descripcion:
*        Copia el contenido de 'img_nueva' en 'img_cache'
************************************************************************/
void
Dec::actualiza_imagen (yuvImage_t *img_cache, yuvImage_t *img_nueva)
{
    int w=img_nueva->w;
    int h=img_nueva->h;

    memcpy (img_cache->lum,   img_nueva->lum,   w*h);
    memcpy (img_cache->cromU, img_nueva->cromU, w*h/4);
    memcpy (img_cache->cromV, img_nueva->cromV, w*h/4);
}



/****************************************************************************

 ------------- IMPLEMENTACION DE LOS METODOS DE LA CLASE: ------------------
 ------------- GOB                                        ------------------

****************************************************************************/


/************************************************************************
* Entradas:
*        lector: se encarga de ir leyendo la cadena de bits H263
*        img_cache: anterior imagen que se decodifico (para sumarle
*      la imagen diferencia decodificada en modo INTER)
*        cab: datos de la imagen (inter/intra, (x,y) actual...)
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
*        img_nueva: imagen tipo yuv que se va a decodificar
* Descripcion:
*        Decodifica un GOB de la imagen.
*        Para ello, primero decodifica la cabecera del GOB (excepto en el
*      primero, que no tiene) y luego decodifica los macrobloques que lo
*      componen.
*        Actualiza la posicion actual (x,y)
**************************************************************************/
int
GOB::dec_gob (Lector &lector, yuvImage_t *img_cache, yuvImage_t *img_nueva, CAB &cab)
{
#ifdef DEBUG
    fprintf(stderr,"Comienza GOB layer\n");
#endif

    //
    // Decodifica la cabecera del GOB.  primero no tiene nunca cabecera
    //
    if ( cab.n_gob == 0 ) {
        cab.hay_cab_gob=0;
    } else {
        dec_cab (lector, cab);
    }


    //
    // Decodificacion de los macrobloques
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
    case OTRO:
        nMacro = (cab.width/16)*((cab.height/cab.nGOBs)/16);
        break;
    }

    //fprintf(stderr,"macrobloquesDEC= %d\n", nMacro);
    for (int i=0; i<nMacro; i++) {
        cab.n_macro=i;
        if ( macroblock.dec_macroblock (lector, img_cache, img_nueva, cab) ) {
            fprintf(stderr,"Error en dec_gob 1\n");
            return 1;
        }
        cab.x+=16;
        if (cab.x == cab.width) {
//          fprintf(stderr,"x=%d, y=%d\n", cab.x, cab.y);
            cab.x=0;
            cab.y+=16;
        }
    }

    return 0;
}



/***********************************************************************
* Entradas:
*        lector: se encarga de ir leyendo la cadena de bits H263
* Salidas:
*        cab: datos de la imagen (cuantificador...)
* Descripcion:
*        Decodifica la cabecera del GOB, si existe
************************************************************************/
void
GOB::dec_cab (Lector &lector, CAB &cab)
{
    u32 campo;
    int alin = 0;

    //
    // GSTUF, para que GBSC (si hay cabecera) este alineado
    //
    if ( (campo=lector.lee_bits(17)) != 0x1 )
    {
        lector.devuelve_bits (17);
        //
        // GSTUF, para que GBSC (si hay cabecera) este alineado
        //
        alin=lector.alinea();
        //
        // GBSC, vemos si hay cabecera o no
        //
        if ( (campo=lector.lee_bits(17)) != 0x1 ) {
#ifdef DEBUG
            fprintf(stderr,"No hay cabecera del GOB\n");
#endif
            lector.devuelve_bits (17 + alin);
            cab.hay_cab_gob=0;
            return;
        }
    }
#ifdef DEBUG
    fprintf(stderr,"GBSC leida, con estos bit alineados %i\n", alin);
#endif
    cab.hay_cab_gob=1;

    // --- GN
    campo=lector.lee_bits(5);
#ifdef DEBUG
    fprintf(stderr,"GN=%i\n", campo);
#endif


    // --- Como no hay CPM, no hay GSBI

    // --- GFID
    campo=lector.lee_bits(2);
#ifdef DEBUG
    fprintf(stderr,"GFID=%i\n", campo);
#endif

    // --- GQUANT
    cab.quant=lector.lee_bits(5);
#ifdef DEBUG
    fprintf(stderr,"GQUANT=%i\n", cab.quant);
#endif
}



/****************************************************************************

 ------------- IMPLEMENTACION DE LOS METODOS DE LA CLASE: ------------------
 ------------- MACROblock                                 ------------------

****************************************************************************/


/************************************************************************
* Entradas:
*        lector: se encarga de ir leyendo la cadena de bits H263
*        img_cache: anterior imagen que se decodifico (para sumarle
*      la imagen diferencia decodificada en modo INTER)
*        cab: datos de la imagen (inter/intra, (x,y) actual...)
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
*        img_nueva: imagen tipo yuv que se va a decodificar
* Descripcion:
*        Decodifica un macrobloque de la imagen.
*        Para ello, primero decodifica la cabecera del macrobloque y luego
*      decodifica los 4 bloques de luminancia y 2 de crominancia.
**************************************************************************/
int
MACROblock::dec_macroblock (Lector &lector,
                            yuvImage_t *img_cache,
                            yuvImage_t *img_nueva,
                            CAB &cab
                           )
{
    int any_AC;


    //
    // Decodifica la cabecera del macrobloque
    //
    if ( dec_cab (lector, cab) ) {
        fprintf(stderr,"Error en dec_macroblock 1\n");
        return 1;
    }


    //
    // Decodifica los 4 bloques de LUMINANCIA
    //                             **********

    // Comprueba si hay coeficientes, aparte del INTRADC
    any_AC = ( (cab.CBPY & 0x8 ) != 0);
    if ( lum.dec_bloque (lector, img_cache->lum, img_nueva->lum, cab, any_AC) ) {
        fprintf(stderr,"Error en dec_macroblock 2\n");
        return 1;
    }
    cab.x+=8;

    // Comprueba si hay coeficientes, aparte del INTRADC
    any_AC = ( (cab.CBPY & 0x4 ) != 0);
    if ( lum.dec_bloque (lector, img_cache->lum, img_nueva->lum, cab, any_AC) ) {
        fprintf(stderr,"Error en dec_macroblock 3\n");
        return 1;
    }
    cab.x-=8;
    cab.y+=8;

    // Comprueba si hay coeficientes, aparte del INTRADC
    any_AC = ( (cab.CBPY & 0x2 ) != 0);
    if ( lum.dec_bloque (lector, img_cache->lum, img_nueva->lum, cab, any_AC) ) {
        fprintf(stderr,"Error en dec_macroblock 4\n");
        return 1;
    }
    cab.x+=8;

    // Comprueba si hay coeficientes, aparte del INTRADC
    any_AC = ( (cab.CBPY & 0x1 ) != 0);
    if ( lum.dec_bloque (lector, img_cache->lum, img_nueva->lum, cab, any_AC) ) {
        fprintf(stderr,"Error en dec_macroblock 5\n");
        return 1;
    }
    cab.x-=8;
    cab.y-=8;


    //
    // Decodifica los 2 bloques de CROMINANCIA
    //                             ***********

    // Comprueba si hay coeficientes, aparte del INTRADC
    any_AC = ( (cab.CBPC & 0x2 ) != 0);
    if ( crom.dec_bloque (lector, img_cache->cromU, img_nueva->cromU, cab, any_AC) ) {
        fprintf(stderr,"Error en dec_macroblock 6\n");
        return 1;
    }

    // Comprueba si hay coeficientes, aparte del INTRADC
    any_AC = ( (cab.CBPC & 0x1 ) != 0);
    if ( crom.dec_bloque (lector, img_cache->cromV, img_nueva->cromV, cab, any_AC) ) {
        fprintf(stderr,"Error en dec_macroblock 7\n");
        return 1;
    }

    return 0;
}



/***********************************************************************
* Entradas:
*        lector: se encarga de ir leyendo la cadena de bits H263
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
*        cab: datos de la imagen (CBPC, CBPY, vector de movimiento...)
* Descripcion:
*        Decodifica la cabecera del macrobloque
************************************************************************/
int
MACROblock::dec_cab (Lector &lector, CAB &cab)
{
    Inf_MCBPC inf_MCBPC;
    Tabla_MCBPC_for_I tabla_MCBPC_for_I;
    Tabla_MCBPC_for_P tabla_MCBPC_for_P;

    Inf_CBPY inf_CBPY;
    Tabla_CBPY tabla_CBPY;

    Inf_MVD inf_MVD;
    Tabla_MVD tabla_MVD;

    u32 campo;
    int pmvx;
    int pmvy;



#ifdef DEBUG
    fprintf(stderr,"Comienza Macro Block layer\n");
#endif

    if (cab.INTER) {
//fprintf(stderr,"INTER-Picture\n");
        // *************************
        // INTER-Picture
        // *************************

        // --- COD
        if (lector.lee_bits (1)) {
            //
            // El macrobloque NO esta codificado
            //
#ifdef DEBUG
            fprintf(stderr,"Macrobloque no codificado\n");
#endif
            cab.INTERmacro=1;
            cab.CBPY=0;
            cab.CBPC=0;
            cab.mvx=0;
            cab.mvy=0;
            actualizar_MV(cab.MV[0], 0, cab);
            actualizar_MV(cab.MV[1], 0, cab);
            return 0;
        } else {
            //
            // El macrobloque esta codificado
            //
            // --- MCBPC
            if ( tabla_MCBPC_for_P.obtener_info (lector, inf_MCBPC) ) {
                fprintf(stderr,"Error en dec_cab 1\n");
                return 1;
            }
            cab.CBPC = inf_MCBPC.CBPC;
#ifdef DEBUG
            fprintf(stderr,"MBtype=%c\n", inf_MCBPC.MBtype);
            fprintf(stderr,"CBPC=%i\n", inf_MCBPC.CBPC);
#endif

            if (inf_MCBPC.MBtype == '3' || inf_MCBPC.MBtype == '4') {
                cab.INTERmacro=0;
            } else {
                cab.INTERmacro=1;
            }


            // --- CBPY
            if ( tabla_CBPY.obtener_info (lector, inf_CBPY) ) {
                fprintf(stderr,"Error en dec_cab 2\n");
                return 1;
            }
            if (cab.INTERmacro) {
                cab.CBPY = inf_CBPY.CBPY_P;
            } else {
                cab.CBPY = inf_CBPY.CBPY_I;
            }
#ifdef DEBUG
            fprintf(stderr,"CBPY=%i\n", cab.CBPY);
#endif


            // --- DQUANT
            if (inf_MCBPC.MBtype == '1' || inf_MCBPC.MBtype == '4') {
                campo=lector.lee_bits(2);
                switch (campo) {
                case 0:
#ifdef DEBUG
                    fprintf(stderr,"DQUANT=-1\n");
#endif
                    cab.quant -= 1;
                    break;
                case 1:
#ifdef DEBUG
                    fprintf(stderr,"DQUANT=-2\n");
#endif
                    cab.quant -= 2;
                    break;
                case 2:
#ifdef DEBUG
                    fprintf(stderr,"DQUANT=+1\n");
#endif
                    cab.quant += 1;
                    break;
                case 3:
#ifdef DEBUG
                    fprintf(stderr,"DQUANT=+2\n");
#endif
                    cab.quant += 2;
                    break;
                }
            }


            // --- MVD
            if (cab.INTERmacro) {
                if ( tabla_MVD.obtener_info (lector, inf_MVD) ) {
                    fprintf(stderr,"Error en dec_cab 3\n");
                    return 1;
                }
//                fprintf(stderr,"inf_MVDx=%d inf_MVDy=%d\n", inf_MVD.x, inf_MVD.y);
                pmvx = calc_pred (cab.MV[0], cab);
//                fprintf(stderr,"pmvx=%d\n", pmvx);

                if ( (pmvx+inf_MVD.x > -31) && (pmvx+inf_MVD.x < 32) ) {
                    cab.mvx = pmvx+inf_MVD.x;
                } else {
                    cab.mvx = pmvx+inf_MVD.y;
                }
                actualizar_MV (cab.MV[0], cab.mvx, cab);
#ifdef DEBUG
                fprintf(stderr,"MVDx=%i\n", cab.mvx);
#endif
                if ( tabla_MVD.obtener_info (lector, inf_MVD) ) {
                    fprintf(stderr,"Error en dec_cab 4\n");
                    return 1;
                }
                pmvy = calc_pred (cab.MV[1], cab);
//                fprintf(stderr,"pmvy=%d\n", pmvy);

                if ( (pmvy+inf_MVD.x > -31) && (pmvy+inf_MVD.x < 32) ) {
                    cab.mvy = pmvy+inf_MVD.x;
                } else {
                    cab.mvy = pmvy+inf_MVD.y;
                }
                actualizar_MV (cab.MV[1], cab.mvy, cab);
#ifdef DEBUG
                fprintf(stderr,"MVDy=%i\n", cab.mvy);
#endif
//if ((cab.mvy > 32) || (cab.mvy< -32) || (cab.mvx > 32) || (cab.mvx < -32)) return 1;
            } else {
                // En macrobloques INTRA, el predictor se pone a cero
                actualizar_MV(cab.MV[0], 0, cab);
                actualizar_MV(cab.MV[1], 0, cab);
            }


            // Como no hay Advanced Prediction Mode, no hay MVD2-4
        }
    } else {
//fprintf(stderr,"INTRA-Picture\n");
        // *************************
        // INTRA-Picture
        // *************************

        cab.INTERmacro=0;

        // --- MCBPC
        if ( tabla_MCBPC_for_I.obtener_info (lector, inf_MCBPC) ) {
            fprintf(stderr,"Error en dec_cab 5\n");
            return 1;
        }
        cab.CBPC = inf_MCBPC.CBPC;
#ifdef DEBUG
        fprintf(stderr,"MBtype=%c\n", inf_MCBPC.MBtype);
        fprintf(stderr,"CBPC=%i\n", inf_MCBPC.CBPC);
#endif



        // --- CBPY
        if ( tabla_CBPY.obtener_info (lector, inf_CBPY) ) {
            fprintf(stderr,"Error en dec_cab 6\n");
            return 1;
        }
        cab.CBPY = inf_CBPY.CBPY_I;
#ifdef DEBUG
        fprintf(stderr,"CBPY=%i\n", cab.CBPY);
#endif

        // --- DQUANT
        if (inf_MCBPC.MBtype == '4') {
            campo=lector.lee_bits(2);
            switch (campo) {
            case 0:
#ifdef DEBUG
                fprintf(stderr,"DQUANT=-1\n");
#endif
                cab.quant -= 1;
                break;
            case 1:
#ifdef DEBUG
                fprintf(stderr,"DQUANT=-2\n");
#endif
                cab.quant -= 2;
                break;
            case 2:
#ifdef DEBUG
                fprintf(stderr,"DQUANT=+1\n");
#endif
                cab.quant += 1;
                break;
            case 3:
#ifdef DEBUG
                fprintf(stderr,"DQUANT=+2\n");
#endif
                cab.quant += 2;
                break;
            }
        }
    }

    return 0;
}



/***************************************************************************
* Entradas:
*        MV: matriz con los vectores de movimiento de macrobloques contiguos
*            [0] fila superior; [1] fila de macrobloques actual
*        cab: n_gob, n_macro, hay_cab_gob, formato (CIF, 4CIF...)
* Salidas:
*        El predictor del v.movim. para la componente dada en MV
* Descripcion:
*        A partir de los v.movim. contiguos MV1, MV2 y MV3 que obtiene de
*      la matriz MV, devuelve el valor del medio, que sera el predictor
***************************************************************************/
int
MACROblock::calc_pred (int MV [4][88], CAB cab)
{
    int mv1, mv2, mv3;
    int mbPorLinea;

    switch (cab.formato) {
    case SQCIF:
        mbPorLinea=8;
        break;
    case QCIF:
        mbPorLinea=11;
        break;
    case CIF:
        mbPorLinea=22;
        break;
    case CIF4:
        mbPorLinea=44;
        break;
    case CIF16:
        mbPorLinea=88;
        break;
    case OTRO:
        mbPorLinea = (cab.width/16);
        break;
    }


    //
    // MV1
    //
    if (cab.n_macro==0)
        mv1=0;
    else
        mv1 = MV[1][cab.n_macro-1];

//fprintf(stderr,"mv1=%d ", mv1);
    //
    // MV2, MV3
    //
    if (  ( (cab.n_macro/mbPorLinea) == 0)  // MV2,MV3 estan en otro GOB superior
          && (  (cab.n_gob==0)  ||          // 1er GOB (i.e. estan fuera del pict.)
                (cab.hay_cab_gob) ) ) {     // El GOB tiene cabecera
        mv2=mv3=mv1;
    } else {
//        fprintf(stderr,"nmacro para mv2=%d\n", cab.n_macro);
        mv2 = MV[0][cab.n_macro];
        if ( cab.n_macro == mbPorLinea-1 ) {
            mv3=0;
        } else {
//            fprintf(stderr,"nmacro para mv3=%d\n", cab.n_macro+1);
            mv3 = MV[0][cab.n_macro+1];
        }
    }

//    fprintf(stderr,"mv2=%d mv3=%d\n", mv2, mv3);

    //
    // PMV: el valor del medio
    //
    return mv1+mv2+mv3 - mmax(mv1,mmax(mv2,mv3)) - mmin(mv1,mmin(mv2,mv3));
}



/***************************************************************************
* Entradas:
*        MV: matriz con los vectores de movimiento de macrobloques contiguos
*            [0] fila superior; [1] fila de macrobloques actual
*        mv: compon. del vector hay que almacenar para posteriores calculos
*        cab: n_macro, formato (CIF, 4CIF...)
* Salidas:
* Descripcion:
*        Almacena la componente del v.movim. para que despues se puedan
*      calcular los predictores
***************************************************************************/
void
MACROblock::actualizar_MV (int MV [4][88], int mv, CAB cab)
{
    int mbPorLinea;

    switch (cab.formato) {
    case SQCIF:
        mbPorLinea=8;
        break;
    case QCIF:
        mbPorLinea=11;
        break;
    case CIF:
        mbPorLinea=22;
        break;
    case CIF4:
        mbPorLinea=44;
        break;
    case CIF16:
        mbPorLinea=88;
        break;
    case OTRO:
        mbPorLinea = (cab.width/16);
        break;
    }


//fprintf(stderr,"actualiza_MV nmacro=%d mod mbporlinea=%d\n", cab.n_macro, mbPorLinea);
    MV[1][ cab.n_macro % mbPorLinea ]=mv;
    MV[0][ cab.n_macro % mbPorLinea ]=mv;
//fprintf(stderr,"ya\n");
}






/****************************************************************************

 ------------- IMPLEMENTACION DE LOS METODOS DE LA CLASE: ------------------
 ------------- Block                                      ------------------

****************************************************************************/

void ChenIDct (short *x, short *y);


/***************************************************************************
* Entradas:
*        cache: componente (Y, Cb o Cr) de la ultima imagen que se decodifico
*        cab: datos de la imagen (inter/intra, (x,y) actual...)
*        any_AC: 1 si hay algun coef. AC no nulo
* Salidas:
*        0 si todo va bien
*        1 si hay problemas
*        nueva: componente (Y, Cb o Cr) de la imagen que se va a decodificar
* Descripcion:
*        Decodifica el bloque actual.
*        Situa la matriz decodificada de 8x8 pixels en la posicion
*      correspondiente de la imagen nueva
***************************************************************************/
int
Block::dec_bloque (Lector &lector, u8 *cache, u8 *nueva, CAB &cab, int any_AC)
{
    int campo;
    Inf_TCOEF inf;
    Tabla_TCOEF tabla;
    int cont=0;

    i16 ristra [64];
    i16 mat[8][8];
    i16 mat_dif[8][8];
    i16 mat_pix[8][8];

    //
    // Si es INTERmacro y no hay coeficientes AC el bloque es 0,
    // directamente se copia la imagen antigua en la nueva
    // (teniendo en cuenta el Motion Vector)
    //
    if (cab.INTERmacro && (!any_AC) ) {
        coge_bloque (cache, mat, cab);
        mete_bloque (nueva, mat, cab);
        return 0;
    }



    //
    // Decodificacion de INTRADC (solo en macrobloques INTRA)
    //                   *******
    if (!cab.INTERmacro) {
        campo=lector.lee_bits(8);
        if (campo == 255)
            campo=1024;
        else
            campo*=8;
        ristra[cont++] = campo;
#ifdef DEBUG
        fprintf(stderr,"INTRADC=%i\n", campo);
#endif
    }


    //
    // Decodificacion de los simbolos TCOEF
    //  (relleno de ristra de coeficientes)
    //
    if (any_AC) {
#ifdef DEBUG
        fprintf(stderr,"Presentes coeficientes AC\n");
#endif
        do {
            if ( tabla.obtener_info (lector, inf) ) {
                fprintf(stderr,"Error en dec_bloque 1\n");
                return 1;
            }
#ifdef DEBUG
            fprintf(stderr,"%i %i %i %i\n", inf.last, inf.run, inf.level, inf.sign);
#endif
            while (inf.run--)
                ristra[cont++]=0;

            //
            // Cuantificacion inversa del coeficiente
            //
            if ( (cab.quant % 2) == 0) { // par
                ristra[cont] = cab.quant * (2*inf.level + 1) - 1;
                if (inf.sign)
                    ristra[cont]*=(-1);
            } else { //impar
                ristra[cont] = cab.quant * (2*inf.level + 1);
                if (inf.sign)
                    ristra[cont]*=(-1);
            }

            //
            // Clipping
            //
            if (ristra[cont] < (-2048))
                ristra [cont]=(-2048);
            if (ristra[cont] > 2047)
                ristra [cont]=2047;
            cont++;

        } while (!inf.last);
    }

    // completar la ristra con 0's hasta el final
    while ( cont!=64)
        ristra[cont++]=0;



    //
    // Pasar en zigzag a una matriz de coeficientes y
    // hacer la DCT inversa para obtener la matriz de pixels
    //
    //
    if (cab.INTERmacro) {
        //
        // Macrobloque tipo INTER
        //
        ristra2mat (ristra, mat_dif);
        ChenIDct ( mat_dif[0] , mat_pix[0] );
        coge_bloque (cache, mat, cab);

#ifdef DEBUG
        fprintf(stderr,"Matriz coef\n");
        escribe_matriz (mat_dif);
        fprintf(stderr,"Matriz de pixel base de prediccion\n");
        escribe_matriz (mat);
        fprintf(stderr,"Matriz de pixel de error\n");
        escribe_matriz (mat_pix);
#endif

        // Suma la matriz de pixels error(recien decodificada) y
        //      la matriz de pixels de la imagen anterior apuntada por VecMov
        //
        suma_mat (mat_pix, mat);
    } else {
        //
        // Macrobloque tipo INTRA
        //
        ristra2mat(ristra, mat);
        ChenIDct ( mat[0] , mat_pix[0] );

#ifdef DEBUG
        fprintf(stderr,"Matriz coef\n");
        escribe_matriz (mat);
        fprintf(stderr,"Matriz pixel\n");
        escribe_matriz (mat_pix);
#endif
    }


    //
    // Meter los pixels decodificados en la zona corresp. de la imagen
    //
#ifdef DEBUG
    fprintf(stderr,"Mete_bloque\n");
#endif
    mete_bloque (nueva, mat_pix, cab);

#ifdef DEBUG
    fprintf(stderr,"Matriz final\n");
    escribe_matriz (mat_pix);
#endif

    return 0;
}



/***********************************************************************
* Entradas:
*        ristra: de 64 coeficientes en fila
* Salidas:
*        matriz: de coeficientes de 8x8
* Descripcion:
*        Recorre la matriz en zigzag y copia en ella la ristra de 64 coef
************************************************************************/
void
Block::ristra2mat(i16 *ristra, i16 matriz[8][8])
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
            matriz [y][x] = ristra [ conv[y][x]-1 ];
        }
    }
}


/*********************************************************************
* Entradas:
*        mat1
*        mat2
* Salidas:
*        mat1 = mat1 + mat2
**********************************************************************/
void
Block::suma_mat (i16 mat1[8][8], i16 mat2[8][8])
{
    for (int x=0; x<8; x++) {
        for (int y=0; y<8; y++) {
            mat1[y][x] += mat2[y][x];
        }
    }
}



/*********************************************************************
* Entradas:
*        mat
* Descripcion:
*        Escribe la matriz en standard error
**********************************************************************/
void
Block::escribe_matriz(i16 mat[8][8])
{
    for (int y=0; y<8; y++) {
        for (int x=0; x<8; x++) {
            fprintf(stderr,"%i ", mat [y][x]);
        }
        fprintf(stderr,"\n");
    }
}




/***********************************************************************
* Entradas:
*        src: componente de LUMINANCIA de una imagen
*        cab: datos de la imagen ( (x,y) , vector de movimiento ...)
* Salidas:
*        mat: de pixels de 8x8
* Descripcion:
*        Mete en mat un bloque de 8x8 situado en (x,y) de la imagen src,
*      con el (mvx, mvy) multiplicado por 2
************************************************************************/
void
Block_lum::coge_bloque (u8 *src, i16 mat[8][8], CAB &cab)
{
    int mvx=cab.mvx;
    int mvy=cab.mvy;
    int x=cab.x;
    int y=cab.y;
    int width=cab.width;



    int xhalf = mvx % 2;  // 2*mvx es impar -> mvx es ~,5
    int yhalf = mvy % 2;

    int xnear= (mvx>=0) ? 1 : -1; // si mvx<0, interpolam con izda, no con dcha
    int ynear= (mvy>=0) ? 1 : -1; // si mvy<0, interpolam con arriba, no abajo


    if (!xhalf && !yhalf) // los dos enteros
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = src [(y+yy+mvy/2)*width + (x+xx+mvx/2)];


    else if (xhalf && !yhalf) // solo x fraccionario
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = (src [(y+yy+mvy/2)*width + (x+xx+mvx/2)] +
                               src [(y+yy+mvy/2)*width + (x+xx+mvx/2 + xnear)] +
                               1) / 2;
    else if (!xhalf && yhalf) // solo y fraccionario
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = (src [(y+yy+mvy/2)*width + (x+xx+mvx/2)] +
                               src [(y+yy+mvy/2 + ynear)*width + (x+xx+mvx/2)] +
                               1) / 2;
    else // los dos x,y faccionarios
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = (src [(y+yy+mvy/2)*width + (x+xx+mvx/2)] +
                               src [(y+yy+mvy/2)*width + (x+xx+mvx/2 + xnear)] +
                               src [(y+yy+mvy/2 + ynear)*width + (x+xx+mvx/2)] +
                               src [(y+yy+mvy/2 + ynear)*width + (x+xx+mvx/2 + xnear)]+
                               2) / 4;
}


/***********************************************************************
* Entradas:
*        src: componente de CROMINANCIA de una imagen
*        cab: datos de la imagen ( (x,y) , vector de movimiento ...)
* Salidas:
*        mat: de pixels de 8x8
* Descripcion:
*        Mete en mat un bloque de 8x8 situado en (x,y) de la imagen src,
*      con el (mvx, mvy) multiplicado por 2
************************************************************************/
void
Block_crom::coge_bloque (u8 *src, i16 mat[8][8], CAB &cab)
{
    int mvx=cab.mvx;
    int mvy=cab.mvy;
    int x=cab.x / 2;
    int y=cab.y / 2;
    int width=cab.width / 2;


    int xhalf = mvx % 4;  // 2*mvx no es multiplo de 4 -> mvx/2 se redondea a ~,5
    int yhalf = mvy % 4;

    int xnear= (mvx>=0) ? 1 : -1;
    int ynear= (mvy>=0) ? 1 : -1;


    if (!xhalf && !yhalf) // los dos enteros
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = src [(y+yy+mvy/4)*width + (x+xx+mvx/4)];


    else if (xhalf && !yhalf) // solo x fraccionario
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = (src [(y+yy+mvy/4)*width + (x+xx+mvx/4)] +
                               src [(y+yy+mvy/4)*width + (x+xx+mvx/4 + xnear)] +
                               1) / 2;
    else if (!xhalf && yhalf) // solo y fraccionario
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = (src [(y+yy+mvy/4)*width + (x+xx+mvx/4)] +
                               src [(y+yy+mvy/4 + ynear)*width + (x+xx+mvx/4)] +
                               1) / 2;
    else // los dos x,y faccionarios
        for (int xx=0; xx<8; xx++)
            for (int yy=0; yy<8; yy++)
                mat[yy][xx] = (src [(y+yy+mvy/4)*width + (x+xx+mvx/4)] +
                               src [(y+yy+mvy/4)*width + (x+xx+mvx/4 + xnear)] +
                               src [(y+yy+mvy/4 + ynear)*width + (x+xx+mvx/4)] +
                               src [(y+yy+mvy/4 + ynear)*width + (x+xx+mvx/4 + xnear)]+
                               2) / 4;
}



/***********************************************************************
* Entradas:
*        mat: de pixels de 8x8
*        cab: datos de la imagen ( (x,y) ...)
* Salidas:
*        dest: componente de LUMINANCIA de una imagen
* Descripcion:
*        Mete en la imagen dest en (x,y) un bloque mat de 8x8
************************************************************************/
void
Block_lum::mete_bloque (u8 *dest, i16 mat[8][8], CAB &cab)
{
    int x=cab.x;
    int y=cab.y;
    int width=cab.width;

    for (int xx=0; xx<8; xx++)
        for (int yy=0; yy<8; yy++)
            //Tener en cuenta conversion signed short -> unsigned char
            if ( mat[yy][xx] > 255 )
                dest [(y+yy)*width + (x+xx)] = 255;
            else if (mat[yy][xx] < 0 )
                dest [(y+yy)*width + (x+xx)] = 0;
            else
                dest [(y+yy)*width + (x+xx)] = mat[yy][xx];
}


/***********************************************************************
* Entradas:
*        mat: de pixels de 8x8
*        cab: datos de la imagen ( (x,y) ...)
* Salidas:
*        dest: componente de CROMINANCIA de una imagen
* Descripcion:
*        Mete en la imagen dest en (x,y) un bloque mat de 8x8
************************************************************************/
void
Block_crom::mete_bloque (u8 *dest, i16 mat[8][8], CAB &cab)
{
    int x    =cab.x / 2;
    int y    =cab.y / 2;
    int width=cab.width / 2;

    for (int xx=0; xx<8; xx++)
        for (int yy=0; yy<8; yy++)
            //Tener en cuenta conversion signed short -> unsigned char
            if ( mat[yy][xx] > 255 )
                dest [(y+yy)*width + (x+xx)] = 255;
            else if (mat[yy][xx] < 0 )
                dest [(y+yy)*width + (x+xx)] = 0;
            else
                dest [(y+yy)*width + (x+xx)] = mat[yy][xx];
}

