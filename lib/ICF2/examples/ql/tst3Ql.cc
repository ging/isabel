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
// $Id: tst3Ql.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------
// PRUEBA DE LISTA DE PUNTEROS A ENTERO + OPERADOR DE ASIGNACION: ql_t<int *>
//      Se hace asignacion de una lista a otra. Como esta 
//      redefinido en operador de asignacion y estoy trabajando
//      con punteros, la asignacion solo copia segun el operador
//      de asignacion
// --------------------------------------------------------------------------


#include <stdio.h>
#include <icf2/ql.hh>


typedef ql_t<int *> intPointerList_t;

class betterIntPointerList_t: public intPointerList_t {
     public:
         betterIntPointerList_t &operator=(betterIntPointerList_t &other) {
              // se liberan los elementos que hubiera en la lista 
              while (len()) {
                  int *aux = head();
                  behead();
                  delete aux;
              }

              // se copian los nuevos elementos  
              for (betterIntPointerList_t::iterator_t i = other.begin();
                                                      i!= other.end();
                                                      i++
                  )
              {
                  int *aux = static_cast <int *>(i);
                  insert(new int(*aux));
              }
                  
         }
};

main()
{

    // Se crean 2 listas de enteros:
    //   s1: numeros impares
    //   s2: numeros pares

    betterIntPointerList_t s1;
    betterIntPointerList_t s2;

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
    for (betterIntPointerList_t::iterator_t i = s1.begin();
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
    for (betterIntPointerList_t::iterator_t i = s2.begin();
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
    // Ojo! no se esta perdiendo memoria porque el operador de
    // asignacion libera la memoria antes de hacer la asignacion!
    s1= s2;

    // Se imprimen los elementos de la primera lista s1:
    // deberian imprimirse los numeros {pares}
    // 0 2 4 6 8
    printf("First list:\n");
    while(s1.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        int *aux= s1.head();
        printf("%d ", *aux);
        // Se elimina el primer elemento de la lista 
        s1.behead();
        // pero tambien hay que eliminar la memoria reservada para
        // alamacenar el entero
        delete aux;
    }
    // La lista s1 tiene ahora cero elementos
    printf("\n");


    // Se imprimen los elementos de la segunda lista xx:
    // deberian imprimirse los numeros {pares}
    // 0 2 4 6 8

    printf("Second list:\n");
    while(s2.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        int *aux= s2.head();
        printf("%d ", *aux);
        // Se elimina el primer elemento de la lista 
        s2.behead();
        // pero tambien hay que eliminar la memoria reservada para
        // alamacenar el entero
        delete aux;
    }

    // La lista s2 tiene ahora cero elementos
    printf("\n");

    return 0;  // keeps compiler happy
}
