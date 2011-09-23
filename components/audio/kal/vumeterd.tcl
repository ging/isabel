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
#     vumeter daemon
#
################################################################

class DVumeterC {
    inherit DaemonC

    # display
    private variable display

    constructor {timeout no_cc_cb _display} {
	DaemonC::constructor Vumeter $timeout vumeter_quit() \
		vumeter_nop() 0 $no_cc_cb isabel_vumeter
    } {}

    destructor {}

    private method Invoke {}

    # kill the vumeter daemon.
    private method CleanUp {}

    private variable pid 0
}

#------------------------------------------------------------

body DVumeterC::constructor {timeout no_cc_cb _display} {

    set display $_display
}

#------------------------------------------------------------

body DVumeterC::destructor {} {

    CleanUp
}

#------------------------------------------------------------

body DVumeterC::Invoke {} {
    global ISABEL_DIR env

    # Control port
    set cport 51008

    # Port where irouter send the vumeter flow.
    set dport 51021

    set cmd "isabel_vumeter -c$cport -d$dport $display"

    set pid [eval exec $cmd &]

    catch {exec echo $pid >> $env(ISABEL_TMP_DIR)/Vumeter.pid}

    DaemonC::Invoke
}

#------------------------------------------------------------

body DVumeterC::CleanUp {} {

    if {$pid != 0} {
	after 100 after idle catch \\\"exec kill    $pid >& /dev/null\\\"
	after 100 after idle catch \\\"exec kill -9 $pid >& /dev/null\\\"
	set pid 0
    }

}

#------------------------------------------------------------
