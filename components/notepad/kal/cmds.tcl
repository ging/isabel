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
#  Commands
#
################################################################



#===============================================================
#
# Auxiliary functions.
#
#===============================================================

#---------------------------------------------------------------
# NotepadSetLabel - set the label at the top of the window to $filename
#---------------------------------------------------------------
process NotepadSetLabel {filename} {
    global wroot

    
    $wroot.f.filename configure -text $filename
}


#===============================================================
#
# Commands.
#
#===============================================================


#---------------------------------------------------------------
# selection_if_any - return selection if it exists, else {}
#   this is from kjx@comp.vuw.ac.nz (R. James Noble)
#---------------------------------------------------------------
process selection_if_any {} {
  if {[catch {selection get} s]} {return ""} {return $s}
}


#---------------------------------------------------------------
# no_selection - true if there is no selection
#---------------------------------------------------------------
process no_selection {} {
  return [catch {selection get} s]
}


#---------------------------------------------------------------
# tke_pipenl command - pipe selection through command (and replace)
#   adds a newline
#---------------------------------------------------------------
process tke_pipenl { command } {
    global writemode notepad myuserid
    
    if {!$writemode} {return}
    if [no_selection] return
    
    set sel [selection_if_any]
    catch {eval exec $command << {$sel}} result
    append result "\n"
    
    set fpos [$notepad index sel.first]
    set lpos [$notepad index sel.last]
    
    $notepad delete $fpos $lpos
    $notepad insert $fpos $result

    set l [string length $result]
    $notepad tag add sel $fpos "$fpos + $l chars"

    network RDOGroup notepad "Cmd slave_edit delete $fpos $lpos"
    network RDOGroup notepad "Cmd slave_edit insert $fpos \{$result\}"
}


#---------------------------------------------------------------
# tke_pipe command - pipe selection through command (and replace)
#   does not add a newline
#---------------------------------------------------------------
process tke_pipe { command } {
    global writemode notepad myuserid
    
    if {!$writemode} return
    if [no_selection] return
    
    set sel [selection_if_any]
    catch {eval exec $command << {$sel}} result
    
    set fpos [$notepad index sel.first]
    set lpos [$notepad index sel.last]
    
    $notepad delete $fpos $lpos
    $notepad insert $fpos $result

    set l [string length $result]
    $notepad tag add sel $fpos "$fpos + $l chars"

    network RDOGroup notepad "Cmd slave_edit delete $fpos $lpos"
    network RDOGroup notepad "Cmd slave_edit insert $fpos \{$result\}"
}


#---------------------------------------------------------------
# tke_read filename - "Load..." with supplied filename
#---------------------------------------------------------------
proc tke_read { filename } { 
    global notepad

    if {![file exists $filename]} then {
	$notepad delete 1.0 end
	$notepad mark set insert 1.0
	NotepadSetLabel "$filename (new file)"
    } else {
	NotepadSetLabel $filename
	$notepad delete 1.0 end
	$notepad insert end  [exec cat $filename]
	$notepad insert end "\n"
	$notepad mark set insert 1.0
    }
    focus_notepad
}


#---------------------------------------------------------------
# cmd_save - write out a file, using $FILENAME if defined
#
# cmd_saveas - write out a file, prompting for a filename
#---------------------------------------------------------------
process save_file {filename} {
    global notepad
    
    if [catch {exec cat > $filename << [$notepad get 1.0 end]} errmsg] {
	tke_alert $errmsg
	return 0
    }
    return 1
}

