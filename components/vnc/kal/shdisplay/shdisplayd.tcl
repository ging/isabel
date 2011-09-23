########################################################################
#
#  ISABEL: A group collaboration tool for the Internet
#  Copyright (C) 2009 Agora System S.A.
#  
#  This file is part of Isabel.
#  
#  Isabel is free software: you can redistribute it and/or modify
#  it under the terms of the Affero GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  Isabel is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  Affero GNU General Public License for more details.
# 
#  You should have received a copy of the Affero GNU General Public License
#  along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
#
########################################################################

#########################################################################
#
# shdisplay.tcl
#
# isabel_shdisplay daemon
#
#########################################################################


class DShDispC {
    inherit DaemonC

    #------------------
    # PUBLIC INTERFACE
    #------------------

    constructor {timeout no_cc_cb} {
	DaemonC::constructor ShDisplay $timeout shdis_quit() \
		shdis_nop() 1 $no_cc_cb isabel_shdisp
    } {}

    # Bind the channel $channel to the local irouter.
    public method Bind {channel}

    #-------------------
    # PRIVATE INTERFACE
    #-------------------

    # rtp data port.
    private variable rtpport 51005

    # rtcp data port.
    private variable rtcpport 51007

    # irouter port
    private variable iport 51006

    # display
    private variable display

    destructor {}

    private method Invoke {}

    # kill the isabel_shdisplay daemons.
    private method CleanUp {}

    private variable pid 0
}

#------------------------------------------------------------

body DShDispC::constructor {timeout no_cc_cb} {

    set display [session Environ cget -scenary_display]
}

#------------------------------------------------------------

body DShDispC::destructor {} {

     CleanUp
}

#------------------------------------------------------------

body DShDispC::Invoke {} {
    global ISABEL_DIR env
    
    if {$env(USER) == "ubuntu"} {
	set notifyopt "-notify /dev/null"
    } else {
	set _logfn $env(ISABEL_SESSION_LOGS_DIR)/notify.ShDisplay
	if { ! [file exists $_logfn]} {
	    exec touch $_logfn
	}
	set notifyopt "-notify $_logfn"
    }

    set cport 51002
    
    set cmd "isabel_video $notifyopt \
             -cport $cport \
             -display $display \
             -noV4L \
             -noWin \
             -shmSrc \
             -rtpPort $rtpport -rtcpPort $rtcpport "

    set pid [eval exec $cmd &]

    WriteTraceMsg "DShDispC::Invoke: \[$pid\] $cmd" ShDisplay
    catch {exec echo $pid >> $env(ISABEL_TMP_DIR)/ShDisplay.pid}

    DaemonC::Invoke
}

#------------------------------------------------------------

body DShDispC::CleanUp {} {

    if {$pid != 0} {
	after 100 after idle catch \\\"exec kill    $pid >& /dev/null\\\"
	after 100 after idle catch \\\"exec kill -9 $pid >& /dev/null\\\"
    }
}

#------------------------------------------------------------

body DShDispC::Bind {channel} {
    global env

    set host 127.0.0.1

    RDO shdis_bind($channel,$host,$iport)
}

#------------------------------------------------------------


