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
// $Id: tst4.cc 20206 2010-04-08 10:55:00Z gabriel $
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




class echoServer_t: public simpleTask_t
{
    public:
        echoServer_t(io_ref &io): simpleTask_t(io) { }; 

        virtual void IOReady(io_ref &io) {
            char b[65536];
            int n= io->read(b, sizeof(b));
    
            if(n> 0)
                io->write(b, n);
            else 
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
    // 14545 y del tipo stream (TCP).
    inetAddr_t addr2(NULL, "14545", serviceAddr_t::STREAM);

    // Abrimos un socket de stream en esa direccion
    streamSocket_t *sock2 = new streamSocket_t(addr2);

    if (sock2->listen(addr2)) {
        fprintf(stderr, "Error tcp server socket:: System halted!\n");
        abort();
    }


    // generamos un servidor de echo al que se le pasa el socket donde
    // estara escuchando.
    // El template tcpServer_t realiza la copia del proceso servidor
    // cuando un cliente se conecta, de esta forma el servidor no se
    // queda bloqueado atendiendo a un cliente y puede atender 
    // simultaneamente mas conexiones desde otros clientes.
    tcpServer_t<echoServer_t> echo2(*sock2);

    s<<&echo2;

    s.run();

    return 0;
}