process cmd_save {} {
    global FILENAME 
    global notepad
    
    if [info exists FILENAME] then {
	if [save_file $FILENAME] {
	    NotepadSetLabel $FILENAME
	}
    } else {
        set new_filename [tke_prompt "Save as:"]
        if {$new_filename != {././/CANCEL//./.} && \
		[save_file $new_filename]} then {
	    set FILENAME $new_filename
	    NotepadSetLabel  $FILENAME
        }
    }
    focus_notepad
}


process cmd_saveas {} {
    global FILENAME savedFilename
    
    if [info exists FILENAME] then {
	set labelText "Save as: $FILENAME"
        set savedFilename $FILENAME
    } else {
	set labelText "Save as:"
    }

    tke_promptdialog "$labelText" savedFilename "saveas_file" \
        "cancel_tke_promptdialog"
}

process saveas_file {fileName} {
    global savedFilename FILENAME
    if {($fileName != "") && (![file exists $savedFilename] || \
        [tke_confirm "File \"$savedFilename\" exists; replace it?"] )} then {
             if [save_file $savedFilename] {
                 set FILENAME $savedFilename
                 NotepadSetLabel $FILENAME
             }
    }
    focus_notepad
}



#---------------------------------------------------------------
# cmd_load - read in a file
#---------------------------------------------------------------

process tke_promptdialog {msg textVariable cmdWhenOK cmdWhenCancel} {
    global wroot
    global $textVariable

    set w $wroot.promptdialog
    
    ::iwidgets::promptdialog $w -modality application -labeltext \
       $msg -title $msg

    set prompt [$w component prompt]
    $prompt configure -textvariable $textVariable -width 25 \
        -command load_file

    $w buttonconfigure "OK" -command "promptdialog_OK_pressed $cmdWhenOK"
    
    $w buttonconfigure "Apply" -text "Browse" -command \
        "create_browse_window $textVariable"
 
    $w buttonconfigure "Cancel" -command \
        "promptdialog_Cancel_pressed $cmdWhenCancel"

    $w activate
}

process promptdialog_OK_pressed {cmdWhenOK} {
    global wroot

    set file [$wroot.promptdialog get]
    catch {destroy $wroot.promptdialog}
    $cmdWhenOK $file
}

process promptdialog_Cancel_pressed {cmdWhenCancel} {
    global wroot

    catch {destroy $wroot.promptdialog}
    $cmdWhenCancel
}

process create_browse_window {textVariable} {
    global wroot

    set browseWin $wroot.promptdialog.browse
    ::iwidgets::fileselectiondialog $browseWin 
    $browseWin buttonconfigure OK -command \
        "select_file_prompt_dialog $textVariable"
    $browseWin activate
}


process select_file_prompt_dialog {textVariable} {
    global wroot

    global $textVariable
    set $textVariable [$wroot.promptdialog.browse get]
    catch {destroy $wroot.promptdialog.browse}
}

process cmd_load {} {
    global writemode 
    
    if {!$writemode} {return}

    tke_promptdialog "Load file..." FILENAME "load_file" \
        "cancel_tke_promptdialog"
}

process load_file {fileName} {
    global FILENAME notepad
    if {$fileName != ""} {
        tke_read $FILENAME
        network RDOGroup notepad "Cmd RequestUpdated"
    }
    focus_notepad
}


process cancel_tke_promptdialog {} {
    focus_notepad
}

#---------------------------------------------------------------
# cmd_print - print the file using lpr
#---------------------------------------------------------------
process cmd_print {} {
    global notepad env
    
    set PRINTER lp
    catch {set PRINTER $env(PRINTER)}
    
    set prname [tke_prompt "Printer Name:" $PRINTER]
    if {$prname != {././/CANCEL//./.}} then {
        if [catch {exec lpr -P$prname << [$notepad get 1.0 end]} errmsg] {
	    tke_alert $errmsg
        } else {
	    set PRINTER $prname
	}
    }
    focus_notepad
}


#---------------------------------------------------------------
# cmd_insfile - read in a file and insert it at the insert mark
#---------------------------------------------------------------
process cmd_insfile {} {
    global writemode insertedFilename
    
    if {!$writemode} {return}
    
    tke_promptdialog "Insert file..." insertedFilename "insert_file" \
        "cancel_tke_promptdialog"
}

process insert_file {fileName} {
    global notepad insertedFilename
    if {$fileName != ""} {
        if [catch {set ins_text [exec cat $insertedFilename]} errmsg] {
            tke_alert $errmsg
        } else {
            $notepad insert insert $ins_text
            network RDOGroup notepad "Cmd RequestUpdated"
        }
    }
    focus_notepad
}

#---------------------------------------------------------------
# Copy selection
#---------------------------------------------------------------
process cmd_copy {} {
    global notepad writemode
    
    if {(!$writemode) || ([no_selection])} {return}
    if {[selection own -displayof $notepad] == "$notepad"} {
        clipboard clear -displayof $notepad
        clipboard append -displayof $notepad [selection get -displayof $notepad]
    }
}



#---------------------------------------------------------------
# Cut selection
#---------------------------------------------------------------
process cmd_cut {} {
    global notepad writemode
    
    if {(!$writemode) || ([no_selection])} {return}
    if {[selection own -displayof $notepad] == "$notepad"} {
        clipboard clear -displayof $notepad
        clipboard append -displayof $notepad [selection get -displayof $notepad]
        set first [$notepad index sel.first]
        set last  [$notepad index sel.last]
        $notepad delete $first $last
        network RDOGroup notepad "Cmd slave_edit delete $first $last"
    }
}


#---------------------------------------------------------------
# Paste selection
#---------------------------------------------------------------
process cmd_paste {} {
    global writemode notepad
    
    if {!$writemode} {return}
    set clipb ""
    catch {set clipb [selection get -displayof $notepad -selection CLIPBOARD]}
    if {$clipb != ""} {
	send_msg $clipb
	$notepad insert insert $clipb
    }
}



#---------------------------------------------------------------
# Paste X selection
#---------------------------------------------------------------
process cmd_xpaste {} {
    global writemode notepad
    
    if {!$writemode} {return}
    if {[set s [selection_if_any]] != ""} {
	send_msg $s
	$notepad insert insert $s
    }
}


#---------------------------------------------------------------
# cmd_select_all - mark the entire text as selected
#---------------------------------------------------------------
process cmd_select_all {} {
    global notepad
    
    $notepad tag add sel 1.0 end 
}


#---------------------------------------------------------------
# cmd_find - non-modal search-and-replace panel
#---------------------------------------------------------------
process set_replace_state {} {
    global wroot
    global writemode 
    
    if {![winfo exists $wroot.find]} {return}

    button $wroot.tmpb
    set normal_foreground_color   [lindex [$wroot.tmpb config -fg] 4]
    set disabled_foreground_color [lindex [$wroot.tmpb config -disabledforeground] 4]
    destroy $wroot.tmpb
    
    if {$writemode} {
	set fgcolor $normal_foreground_color
	set state normal
    } else {
	set fgcolor $disabled_foreground_color
	set state disabled
    }
    $wroot.find.t.replace.l config -fg $fgcolor
    $wroot.find.t.replace.e config -state $state -fg $fgcolor
    $wroot.find.b.replace config -state $state
}


process cmd_find {} {
    global wroot
    global notepad normalbg
    
    global SEARCHFOR			;# so it can be default
    append SEARCHFOR {}			;# make sure it's defined
    global REPLACEWITH			;# so it can be default
    append REPLACEWITH {}		;# make sure it's defined
    global FINDISACTIVE			;# only one active at a time
    
    if [winfo exists $wroot.find] {
	wm deiconify $wroot.find
	raise $wroot.find
	windowpos $wroot.find
	set_replace_state
	return 0
    }
    
    toplevel $wroot.find
    wm title $wroot.find "Find Panel"
    frame $wroot.find.t
    frame $wroot.find.t.search
    label $wroot.find.t.search.l -text "Search for:" -width 16 -anchor e \
	    -bg $normalbg
    entry $wroot.find.t.search.e -relief sunken -width 40
    frame $wroot.find.t.replace
    label $wroot.find.t.replace.l -text "Replace with:" -width 16 -anchor e \
	    -bg $normalbg
    entry $wroot.find.t.replace.e -relief sunken -width 40
    frame $wroot.find.b
    frame $wroot.find.b.r -height 2 -width 200 -borderwidth 1 -relief sunken
    
    button $wroot.find.b.search -text Search -width 8 -command {
	tke_destroy_alert
        tke_find [set SEARCHFOR [$wroot.find.t.search.e get]]
    }
    
    button $wroot.find.b.replace -text Replace -width 8 -command {
	tke_destroy_alert
        set SEARCHFOR [$wroot.find.t.search.e get]
	
        if {[catch {$notepad index sel.first}]} {
	    tke_find $SEARCHFOR
        } else {
      	    tke_replace [set REPLACEWITH [$wroot.find.t.replace.e get]]
      	    tke_find $SEARCHFOR
        }
    }
    button $wroot.find.b.cancel -text Cancel -width 8 -command {
	tke_destroy_alert
        set REPLACEWITH [$wroot.find.t.replace.e get]
        set SEARCHFOR [$wroot.find.t.search.e get]
        focus_notepad
        catch {destroy $wroot.find}
    }
    
    # return in search field searches and closes panel:
    bind $wroot.find.t.search.e <Key-Return> \
	    {focus $wroot.find.t.replace.e}
    
    # return in replace field searches but doesn't close panel:
    bind $wroot.find.t.replace.e <Key-Return> \
	    {$wroot.find.b.replace invoke}
    
    # tab switches fields:
    bind $wroot.find.t.search.e <Key-Tab> \
	    {focus $wroot.find.t.replace.e}
    bind $wroot.find.t.replace.e <Key-Tab> \
	    {focus $wroot.find.t.search.e}
    
    # Control-C, Meta-C, and Meta-period cancel:
    bind $wroot.find.t.search.e <Control-c> {$wroot.find.b.cancel invoke}
    bind $wroot.find.t.search.e <Meta-c> {$wroot.find.b.cancel invoke}
    bind $wroot.find.t.search.e <Meta-period> {$wroot.find.b.cancel invoke}
    bind $wroot.find.t.replace.e <Control-c> {$wroot.find.b.cancel invoke}
    bind $wroot.find.t.replace.e <Meta-c> {$wroot.find.b.cancel invoke}
    bind $wroot.find.t.replace.e <Meta-period> {$wroot.find.b.cancel invoke}
    
    $wroot.find.t.search.e delete 0 end
    $wroot.find.t.search.e insert end $SEARCHFOR
    $wroot.find.t.replace.e delete 0 end
    $wroot.find.t.replace.e insert end $REPLACEWITH
    
    set_replace_state
    
    pack $wroot.find.t.search.l .find.t.search.e -side left -fill both
    pack $wroot.find.t.replace.l .find.t.replace.e -side left -fill both
    pack $wroot.find.t.search .find.t.replace -side top -expand true -pady 5
    pack $wroot.find.b.r -side top -fill x
    pack $wroot.find.b.cancel .find.b.replace .find.b.search \
	    -side right -pady 10 -padx 10
    pack $wroot.find.t -side top -fill both -padx 10 -pady 5
    pack $wroot.find.b -side bottom -fill x
    
    windowpos $wroot.find
    
    focus $wroot.find.t.search.e
    
}


#---------------------------------------------------------------
# tke_find string - find and select the string
#
# WARNING: since this takes a copy of the file, it could use a LOT 
# of memory!  
# should be rewritten to use a different mark than insert.
#---------------------------------------------------------------
process tke_find { string } {
    global notepad
    
    set foundpos [string first $string [$notepad get insert end]]
    
    if {$foundpos == -1} then {
	tke_alert "Not found. Wrapping search."
	$notepad mark set insert 1.0
	return 0
    }
    # deselect any already-selected text:
    catch {$notepad tag remove sel sel.first sel.last}
    # select the range that matched:
    set lastpos [expr {$foundpos + [string length $string]}]
    $notepad tag add sel "insert + $foundpos chars" "insert + $lastpos chars"
    # move insert just after the match (so we can continue from there)
    $notepad mark set insert "insert + $lastpos chars"
    $notepad see insert
}


#---------------------------------------------------------------
# tke_replace string - replace the selection with the string
#---------------------------------------------------------------
process tke_replace { string } {
    global notepad
    
    if {[catch {$notepad index sel.first}]} {
	tke_alert "Text to replace must be selected first"
    } else { 
	set first [$notepad index sel.first]
	set last  [$notepad index sel.last]
	$notepad delete $first $last
	$notepad insert $first $string
	$notepad mark set insert $first
	network RDOGroup notepad "Cmd slave_edit delete $first $last"
	network RDOGroup notepad "Cmd slave_edit insert $first $string"
    }
}


#---------------------------------------------------------------
# cmd_change_font - change font
#---------------------------------------------------------------
process notepadGetFont {} {
    global wroot

    return [$wroot.f.buttons.font get]
}

process notepadGetWeight {} {
    global wroot

    return [$wroot.f.buttons.weight get]
}

process notepadGetPoint {} {
    global wroot

    set val [$wroot.f.buttons.points get]
    set valLen [string length $val]
    return [string range $val 0 [expr $valLen-2]]
}

process change_font {} {
    global notepad
    set font [notepadGetFont]
    set weight [notepadGetWeight]
    set points [notepadGetPoint]
    cmd_change_font $font $weight $points
    network RDOGroup notepad "Cmd cmd_change_font $font $weight $points"
}

process cmd_change_font {font weight points} {
    global wroot
    global notepad

    $wroot.f.buttons.font select $font
    $wroot.f.buttons.weight select $weight
    $wroot.f.buttons.points select ${points}p
    $notepad configure -font \
        "-*-$font-$weight-R-Normal--*-[expr $points * 10]-*-*-*-*-*-*"
}

