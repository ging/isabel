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
#     audio daemon
#
################################################################


class DAudioC {
    inherit DaemonC

    # My local address (host). It is used in the data socket to make the bind.
    private variable myHost 127.0.0.1

    # The port where this audio daemon will receive rtp data packets.
    private variable adport 51001

    # The host and the port where this audio daemon will send rtp data packets.
    private variable bindPort 51002
    private variable bindHost 127.0.0.1

    constructor {timeout no_cc_cb} {
        DaemonC::constructor Audio $timeout audio_quit() \
                audio_nop() 1 $no_cc_cb isabel_audio
    } {}

    destructor {}

    public method Bind {}

    private method Invoke {}

    # kill the audio daemons.
    private method CleanUp {}

    private variable pid 0
}

#------------------------------------------------------------

body DAudioC::constructor {timeout no_cc_cb} {
    global env

    set _using_mcu 0

    if {[info exists env(ISABEL_STREAMING_ENABLED)] && $env(ISABEL_STREAMING_ENABLED) == "1"} {
	set _using_mcu 1
    }

    if {[info exists env(ISABEL_IOS_GATEWAY_ENABLED)] && $env(ISABEL_IOS_GATEWAY_ENABLED) == "1"} {
	set _using_mcu 1
    }

    if {[info exists env(ISABEL_LSE_SERVER_HOST)] && $env(ISABEL_LSE_SERVER_HOST) != ""} {
	set _using_mcu 1
    }

    if {$_using_mcu} {
	set adport 51025
	set bindPort 51026
	set myHost 127.0.0.2
    }
}

#------------------------------------------------------------

body DAudioC::destructor {} {

    CleanUp
}

#------------------------------------------------------------

body DAudioC::Bind {} {
    
    RDO audio_bind($bindHost,$bindPort)
}

#------------------------------------------------------------

body DAudioC::Invoke {} {
    global ISABEL_DIR env

    if {$env(USER) == "ubuntu"} {
	set notifyopt "-notify /dev/null"
    } else {
	set _logfn $env(ISABEL_SESSION_LOGS_DIR)/notify.Audio
	if { ! [file exists $_logfn]} {
	    exec touch $_logfn
	}
	set notifyopt "-notify $_logfn"
    }

    # Control port
    set cport 51007

    # Port where irouter wait for the vumeter flow sent by the audio daemon.
    set vdport 51022

    # Port where vumeter wait for local vumeter flow sent by the audio daemon.
    set gdport 51021

    set cmd "isabel_audio -cport $cport -myHost $myHost -rtpPort $adport -vuPort $vdport -vuPlayPort $gdport $notifyopt"

    set pid [eval exec $cmd &]

    catch {exec echo $pid >> $env(ISABEL_TMP_DIR)/Audio.pid}

    DaemonC::Invoke
}    

#------------------------------------------------------------

body DAudioC::CleanUp {} {

    if {$pid != 0} {
	after 100 after idle catch \\\"exec kill    $pid >& /dev/null\\\"
	after 100 after idle catch \\\"exec kill -9 $pid >& /dev/null\\\"
	set pid 0
    }

}

#------------------------------------------------------------
