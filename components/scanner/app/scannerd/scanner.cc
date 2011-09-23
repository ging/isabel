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

#include <icf/general.h>
#include <icf/task.h>
#include <icf/strOps.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "scanner.hh"
#include "scannerApp.hh"


scannerTask_t::scannerTask_t( char *_isaDir) : task_t( 0)
{
    scannerOK = true;
    scannerStatus = 0;
    scannerWaiting = false;
    isaDir = strdup( _isaDir);

    /* try to open the device */
#ifdef Solaris
    int fd = open( "/dev/isasg", O_RDWR);
    if( fd == -1)
        scannerOK = false;
    close( fd);
#if 0
    if( scannerOK) {
        chmod( "/dev/isasg", 0666);
    }
#endif
#endif

    dpi = 100; // screen horizontal approx 91 and vertical approx 95
    xres = 1152;
    yres = 900;
    width = 210; // mm
    height = 297; // mm
    format = scanBW;
    portrait = true;
    crop = false;
    s_brightness = 0.0;
    s_contrast = -0.0;
    tmpStr = (char *)malloc(1024);
}; 

scannerTask_t::~scannerTask_t()
{
    if( scannerOK) {
        chmod( "/dev/isasg", 0600);
    }
    free( isaDir);
    free(tmpStr);
}; 

void
scannerTask_t::heartBeat( void)
{
    // if this is called we are then waiting for a process to exit
    // when we get the process status we reset period
    if( waitpid( scanPid, &scannerStatus, WNOHANG) ) {
        debugMsg( dbg_App_Normal, "scannertask_t::heartBeat","child died");
        set_period( 0);
        // this will be zero if no error happened
        scannerStatus = WIFEXITED( scanPid);
        if( scannerStatus)
            scannerStatus = -1;
        scannerWaiting = false;
    }
}; 

bool
scannerTask_t::docsize( int _w, int _h)
{
    width = _w;
    height = _h;
    return false;
}

bool
scannerTask_t::size( int _w, int _h)
{
    xres= _w;
    yres = _h;
    return false;
}

bool
scannerTask_t::contrast( int _contrast)
{
    _contrast -= 50;
    _contrast *= 2;
    s_contrast = ((float) _contrast)/100.0;
    if( s_contrast < -1.00000 ) {
      s_contrast = -1.0;
    }
    if( s_contrast > 1.00000 ) {
      s_contrast = 1.0;
    }
    return false;
}

bool
scannerTask_t::brightness( int _b)
{
    _b-= 50;
    _b*= 2;
    s_brightness = ((float) _b)/100.0;
    if( s_brightness < -1.00000 ) {
      s_brightness = -1.0;
    }
    if( s_brightness > 1.00000 ) {
      s_brightness = 1.0;
    }
    return false;
}

bool
scannerTask_t::orientation( bool _orient)
{
    portrait = _orient;
    return false;
}

bool
scannerTask_t::autocrop( bool _crop)
{
#if 0
    crop= _crop;
#else
    crop= false;
#endif
    return false;
}

bool
scannerTask_t::quality( int _q)
{
    dpi = _q;
    return false;
}

bool
scannerTask_t::setformat( char *_format)
{
    // printf("_format es %s\n", _format);
    if( !strcasecmp( _format, "bw") ) {
        // printf("setting bw\n");
        format = scanBW;
    } else if( !strcasecmp( _format, "dithered") ) {
        // printf("setting dithered\n");
        format = scanDITHERED;
    } else if( !strcasecmp( _format, "gray") ) {
        // printf("setting gray\n");
        format = scanGRAY;
    } else if( !strcasecmp( _format, "color") ) {
        // printf("setting color\n");
        format = scanCOLOR;
    } else {
        // printf("setting undefined\n");
        return true;
    }
    // printf("returning false\n");
    return false;
}

int
scannerTask_t::checklast( void)
{
    if( scannerWaiting)
        return 1;
    if( scannerStatus)
        return -1;
    return 0;
}

