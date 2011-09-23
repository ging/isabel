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
#     irouter daemon
#
################################################################

class DIrouterC {
    inherit DaemonC

    
    constructor {no_cc_cb} {
	DaemonC::constructor Irouter 30000 ir7_quit() \
		ir7_nop() 1 $no_cc_cb isabel_irouter
    } {
    }

    destructor {CleanUp}

    private method Invoke {}

    # kill the irouter daemon.
    private method CleanUp {}

    private variable pid 0
}

#------------------------------------------------------------

body DIrouterC::Invoke {} {
    global ISABEL_DIR env

    # Session Port: it is the base_data_port.
    #
    # set bdport [session Environ cget -session_port]
    #
    set bdport 53020
    if {$bdport == ""} {
	set bdportopt ""
    } else {
	set bdportopt "-bdport $bdport"
    }

    # Multicast
    set mcast   [session Environ cget -use_multicast]
    set mcastopts ""

    if { $mcast } {

        if {[info exists env(ISABEL_USE_SSM)] } {
            set ssmparent $env(ISABEL_USE_SSM)

            set CONNECT_TO_IP [session Environ cget -connect_to_ip]

            if { 0 } {
            if { $CONNECT_TO_IP=="" } {
                # I'm the parent in the SSM
                set ssmparent [session Environ cget -hostname]
            } else {
                if {[session Environ cget -use_flowserver] == 1} {
                    # I'm the parent in the SSM connected to other node
                    set ssmparent [session Environ cget -hostname]
                } else {
                    # I'm a leave in the SSM
                    set ssmparent $CONNECT_TO_IP
                }
            }
            }

            set mcastopts "-SSMcast $ssmparent"
        } else {
            set mcastopts "-ASMcast"
        }

	set mcastbw [session Environ cget -mcast_bw]
	set ttl [session Environ cget -mcast_ttl]
	append mcastopts " -mcastBw ${mcastbw}000 -cfg none -ttl $ttl"

	set audiog      [session Environ cget -mcast_audio_group]
	if {$audiog != ""} {
	    append mcastopts " -mcastGroup audio=$audiog"
	}

	set vumeterg      [session Environ cget -mcast_vumeter_group]
	if {$vumeterg != ""} {
	    append mcastopts " -mcastGroup vumeter=$vumeterg"
	}

	set videog      [session Environ cget -mcast_video_group]
	if {$videog != ""} {
	    append mcastopts " -mcastGroup video=$videog"
	}

	set ftpg        [session Environ cget -mcast_ftp_group]
	if {$ftpg   != ""} {
	    append mcastopts " -mcastGroup slidesFtp=$ftpg"
	}

	set pointerg   [session Environ cget -mcast_pointer_group]
	if {$pointerg != ""} {
	    append mcastopts " -mcastGroup pointer=$pointerg"
	}

	set shdisplayg [session Environ cget -mcast_shdisplay_group]
	if {$shdisplayg != ""} {
	    append mcastopts " -mcastGroup shDisplay=$shdisplayg"
	}
    }

    # Flowserver
    set use_fserver [session Environ cget -use_flowserver]
    set connect_to_ip  [session Environ cget -connect_to_ip]
    if {$use_fserver == 0} {
	set fsopts ""
    } elseif {[catch {exec isabel_localaddress $connect_to_ip}] == 0} { 
	set fsopts ""
    } else {
	set fsupbw   [session Environ cget -fs_upbw]
	set fsdownbw [session Environ cget -fs_downbw]
	set fsopts "-flowServer $connect_to_ip -upStreamBw ${fsupbw}000 -downStreamBw ${fsdownbw}000"
    }


    # FEC
    if 0 {
	set irfec [session Environ cget -irouter_fec]
	set fecopts ""
	if {$irfec == ""} {
	    set fecopts ""
	} else {
	    set fecopts " -parity $irfec "
	}
    } else {
	set fecopts ""
    }
    
    # Mixer audio channels.
    set iram [session Activity GetResource Audio irouter.mixer 1]
    if {$iram == "1" || [string toupper [string trim $iram]] == "TRUE"} {
	set _codec [session Activity GetResource Audio initialcodec gsm-8KHz] 
	set amopts " -audioMixer $_codec"
    } else {
	set amopts ""
    }


    if {[info exists env(ISABEL_IROUTER_DEBUG_LEVEL)]} {
	set debugopts "-debugLevel $env(ISABEL_IROUTER_DEBUG_LEVEL) -debugFile /tmp/irouter.[pid]"
    } else {
	set debugopts ""
    }

    if {$env(USER) == "ubuntu"} {
	set notifyopt "-notify /dev/null"
    } else {
	set _logfn $env(ISABEL_SESSION_LOGS_DIR)/notify.Irouter
	if { ! [file exists $_logfn]} {
	    exec touch $_logfn
	}
	set notifyopt "-notify $_logfn"
    }

    set cport 51000

    switch -- [session Environ cget -role] {
	Interactive { 
	    set client_flows -allClients

	    set gwsip_target ""
	}
        SipGateway { 
	    set client_flows -allClients

	    set gwsip_target "-gateway ::ffff:127.0.0.1"
	}
        FlashGateway { 
	    set client_flows -allClients

	    set gwsip_target "-gateway ::ffff:127.0.0.1"
	}
	MCU { 
	    # There are no clients.
	    set client_flows ""

	    set gwsip_target ""
	}
    }

    set cmd "isabel_irouter -cport $cport -linkMgr $client_flows $gwsip_target $mcastopts $fsopts \
             $amopts $fecopts $bdportopt $debugopts $notifyopt"

    set pid [eval exec $cmd &]

    catch {exec echo $pid >> $env(ISABEL_TMP_DIR)/Irouter.pid}

    DaemonC::Invoke
}

#------------------------------------------------------------

body DIrouterC::CleanUp {} {

    if {$pid != 0} {
	after 100 after idle catch \\\"exec kill    $pid >& /dev/null\\\"
	after 100 after idle catch \\\"exec kill -9 $pid >& /dev/null\\\"
	set pid 0
    }
}

#------------------------------------------------------------
