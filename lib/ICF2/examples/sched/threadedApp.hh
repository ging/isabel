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
//
// $Id: threadedApp.hh 20206 2010-04-08 10:55:00Z gabriel $
//
// 2scheds.cc and threadedApp.hh are an example of having two schedulers
// running at a time
//

#include <string.h>

#include <icf2/icfMessages.hh>
#include <icf2/task.hh>
#include <icf2/stdTask.hh>

enum threadedAppMsg_e {
    INSERT_TASK_MSG=0x9300
};

class insertTaskMsg_t: public tlMsg_t
{
public:
    task_ref __t;

    insertTaskMsg_t(const task_ref t)
    :tlMsg_t(INSERT_TASK_MSG), __t(t)
    {}
};

class threadedApp_t: public virtual threadedLoop_t
{
private:

    class mySched_t: public sched_t
    {
    public:
       mySched_t(int numTask=128):sched_t(numTask) {}

	void run(void) { sched_t::run(); }
    };

    class myThreadedApp_t: public virtual threadedLoop_t
    {
    private:
        application_t *__theApp;

        enum someMsg_e {
            START_MSG=0x9500
        };

        class startMsg_t: public virtual tlMsg_t
        {
        public:
            startMsg_t(void):tlMsg_t(START_MSG) {}
        };

            class fakeCookie_t: public simpleTask_t
            {
            public:
                fakeCookie_t(void):simpleTask_t(1000000){ };

                virtual void heartBeat(void) { }

                virtual char *className(void) const { return "fakeCookie_t"; }
            };

    public:

        myThreadedApp_t(void) {
            __theApp= new application_t;

	    // Si no inserto una tarea, no funciona...
	    // pero no se porque :-(
	    (*__theApp) << new fakeCookie_t;
        }

        virtual void tlRun(void) {
            tlPostMsg(new startMsg_t());
	    threadedLoop_t::tlRun();
        }

	virtual void insertTask(const task_ref &task) {
	    (*__theApp) << task;
	}

    protected:

        virtual bool dispatchMsg(tlMsg_t *msg) {
            switch (msg->msgKind) {
	    case START_MSG:
	        printf("myThreadedApp_t START_MSG\n");
	        __theApp->run();
	        return true;
	    default:
	        return threadedLoop_t::dispatchMsg(msg);
	    }
        }

        const char *className(void) const { return "myThreadedApp_t"; }
    };

    myThreadedApp_t *__theApp;

public:

    threadedApp_t(void) {
        __theApp= new myThreadedApp_t;
    }

    virtual void tlRun(void) {
        __theApp->tlRun();
	threadedLoop_t::tlRun();
    }

    virtual bool dispatchMsg(tlMsg_t *msg) {
        switch(msg->msgKind) {
	case INSERT_TASK_MSG:
	    {
	        insertTaskMsg_t *m= static_cast<insertTaskMsg_t*>(msg);

		__theApp->insertTask(m->__t);
		return true;
	    }
	default:
	    return threadedLoop_t::dispatchMsg(msg);
	}
    }

    const char *className(void) const { return "threadedApp_t"; }
};

