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
// $Id: tst3.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <stdio.h>

#include <icf2/general.h>
#include <icf2/ql.hh>
#include <icf2/io.hh>
#include <icf2/sockIO.hh>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>


// ----------------------------------------------------------------
// echoServer_t:
//
//     Tarea que esta esperando paquetes en el socket que se le
//     pasa como parametro en el constructor.
//     Cuando hay actividad en ese socket el planificador llamara
//     al metodo IOReady de la tarea y le pasara como parametro el
//     socket donde se ha producido la actividad.
//     Como es un servidor de echo, leera del socket y lo que lea
//     lo vuelve a mandar como respuesta.
// ----------------------------------------------------------------


class echoServer_t: public simpleTask_t
{
    public:
        echoServer_t(io_ref &io): simpleTask_t(io) { }; 

        virtual void IOReady(io_ref &io) {
            char b[65536];

            // Lee del socket
            int n= io->read(b, sizeof(b));

            if(n> 0)
                // La lectura ha ido bien, lo vuelve a escribir como respuesta
                io->write(b, n);
            else 
                // La lectura no ha ido bien. Saca la tarea del planificador.
                // El metodo get_owner devuelve el planificador donde se
                // ha insertado la tarea. Con el operador menos (-) se extrae
                // del planificador la tarea actual.
                *get_owner()-this;
        };
};


main()
{
    // generamos el sched_t, planificador para atender eventos 
    // temporales y de entrada salida
    sched_t s(128);

    // Para abrir un socket servidor es necesario dar una direccion
    // de origen + puerto donde estara escuchando el servidor.    
    // Generamos una direccion con el nombre de maquina=NULL, es decir
    // para todas las interfaces de red de esa maquina, con el puerto=
    // 14545 y del tipo datagrama.
    inetAddr_t addr1(NULL, "14545", serviceAddr_t::DGRAM);

    // Abrimos un socket de datagramas en esa direccion
    io_ref sock1 = new dgramSocket_t (addr1);

    // generamos un servidor de echo al que se le pasa el socket donde
    // estara escuchando.
    echoServer_t echo1(sock1);

    // lo metemos en el pplanificador
    s<<&echo1;

    // ponemos en marcha el pplanificador
    s.run();

    return 0;
}

