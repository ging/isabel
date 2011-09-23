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
// $Id: tst4Ql.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


// ------------------------------------------------------------------
// PRUEBA DE LISTA DE PUNTEROS INTELIGENTES DE ENTERO : ql_t<int_ref>
//    Los elementos de la lista son punteros inteligentes, por tanto
//    no hay que liberarlos.
// ------------------------------------------------------------------


#include <stdio.h>
#include <icf2/ql.hh>

// Para poder implementar un tipo como un puntero inteligente 
// es necesario realizar una clase cumpla las siguientes condiciones:
//     - deriva de collectible_t
//     - se define una clase amiga: 
//           friend class smartReference_t<mi_tipo_t>
//     - aunque no es obligatorio, por convenio definimos el siguiente
//       tipo con el sufijo _ref:
//           typedef smartReference_t<mi_tipo_t> mi_tipo_ref;
// el puntero inteligente es de tipo mi_tipo_ref y para crearlo es
// necesario utilizar la siguiente construccion:
//     mi_tipo_ref x = new mi_tipo_t;
// es decir se crea un puntero normal y se iguala al puntero inteligente.

class int_t: public collectible_t
{
    private: 
        int _i; 
    public:
        int_t(int i): _i(i) {}
        const int getValue() const { return _i;}

        ~int_t() {
            printf("Destroying int_t(%d)\n", _i);
        }
    
    friend class smartReference_t<int_t>;
};

typedef smartReference_t<int_t> int_ref;

typedef ql_t<int_ref> intRefList_t;

main()
{

    // Se crean 2 listas de punteros inteligentes a enteros:
    //   s1: numeros impares
    //   s2: numeros pares

    intRefList_t s1;
    intRefList_t s2;

    // se insertan los numeros impares en s1,
    // tambien se podrian haber insertado de la siguiente forma:
    //    s1.insert(new int_t(1)); 
    // o tambien:
    //   int_ref i1 = new int_t(1);
    //   s1 << i1;
    // utilizando el metodo insert en vez del el operador <<
    s1 << new int_t(1) << new int_t(3) << new int_t(5) 
       << new int_t(7) << new int_t(9);
    // se insertan los numeros pares en s2
    s2 << new int_t(0) << new int_t(2) << new int_t(4) 
       << new int_t(6) << new int_t(8); 

    // se imprimen los elementos de la lista s1:
    // 1 3 5 7 9
    printf("First list:\n");
    for (intRefList_t::iterator_t i = s1.begin();
                                  i!= s1.end();
                                  i++
        )
    {
         int_ref aux = static_cast<int_ref>(i); 
         printf("%d ", aux->getValue());
    }
    printf("\n");


    // se imprimen los elementos de la lista s2:
    // 0 2 4 6 8
    printf("Second list:\n");
    for (intRefList_t::iterator_t i = s2.begin();
                                  i!= s2.end();
                                  i++
        )
    {
         int_ref aux = static_cast<int_ref>(i); 
         printf("%d ", aux->getValue());
    }
    printf("\n");



    printf("Mixing lists...\n");

    // s1 pasa a contener la lista de numeros {pares}
    // Ojo! la hacer la asignacion, los elementos de la
    // lista s1 dejan de usarse y su cuenta de referencias
    // llega a cero, por tanto se destruyen automaticamente
    // y los elementos de s2 aumentan en 1 su cuenta de referencias
    // esto indica que hay otra entidad que los esta usando
    s1= s2;

    // Se imprimen los elementos de la primera lista s1:
    // deberian imprimirse los numeros {pares}
    // 0 2 4 6 8
    printf("First list:\n");
    while(s1.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        int_ref aux= s1.head();
        printf("%d ", aux->getValue());
        // Se elimina el primer elemento de la lista 
        s1.behead();
        // no hay que eliminar la memoria reservada para
        // almacenar el entero ya que es un puntero inteligente,
        // se liberara de forma automatica cuando nadie la use.
        // En este punto no se libera porque s2 esta usando esos
        // elementos, es decir la cuenta de referencias aun no
        // ha llegado a cero, vale uno.
    }
    // La lista s1 tiene ahora cero elementos
    printf("\n");


    // Se imprimen los elementos de la segunda lista xx:
    // deberian imprimirse los numeros {pares}
    // 0 2 4 6 8

    printf("Second list:\n");
    while(s2.len()) {
        // Se obtiene el primer elemento de la lista y se imprime
        int_ref aux= s2.head();
        printf("%d\n", aux->getValue());
        // Se elimina el primer elemento de la lista 
        s2.behead();
        // no hay que eliminar la memoria reservada para
        // almacenar el entero ya que es un puntero inteligente
        // La memoria de cada elemento se va a eliminar en este
        // punto del bucle porque nadie mas la esta usando.
    }

    // La lista s2 tiene ahora cero elementos
    printf("\n");

    return 0;  // keeps compiler happy
}
