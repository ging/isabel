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
###############################################################
#
#   Description:
#
#     ftp daemon
#
################################################################


class DFtpC {
    inherit DaemonC

    # ftp data port.
    private variable dport 51013

    #private variable bwfactor 1
    #private variable bw 

    constructor {timeout no_cc_cb} {
	DaemonC::constructor Ftp $timeout cftp_quit() \
		cftp_nop() 1 $no_cc_cb isabel_cftp
    } {}

    destructor {}

    private method Invoke {}

    # kill the ftp daemon.
    private method CleanUp {}

    private variable pid 0

    #-- BW ---------------

    #public method SetBWFactor {f}
    #public method SetBW {_bw}
}

#------------------------------------------------------------

body DFtpC::constructor {timeout no_cc_cb} {

}

#------------------------------------------------------------

body DFtpC::destructor {} {

    CleanUp
}

#------------------------------------------------------------

body DFtpC::Invoke {} {
    global ISABEL_DIR env

    if {$env(USER) == "ubuntu"} {
	set notifyopt "-notify /dev/null"
    } else {
	set _logfn $env(ISABEL_SESSION_LOGS_DIR)/notify.Ftp
	if { ! [file exists $_logfn]} {
	    exec touch $_logfn
	}
	set notifyopt "-notify $_logfn"
    }

    set cport 51004

    set cmd "isabel_cftp -c $cport -v $dport $notifyopt"

    set pid [eval exec $cmd &]

    catch {exec echo $pid >> $env(ISABEL_TMP_DIR)/Ftp.pid}

    DaemonC::Invoke
}

#------------------------------------------------------------

body DFtpC::CleanUp {} {

    if {$pid != 0} {
	after 100 after idle catch \\\"exec kill    $pid >& /dev/null\\\"
	after 100 after idle catch \\\"exec kill -9 $pid >& /dev/null\\\"
	set pid 0
    }
}

#------------------------------------------------------------

