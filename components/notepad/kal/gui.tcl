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
#  Cooperative Notepad
#
################################################################


#===============================================================
#
#   Notepad iconify/deiconify
#
#===============================================================

#---------------------------------------------------------------
# show notepad window
#---------------------------------------------------------------
process NotepadMap { } {
    global wroot
    global notepad_geom
    
    catch {wm deiconify $wroot}
    
   wm geometry $wroot $notepad_geom
}


#---------------------------------------------------------------
#  hide notepad window
#---------------------------------------------------------------
process NotepadUnmap { } {
    global notepad_geom
    global wroot
    
if {0} {

    if {[winfo exists $wroot]} {
        if {[wm state $wroot] == "normal"} {
            # Esta mapeado. tomo la geometria actual por si la he
	    # cambiado manualmente.
            set notepad_geom [wm geometry $wroot]
        }
    }
}
    catch {wm withdraw $wroot}
}



#===============================================================
#
# Auxiliary functions.
#
#===============================================================

#---------------------------------------------------------------
# Execute a method in a remote text widget.
#---------------------------------------------------------------
process slave_edit {args} {
    global notepad updated
    
    if {!$updated} return

    $notepad config -state normal
    eval $notepad $args
    $notepad config -state disabled
}


#---------------------------------------------------------------
# centra el texto para que se vea el cursor
#---------------------------------------------------------------
proc centerinsert {} {
    global notepad
    $notepad see insert
}



#---------------------------------------------------------------
# focus_notepad
#---------------------------------------------------------------
process focus_notepad {} {
    global notepad writemode
    
    if {$writemode} {
	focus $notepad
    }
}

#===============================================================
#
#  Set to everybody
#
#===============================================================

#---------------------------------------------------------------
# envia un string a todo el mundo.
#---------------------------------------------------------------
process send_msg {msg} { 
    global notepad
    
    if {$msg == ""} return
    
    set range           [$notepad tag range sel]
    set insertion_point [$notepad index insert]
    
    if {$range != ""} {
	set from            [lindex $range 0]
	set to              [lindex $range 1]
	
	if {[$notepad compare $from <= $insertion_point] &&
	[$notepad compare $to   >= $insertion_point]} {
	    network RDOGroup notepad "Cmd slave_edit delete $from $to"
	    network RDOGroup notepad "Cmd slave_edit insert $from $msg"
	    return
	}
    }
    
    network RDOGroup notepad "Cmd slave_edit insert $insertion_point [list $msg]"
}


#---------------------------------------------------------------
#  send a backspace
#---------------------------------------------------------------
process send_backspace {} {
    global notepad
    
    set range [$notepad tag range sel]
    if {$range == ""} {
	set i_point [$notepad index insert]
	network RDOGroup notepad "Cmd slave_edit delete \"$i_point - 1 char\" $i_point"
    } else {
	set from [lindex $range 0]
	set to   [lindex $range 1]
	network RDOGroup notepad "Cmd slave_edit delete $from $to"
    }
}


#---------------------------------------------------------------
#  send a delete
#---------------------------------------------------------------
process send_delete {} {
    global notepad myuserid
    
    set range [$notepad tag range sel]
    if {$range == ""} {
	set i_point [$notepad index insert]
	network RDOGroup notepad "Cmd slave_edit delete $i_point \"$i_point + 1 char\""
    } else {
	set from [lindex $range 0]
	set to   [lindex $range 1]
	network RDOGroup notepad "Cmd slave_edit delete $from $to"
    }
}


#===============================================================
#
#  Menus
#
#===============================================================


process CreateFileMenu {} {
    global wroot

    
    if [winfo exists $wroot.f.f1.file.m] {
	destroy $wroot.f.f1.file.m
    }
    menu $wroot.f.f1.file.m -tearoff 0
    
    # todos los participantes tienen el mismo menu File
    # pero solo los coordinadores podran pulsar las opciones
    # "Load..." e "Insert File..."
    $wroot.f.f1.file.m add command -label "Read coordinator" -command {
	if {!$writemode} {
            # no soy coordinador, me sincronizo
	    RequestUpdated
	}
    }
    $wroot.f.f1.file.m add separator
    
    $wroot.f.f1.file.m add command -label "Load . . ." -command cmd_load
    $wroot.f.f1.file.m add command -label "Insert File . . ." \
	    -command {cmd_insfile}
    $wroot.f.f1.file.m add separator
    $wroot.f.f1.file.m add command -label "Save" -command "cmd_save"
    $wroot.f.f1.file.m add command -label "Save as . . ." -command "cmd_saveas"
    $wroot.f.f1.file.m add separator
    $wroot.f.f1.file.m add command -label "Print . . ." -command {cmd_print}
}


