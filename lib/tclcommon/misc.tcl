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
######################################################################
#
#  HEADER FILE:  misc.tcl
#
#  Contents:
#
#       Definition of processes.
#	General editing functions and bindings
#	TKE functions (alert box, confirm, prompt)
#	RepeatCmd function (periodically repeats a given command)
#	Error handler (tkerror)
#       Image
#       echo
#       max min
#       ldelete
#       ExecComp

################################################################
#
#  Define a new process and check that it is nor redefined.
#
################################################################


set _traceproc "
"

proc process {pname pargs pbody} {
    global _traceproc env
    
    if {[info procs    $pname] != "" } {
	puts stderr "Source error: redefinition of process \"$pname\"."
	puts stderr [info body $pname]
	exit 1
    }
    if {[info commands $pname] != ""    } {
	puts stderr "Source error: redefinition of command \"$pname\"."
	exit 1
    }
    if {[info exists env(ISABEL_TRACEPROC)]} {
	if {$env(ISABEL_TRACEPROC) == 1} {
	    if {[lsearch -exact $_traceproc $pname] != -1} {
		proc $pname $pargs " 
	           set _i_level \[info level\]
	           puts -nonewline \"*  \$_i_level\"
		   set _indent \[format \"%\$\{_i_level\}s\" \" \"\]
	           puts -nonewline \$_indent\$_indent
	           puts -nonewline \"$pname \"
	           foreach _i_ \{$pargs\} \{
		       set _argi_ \[lindex \$_i_ 0\]
		       puts -nonewline \" \$_argi_=\"
		       eval puts -nonewline \$\$_argi_
		       unset  _argi_
		   \}
	           puts \"\"
		   flush stdout
                   unset _i_level _indent 
		   $pbody
	       "
	       return
	    }
	}
    }
    proc $pname $pargs $pbody
}


################################################################
#
#  estdisticas de objetos
#
################################################################

proc dbg_estadisticas {{_namespace ::}} {
    
    if {[catch "namespace eval $_namespace find objects" objs]} {
	return
    }
    
    set res ""
    foreach obj $objs {
	if {[string index $obj 0] != ":"} {
	    set res "$res $obj"
	}
    }
    
    set len [llength $res]
    if {$len != 0} {
	puts "Namespace = $_namespace"
	puts "   $len >> [lsort $res]"
    }
    
    foreach ns [lsort [namespace children $_namespace]] {
	namespace eval $ns dbg_estadisticas $ns
    }	
}


proc dbg_repetir_estadisticas {} {
    
    puts ===========================================
    dbg_estadisticas
    
    after 600000 dbg_repetir_estadisticas
}

################################################################
#
#  eval depuration
#
################################################################

if 0 {
    
    rename eval __RealEval__
    
    proc eval {args} {
	
	# puts "EVAL>> $args"
	DumpStackTrace "EVAL>> $args"
	
	set cmd ""
	
	foreach i $args {
	    set cmd "$cmd $i"
	}
	
	
	#__RealEval__ $cmd
	uplevel 1 $cmd
    }
}

################################################################
#
#  Write depuration traces.
#
################################################################

process TRAZA args { puts $args ; flush stdout }


process DumpStackTrace {msg} {
    
    puts "======= DumpStackTrace:: $msg ============"
    set level [info level]
    for {set i 0} {$i <= $level} {incr i} {
	for {set j 0} {$j <= $i} {incr j} {
	    puts -nonewline " "
	}
	
	puts [info level $i]
    }
    puts "================================================"
}


######################################################################
#
#  TKE Functions
#
######################################################################
#
#  tke_alert
#  tke_confirm
#  tke_prompt
#

#  arranges for transient window to be near main window
#
process windowpos { w } {

    set swidth [winfo screenwidth .]
    set sheight [winfo screenheight .]
    set vx [expr {-[winfo vrootx .]}]
    set vy [expr {-[winfo vrooty .]}]
    set vwidth  [expr {$vx + $swidth}]
    set vheight [expr {$vy + $sheight}]
    if [winfo ismapped .] {
        set x [winfo x .]
        set y [winfo y .]
    } else {
        set x [expr {$vx + $swidth/3}]
        set y [expr {$vy + $sheight/3}]
    }
    set xpos [expr {$x + [winfo width .]/2}]
    if {$xpos < $vx} {set xpos [expr {$vx + 50}]}
    if {$xpos > $vwidth - $swidth/4 - 10} {
        set xpos [expr {$vwidth - $swidth/2 + 100}]
    }
    set ypos [expr {$y + [winfo height .]/3}]
    if {$ypos < $vy} {set ypos [expr {$vy + 50}]}
    if {$ypos > $vheight - $sheight/4 - 10} {
        set ypos [expr {$vheight - $sheight/2 + 100}]
    }

    wm geometry $w +${xpos}+${ypos}
}


######################################################################
#  tke_alert - displays an alert box
######################################################################
#  msg: message to be displayed
#

set STATUS_LINE .statusLine
set TKEST_msgid -1

proc tke_stdelmsg {msgid} {
    global STATUS_LINE TKEST_msgid

    if {$TKEST_msgid == $msgid} {
	$STATUS_LINE config -text ""
    }
}


process tke_alert {msg {wid ""}} {

    global STATUS_LINE TKEST_msgid

    if {$wid == "" && ![winfo exists .alert] && \
	[winfo exists $STATUS_LINE] && [winfo ismapped $STATUS_LINE]} {
	$STATUS_LINE config -text $msg
	set TKEST_msgid "msg[tke_prcounter]"
	after 10000 "tke_stdelmsg $TKEST_msgid"
	return
    }

    set alert .alert$wid

    if [winfo exists $alert] {
	destroy $alert
    }

    toplevel $alert
    # wm transient $alert .
    wm title $alert "alert"
    message $alert.msg -text $msg -width 300 -anchor w
    frame $alert.r -height 2 -width 200 -borderwidth 1 -relief sunken
    frame $alert.b
    button $alert.b.ok -text Continue -bd 4 -width 8 \
	    -command "destroy $alert"
    pack append $alert.b $alert.b.ok {right padx 10 pady 10}
    pack append $alert \
	    $alert.msg {top fill expand padx 10} \
	    $alert.b {bottom fill} \
            $alert.r {bottom fillx}
    windowpos $alert
    bind $alert <Key-Return> "$alert.b.ok invoke"

    focus $alert
    grab $alert
#   tkwait window $alert
}

set tke_semaph 0

proc tke_destroy_alert {{wid ""}} {
    global tke_semaph
    if {$tke_semaph} {return}
    set tke_semaph 1
    set alert .alert$wid
    catch {destroy $alert}
    set tke_semaph 0
}

######################################################################
#  tke_confirm - displays a Cancel/OK dialog box
#  returns true (1) on OK; false (0) on Cancel
######################################################################
#
#  msg: message to be displayed
#

set CONFIRM 1
set confirm_wprefix tkeconf

process tke_confirm {msg {wid ""} {timeout 0} {grab 1}} {
    global confirm_wprefix confirm_result
    global CONFIRM		;# user preference---whether to ask

    if { ! $CONFIRM } {
        return 1
    }
    if {$wid == ""} {
	set wid [tke_prcounter]
    }
    set c .$confirm_wprefix$wid

    if [winfo exists $c] {
	after cancel tke_confTimeout $wid
	destroy $c
	return 0
    }

    set confirm_result($wid) 0
    toplevel $c
    wm title $c "confirm"
    
    message $c.msg -width 300 -anchor w -text $msg
    frame $c.b
    frame $c.b.r -height 2 -width 200 -borderwidth 1 -relief sunken
    button $c.b.ok -text OK -bd 4 -width 8 -command "tke_confOK $wid"
    button $c.b.cancel -text Cancel -width 8 \
	    -command "tke_confCancel $wid"
    
    if {$timeout > 0} {
	after ${timeout}000 tke_confTimeout $wid
    }

    pack append $c.b \
            $c.b.r {top fillx} \
	    $c.b.cancel {right padx 20 pady 20} \
	    $c.b.ok {right pady 20}
    pack append $c  \
	    $c.msg {top fill expand padx 10} \
	    $c.b {bottom fillx}
    windowpos $c
    
    bind $c <Return>      "$c.b.ok invoke"
    bind $c <Control-c>   "$c.b.cancel invoke"
    bind $c <Meta-c>      "$c.confirm.b.cancel invoke"
    bind $c <Meta-period> "$c.confirm.b.cancel invoke"

    focus $c
    after 100 raise $c
    if {$grab} {
	grab $c
    }
    tkwait window $c
    return $confirm_result($wid)
}

process tke_confOK {wid} {
    global confirm_wprefix confirm_result

    set confirm_result($wid) 1
    destroy .$confirm_wprefix$wid
}

process tke_confCancel {wid} {
    global confirm_wprefix confirm_result

    set confirm_result($wid) 0
    destroy .$confirm_wprefix$wid
}

process tke_confTimeout {wid} {
    global confirm_wprefix confirm_result

    set confirm_result($wid) 0
    destroy .$confirm_wprefix$wid
}


######################################################################
#  tke_prompt - prompts the user for information
######################################################################
#  prompt_label:
#	label to display such as "File Name:"
#  prompt_value:
#	initial value for entry (eg. "~/include/mfile.h")
#  ok_invoke: (default none)
#	function to be invoked when "OK" button is pressed
#	for instance load_file
#

set prompt_wprefix tkepr

process tke_prompt {prompt_label {prompt_value ""} {ok_invoke {}} {wid ""}} {

    global prompt_wprefix prompt_result
    
    if {$wid == ""} {
	set wid [tke_prcounter]
    }
    set t .$prompt_wprefix$wid
    
    if [winfo exists $t] {
	destroy $t
    }
    
    set prompt_result($wid) {././/CANCEL//./.}
    toplevel $t
    # wm transient $t .
    wm title $t "prompt"
    
    message $t.msg -width 300 -anchor w -text $prompt_label
    entry $t.field -relief sunken -width 40
    frame $t.b -relief sunken
    frame $t.b.r -height 2 -width 200 -borderwidth 1 -relief sunken
    
    button $t.b.ok -text OK -bd 4 -width 8 \
	    -command "tke_prOK $wid $ok_invoke"
    #	return a sentinel value on cancel:
    button $t.b.cancel -text Cancel -width 8 \
	    -command "tke_prCancel $wid"
    
    pack append $t.b \
	    $t.b.r {top fillx} \
	    $t.b.cancel {right padx 20 pady 20} \
	    $t.b.ok {right pady 20}
    pack append $t \
	    $t.msg {top fill expand padx 10} \
	    $t.field {top padx 10 pady 10} \
	    $t.b {bottom fillx}
    windowpos $t
    
    $t.field delete 0 end
    $t.field insert end $prompt_value
    
    bind $t.field <Return>      "$t.b.ok invoke"
    bind $t.field <Control-c>   "$t.b.cancel invoke"
    bind $t.field <Meta-c>      "$t.b.cancel invoke"
    bind $t.field <Meta-period> "$t.b.cancel invoke"
    bind $t.field <Tab> {
	set f [%W get]
	%W delete 0 end
	%W insert end [tabexpand $f]
    }
 
    #   focus $t.field
    grab $t
    tkwait window $t
    if [winfo exists .alert$wid] {destroy .alert$wid}
    return $prompt_result($wid)
}

process tke_prOK {wid {ok_invoke {}}} {
    global prompt_wprefix prompt_result

    set t .$prompt_wprefix$wid
    set value [$t.field get]
    if {$ok_invoke != {}} {
	set errormsg [$ok_invoke $value]
	if {$errormsg != ""} {
	    tke_alert $errormsg $wid
	    return
	}
    }
    set prompt_result($wid) $value
    destroy $t
}

process tke_prCancel {wid} {
    global prompt_wprefix prompt_result

    set prompt_result($wid) {././/CANCEL//./.}
    destroy .$prompt_wprefix$wid
}



###########################################################################
# tke_prcounter: returns a unique prompt window id number
###########################################################################

set tke_prcnt 0
process tke_prcounter {} {
   global tke_prcnt
   incr tke_prcnt
   return $tke_prcnt
}


######################################################################
# tabexpand f - expand filename prefix as much as possible
######################################################################
# note: if the filename has *, ?, or [...] in it, they will be used
#       as part of the globbing pattern.  i declare this a feature.

process tabexpand { f } {
  set expansion [longestmatch [glob -nocomplain "${f}*"]]
  if {$expansion == ""} {return $f}
  # make sure it doesn't already end in "/"
  set expansion [string trimright $expansion "/"]
  if [file isdirectory $expansion] {append expansion "/"}
  return $expansion
}

######################################################################
# longestmatch l - longest common initial string in list l
#   used by tab-expansion in filename dialogue box
######################################################################
# this needs commenting desperately

process longestmatch { l } {
  case [llength $l] in {
    {0} { return {} }
    {1} { return [lindex $l 0] }
  }
  set first [lindex $l 0]
  set matchto [expr {[string length $first] - 1}]
  for {set i 1} {$i < [llength $l]} {incr i} {
    set current [lindex $l $i]
    # if they don't match up to matchto, find new matchto
    if { [string compare \
           [string range $first 0 $matchto] \
           [string range $current 0 $matchto]] } {
      # loop, decreasing matchto until the strings match that far
      for {} \
          {[string compare \
              [string range $first 0 $matchto] \
              [string range $current 0 $matchto]] } \
          {incr matchto -1 } \
          {}			;# don't need to do anything in body
    } ;# end if they didn't already match up to matchto
  } ;# end for each element in list
  if {$matchto < 0} then {
    return {}
  } else {
    return [string range $first 0 $matchto]
  }
}



##################################################################
#
# RepeatCmd
#
# Repeat the execution of a comand periodically
#
##################################################################

process RepeatCmd {cmd t} {
   eval $cmd
   after $t RepeatCmd $cmd $t
}


################################################################
#
#		Error Manager
#
################################################################

if { 0 } {

proc tkerror {info} {
    case $info in {
        Tcp_PacketReceive*error*reading*file*unknown*error*(0) {
            set f [lindex [split $info] 4]
            filehandler $f
            close $f
            }
        RPC*error*Tcm_ProcessIncomingMessages {
            }
	file*isn*open {
	    catch {socket_error e [lindex $info 1]}
	}
        default {
	    global errorInfo
            error [concat "Unknown error condition" $errorInfo]
            }
    }
}

}


################################################################
#
#	Image
#
################################################################

proc CreateImage {name file {force 0}} {
    global env
    
    set ext [file extension $file]
    switch -- $ext {
	.gif {
	    set type photo
            set _extwd gif 
	}
	.xbm {
	    set type bitmap
            set _extwd xbm
	}
	default {
	    set type photo
	    set file gif/bug.gif
	}
    }
    
    if {[file readable $file]} {
	set theimgfile $file
    } elseif {[file readable $env(ISABEL_WORKDIR)/$file]} {
	set theimgfile $env(ISABEL_WORKDIR)/$file
    } elseif {[file readable $env(ISABEL_DIR)/lib/$file]} {
	set theimgfile $env(ISABEL_DIR)/lib/$file
    } elseif {[file readable $env(ISABEL_DIR)/lib/images/$file]} {
	set theimgfile $env(ISABEL_DIR)/lib/images/$file
    } elseif {[file readable $env(ISABEL_DIR)/lib/images/$_extwd/$file]} {
	set theimgfile $env(ISABEL_DIR)/lib/images/$_extwd/$file
    } else {
	set type       photo
	set theimgfile $env(ISABEL_DIR)/lib/images/gif/bug.gif
    }

    if {$force || [lsearch -exact [image names] $name] == -1} {
	image create $type $name -file $theimgfile
    }
    
    return $name
}

################################################################
#
# Initialize a scale without invoking the associated command.
#
################################################################

proc InitScaleValue {scale value cmd} {

    if {![winfo exist $scale]} return

    set state [$scale cget -state]
    $scale conf -state normal
    $scale set $value
    $scale conf -state $state
    after idle after 1000 \
	    catch [list [list [list $scale conf -command $cmd]]]
}

proc InitScale {scale cmd} {

    if {![winfo exist $scale]} return

    after idle after 1000 \
	    catch [list [list [list $scale conf -command $cmd]]]
}

################################################################
#
# Set values of a optionmenu widget without invoking the associated command.
#
################################################################

proc SetOptionmenuValues {om values} {

    if {![winfo exist $om]} return

    set cmd [$om cget -command]
    $om configure -command ""
    set state [$om cget -state]
    $om configure -state normal
    $om delete 0 end
    foreach value $values {
	$om insert end $value
    }
    $om configure -state $state
    after idle catch \{$om configure -command [list $cmd]\}
}

################################################################
#
# IsDaemonRunning
#
################################################################

proc  IsDaemonRunning {daemonName} {

    if 0 {
	if {[catch {exec ps -guaxww | grep -v grep | grep $daemonName} msg]} {
	    return 0
	} else {
	    return 1
	}
    }

    if {[exec isabel_fp $daemonName] == ""} {
	return 0
    } else {
	return 1
    }
}

################################################################
#
# echo
#
################################################################

proc echo args {

    puts $args
    flush stdout

}

################################################################
#
# max min
#
################################################################

proc max {a b} {
    if {$a < $b} {
	return $b
    } else {
	return $a
    }
}

proc min {a b} {
    if {$a < $b} {
	return $a
    } else {
	return $b
    }
}

################################################################
#
# server_info
#
################################################################

proc server_info {a b} {
    return $b
}

################################################################
#
# ldelete
#
################################################################

proc ldelete {list elem} {

    set pos [lsearch -exact $list $elem]
    if {$pos != -1} {
	return [lreplace $list $pos $pos]
    } else {
	return $list
    }
}

################################################################
#
# ExecComp
#
# Executes the command $arg if the component $name exists, and
# the site [SiteManager::MySiteId] is joined to the component.
# %A is replaced with the applid of the component $name.
################################################################

proc ExecComp {name args} {

    if {[GlobalState::ComponentExists $name]} {

	set _my_site_id [SiteManager::MySiteId]

	if {[lsearch -exact [GlobalState::GetSitesByState $name Running] $_my_site_id] != -1} {
	    regsub -all %A $args $name cmd
	    if {[catch {uplevel 1 $cmd} res]} {
		ErrorTraceMsg "$name: $args produces:\n$res"
		return ""
	    }
	    return $res
	}
    }
    return ""
}


################################################################
# Create a log filename.
# Format:
#   1- The log directory
#   2- The given prefix filename
#   3- The date
#   4- The file extension. Default extension is "log".
################################################################

proc CreateLogFilename {_prefix {_extension ""}} {
    global env
    
    set _seconds [clock seconds]
    set _date [clock format $_seconds -format "%Y%m%d.%H%M%S"]
    
    if {$_extension == ""} {
	set _extension log
    }

    return $env(ISABEL_SESSION_LOGS_DIR)/$_prefix.$_date.$_extension
}

################################################################
# Return one value option from a option-value list.
# $list is the option-value list.
# $optname is option name to search.
# $default is the value to return if $optname option is not found.
################################################################
 
proc getOptionListValue {list optname {default ""}} {

    set pos [lsearch -exact $list $optname]
    if {$pos == -1} {
	return $default	
    } else {
	return [lindex $list [expr $pos+1]]
    }
}


