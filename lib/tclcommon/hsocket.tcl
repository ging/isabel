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
#==================================================================
#
#  hsocket - socket handler
#
#    hsocket(opened)        - list of opened sockets.
#
#    hsocket($sap,readable) - process to execute when the socket
#                             is readable.
#
#    hsocket($sap,writable) - process to execute when the socket
#                             is writable.
#
#    hsocket($sap,atclose)  - list of process to execute when the
#                             socket is closed.
#    
#==================================================================

set hsocket(opened) ""


#
# create new entries for $sap if they don't exist
#
proc hs_open {sap} {
    global hsocket
    
    set pos [lsearch -exact $hsocket(opened) $sap]
    if {$pos == -1} {
	set hsocket($sap,atclose)  ""
	set hsocket($sap,writable) ""
	set hsocket($sap,readable) ""
	lappend hsocket(opened) $sap
    }
}

#
# delete the entries associated with $sap.
#
proc hs_close {sap} {
    global hsocket
    
    catch {fileevent $sap readable ""}
    catch {fileevent $sap writable ""}

    catch {unset hsocket($sap,readable)}
    catch {unset hsocket($sap,writable)}
    catch {unset hsocket($sap,atclose)}

    set pos [lsearch -exact $hsocket(opened) $sap]
    if {$pos != -1} {
	set hsocket(opened) [lreplace $hsocket(opened) $pos $pos]
    }

    #after 5000 catch "close $sap"
    close $sap
}


#
# Set the command to execute when the socket $sap is readable.
#
proc hs_readable {sap {cmd ""}} {
    global hsocket

    hs_open $sap
    set hsocket($sap,readable) $cmd
    catch {fileevent $sap readable "hs_fileevent $sap r"}
}


#
# Set the command to execute when the socket $sap is writable.
#
proc hs_writable {sap {cmd ""}} {
    global hsocket

    hs_open $sap
    set hsocket($sap,writable) $cmd
    catch {fileevent $sap writable "hs_fileevent $sap w"}
}


#
# Set commands to execute when the socket $sap is closed.
# $opt must be: -append, -set, -appendUnique or -delete 
#
proc hs_atclose {sap opt cmd} {
    global hsocket

    hs_open $sap

    switch -- $opt {
	-append {
	    lappend hsocket($sap,atclose) $cmd
	}
	-set {
	    set hsocket($sap,atclose) $cmd
	}
	-delete {
	    set pos [lsearch -exact $hsocket($sap,atclose) $cmd]
	    if {$pos != -1} {
		set hsocket($sap,atclose) \
			[lreplace $hsocket($sap,atclose) $pos $pos]
	    }
	}
	-appendUnique {
	    set pos [lsearch -exact $hsocket($sap,atclose) $cmd]
	    if {$pos == -1} {
		lappend hsocket($sap,atclose) $cmd
	    }
	}
    }
    catch {fileevent $sap readable "hs_fileevent $sap r"}
}


#
# The real fileevent process that capture the writable, readable and
# atclose events for the socket $sap.
#
#
proc hs_fileevent {sap {rw ""}} {
    global hsocket

    if {[eof $sap]} {

	set closeCBs $hsocket($sap,atclose)

	hs_close $sap

	foreach cmd $closeCBs {
	    if {[catch {uplevel #0 $cmd} emsg]} {
		ErrorTraceMsg "Hsockets atclose: \"$cmd\" produces \"$emsg\""
	    }
	}
	return
    }
    
    if {$rw == "r"} {
	if {$hsocket($sap,readable) != ""} {
	    if {[catch {uplevel #0 $hsocket($sap,readable)} emsg]} {
		gets $sap
		ErrorTraceMsg "Hsockets readable: \"$hsocket($sap,readable)\" produces \"$emsg\""
	    }
	} else {
	    gets $sap
	}
    } else {
	if {[catch {uplevel #0 $hsocket($sap,writable)} emsg]} {
	    ErrorTraceMsg "Hsockets writable: \"$hsocket($sap,writable)\" produces \"$emsg\""
	}
    }
}
