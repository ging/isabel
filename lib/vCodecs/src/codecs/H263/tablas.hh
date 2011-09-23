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
// $Id: tablas.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __tablas_hh__
#define __tablas_hh__

#include "lector.hh"
#include "escritor.hh"

typedef struct {
    u8 last;
    u8 run;
    i8 level;
    u16 sign;
} Inf_TCOEF;

typedef struct {
    u8 n_bits;
    u16 code;
    Inf_TCOEF informacion;
} Fila_TCOEF_dec;



typedef struct {
    u8 n_bits;
    u16 code;
} Fila_TCOEF_cod;



class Tabla_TCOEF {
private:
    static Fila_TCOEF_cod tabla_cod [2][41][13];
    static Fila_TCOEF_dec tabla_dec [];
    static int max_index;

public:
    int obtener_info  (Lector &lector, Inf_TCOEF &inf);
    int escribir_info (Escritor &escritor, Inf_TCOEF inf);
};

/*******************************************************/

typedef struct {
    char MBtype; // char, porque stuffing sera 's'
    u8 CBPC;
} Inf_MCBPC;

typedef struct {
    u8 n_bits;
    u16 code;
    Inf_MCBPC informacion;
} Fila_MCBPC;

class Tabla_MCBPC_for_I {
private:
    static Fila_MCBPC tabla [];
    static int max_index;

public:
    int obtener_info  (Lector &lector, Inf_MCBPC &inf);
    int escribir_info (Escritor &escritor, Inf_MCBPC inf);
};

/*******************************************************/

class Tabla_MCBPC_for_P {
private:
    static Fila_MCBPC tabla [];
    static int max_index;

public:
    int obtener_info  (Lector &lector, Inf_MCBPC &inf);
    int escribir_info (Escritor &escritor, Inf_MCBPC inf);
};

/*******************************************************/

typedef struct {
    u8 CBPY_I;
    u8 CBPY_P;
} Inf_CBPY;

typedef struct {
    u8 n_bits;
    u8 code;
    Inf_CBPY informacion;
} Fila_CBPY;

class Tabla_CBPY {
private:
    static Fila_CBPY tabla [];
    static int max_index;

public:
    int obtener_info (Lector &lector, Inf_CBPY &inf);
    int escribir_info_for_I (Escritor &escritor, Inf_CBPY inf);
    int escribir_info_for_P (Escritor &escritor, Inf_CBPY inf);
};

/*******************************************************/

typedef struct {
    int x;
    int y;
} Inf_MVD;

typedef struct {
    u8 n_bits;
    u16 code;
    Inf_MVD informacion;
} Fila_MVD;

class Tabla_MVD {
private:
    static Fila_MVD tabla [];
    static int max_index;

public:
    int obtener_info (Lector &lector,Inf_MVD &inf);
};

/*******************************************************/

#endif
