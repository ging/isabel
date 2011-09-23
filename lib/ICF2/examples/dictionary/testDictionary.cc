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
// $Id: testDictionary.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/dictionary.hh>

// --------------------------------------------------------------------
// x_t:
//
//   Clase x_t sobre la que vamos a poder crear un puntero inteligente
//   porque deriva de collectible_t y tiene definida como clase
//   amiga a smartReference_t<x_t>
//   Los punteros inteligentes de x_t se crean a partir del tipo
//   smartReference_t<x_t> que por convenio definiremos como
//   tipo x_ref.
//   Observar: -  se llama al constructor con una cadena de caracteres
//                que se almacena en __s  y se imprime para saber 
//                el objeto que se esta creando.
//             -  en el destructor se imprime la cadena alamacenada en
//                __s para saber el objeto que se esta destruyendo
//
//   El metodo identify() imprime lo almacenado en la cadena __s, se
//   usa para saber cual es el objeto con el que estamos trabajando
// --------------------------------------------------------------------


class x_t: public virtual collectible_t
{
    private:
        char *__s;

    public:
        x_t(char *s): __s(strdup(s)) { printf("+%s\n", __s); }


    private:
        virtual ~x_t(void) { 
            printf("-%s\n", __s); 
            free(__s);            // se libera la memoria reservada
                                  // strdup reserva con malloc y por
                                  // tanto hay que liberar con free.
                                  // REGLA:
                                  //  RESERVA con malloc se LIBERA con free
                                  //  RESERVA con new    se LIBERA con delete
        }


    public:
        void identify(void) { printf("id=%s\n", __s); }

        friend class smartReference_t<x_t>;
};

// --------------------------------------------------------------------
// x_ref = smartReference_t<x_t>:
//
//    Es un puntero inteligente a x_t.
// --------------------------------------------------------------------

typedef smartReference_t<x_t> x_ref;


// --------------------------------------------------------------------
// xList_t = ql_t<x_ref>:
//
//    Es una lista de punteros inteligentes a x_t.
// --------------------------------------------------------------------
typedef ql_t<x_ref> xList_t;


// --------------------------------------------------------------------
// xList_ref = smartReference_t<xList_t>:
//
//    Se utiliza para crear un puntero inteligente a una lista de 
//    punteros inteligentes a x_t. Es decir, una lista que no hay
//    que destruir, se destruye automaticamente cuando se deje de 
//    usar.
// --------------------------------------------------------------------
typedef smartReference_t<xList_t> xList_ref;


// --------------------------------------------------------------------
// intList_t = ql_t<int>:
//
//    Es una lista de enteros.
// --------------------------------------------------------------------
typedef ql_t<int> intList_t;


// --------------------------------------------------------------------
// intList_t = ql_t<int>:
//
//    Es un puntero inteligente a una lista de enteros.
// --------------------------------------------------------------------
typedef smartReference_t<intList_t> intList_ref;

// --------------------------------------------------------------------
// xDictionary_t = dictionary_t<int, x_ref>:
//
//    Es un diccionario cuyas claves son enteros y el valor asocidado
//    a una clave son punteros inteligentes a x_t.
// --------------------------------------------------------------------
typedef dictionary_t<int, x_ref> xDictionary_t;


// --------------------------------------------------------------------
// xDictionary_ref = smartReference_t<xDictionary_t>:
//
//    Es un puntero inteligente a un diccionario cuyas claves 
//    son enteros y el valor asocidado a una clave son punteros 
//    inteligentes a x_t.
// --------------------------------------------------------------------
typedef smartReference_t<xDictionary_t> xDictionary_ref;


