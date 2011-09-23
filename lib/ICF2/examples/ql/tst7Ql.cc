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
// $Id: tst7Ql.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------
// EJEMPLO DE USO DE LAS LISTAS
// -----------------------------------------------------------
//
//------------------------------------------------------------


#include <icf2/ql.hh>
#include <icf2/item.hh>

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
        x_t(char *s): __s(s) { printf("+%s\n", __s); }

    private:
        virtual ~x_t(void)        { printf("-%s\n", __s); }

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


main()
{
    // xl1 y xl2 son 2 punteros inteligentes a listas
    xList_ref xl1= new xList_t;
    xList_ref xl2= new xList_t;

    // inserta en xl1 2 punteros inteligentes a x_t: "obj1:1" y "obj11:2"
    // el operador << actua como metodo de insercion, el uso de este
    // operador es equivalente a utilizar el metodo "insert" de la 
    // siguiente forma:
    //   xl1->insert(new x_t("obj1:1");
    //   xl1->insert(new x_t("obj1:2");
    //
    // como se estan creando 2 objetos de clase x_t se imprimira: 
    // +obj1:1 y +obj1:2
    *xl1<< new x_t("obj1:1")<< new x_t("obj1:2");

    // inserta en xl2 dos punteros inteligentes a x_t: "obj2:1" y "obj12:2"
    // tambien se podia haber escrito usando el metodo "insert" de la 
    // siguiente forma:
    //   xl2->insert(new x_t("obj2:1");
    //   xl2->insert(new x_t("obj2:2");
    //
    // como se estan creando 2 objetos de clase x_t se imprimira: 
    // +obj2:1 y +obj2:2
    *xl2<< new x_t("obj2:1")<< new x_t("obj2:2");

    printf("\n");

    // Recorremos las listas utilizando el iterador definido en las
    // listas, iterator_t e imprimimos el objeto con el que estamos
    // trabajando:
    //     para xl1 deberia imprimirse: id=obj1:1 y id=obj1:2
    //     para xl2 deberia imprimirse: id=obj2:1 y id=obj2:2
    for(xList_t::iterator_t i= xl1->begin(); i!= xl1->end(); i++) {
        // como i es de tipo iterator hay que hacer el casting
        // al tipo real de los objetos de la lista
        x_ref aux= static_cast<x_ref>(i);
        aux->identify();
    }

    for(xList_t::iterator_t j= xl2->begin(); j!= xl2->end(); j++) {
        // equivalente a lo anterior pero de forma abreviada
        static_cast<x_ref>(j)->identify();
    }

    printf("\n");

    {
        // comienzo de ambito

        // xl3 y xl4 son 2 punteros inteligentes a listas
        xList_ref xl3= new xList_t;
        xList_ref xl4= new xList_t;

        // inserta en xl3 2 punteros inteligentes a x_t: "obj3:1" y "obj13:2"
        // el operador << actua como metodo de insercion, el uso de este
        // operador es equivalente a utilizar el metodo "insert" de la 
        // siguiente forma:
        //   xl3->insert(new x_t("obj3:1");
        //   xl3->insert(new x_t("obj3:2");
        //
        // como se estan creando 2 objetos de clase x_t se imprimira: 
        // +obj3:1 y +obj3:2
        
        *xl3<< new x_t("obj3:1")<< new x_t("obj3:2");

        // inserta en xl4 2 punteros inteligentes a x_t: "obj4:1" y "obj14:2"
        // el operador << actua como metodo de insercion, el uso de este
        // operador es equivalente a utilizar el metodo "insert" de la 
        // siguiente forma:
        //   xl4->insert(new x_t("obj4:1");
        //   xl4->insert(new x_t("obj4:2");
        //
        // como se estan creando 2 objetos de clase x_t se imprimira: 
        // +obj4:1 y +obj4:2
        
        *xl4<< new x_t("obj4:1")<< new x_t("obj4:2");


        // se libera lo que hubiera almacenado en xl1 porque ahora
        // va a apuntar a xl4. Por tanto, desparece la lista con los
        // elementos obj1:1 y obj1:2 y como nadie mas los estaba usando
        // (es decir no estaban referenciados por nadie mas) se llama
        // a sus destructores y se imprimira:
        // -obj1:1 y -obj1:2
        xl1= xl4;

        // fin de ambito

        // desaparecen las variables automaticas de este ambito, es
        // decir las listas xl3 y xl4, como nadie mas estaba usando
        // xl3, esta desparece llamandose a los destructores de sus
        // elementos, por tanto se imprime:
        // -obj3:1 y -obj3:2
        // sin embargo la lista xl4 no desaparece porque xl1 la esta
        // usando y esta variable, xl1, pertenece a un ambito mas
        // externo. Por tanto los objetos obj4:1 y obj4:2 no 
        // desaparecen
    }

    printf("\n");

    // Recorremos las listas utilizando el iterador definido en las
    // listas, iterator_t e imprimimos el objeto con el que estamos
    // trabajando:
    //     para xl1 deberia imprimirse: id=obj4:1 y id=obj4:2
    //     para xl2 deberia imprimirse: id=obj2:1 y id=obj2:2
 
    for(xList_t::iterator_t i= xl1->begin(); i!= xl1->end(); i++)
        static_cast<x_ref>(i)->identify();
    for(xList_t::iterator_t j= xl2->begin(); j!= xl2->end(); j++)
        static_cast<x_ref>(j)->identify();

    printf("\n");

    // fin del programa, desparecen todas las variables automaticas,
    // es decir xl1 y xl2 en el orden inverso a como fueron creadas
    // (primero xl2 y despues xl1)
    // se llama a los destructores de los elementos de la listas xl2:
    //     se imprime: -obj2:1 -obj2:2
    // se llama a los destructores de los elementos de la listas xl1:
    //     se imprime: -obj4:1 -obj4:2
    
}
