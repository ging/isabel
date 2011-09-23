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
#ifndef __SCANNER_HH__
#define __SCANNER_HH__


#include <icf/general.h>
#include <icf/task.h>

struct scannerTask_t : public task_t {
    bool scannerOK;

    scannerTask_t( char *);
    virtual ~scannerTask_t();
    virtual void heartBeat(void);
    virtual bool scan( char *);
    virtual bool docsize( int, int);
    virtual bool size( int, int);
    virtual bool setformat( char *);
    virtual bool orientation( bool);
    virtual bool autocrop( bool);
    virtual bool quality( int);
    virtual int checklast( void);
    bool brightness( int _b);
    bool contrast( int _b);
protected:
    bool scannerWaiting;
    int scannerStatus;
    pid_t scanPid;
    char *isaDir;
    void _scan( char *);

    enum scannerFormats_t {
        scanBW,
        scanDITHERED,
        scanGRAY,
        scanCOLOR
    };

    int width, height;
    int xres, yres;
    int format;
    int dpi;
    bool portrait;
    bool crop;
    float s_brightness, s_contrast;
    char *scanargs[10];
    char *tmpStr;
};



#endif
