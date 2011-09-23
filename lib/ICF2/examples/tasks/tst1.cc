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
// $Id: tst1.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <stdio.h>

#include <icf2/general.h>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>


// cookie_t:
// 
//    Clase que deriva de simpleTask_t y pone un periodo
//    de 1 sec = 1000000 microsec, transcurrido ese tiempo 
//    queremos que se imprima la palabra "cookie!" y vuelve
//    a activarse el periodo.
//
//    el sched_t avisa a la tarea de que el periodo que
//    ha puesto se ha cumplido llamando a su metodo heartBeat, 
//    que lo tiene redefinido para que haga lo que queramos, en
//    nuestro caso imprime la palabra "cookie!"


class cookie_t: public simpleTask_t
{
    public:
        // inicializamos la tarea con un periodo dado en micorsegundos
        cookie_t(void): simpleTask_t(1000000) {;};

        virtual void heartBeat(void) { printf("cookie! "); fflush(stdout);};
	
};


main()
{
    // generamos el sched_t, planificador para atender eventos 
    // temporales y de entrada salida
    sched_t s(128);

    // generamos la tarea que queremos meter en el planificador
    cookie_t t;

    // metemos la tarea, con el opeador <<, seria equivalente
    // utilizar el metodo insertTask:
    // s.insertTask(&t);
    s<<&t;

    // ponemos en marcha el planificador, si se nos olvida esta
    // instruccion el programa terminara sin mas, porque el planificador
    // no arranca y nunca se atenderan los eventos temporales ni los
    // de entrada/salida
    s.run();


    // a este punto solo vamos a llegar cuando nos quedemos sin tareas
    // en el planificador, lo que va a ocurrir normalmente cuando se
    // termine el programa. Por eso, despues de run() no vamos a poner
    // ninguna instruccion porque solo se va a ejecutar cuando se termine
    // el programa.
    return 0;
}

