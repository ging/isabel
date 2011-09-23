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
// $Id: testSmartReference1.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------
// EJEMPLO DE USO DE LOS PUNTEROS INTELIGENTES
// -----------------------------------------------------------
//
//    En este ejemplo se ve como se crean las referencias
//    (cuando se llama al constructor se imprime +obji) y
//    cuando se destruyen automaticamente sin llamar a
//    "delete" (en el destructor se imprime -obji)
//------------------------------------------------------------

#include <icf2/smartReference.hh>

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
// --------------------------------------------------------------------

class x_t: public virtual collectible_t
{
    private:
    char *__s;

    public:
        x_t(char *s): __s(s) { printf("+%s\n", __s); }

    private:
        virtual ~x_t(void)        { printf("-%s\n", __s); }

        friend class smartReference_t<x_t>;
};

typedef smartReference_t<x_t> x_ref;


main()
{
    // x1 = puntero inteligente de la clase x_t
    // se llama al constructor de x_t y se imprime "+obj1"
    x_ref x1= new x_t("obj1");
    // x2 = puntero inteligente de la clase x_t
    // se llama al constructor de x_t y se imprime "+obj2"
    x_ref x2= new x_t("obj2");

    {
        //nuevo ambito

        // x3 = puntero inteligente de la clase x_t
        // se llama al constructor de x_t y se imprime "+obj3"
        x_ref x3= new x_t("obj3");
        
        {
            //nuevo ambito

            // x1 = nuevo puntero inteligente de la clase x_t
            // x1 deja de apuntar al objeto obj1 y ahora apunta 
            // al objeto obj4. Como no hay nadie mas que estuviera
            // apuntando al obj1, su cuenta de referencias llegara
            // a cero y se borrara el objeto obj1.
            // por tanto se llama al destructor de x_t y se imprime "-obj1"
            // Ademas se crea un nuevo puntero inteligente a la clase x_t
            // y se almacena en x1, como se llama al constructor de x_t
	    // se imprime "+obj4"
            x1= new x_t("obj4");

            // fin de ambito
        }

        // fin de ambito
        // al terminar este ambito desaparece la variable
        // automatica x3 como nadie mas estaba apuntando a 
        // obj3 su cuenta de referencias
        // llegara a cero y se eliminara el objeto.
        // por tanto se llama al destructor de x3 y se imprime "-obj3"
    }

    // fin del programa
    // hay que eliminar todas las variables automaticas del programa
    // en orden inverso a como fueron creadas (primero x2 y despues x1)
    // se eliminaran las referencias x1 y x2 porque se liberan estas
    // variables automaticas, como nadie mas apuntaba a los objetos
    // obj4 y obj2 sus cuentas de referencias llegaran a cero y se
    // destruiran.
    // se llama al destructor de x2 y se imprime "-obj2"
    // y se llama al destructor de x1 y se imprime "-obj4"
}
