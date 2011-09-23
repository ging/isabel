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

#include <string.h>
#include <stdlib.h>
#include <icf2/parseCmdLine.hh>


int
main(int argc, char *argv[])
{
    // enumerado para definir las opciones
    enum myOptions { optPort, optHost, optDummy};

    // Lista que contiene la cadena de caracteres asociada
    // a una opcion y su valor enumerado 
    optionDefList_t opt;

    char *port =NULL;
    char *host =NULL;

    // Relleno la lista de opciones, asocio cadena
    // de caracteres con valor enumerado. Si la cadena
    // de caracteres comienza por @ la opcion lleva
    // un valor asociado, ej: este programa se puede
    // invocar con:
    //  ./main -port <valorPuerto> -host <valorHost> -dummy
    // la opcion -dummy no lleva valor asociado, las 
    // opciones host y port si. Da igual el orden 
    // en el que se presenten las opciones. 
    opt
        << new optionDef_t("@port", optPort)
        << new optionDef_t("@host", optHost)
        << new optionDef_t("dummy", optDummy);
	
    // analizo las opciones que se le han pasado
    // por linea de comandos y obtengo una lista de 
    // parametros, cada elemento tiene los siguientes
    // campos:
    //    parId= enumerado que identifica la opcion
    //    parValue = valor asociado a dicha opcion
    appParamList_t *parList= getOpt(opt, argc, argv);

    // recorro la lista de parametros, mientras
    // haya elementos.
    for( ; parList->len(); parList->behead()) {
        // para el primer elemento de la lista,
        // compruebo que el enumerado es uno de los
        // que hemos definido y sacamos su valor.
        switch(parList->head()->parId) {
        case optPort: 
	        port= strdup(parList->head()->parValue);
	        break;

        case optHost:
	        host= strdup(parList->head()->parValue);
	        break;
 
        case optDummy:
	        printf("Dummy parameter!\n");
	        break;
        default:
	        fprintf(stderr, "unknown parameter\n");
                break;
        }
    }

    // Ya no se han pasado mas parametros por linea de comandos
	
    printf("host=[%s] port=[%s]\n", host, port);

    if (host)
        free(host);
    if (port)
        free(port);

    return 0;
}
