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

################################################################
#
#                           PINGS
#
#       Checks that the connected processes are alive.
#
################################################################


#---------------------------------------------------------------
# pingTable
#
# This table stores information about all the processes connected to me.
#
# These processes are excited periodically in order to see if they are alive.
# When a process does not respond, then it is suppossed that this process
# is death and the information related with it is updated to set that fact.
#
# This table is implemented as an array where the index is the "sap" to
# connect with the process. The value stored in each entry of the table 
# is a list with the following elements:
#   position 0 -> command to execute to check that the process is alive.
#                 This command returns: 1 if the process is alive.
#                                       0 if it is death.
#   position 1 -> command to execute when the process die.
#---------------------------------------------------------------

global pingTable


#---------------------------------------------------------------
# CreatePingEntry
#
# Define a new process to ping.
#---------------------------------------------------------------

process CreatePingEntry {sap check_cmd close_cmd} {
    global pingTable

    if {$sap == "filelocal"  ||
        $sap == "unassigned"    } {
	return
    }
    
    set pingTable($sap) [list $check_cmd $close_cmd]

    hs_atclose $sap -append $close_cmd
}


#---------------------------------------------------------------
# DeletePingEntry
#
# Delete a entry in the pingTable.
#---------------------------------------------------------------

process DeletePingEntry sap {
    global pingTable

    if {[info exists pingTable($sap)]} {
	set close_cmd [lindex $pingTable($sap) 1]
	hs_atclose $sap -delete $close_cmd
	unset pingTable($sap)
    }
}


#---------------------------------------------------------------
# CheckPing
#
# Check that process associated to "sap" is alive.
# If the process is death, then it is removed.
#---------------------------------------------------------------

process CheckPing {sap} {
    global pingTable

    if {[info exists pingTable($sap)]} {
	set check_cmd [lindex $pingTable($sap) 0]
	set close_cmd [lindex $pingTable($sap) 1]

	if {[eval $check_cmd] == 0} {
	    DeletePingEntry $sap
	    eval $close_cmd
	}
    } else {
	unset pingTable($sap)
    }
}


#---------------------------------------------------------------
# StartPing
#
# Check periodically the ping values to remove death processes.
#---------------------------------------------------------------

process StartPing {} {
   global pingTable

   if {[catch {set l [array names pingTable]}] == 0} {
      foreach sap $l {
	  catch {CheckPing $sap}
      }
   }
   after 8000 after idle StartPing
}


################################################################
#
# Ping
#
################################################################

process Ping {} {
}


#---------------------------------------------------------------

StartPing

#---------------------------------------------------------------



