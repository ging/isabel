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
// $Id: shmSrcStub.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <Isabel_SDK/dlm.hh>
#include <Isabel_SDK/systemRegistry.hh>


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "shmSrcApp.hh"

#define PRG_NAME   "shmSrc"
#define PRG_DESC   "V4l to Shared Memory support for ISABEL"

static int moduleInitFunction(int, char **);
static int moduleShutdownFunction(void);

DLM_CONTROL_STRUCT
(
    1, 0,
    PRG_NAME,
    PRG_DESC,
    "(c) Copyright 2006-2007 Agora Systems S.A.",
    moduleInitFunction,
    moduleShutdownFunction
);



shmSrcApp_t *theApp;



//
// descriptor class
//
class shmSrcProgramDescriptor_t: public programDescriptor_t
{
private:
public:
   virtual const char *getName       (void) const { return PRG_NAME; }
   virtual const char *getDescription(void) const { return PRG_DESC; }

   friend class smartReference_t<programDescriptor_t>;
};




//
// program class
//
class shmSrcProgram_t: public abstractProgram_t
{
    virtual void main(int argc, char **argv) {

        shmSrcApp_t *theApp= ::theApp= new shmSrcApp_t(argc, argv);

        theApp->run();
    }
};

class shmSrcProgramFactory_t: public programFactory_t
{
public:
    virtual program_ref createProgram(void) {
        return new shmSrcProgram_t;
    }
};


//
// housekeeping functions
//
static int
moduleInitFunction(int argc, char **argv)
{
    NOTIFY("%s: starting up\n", DLM_NAME);

    registerProgramFactory(
        new shmSrcProgramDescriptor_t,
        new shmSrcProgramFactory_t
    );

    return 0;
}

static int
moduleShutdownFunction(void)
{
    NOTIFY("%s: shutting down\n", DLM_NAME);

    return 0;
}