main()
{
    // xd1 = un puntero inteligente a un diccionario
    xDictionary_ref xd1= new xDictionary_t;
    // xd2 = un puntero inteligente a un diccionario
    xDictionary_ref xd2= new xDictionary_t;

    // Inserta en xd1, para la clave "1", el puntero inteligente
    // a x_t inicializado con "obj1:1" (se imprime "+obj1:1" cuando se
    // llama al constructor de x_t) y para la clave "2", el puntero
    // inteligente a x_t inicializado con "obj1:2" (se imprime "+obj1:2"
    // cuando se llama al constructor de x_t)
    xd1->insert(1, new x_t("obj1:1"));
    xd1->insert(2, new x_t("obj1:2"));

    // Inserta en xd2, para la clave "1", el puntero inteligente
    // a x_t inicializado con "obj2:1" (se imprime "+obj2:1" cuando se
    // llama al constructor de x_t) y para la clave "2", el puntero
    // inteligente a x_t inicializado con "obj2:2" (se imprime "+obj2:2"
    // cuando se llama al constructor de x_t)
    xd2->insert(1, new x_t("obj2:1"));
    xd2->insert(2, new x_t("obj2:2"));

    printf("\n");

    // Obtiene un puntero a lista de todas las claves que hay en el 
    // diccionario xd1, es decir obtiene una ql_t<int> * que la almacena
    // en il1 que es un puntero inteligente a una lista de enteros.
    intList_ref il1= xd1->getKeys();

    // Obtiene un puntero a lista de todas las claves que hay en el 
    // diccionario xd2, es decir obtiene una ql_t<int> * que la almacena
    // en il2 que es un puntero inteligente a una lista de enteros.
    intList_ref il2= xd2->getKeys();

    // Recorre la lista con todas las claves del diccionario il1 y
    // busca para cada una de estas claves el valor asociado en el
    // diccionario, e imprime su valor. En el caso de il1, deberia
    // imprimir id=obj1:1 e id=obj1:2
    for(intList_t::iterator_t i= il1->begin(); i!= il1->end(); i++) {
        int aux = static_cast<int>(i);
        xd1->lookUp(aux)->identify();
    }

    // Recorre la lista con todas las claves del diccionario il2 y
    // busca para cada una de estas claves el valor asociado en el
    // diccionario, e imprime su valor. En el caso de il2, deberia
    // imprimir id=obj2:1 e id=obj2:2
    for(intList_t::iterator_t j= il2->begin(); j!= il2->end(); j++)
        xd2->lookUp(j)->identify(); // equivalente, pero forma abreviada

    printf("\n");

    {
        // principio de ambito

        // xd3 = un puntero inteligente a un diccionario
        xDictionary_ref xd3= new xDictionary_t;
        // xd4 = un puntero inteligente a un diccionario
        xDictionary_ref xd4= new xDictionary_t;

        
        // Inserta en xd3, para la clave "1", el puntero inteligente
        // a x_t inicializado con "obj3:1" (se imprime "+obj3:1" cuando se
        // llama al constructor de x_t) y para la clave "2", el puntero
        // inteligente a x_t inicializado con "obj3:2" (se imprime "+obj3:2"
        // cuando se llama al constructor de x_t)
        xd3->insert(1, new x_t("obj3:1"));
        xd3->insert(2, new x_t("obj3:2"));


        // Inserta en xd4, para la clave "1", el puntero inteligente
        // a x_t inicializado con "obj4:1" (se imprime "+obj4:1" cuando se
        // llama al constructor de x_t) y para la clave "2", el puntero
        // inteligente a x_t inicializado con "obj4:2" (se imprime "+obj4:2"
        // cuando se llama al constructor de x_t)
        xd4->insert(1, new x_t("obj4:1"));
        xd4->insert(2, new x_t("obj4:2"));


        // Libera lo que habia en xd1 que como deja de ser referenciado
        // se llama a los destructores de todos los elementos de ese diccio-
        // nario, en particular de destruyen obj1:1 y obj1:2, al llamar
        // a sus destructores se imprmira "-obj1:1" y "-obj1:2"
        // Y se alamacenara en xd1 lo mismo que hay en xd4
        xd1= xd4;

        // fin de ambito

        // las variables automaticas en este ambito desaparecen, es decir,
        // se destruyen xd3 y xd4. Como el contenido de xd3 deja de ser
        // utilizado, se destruye, invocandose a los detructores de obj3:1
        // y obj3:2, es decir, se imprime "-obj3:1" y "-obj3:2" como ahora
        // xd1 que pertenece al ambito mas externo apunta a losmismos datos
        // que xd4,el contenido dexd4 no puede destruirse porque lo esta
        // usando xd1.
    }

    printf("\n");

    // Obtiene un puntero a lista de todas las claves que hay en el 
    // diccionario xd1, es decir obtiene una ql_t<int> * que la almacena
    // en il1 que es un puntero inteligente a una lista de enteros.
    il1= xd1->getKeys();

    // Obtiene un puntero a lista de todas las claves que hay en el 
    // diccionario xd2, es decir obtiene una ql_t<int> * que la almacena
    // en il2 que es un puntero inteligente a una lista de enteros.
    il2= xd2->getKeys();

    // Recorre la lista con todas las claves del diccionario il1 y
    // busca para cada una de estas claves el valor asociado en el
    // diccionario, e imprime su valor. En el caso de il1, deberia
    // imprimir id=obj4:1 e id=obj4:2
    for(intList_t::iterator_t i= il1->begin(); i!= il1->end(); i++)
        xd1->lookUp(i)->identify();


    // Recorre la lista con todas las claves del diccionario il2 y
    // busca para cada una de estas claves el valor asociado en el
    // diccionario, e imprime su valor. En el caso de il2, deberia
    // imprimir id=obj2:1 e id=obj2:2
    for(intList_t::iterator_t j= il2->begin(); j!= il2->end(); j++)
        xd2->lookUp(j)->identify();

    printf("\n");

    // fin del programa 
    // se destruyen todas las variables automaticas que quedan, es
    // decir xd1 y xd2 en orden inverso a como fueron creadas (primero
    // xd2 y despues xd1.
    // Al destruirse xd2 sus elementos dejan de estar referenciados
    // y se llama a sus destructores, de obj2:1 y de obj2:2, por tanto
    // se imprime "-obj2:1" y "-obj2:2".
    // Al destruirse xd1 sus elementos dejan de estar referenciados
    // y se llama a sus destructores, de obj4:1 y de obj4:2, por tanto
    // se imprime "-obj4:1" y "-obj4:2".
}
