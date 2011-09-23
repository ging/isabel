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
// $Id: tst2Ql.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


// ------------------------------------------------------------------
// PRUEBA DE LISTA DE PUNTEROS A ENTERO (NO FUNCIONA!!!): ql_t<int *>
//      Se hace asignacion de una lista a otra. Como no esta 
//      redefinido en operador de asignacion y estoy trabajando
//      con punteros, la asignacion solo copia los punteros y
//      no el contenido... (puede dar problemas, como en este
//      ejemplo)
// ------------------------------------------------------------------


#include <stdio.h>
#include <icf2/ql.hh>


typedef ql_t<int *> intPointerList_t;

main()
{

    // Se crean 3 listas de enteros:
    //   s1: numeros impares
    //   s2: numeros pares

    intPointerList_t s1;
    intPointerList_t s2;

    // se insertan los numeros impares en s1,
    // tambien se podrian haber insertado de la siguiente forma:
    //    s1.insert(new int(1)); 
    //    s1.insert(new int(3)); 
    //    s1.insert(new int(5)); 
    //    s1.insert(new int(7)); 
    //    s1.insert(new int(9));
    // utilizando el metodo insert en vez del el operador <<
    s1 << new int(1) << new int(3) << new int(5) << new int(7) << new int(9);
    // se insertan los numeros pares en s2
    s2 << new int(0) << new int(2) << new int(4) << new int(6) << new int(8); 

    // se imprimen los elementos de la lista s1:
    // 1 3 5 7 9
    printf("First list:\n");
    for (intPointerList_t::iterator_t i = s1.begin();
                               i!= s1.end();
                               i++
        )
    {
         int *aux = static_cast<int *>(i); 
         printf("%d ", *aux);
    }
    printf("\n");


    // se imprimen los elementos de la lista s2:
    // 0 2 4 6 8
    printf("Second list:\n");
    for (intPointerList_t::iterator_t i = s2.begin();
                               i!= s2.end();
                               i++
        )
    {
         int *aux = static_cast<int *>(i); 
         printf("%d ", *aux);
    }
    printf("\n");


    printf("Mixing lists...\n");

    // s1 pasa a contener la lista de numeros {pares}
    // Ojo! se esta perdiendo memoria porque los numeros impares
    // reservados para s1 se pierden y no se liberan!!!
    s1= s2;

    // Se imprimen los elementos de la primera lista s1:
    // deberian imprimirse los numeros {pares}
    // 0 2 4 6 8
    printf("First list:\n", s1.len());
    while(s1.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        int *aux= s1.head();
        printf("%d ", *aux);
        // Se elimina el primer elemento de la lista 
        s1.behead();
        // pero tambien hay que eliminar la memoria reservada para
        // almacenar el entero
        delete aux;
    }
    // La lista s1 tiene ahora cero elementos
    printf("\n");


    // Se imprimen los elementos de la segunda lista s2:
    // deberian imprimirse los numeros {pares}
    // 0 2 4 6 8
    // no se hace bien porque se supone que ya se han liberado!!!
    // al borrar los elementos de s1 se estan borrando los elementos
    // de s2, esto es porque cuando se ha realizado la copia con:
    // s1= s2;
    // lo unico que se han copiado son los punteros y no las zonas de
    // memoria! por tanto al liberar los elementos de s1 se estan
    // liberando tambien los elementos de s2.

    printf("Second list:\n");
    while(s2.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        int *aux= s2.head();
        printf("%d ", *aux);
        // Se elimina el primer elemento de la lista 
        s2.behead();
        // pero tambien hay que eliminar la memoria reservada para
        // almacenar el entero
        delete aux;
    }

    printf("\n");

    return 0;  // keeps compiler happy
}
