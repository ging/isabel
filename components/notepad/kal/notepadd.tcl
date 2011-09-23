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
#  
#  Variables:
#
#    updated         :  my content is synchronized with the coordinator.
#
#    writemode       :  write mode
#    coorduserid     :  userid of the current coordinator
#
#    notepad          :  name of the text widget
#
#    notepad_geom     :  geometry
#    notepad_font     :  font
#
################################################################


#===============================================================
#
# Add or remove partners.
#
#===============================================================

process NotepadAddPartner {userid} {
    
}


process NotepadRemovePartner {userid} {
    global myapplid
    
    Notepad_del_user $myapplid $userid
}


#===============================================================
#
# Quit
#
#===============================================================

process NotepadQuit {} {
   
    network quit
    exit
}


#===============================================================
#
# Read the text from the coordinator.
#
#===============================================================

#---------------------------------------------------------------
# Set the coordinator to $coord and the content to $content.
#---------------------------------------------------------------
process SetContent {coord lockFloor content} {
    global coorduserid updated myapplid myuserid lock
    
    set coorduserid $coord

    set updated 1
    
    slave_edit delete 1.0 end
    slave_edit insert 1.0 $content
    slave_edit mark set insert 1.0
    focus_notepad
    if {$lockFloor} { put_lock }

    #
    # A~ade a la lista de usuarios actualizados en CoordManager
    #
    Notepad_add_sync_user $myapplid $myuserid 0
}


#---------------------------------------------------------------
# If I am the coordinator, I will update the state of user $userid
#---------------------------------------------------------------
process SendContent {userid} {
    global coorduserid myuserid notepad lock
    
    if {$coorduserid == $myuserid} {
	
	set alltext [$notepad get 1.0 "end - 1 char"]
	
	foreach _site_id [SiteManager::GetSites] {
	    if {$userid == [SiteManager::GetSiteInfo $_site_id chid]} { 
		
		network SiteRequest $_site_id COMP Notepad \
		    Cmd SetContent $myuserid $lock $alltext
		
		set font   [notepadGetFont]
		set weight [notepadGetWeight]
		set point  [notepadGetPoint]
		
		network SiteRequest $_site_id COMP Notepad \
		    Cmd cmd_change_font $font $weight $point

		return 
	    }
	}
    }
}

#---------------------------------------------------------------
# Request everybody to be updated.
# Only the coordinator will respond.
#---------------------------------------------------------------
process RequestUpdated {} {
    global updated coorduserid myuserid notepad myapplid
    
    set updated 0
    
    # 
    # Aviso al CoordManager para que me quite de lista de actualizados
    #
    Notepad_del_sync_user $myapplid $myuserid 0
    
    #echo ok
    network RDOGroup notepad "Cmd SendContent $myuserid"
}


#===============================================================
#
#  Synchronization of new partners.
#
#===============================================================


process NotepadSync userid {
    global myapplid coorduserid myuserid wroot

    if {$coorduserid == $myuserid} {
	
	# Send the text.
	SendContent $userid
	
	# Map the window if necessary.
	set state [wm state $wroot]
	if {$state == "normal"} {


	    foreach _site_id [SiteManager::GetSites] {
		if {$userid == [SiteManager::GetSiteInfo $_siteid chid]} { 
	    
		    network SiteRequest $_site_id COMP Notepad Cmd NotepadMap

		    return
		}
	    }
	}
    }
}



#===============================================================
#
# I will be the coordinator because I am the first partner.
#
#===============================================================

process StartAsCoordinator {} {
    global myapplid myuserid updated

    set updated 1
    
    #echo ok
    Notepad_set_coordinator $myapplid $myuserid 0 \
	    "network RDOGroup notepad \"Cmd notepad_notify_new_coordinator %U %P\""
    
    Notepad_add_sync_user $myapplid $myuserid 0

    write_mode_on
}

#===============================================================
#
# Main.
#
#===============================================================

process ISABEL_Notepad {} {
    global env argc argv myapplid myuserid \
	myhost notepad updated writemode myname notepad_geom \
	ISABEL_DIR coorduserid reghost lock wroot
 
    set myapplid  Notepad
    set myuserid  [SiteManager::MyChId]
    set myname    $env(ISABEL_SITE_ID)
    set reghost   [lindex $argv 6]
    
    set wroot .notepadtl

    set notepad      $wroot.main.notepad

    set coorduserid ""
    set updated     0
    set writemode   0
    set lock        0
    set myhost      [exec hostname]
    
    set _display [session Environ cget -scenary_display]

    toplevel $wroot -screen $_display

    wm protocol $wroot WM_DELETE_WINDOW "wm withdraw $wroot"

    wm title      $wroot Notepad
    wm iconname   $wroot Notepad
    wm iconbitmap $wroot @$ISABEL_DIR/lib/images/xbm/apple_write.xbm
    
    set _layout [session Activity GetResource Notepad layout +400+50]
    set notepad_geom  [dvirtual V2R_Layout $_layout]
    
    wm withdraw   .
    
    CreateInterface
    
    write_mode_off
    
    network join_group notepad
}