process CreateEditMenu {} {
    global wroot
    global notepad
    
    menu $wroot.f.f1.edit.m -tearoff 0
    
    $wroot.f.f1.edit.m add command -label {Cut} -command {cmd_cut}
    $wroot.f.f1.edit.m add command -label {Copy} -command {cmd_copy}
    $wroot.f.f1.edit.m add command -label {Paste} -command {cmd_paste}
    $wroot.f.f1.edit.m add command -label {Select All} -command {cmd_select_all}
    $wroot.f.f1.edit.m add separator
    $wroot.f.f1.edit.m add command -label {Find . . .} -command {cmd_find}
}

process CreateToolsMenu {} {
    global wroot

    menu $wroot.f.f1.pipe.m -tearoff 0
    $wroot.f.f1.pipe.m add command -label {Format} -command {
        tke_pipenl {fmt}
    }
    $wroot.f.f1.pipe.m add separator
    $wroot.f.f1.pipe.m add command -label {Capitalise} -command {
        tke_pipe {tr \[a-z\]ñáéíóú \[A-Z\]ÑÁÉÍÓÚ}
    }
    $wroot.f.f1.pipe.m add command -label {Lowercase} -command {
        tke_pipe {tr \[A-Z\]ÑÁÉÍÓÚ \[a-z\]ñáéíóú}
    }
    $wroot.f.f1.pipe.m add command -label {Toggle Case} -command {
        tke_pipe {tr \[A-Z\]\[a-z\]ñÑáéíóúÁÉÍÓÚ \[a-z\]\[A-Z\]ÑñÁÉÍÓÚáéíóú}
    }
    $wroot.f.f1.pipe.m add separator
    $wroot.f.f1.pipe.m add command -label {Indent} -command {
        tke_pipenl {sed "s/^/  /"}
    }
    $wroot.f.f1.pipe.m add command -label {Unindent} -command {
        tke_pipenl {sed "s/^  //
	s/^\t/      /"}
    }
    $wroot.f.f1.pipe.m add command -label {Quote Email} -command {
        tke_pipenl {sed "s/^/> /"}
    }
    $wroot.f.f1.pipe.m add separator
    $wroot.f.f1.pipe.m add command -label {Sort by ASCII Sequence} -command {
        tke_pipenl {sort}
    }
    $wroot.f.f1.pipe.m add command -label {Sort Numerically} -command {
        tke_pipenl {sort -n}
    }
    $wroot.f.f1.pipe.m add command -label {Sort Alphabetically} -command {
        tke_pipenl {sort -if}
    }
}

#===============================================================
#
#  Create user interface
#
#===============================================================

process CreateInterface {} {
    global wroot

    global writemode notepad
    global normalbg activebg darkbg
    global notepad_font
    global myapplid userport myuserid
    global env

    set notepad_font [session Activity GetResource Notepad font ""]

    set notepad_title [session Activity GetResource Notepad title ""]

    wm title $wroot $notepad_title

    button $wroot.bbb ; scrollbar $wroot.sss
    set normalbg [lindex [$wroot.bbb config -bg] 4]
    set activebg [lindex [$wroot.bbb config -activebackground] 4]
    set darkbg   [lindex [$wroot.sss config -activebackground] 4]
    destroy $wroot.bbb ; destroy $wroot.sss
    
    frame $wroot.main -relief sunken -bd 3
    
    #--------------------------------------------
    
    BalloonClass e_balloon
    
    frame $wroot.f
    frame $wroot.f.f1
    
    menubutton $wroot.f.f1.file -text File  -menu $wroot.f.f1.file.m -relief flat
    menubutton $wroot.f.f1.edit -text Edit  -menu $wroot.f.f1.edit.m -relief flat
    menubutton $wroot.f.f1.pipe -text Tools -menu $wroot.f.f1.pipe.m -relief flat
    
    CreateFileMenu
    CreateEditMenu
    CreateToolsMenu
    
    #--------------------------------------------
    
    frame $wroot.f.buttons
    
    CreateImage npload  gif/open.gif
    CreateImage npsave  gif/save.gif
    CreateImage npprint gif/print.gif
    CreateImage npcut   gif/cut.gif
    CreateImage npcopy  gif/copy.gif
    CreateImage nppaste gif/paste.gif
    
    button $wroot.f.buttons.load  -image npload  -command cmd_load
    e_balloon add $wroot.f.buttons.load "Load"
    button $wroot.f.buttons.save  -image npsave  -command cmd_save
    e_balloon add $wroot.f.buttons.save "Save"
    button $wroot.f.buttons.print -image npprint -command cmd_print
    e_balloon add $wroot.f.buttons.print "Print"
    button $wroot.f.buttons.cut   -image npcut   -command cmd_cut
    e_balloon add $wroot.f.buttons.cut "Cut"
    button $wroot.f.buttons.copy  -image npcopy  -command cmd_copy
    e_balloon add $wroot.f.buttons.copy "Copy"
    button $wroot.f.buttons.paste -image nppaste -command cmd_paste
    e_balloon add $wroot.f.buttons.paste "Paste"

    ::swc::Optionmenu $wroot.f.buttons.font -narrow 1 \
           -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
           -labelmargin 0 \
           -labelfont -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
           -command change_font

    $wroot.f.buttons.font insert end courier helvetica times fixed clean
    
    ::swc::Optionmenu  $wroot.f.buttons.weight -narrow 1 \
           -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
           -labelmargin 0 -command change_font

    $wroot.f.buttons.weight insert end bold medium

    ::swc::Optionmenu $wroot.f.buttons.points -narrow 1 \
          -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
          -labelmargin 0 -command change_font

    $wroot.f.buttons.points insert end \
           2p 6p 7p 8p 9p 10p 12p 13p 14p 15p 16p 18p 19p 20p \
           24p 25p 26p 30p 33p 34p 40p

    $wroot.f.buttons.points select 12p

    pack $wroot.f.buttons.load \
	    $wroot.f.buttons.save \
	    $wroot.f.buttons.print \
	    $wroot.f.buttons.cut \
	    $wroot.f.buttons.copy \
	    $wroot.f.buttons.paste \
            $wroot.f.buttons.font \
            $wroot.f.buttons.weight \
            $wroot.f.buttons.points \
            -side left
 
    #--------------------------------------------

    label $wroot.f.filename -text (none) \
        -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*

    #--------------------------------------------
    
    frame $wroot.f.f2 -relief sunken -bd 1
    label $wroot.f.f2.l1 -text "Floor: " \
      -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*
    label $wroot.f.f2.l2  -foreground black  -background $normalbg \
      -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*

    CreateImage e_floor     xbm/floor.xbm
    CreateImage e_no_floor  xbm/no_floor.xbm
    CreateImage e_lock      gif/lock.gif
    CreateImage e_unlock    gif/unlock.gif

    checkbutton $wroot.f.f2.write -image e_floor \
      -indicatoron 0 -selectcolor $activebg \
      -selectimage e_no_floor -command get_floor
    e_balloon add $wroot.f.f2.write "Get Floor"

    button $wroot.f.f2.lock -image e_unlock -command lock_floor 
    e_balloon add $wroot.f.f2.lock "Lock Floor"

    
    if {0} {
    checkbutton $wroot.f.f2.write -relief raised -variable writemode \
	    -text "Get Floor" -command get_floor
    }
    
    #--------------------------------------------
    
    text $notepad \
	-wrap word \
	-font "-*-courier-bold-R-Normal--*-120-*-*-*-*-*-*" \
	-background white \
        -insertbackground black \
	-yscrollcommand "$wroot.main.sb set"

    if {$notepad_font != ""} {
	$notepad configure -font $notepad_font
    }
    scrollbar $wroot.main.sb -command "$notepad yview"
    
    bindtags $notepad [concat CoopText [bindtags $notepad]]
    
    bind CoopText <Control-d>	{send_delete}
    bind CoopText <Control-h>	{break}
    bind CoopText <Control-i>   {send_msg \t}
    #   bind CoopText <Control-k>  ...implementar...borrar hasta final linea
    bind CoopText <Control-k>   {break}
    bind CoopText <Control-o>	{break}
    bind CoopText <Control-t>	{break}
    bind CoopText <Control-v>	{break}
    #   ...implementar borrar palabra pero con bindings de emacs...
    bind CoopText <Control-x> {break}
    bind CoopText <Control-w> {break}
    bind CoopText <Control-y>	{break}
    
    bind CoopText <Key-Insert>  {break}
    bind CoopText <Any-Meta-KeyPress>   {break}
    bind CoopText <Any-Alt-KeyPress>    {break}
    bind CoopText <Escape>		{break}
    
    bind CoopText <KeyPress>	{send_msg %A}
    bind CoopText <Return>      {send_msg \n}
    bind CoopText <KP_Enter>    {send_msg \n}
    bind CoopText <BackSpace>   {send_backspace}
    bind CoopText <Delete>      {send_delete}
    
    bind CoopText <Key-F18>   	{cmd_paste; break}
    bind CoopText <Key-F20>   	{cmd_cut; break}
    
    bind CoopText <ButtonRelease-2> {cmd_xpaste; break}
    bind CoopText <Button-2>	{break}
    
    #--------------------------------------------
    
    pack $wroot.f.f1.file -side left
    pack $wroot.f.f1.edit -side left
    pack $wroot.f.f1.pipe -side left
    pack $wroot.f.f1      -side left
    
    #  pack $wroot.f.f2.write -side left 
    pack $wroot.f.f2       -side right
    pack $wroot.f.f2.l1 -side left
    pack $wroot.f.f2.l2 -side left
    pack $wroot.f.f2.write $wroot.f.f2.lock -side left

    pack $wroot.f.buttons  -side left -expand 1

    pack $wroot.f.filename -side left -expand 1
    
    pack $wroot.main.sb -side right -fill y
    pack $notepad  -side left -fill both -expand true
    
    pack $wroot.f     -side top -fill x
    pack $wroot.main  -side top -fill both -expand true  -padx  3 -pady 3

    #--------------------------------------------
    
    NotepadSetLabel "(no name)"    

    #--------------------------------------------
    
    NotepadUnmap
}


#===============================================================
# 
# Write permision
#
#===============================================================


process write_mode_on {} {
    global wroot

    global notepad updated writemode myuserid myapplid coorduserid myname
    global lock 

    if {!$updated} {
	# WriteTraceMsg "You must wait for coordinator's text before editing"
	tke_alert "You must wait for coordinator's text before editing"
	$wroot.f.f2.write deselect
	set writemode 0
	return 0
    }

    if {$lock} {
	tke_alert "This page is locked"
	$wroot.f.f2.write deselect
	set writemode 0
        return 0
    }

    network RDOGroup notepad "Cmd write_mode_off"
    
    $wroot.f.f2.write select
    $wroot.f.f2.l2 configure -text $myname
    set coorduserid $myuserid
    set writemode 1
    
    network RDOGroup notepad "Cmd set coorduserid $myuserid"

    network RDOGroup notepad "Cmd $wroot.f.f2.l2 configure -text [list $myname]"
    
    focus_notepad

    NotepadSetState normal

    return 1
}


process write_mode_off {} {
    global wroot

    global notepad writemode
    
    $wroot.f.f2.write deselect
    focus $wroot.f.f2
    NotepadSetState disabled
    set writemode 0
}

#===============================================================
# 
#  State
#
#===============================================================



#---------------------------------------------------------------
# NotepadSetState: normal o disable.
#---------------------------------------------------------------
process NotepadSetState {state} {
    global wroot
    global notepad
    
    $notepad config -state $state
    
    # Set buttons state
    foreach b {load cut copy paste} {
	if {[winfo exists $wroot.f.buttons.$b]} {
	    $wroot.f.buttons.$b conf -state $state
	}
    }
    
    # enable/disable cut and paste menu options
    $wroot.f.f1.edit.m entryconfigure 0 -state $state
    $wroot.f.f1.edit.m entryconfigure 2 -state $state
    
    # enable/disable load and insert file
    $wroot.f.f1.file.m entryconfigure "Load . . ." -state $state
    $wroot.f.f1.file.m entryconfigure "Insert File . . ." -state $state
    
    # enable/disable replace
    set_replace_state
    
    # enable/disable pipe command
    $wroot.f.f1.pipe config -state $state
   
    # enable/disable lock button
    $wroot.f.f2.lock config -state $state
}


#===============================================================
# 
# Floor
#
#===============================================================


process get_floor {} {
    global wroot
    global myapplid myuserid writemode

    if {$writemode} {
        # I was already the coordinator
        $wroot.f.f2.write select
        return
    }

    if {[write_mode_on]} {
	# 
	# Avisa al CoordManager del nuevo coordinador
	#
	Notepad_change_coordinator $myapplid $myuserid 0
    }
}


process put_lock {} {
    global wroot
    global lock
    set lock 1
    $wroot.f.f2.lock configure -image e_lock
    e_balloon add $wroot.f.f2.lock "Lock Floor"
}

process put_unlock {} {
    global wroot
    global lock e_balloon
    set lock 0
    $wroot.f.f2.lock configure -image e_unlock
    e_balloon add $wroot.f.f2.lock "Unlock Floor"
}

process lock_floor {} {
    global lock
    if {$lock} {
        # he quitado el cerrojo
        put_unlock
	network RDOGroup notepad "Cmd put_unlock"
    } else {
        # he puesto el cerrojo
        put_lock
	network RDOGroup notepad "Cmd put_lock"
    }
}







