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
// $Id: config.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

/****************************************************************************

   Descripcion:
          Definicion de los tres parametros configurables del
          codificador.

****************************************************************************/


#ifndef __CONFIG_HH__
#define __CONFIG_HH__

//
// Parametros configurables del CODIFICADOR
//


//
// Valor del cuantificador
// Rango de valores permitidos:
// min=1  (mejor calidad, pero mas bits necesarios)
// max=31 (peor calidad, menos bits ocupa la imagen)
//
#define QUANT 10


//
// Cada cuantas imagenes se codifica una INTRA
// Rango de valores permitidos:
// min=0   (todas las imagenes INTRA)
// max=132 (impuesto por la Recomendacion H.263)
//
#define MAX_INTER_SEGUIDAS 25


//
// Umbral de diferencia de energia de macrobloques para decidir
// la codificacion del macrobloque INTRA/INTER
//
// Rango de valores (calculo empirico)
// min=500  (mayoria INTRA -> mayor calidad + mas ancho de banda)
// max=5000 (mayoria INTER -> menor calidad + menos ancho de banda)
//
#define SAD 2000


#endif
