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
// $Id: stdTask.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__stdtask_hh__
#define __icf2__stdtask_hh__

#include <string.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/ql.hh>
#include <icf2/sockIO.hh>
#include <icf2/task.hh>
#include <icf2/sched.hh>
#include <icf2/fileIO.hh>
#include <icf2/strOps.hh>
#include <icf2/parseCmdLine.hh>

extern unsigned tcpServerCount;

template <class T> class _Dll_ tcpServer_t: public simpleTask_t
{
public:
    tcpServer_t(socketIO_t &io):
    simpleTask_t(static_cast<io_ref>(&io))
    {
    }

    virtual ~tcpServer_t(void) {
    }

    virtual void IOReady(io_ref &io) {
        io_t           *ioxx= static_cast<io_t *>(io);
        streamSocket_t *iioo= static_cast<streamSocket_t *>(ioxx);

        io_ref          iioo2(iioo->accept());

        task_ref newTask= new T(iioo2);

        if(get_owner())
            *get_owner()<<newTask;
        tcpServerCount++;
    }

    virtual const char *className(void) const { return "<tcpServer_t>"; }
};

template <class T> class _Dll_ enhTcpServer_t: public tcpServer_t<T>
{
public:

    void *initData;

    enhTcpServer_t(socketIO_t &io, void *_initData= NULL): tcpServer_t<T>(io) {
        initData=_initData;
    }

    virtual ~enhTcpServer_t(void) {
        ;
    }


    virtual void IOReady(io_ref &io) {
        io_t            *ioxx(static_cast<io_t*>(io));
        streamSocket_t  *iioo(static_cast<streamSocket_t*>(ioxx));
        io_ref           iioo2(iioo->accept());

        task_ref newTask;

        if(initData)
            newTask= new T(iioo2, initData);
        else
            newTask= new T(iioo2);

        if(this->get_owner())
            *this->get_owner()<<newTask;
        tcpServerCount++;
    }

    virtual const char *className(void) const { return "<enhTcpServer_t>"; }
};




//
// interface -- esta un poco raro, pero es la unica manera que he
//              encontrado para que el gcc se digne en compilarlo
//
template<class T> class _Dll_ binding_t
{
public:
    const char *name;
    char const *(T::*function)(int, char **);
};



#define INTER_VRFY_ARGC(b,x)                                  \
    if(argc!= (x)) {                                          \
        sprintf(b, "ERROR= incorrect parameters\n");          \
        return b;                                             \
    }

#define INTER_VRFY_ARGC_range(b,x,y)                          \
    if((argc< (x)) || (argc> (y))) {                          \
        sprintf(b, "ERROR= incorrect parameters\n");          \
        return b;                                             \
    }

#if 0
template
    <class T, binding_t<T> *_methods>
    class __interface_t: public virtual item_t, public T
{
protected:
    binding_t<T> *methods;
    int n_methods;

    char buffer[1024];    // usado por ioready
    char *ptr;            // usado por ioready
    int  remains;         // usado por ioready

public:
//    interface_t(io_t &i): simpleTask_t(i) {
//        n_methods= 0;
//        methods= _methods;
//        while(methods[n_methods].name)
//            n_methods++;
//
//        memset(buffer, 0, sizeof(buffer));
//        ptr= buffer;
//        remains= 0;
//    }
    __interface_t(void) {
        n_methods= 0;
        methods= _methods;
        while(methods[n_methods].name)
            n_methods++;

        memset(buffer, 0, sizeof(buffer));
        ptr= buffer;
        remains= 0;
    }
    virtual ~__interface_t(void) {
    }


private:
    char retVal[512];
public:
    char * dispatch(int argc, char **argv) {
//        static  char    retVal[512];
        char    *res= NULL;
        int i;


        for(i= 0; i< n_methods; i++) {
            if(strcmp(methods[i].name, argv[0])== 0) {
//                res= (methods[i].function)(argc, argv);
                res= (this->*methods[i].function)(argc, argv);
                if(!res)
                    res="";

                return res;
            }
        }

        if(!res) {
            sprintf(retVal, "ERROR= Unknown directive or Syntax error\n");
        }

        return retVal;
    }
};
#else
template
    <class T, class M, M *_methods>
    class _Dll_ __interface_t: public T
{
protected:
    io_ref __myFileIO;
    M *methods;
    int n_methods;

    char buffer[1024];    // usado por ioready
    char *ptr;            // usado por ioready
    int  remains;         // usado por ioready
    int  readedBytes;

public:
//    interface_t(io_t &i): simpleTask_t(i) {
//        n_methods= 0;
//        methods= _methods;
//        while(methods[n_methods].name)
//            n_methods++;
//
//        memset(buffer, 0, sizeof(buffer));
//        ptr= buffer;
//        remains= 0;
//    }
    __interface_t(void) {
        n_methods= 0;
        methods= _methods;
        while(methods[n_methods].name)
            n_methods++;

        memset(buffer, 0, sizeof(buffer));
        ptr= buffer;
        remains= 0;
    }
    virtual ~__interface_t(void) {
    }

private:
    char retVal[512];
public:
    char const * dispatch(int argc, char **argv) {
//        static  char    retVal[512];
        char const *res= NULL;
        int i;

        for(i= 0; i< n_methods; i++) {
            if(strcmp(methods[i].name, argv[0])== 0) {
//                res= (methods[i].function)(argc, argv);
                res= (this->*methods[i].function)(argc, argv);
                if(!res)
                    res="";

                return res;
            }
        }

        if(!res) {
            sprintf(retVal, "ERROR= Unknown directive or Syntax error\n");
        }

        return retVal;
    }

