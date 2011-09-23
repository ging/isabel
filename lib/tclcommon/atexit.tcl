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


rename exit __RealExit__


process exit {{returncode 0}} {
    AtExit::Exit $returncode
}




class AtExit {

    # Executes all the commands in the callback list, then
    # destroyes created windows, then cancels all pending
    # "after" commands, and finally it exits the application.
    public proc Exit {{returncode 0}}

    # Executes all the commands in the callback list, then
    # destroyes created windows, then cancels all pending
    # "after" commands, then display a message window with the
    # messages given in $args, and finally it exits the application.
    public proc ExitMsg {returncode args}

    # Updates the callback list.
    public proc Append       {cmd}
    public proc AppendUnique {cmd}
    public proc Set          {cmd}
    public proc Delete       {cmd}


    private common atexitlist ""
    
    # Executes the commands in the callback list.
    private proc ExecCB {}

    # Destroyes windows
    private proc DestroyWindows {}

    # Cancels afters
    private proc CancelAfters {}
}

#-------------------------------------------------------

body AtExit::Append {cmd} {
    
    lappend atexitlist $cmd
}

#-------------------------------------------------------

body AtExit::AppendUnique {cmd} {

    set pos [lsearch -exact $atexitlist $cmd]
    if {$pos == -1} {
	lappend atexitlist $cmd
    }
}

#-------------------------------------------------------

body AtExit::Set {cmd} {

    set atexitlist $cmd
}

#-------------------------------------------------------

body AtExit::Delete {cmd} {

    set pos [lsearch -exact $atexitlist $cmd]
    if {$pos != -1} {
	set atexitlist [lreplace $atexitlist $pos $pos]
    }
}

#-------------------------------------------------------

body AtExit::Exit {{returncode 0}} {

    WriteTraceMsg "Exiting"
    ExecCB
    
    WriteTraceMsg "- ISABEL exit code: $returncode"

    close stdout    
    close stderr

    __RealExit__ $returncode
}

#-------------------------------------------------------

body AtExit::ExitMsg {returncode args} {

    WriteTraceMsg "Exiting:: $args"    
    
    ExecCB
    DestroyWindows
    CancelAfters 
    
    if {$args != "\{\}"} {
	eval ErrorMsgC #auto close -grab -timeout 10000 $args
    } else {
	after 2000
    }
    
    WriteTraceMsg "- ISABEL exit code: $returncode"

    close stdout    
    close stderr

    __RealExit__ $returncode
}

#-------------------------------------------------------

body AtExit::CancelAfters {} {
    
    foreach i [after info] {
	catch {after cancel $i}
    }
}

#-------------------------------------------------------

body AtExit::DestroyWindows {} {
    
    foreach i [winfo children .] {
	destroy $i
    }
    wm withdraw .
}

#-------------------------------------------------------

body AtExit::ExecCB {} {

    foreach cmd $atexitlist {
	if {[catch {uplevel "#0" $cmd} emsg]} {
	    ErrorTraceMsg "atexit: $emsg"
	}
    }
}

