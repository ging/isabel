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
// $Id: codec.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

/****************************************************************************

   Descripcion:
          Definicion de las clases Cod y Dec, asi como de las
          clases comunes GOB, MACROblock y Block

****************************************************************************/


#ifndef __CODEC_HH__
#define __CODEC_HH__

#include "lector.hh"
#include "escritor.hh"
#include "H263Impl.hh"

#define mmax(a, b)        ((a) > (b) ? (a) : (b))
#define mmin(a, b)        ((a) < (b) ? (a) : (b))


typedef enum {SQCIF=1, QCIF=2, CIF=3, CIF4=4, CIF16=5, OTRO=7} H263SourceFormat;


typedef struct {
    H263SourceFormat formato;
    unsigned width;
    unsigned height;
    int INTER;
    int INTERmacro;
    int quant;
    int CBPC;
    int CBPY;
    int hay_cab_gob;
    int n_gob;
    int n_macro;
    int x;
    int y;
    int mvx;
    int mvy;
    int MV[2][4][88]; // [x/y][fila_anterior(0) o actual(1)][columna]     ////////////////////7777777 2 2 88

    int nGOBs;
} CAB;



class Block {
protected:
    void ristra2mat(i16 *ristra, i16 matriz [8][8]);
    void mat2ristra(i16 matriz [8][8], i16 *ristra);
    void suma_mat (i16 matriz1 [8][8], i16 matriz2 [8][8]);
    void resta_mat (i16 matriz1 [8][8], i16 matriz2 [8][8]);
    void escribe_matriz (i16 mat[8][8]);

    virtual void coge_bloque (u8 *src, i16 mat[8][8], CAB &cab) = 0;
    virtual void mete_bloque (u8 *dest, i16 mat[8][8], CAB &cab) = 0;

public:
    virtual ~Block(void) {};

    int dec_bloque (Lector &lector, u8 *cache, u8 *nueva, CAB &cab, int any_AC);
    int escribe_ristra (u8 *cache, u8*nueva, i16 *ristra, CAB &cab);
    int cod_bloque(Escritor &escritor, i16 *ristra, int n_AC, CAB &cab);
};



class Block_lum : public Block {
private:
    virtual void coge_bloque (u8 *src, i16 mat[8][8], CAB &cab);
    virtual void mete_bloque (u8 *dest, i16 mat[8][8], CAB &cab);

public:
    virtual ~Block_lum(void) {};
};

class Block_crom : public Block {
private:
    virtual void coge_bloque (u8 *src, i16 mat[8][8], CAB &cab);
    virtual void mete_bloque (u8 *dest, i16 mat[8][8], CAB &cab);

public:
    virtual ~Block_crom(void) {};
};




class MACROblock {
private:
    Block_lum lum;
    Block_crom crom;
    int calc_pred (int MV [4][88], CAB cab);  /////////////////////////////////////////77     2 88
    void actualizar_MV (int MV [4][88], int mv, CAB cab); /////////////////////////////////    4 88

    int dec_cab (Lector &lector, CAB &cab);
    int cod_cab (Escritor &escritor, CAB &cab);

public:
    int dec_macroblock (Lector &lector, yuvImage_t *img_cache, yuvImage_t *img_nueva, CAB &cab);
    int cod_macroblock (Escritor &escritor, yuvImage_t *img_cache, yuvImage_t *img_nueva, CAB &cab);

};


class GOB {
private:
    MACROblock macroblock;
    void dec_cab (Lector &lector, CAB &cab);

public:
    int dec_gob (Lector &lector, yuvImage_t *img_cache, yuvImage_t *img_nueva, CAB &cab);
    int cod_gob (Escritor &escritor, yuvImage_t *img_cache, yuvImage_t *img_nueva, CAB &cab, H263EncodedImage_t *);
};



class Dec {
private:
    CAB cab;
    Lector lector;
    GOB gob;

    void actualiza_imagen (yuvImage_t *img_cache, yuvImage_t *img_nueva);
    int dec_cab (CAB &cab);
    int dec_cab_noplus (CAB &cab);
    int dec_cab_plus (CAB &cab);

public:
    yuvImage_t * dec_pic (H263DecodeSession_t *, unsigned char *);
};



class Cod {
private:
    CAB cab;
    Escritor escritor;
    GOB gob;
int elQuant;

    void actualiza_imagen (yuvImage_t *img_cache, yuvImage_t *img_nueva);
    void cod_cab (CAB &cab, H263EncodedImage_t *img_H263);
    void cod_cab_noplus (CAB &cab, H263EncodedImage_t *img_H263);
    void cod_cab_plus (CAB &cab, H263EncodedImage_t *img_H263);

public:
    H263EncodedImage_t * cod_pic (H263EncodeSession_t *,
                                  yuvImage_t *,
                                  int mode,
                                  float quality
                                 );
};


#endif