    virtual const char *className(void) const { return "__interface_t"; }
};
#endif

template
<class T, ::binding_t<T> *_methods>
    class _Dll_ interface_t:
#ifdef __THREADED_ICF
        public threadedTask_t,
#else
        public simpleTask_t,
#endif
        public __interface_t<T, ::binding_t<T>, _methods>
{
public:
    interface_t(const io_ref &i):
#ifdef __THREADED_ICF
    threadedTask_t(i)
#else
    simpleTask_t(i)
#endif
    {
        this->__myFileIO=NULL;
    }

    interface_t(char *fname)  {
        this->__myFileIO = new fileIO_t(fname, fileIO_t::READ_ONLY);
        if (this->__myFileIO->sysHandle() < 0) {
            NOTIFY("interface_t:: problems when opening file=[%s]\n", fname);
            this->__myFileIO=NULL;
            return;
        }
    }

    virtual ~interface_t(void) {
        tcpServerCount--;
        if (tcpServerCount==0) {
            //if it is the last control socket
            NOTIFY("quitting due to the lack of control sockets\n");
            exit(0);
        }
    }

    virtual bool parseFile() {
        if (!this->__myFileIO.isValid()) {
            NOTIFY("interface_t::parseFile: the config file is not valid\n");
            return false;
        }

        do {
            IOReady(this->__myFileIO);
        } while (this->readedBytes!=0);

        return true;
    }

    virtual void IOReady(io_ref &io) {
        int         n= io->read(this->buffer + this->remains, sizeof(this->buffer) - this->remains);
        char        **argv;
        int         argc;
        char const *s;

        this->readedBytes=n;
#if defined(WIN32) || defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        if(n<=0)
        {
            if(get_owner())
                get_owner()->removeTask(static_cast<task_ref>(this));
//              bailOut();
            return;
        }
#else
#error "Please, define behaviour when read returns <=0"
#endif

        n+= this->remains;
        this->ptr= this->buffer;
        while(n) {
            char *aux= strchr(this->ptr, '\n');

            if(aux && (n >= aux-this->ptr)) // podria encontrar fuera de la cadena
                *aux= 0;
            else {
                memcpy(this->buffer, this->ptr, n);
                this->ptr= this->buffer+n;

                memset(this->ptr, 0, sizeof(this->buffer) - n);
                this->remains= n;

                return;
            }

            if((this->ptr[0]== '\000') || (this->ptr[0]== '#')) {
                n-= (aux+1-this->ptr);
                this->ptr= aux+1;
                continue;
            }

            argv= buildFormattedArgv(this->ptr, argc);
            if(argc< 0) {
                char auxRetVal[512];

                sprintf(auxRetVal, "ERROR= Syntax error\n");
                io->write(auxRetVal, strlen(auxRetVal));

                return;
            }

#if 0
            NOTIFY("----------------------------\n");
            NOTIFY("REQUEST->\n");
            for(int i= 0; i< argc; i++)
                NOTIFY("\targv[%d]=%s\n",i,argv[i]);
#endif

            s= this->dispatch(argc, argv);
            if(s== (char const *) -1)
                return;

            if(s)
            {
                io->write(s, strlen(s));
            }

#if 0
            NOTIFY("REPLY-> \t%s\n",s);
            NOTIFY("----------------------------\n");
#endif

            freeArgv(argc, argv);

            n-= (aux+1-this->ptr);
            this->ptr= aux+1;
        }
        memset(this->buffer, 0, sizeof(this->buffer));
        this->ptr= this->buffer;
        this->remains= 0;
    }

    virtual const char *className(void) const { return "interface_t"; }
};

//
//
// Esto deberia estar en un modulo aparte
//
//
class confFile_t: public fileIO_t
{
public:
    confFile_t(const char *f): fileIO_t(f, READ_ONLY) { ; }

    virtual int read(void *b, int n) {
        int retVal= io_t::read(b, n);

        ::write(2, b, retVal);

        return retVal;
    }

    virtual int write(const void *b, int n) {

        return ::write(2, b, n);
    }
};

class shellIO_t: public io_t
{
public:
    int out;

    shellIO_t(int input, int output): io_t(input), out(output) { ; }

    virtual int read(void *b, int n) {
        return io_t::read(b, n);
    }

    virtual int write(const void *b, int n) {
        return ::write(out, b, n);
    }
};

//
//
// Esto deberia estar en un modulo aparte
//
//
class _Dll_ application_t: public sched_t
{
private:
    virtual void __appInit(void);


public:
    application_t(int nTasks= 128);
    application_t(int &argc, argv_t &argv);
    application_t(int &argc, argv_t &argv,int nTasks);

    virtual appParamList_t *getOpt(optionDefList_t &op, int &argc, argv_t &argv);
    virtual const char *className(void) const { return "application_t"; }
};

#endif

