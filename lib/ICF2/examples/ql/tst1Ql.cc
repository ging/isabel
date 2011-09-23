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
// $Id: tst1Ql.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


// ------------------------------------------------------------------
// PRUEBA DE LISTA DE ENTEROS : ql_t<int>
// ------------------------------------------------------------------


#include <stdio.h>
#include <icf2/ql.hh>

typedef ql_t<int> intList_t;

main()
{

    // Se crean 3 listas de enteros:
    //   s1: numeros impares
    //   s2: numeros pares
    //   xx: numeros impares + numeros pares

    intList_t s1;
    intList_t s2;
    intList_t xx;

    // se insertan los numeros impares en s1,
    // tambien se podrian haber insertado de la siguiente forma:
    //    s1.insert(1); s1.insert(3); s1.insert(5); s1.insert(7); s1.insert(9)
    // utilizando el metodo insert en vez del el operador <<
    s1 << 1 << 3 << 5 << 7  << 9;
    // se insertan los numeros pares en s2
    s2 << 0 << 2 << 4 << 6 << 8; 
    // se insertan los elementos de la lista s1 y s2 es xx, por tanto,
    // se insertan numeros impares y pares en  xx
    xx << s1 << s2;

    // se imprimen los elementos de la lista s1:
    // 1 3 5 7 9
    printf("First list:\n");
    for (intList_t::iterator_t i = s1.begin();
                               i!= s1.end();
                               i++
        )
    {
         int aux = static_cast<int>(i); 
         printf("%d ", aux);
    }
    printf("\n");


    // se imprimen los elementos de la lista s2:
    // 0 2 4 6 8
    printf("Second list:\n");
    for (intList_t::iterator_t i = s2.begin();
                               i!= s2.end();
                               i++
        )
    {
         int aux = static_cast<int>(i); 
         printf("%d ", aux);
    }
    printf("\n");


    // se imprimen los elementos de la lista xx:
    // 1 3 5 7 9 0 2 4 6 8
    printf("Third list:\n");
    for (intList_t::iterator_t i = xx.begin();
                               i!= xx.end();
                               i++
        )
    {
         int aux = static_cast<int>(i); 
         printf("%d ", aux);
    }
    printf("\n");


    printf("Mixing lists...\n");

    // s1 pasa a contener la lista de numeros {impares} + {pares}
    s1= xx;

    // se inserta en s2 la lista xx, ahora s2 tendra {pares} +
    // {impares} + {pares}
    s2<< xx;

    // Se imprimen los elementos de la primera lista s1:
    // deberian imprimirse los numeros {impares} + {pares}
    // 1 3 5 7 9 0 2 4 6 8
    printf("First list:\n");
    while(s1.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        printf("%d ", s1.head());
        // Se elimina el primer elemento de la lista 
        s1.behead();
    }
    // La lista s1 tiene ahora cero elementos
    printf("\n");

    // Se imprimen los elementos de la segunda lista s2:
    // deberian imprimirse los numeros {pares} + {impares} + {pares}
    // 0 2 4 6 8 1 3 5 7 9 0 2 4 6 8
    printf("Second list:\n");
    while(s2.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        printf("%d ", s2.head());
        // Se elimina el primer elemento de la lista 
        s2.behead();
    }
    // La lista s2 tiene ahora cero elementos
    printf("\n");

    // Se imprimen los elementos de la tercera lista xx:
    // deberian imprimirse los numeros {impares} + {pares}
    // 1 3 5 7 9 0 2 4 6 8
    printf("xx list:\n");
    while(xx.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        printf("%d ", xx.head());
        // Se elimina el primer elemento de la lista 
        xx.behead();
    }
    // La lista xx tiene ahora cero elementos
    printf("\n");

    return 0;  // keeps compiler happy
}