bool
scannerTask_t::scan( char *fName)
{
    // Execute a pipe of programs and set_period 
    debugMsg( dbg_App_Normal, "scannertask_t::scan", "entering");
    switch( (scanPid = fork())) {
        case 0:
            debugMsg( dbg_App_Normal, "scannertask_t::scan", "child");
            // child
            // seteuid( 0);
            _scan( fName);
            // we should never get here
            exit( -1);
            break;
        case -1:
            scannerStatus = -1;
            return 1;
            break;
        default:
            // father
            // check every 100 milliseconds
            set_period( 100);
            scannerWaiting = true;
            scannerStatus = 0;
            break;
    }
    return 0;
};


void
scannerTask_t::_scan( char *fName)
{
    int i = 0;
    char tmpbuf[64];
    // fName is the end of the pipe

    debugMsg( dbg_App_Normal, "scannertask_t::_scan", "starting");
    scanargs[i++] = "-(sh)";
#ifdef Linux
    scanargs[i++] = "-i";
#endif
    scanargs[i++] = "-c";
    scanargs[i++] = tmpStr;
    scanargs[i++] = NULL;
    // hpscan
    strcpy( tmpStr, "");
    strcat( tmpStr, isaDir);
    strcat( tmpStr,"/isabel_hpscan");
    // args to hpscan
    strcat( tmpStr, " -quiet");
    sprintf( tmpbuf, " -dpi %d", dpi);
    strcat( tmpStr, tmpbuf);
    if( portrait ) {
      sprintf( tmpbuf, " -xres %d", xres);
      strcat( tmpStr, tmpbuf);
      sprintf( tmpbuf, " -yres %d", yres);
      strcat( tmpStr, tmpbuf);
    } else {
      sprintf( tmpbuf, " -xres %d", yres);
      strcat( tmpStr, tmpbuf);
      sprintf( tmpbuf, " -yres %d", xres);
      strcat( tmpStr, tmpbuf);
    }
    strcat( tmpStr, " -mm");
    sprintf( tmpbuf, " -width %d", width);
    strcat( tmpStr, tmpbuf);
    sprintf( tmpbuf, " -height %d", height);
    strcat( tmpStr, tmpbuf);
#if 1
    sprintf( tmpbuf, " -bright %f", s_brightness);
    strcat( tmpStr, tmpbuf);
    sprintf( tmpbuf, " -cont %f", s_contrast);
    strcat( tmpStr, tmpbuf);
#endif

    strcat( tmpStr, " -format");
    switch( format) {
        case scanBW:
            strcat( tmpStr, " thresholded");
            break;
        case scanDITHERED:
            strcat( tmpStr, " dithered");
            break;
        case scanGRAY:
            strcat( tmpStr, " grayscale");
            break;
        case scanCOLOR:
            strcat( tmpStr, " color");
            break;
    }

    if( crop ) {
        strcat( tmpStr, " |");
        strcat( tmpStr, isaDir);
        strcat( tmpStr, "/isabel_pnmcrop");
    }
    if( !portrait ) {
        strcat( tmpStr, " |");
        strcat( tmpStr, isaDir);
        // strcat( tmpStr, "/isabel_pnmrotate -noantialias -90");
	if( format == scanBW)
          strcat( tmpStr, "/isabel_bwrotate -right");
	else
          strcat( tmpStr, "/isabel_pnmrotate -90");
    }

#if 1
    if( (format == scanBW) || (format == scanDITHERED) ) {
      strcat( tmpStr, " |");
      strcat( tmpStr, isaDir);
      strcat( tmpStr, "/isabel_fastpbm");
    }
#endif

    if( format == scanCOLOR ) {
        // we need to quantize colors to 150 for example
        strcat( tmpStr, " |");
        strcat( tmpStr, isaDir);
#if 1
        strcat( tmpStr, "/isabel_ppmquant 150");
#endif
    }
    if( format != scanBW ) {
      strcat( tmpStr, " |");
      strcat( tmpStr, isaDir);
      strcat( tmpStr, "/isabel_ppmtogif");
    }

    strcat( tmpStr, " 2>/dev/null");
    strcat( tmpStr, " >");
    strcat( tmpStr, fName);

#if 0
    for( int j = 0; j < 4 ; j++) {
        // debugMsg( dbg_App_Normal, "scannertask_t::_scan",
        printf(
            "scanarg[%d] %s", j, scanargs[j]);
    }
    printf( "hola %s\n", tmpStr);
#endif
#if 1
    execv( "/bin/sh", scanargs);
#endif
    exit( -1);
}
