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
// $Id: cond1.cc 21197 2010-09-16 08:53:34Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <unistd.h>

#include <icf2/general.h>
#include <icf2/io.hh>
#include <icf2/sockIO.hh>
#include <icf2/task.hh>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>

#include <icf2/conditionalVar.hh>
#include <icf2/threadedLoop.hh>


// --------------
// CLASE conVar_t
// --------------
// Esta clase hereda 2 metodos, y NO hay que redefinirlos:
//    + void signal(void *)
//        Cambia los datos
//    + void *wait(void):
//        Espera algun cambio en los datos y los devuelve
//
// Hay que redefinirse los siguientos metodos:
//      + virtual void doChange(void *value)=0;
//             Este metodo se invoca al hacer signal(void *) 
//             con sus mismos argumentos y se llama poniendo 
//             previamente un cerrojo de acceso
//             exclusivo que se liberara despues de terminar
//             de ejecutarlo. Por tanto, el metodo doChange
//             define el comprotamiento de "cambiar datos".
//
//      + virtual int  evalCondition(void)=0; 
//             Al hacer un wait() en realidad se pone un cerrojo
//             de acceso exclusivo para evitar que dos threads
//             accedan a los datos a la vez. Y despues se realiza
//             espera activa hasta que los datos cambien. Durante la
//             espera activa se liberan los cerrojos, para permitir
//             a otro thread que los cambie. Una vez se hayan cambiado
//             se ponen de nuevo los cerrojos y se llama al metodo
//             whenTrueLocked().
//             Este metodo define el comportamiento de 
//             "los datos han cambiado?".
//             Si 2 threads realizan wait() sobre la misma 
//             condVar_t a la vez, los dos realizaran la espera
//             activa y tendran acceso a los datos dependiendo de
//             como se haya definifo evalCondition().
//
//      + virtual void *whenTrueLocked(void)=0;
//             Se invoca despues de salir de la espera activa
//             porque los datos han cambiado. Puede retornar un
//             valor, por ejemplo los nuevos datos. Las operaciones
//             que se hagan en este metodo se realizaran bajo la
//             condicion de acceso exclusivo porque aun sigue puesto
//             el cerrojo.       
//
//      + virtual void *whenTrueUnlocked(void *value)=0; 
//             Se invoca despues de terminar whenTrueLocked, con los
//             datos que este devuelve. Las operaciones de este metodo
//             se realizan sin la condicion de aceso exclusivo.
//



class counterCond_t: public condVar_t {
    protected:
        int oldCount;
        int newCount;

    public:
        counterCond_t(void): oldCount(-1), newCount(-1) { }
        virtual ~counterCond_t(void) {}

    protected:
        virtual int  evalCondition(void) {
           return (newCount != oldCount);
        }
        virtual void *whenTrueLocked(void) {
            assert(newCount != oldCount);
            oldCount = newCount;
            return &oldCount;
        }

        virtual void *whenTrueUnlocked(void *value) {
            return value;
        }

        virtual void  doChange(void *value) {
            int *count = static_cast<int *>(value);
            newCount =*count;
        }
};


class countTask_t: public simpleTask_t {
    private:
        condVar_t *cond;
        int period;
        int cont;
    public:
        virtual void __pstInSchedRites(void) { set_period(period); }

        countTask_t(int p, condVar_t *c): cond(c), period(p) { cont =0; }
       
        virtual void heartBeat(void) {
            printf("Sending signal from task with period=%d\n", period);
            cond->signal(&cont);
            cont++;
        }
};


class paint_t: public virtual threadedLoop_t {
    public:
        counterCond_t *counterCond;
        paint_t(counterCond_t *c): counterCond(c) {}

        void wait(void) {
             while (true) {
                 printf("Esperando\n");
                 int *n = static_cast<int *>(counterCond->wait());
                 printf("Data=%d\n", *n); 
                 printf("---------------------------\n");
//                 sleep(10);
             }
        } 

        virtual bool dispatchMsg(tlMsg_t *) { wait(); }

        virtual ~paint_t() { delete counterCond; }

};



main(int argc, char *argv[])
{
    sched_t	s(128);
    
    counterCond_t *cond = new counterCond_t();
    paint_t *paint = new paint_t(cond); 
    paint->tlRun();
    paint->tlPostMsg(new tlMsg_t(30000));

    countTask_t *c1 = new countTask_t(5000000,cond);
    countTask_t *c2 = new countTask_t(1000000,cond);

    s<< c1 << c2;

    s.run();

    return 0;
}

