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
// $Id: tst2.cc 20206 2010-04-08 10:55:00Z gabriel $
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


// --------------------------------------------------------------
// cookie_t:
// 
//    Clase que deriva de simpleTask_t y pone un periodo
//    de 1 sec = 1000000 microsec, transcurrido ese tiempo 
//    queremos que se imprima la palabra "cookie!" y vuelve
//    a activarse el periodo. Ademas si se activa una alarma
//    el planificador, cumplido el tiempo de la alarma, llamara
//    al metodo timeOut
//
//    el sched_t avisa a la tarea de que el periodo que
//    ha puesto se ha cumplido llamando a su metodo heartBeat, 
//    que lo tiene redefinido para que haga lo que queramos, en
//    nuestro caso imprime la palabra "cookie!"
// --------------------------------------------------------------

class cookie_t: public simpleTask_t
{
    alarm_t *al;
    public:
        cookie_t(void): simpleTask_t(1000000), al(NULL) {;};

        virtual void heartBeat(void) { printf("cookie! "); fflush(stdout);
        };
	
        virtual void timeOut(const alarm_ref &a) {
            char *bobada[]= {
                "glup!",
                "glob!",
                "argh!",
                "that's all folks",
                NULL
            };
            printf("%s\n", a->args?a->args:"hi!\n");
            al = NULL;
        };
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
    s.insertTask(&t);


    // Activamos una alarma en la tarea t, que se cumplira al
    // cabo de 1.5 sec, y a la que no le pasamos parametros.
    // Tras los 1.5 sec, el planificador llamara al metodo
    // timeOut de la tarea, con los argumentos que se le hayan 
    // pasado, en este caso NULL
    t.set_alarm(1500000, NULL);


    // ponemos en marcha el planificador, si se nos olvida esta
    // instruccion el programa terminara sin mas, porque el planificador
    // no arranca y nunca se atenderan los eventos temporales ni los
    // de entrada/salida
    s.run();

    return 0;
}

