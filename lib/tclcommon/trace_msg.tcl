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
#   Description
#
#      Historic information.
#
################################################################


class LogsMessagesC {

    #------------------
    # PUBLIC INTERFACE
    #------------------

    # Static constructor
    public proc Initialice {myLogId_}

    public proc WriteTraceMsg {msg {key ""}}
    public proc WarningTraceMsg {msg}
    public proc ErrorTraceMsg {msg}
    public proc DebugTraceMsg {msg {level 1}} 

    #-------------------
    # PRIVATE INTERFACE
    #-------------------

    # My log id.
    # Name of identify the executable.
    private common myLogId ""

    # Processes to logs
    private common logs ""

    # Returns the current date
    private proc GetDate {}
}

#---------------------------------------------------------------

body LogsMessagesC::Initialice {myLogId_} {

    set myLogId $myLogId_
}

#---------------------------------------------------------------

body LogsMessagesC::WriteTraceMsg {msg {key ""}} {

    if {$key == ""} {
	set topt $myLogId
    } else {
	set topt $myLogId:$key
    }
    puts "[GetDate] -- $topt --  $msg"
}

#---------------------------------------------------------------

body LogsMessagesC::WarningTraceMsg {msg} {

    puts "[GetDate] -- $myLogId -- WARNING:: $msg"
}

#---------------------------------------------------------------

body LogsMessagesC::ErrorTraceMsg {msg} {

    puts "[GetDate] -- $myLogId -- ERROR:: $msg"
}

#---------------------------------------------------------------

body LogsMessagesC::DebugTraceMsg {msg {level 1}} {
    global ISABEL_DEBUG

    if {$ISABEL_DEBUG == 0} return
    if {$level > $ISABEL_DEBUG} return

    puts "[GetDate] -- $myLogId -- DEBUG:: $msg"
}

#---------------------------------------------------------------

body LogsMessagesC::GetDate {} {

    clock format [clock seconds]
}

#---------------------------------------------------------------

#===============================================================
# WriteTraceMsg
# LogTraceMsg
# DebugTraceMsg
# ErrorTraceMsg
#===============================================================

if {![info exists ISABEL_DEBUG]} {
    set ISABEL_DEBUG 0
}

process WriteTraceMsg {msg {key ""}} {

    LogsMessagesC::WriteTraceMsg $msg $key
}


process DebugTraceMsg {msg {level 1}} {

    LogsMessagesC::DebugTraceMsg $msg $level
}


process WarningTraceMsg msg {

    LogsMessagesC::WarningTraceMsg $msg
}

process ErrorTraceMsg msg {

    LogsMessagesC::ErrorTraceMsg $msg
}


process tkerror err {
    global errorInfo
    
    set info "- Tcl error:\n$err\n\n$errorInfo\n"
    ErrorTraceMsg $info 

    DumpStackTrace "tkerror: $err"

    return
}

proc bgerror err {
    global errorInfo
    
    if 0 {
	set info "$err\n$errorInfo\n"
    } else {
	set info "$err\n"
    }
    ErrorTraceMsg $info 

    DumpStackTrace "err $info"

    return
}

