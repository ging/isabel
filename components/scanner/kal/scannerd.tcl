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
#     HP scanner daemon
#
################################################################

class DScannerC {
    inherit DaemonC

    constructor {timeout no_cc_cb} {
	DaemonC::constructor Scanner $timeout scanner_quit() \
		scanner_nop() 1 $no_cc_cb isabel_scanner
    } {}

    destructor {}

    private method Invoke {}

    # kill the scanner daemon.
    private method CleanUp {}
}

#------------------------------------------------------------

body DScannerC::constructor {timeout no_cc_cb} {

}

#------------------------------------------------------------

body DScannerC::destructor {} {

    # CleanUp
}

#------------------------------------------------------------

body DScannerC::Invoke {} {
    global ISABEL_DIR

    # kill old scanner daemon.
    CleanUp

    if {[IsDaemonRunning isabel_scanner]} {
	set m1 "- ERROR: There is already an isabel_scanner daemon running."
	set m2 "  and I can't kill it in order to start a new one."
	ErrorTraceMsg $m1\n$m2
	return -1
    }

    set cmd "isabel_scanner -cport 0 -libExecDir $ISABEL_DIR/libexec"

    if {[catch {set cport [eval exec $cmd]} emsg]} {
	ErrorTraceMsg "- I can't invoke Scanner daemon.\n$emsg"
	return -1
    }

    DaemonC::Invoke
}

#------------------------------------------------------------

body DScannerC::CleanUp {} {

    catch {exec fp -k isabel_scanner isabel_hpscan}
}

#------------------------------------------------------------
