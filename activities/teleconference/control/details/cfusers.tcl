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

#########################################################################
#
# cfusers.tcl
#
# User selection window.
#
# Notes:
#   The key used to identify the sites (connected or not) is their site_id.
#
#########################################################################

class TC_PartnerList {

    #------------------
    # PUBLIC INTERFACE
    #------------------

    # Create the widget in $w
    constructor {w} {}
    
    #-- Video ----------

    # Display the given labels list ($ll)  
    public method SetVideoLabels {ll}

    # Returns a ordered (1..11) site_id list for the selected videos
    # from position 1 to position $lastcol.
    # Unselected video sites returns "" in its position.
    public method GetVideoSites {{lastcol 11}} 

    # set video selection from an site_id list
    public method SetVideoSites {site_idList} 

    #-- Audio ----------

    # get audio selection (return site_id's)
    public method GetAudioSites {} 

    # Sets the Audio radiobuttons.
    public method SetAudioSites {site_idList}

    #-- Control ----------

    # get control selection (return site_id's)
    public method GetCtrlSites {} 

    # Sets the Control radiobuttons.
    public method SetCtrlSites {site_idList}

    #-------------------
    # PRIVATE INTERFACE
    #-------------------
    
    # widget where it is packed
    private variable topw

    # userid of the connected users
    private variable connected_sites

    # list with the site_id of the declared users.
    private variable declared ""

    # $userDescr($site_id): Fullname of user with the given site_id.
    private variable userDescr

    # vbutton(#) where # must be 1..11
    # The stored value is the site_id of the selected site, 
    # or 0 if the position # is not selected.
    private common vbutton

    # $lastvbutton(#) stores the last value in $vbutton(#), just
    # before calling method updateVbutton.
    private common lastvbutton

    # abutton($site_id): 1 if site_id has the audio opened.
    private common abutton
    
    # cbutton($site_id): 1 if site_id is a control site.
    private common cbutton

    # $alpha($site_id) is the only alphabetic string obtained from $site_id.
    private variable alpha

    private variable bg
    private variable fg

    #
    # Images
    #
    private common noCheckSelImg
    private common noRadioSelImg
    private common videoSelImg 
    private common audioSelImg 
    private common ctrlSelImg 


    set noCheckSelImg [image create bitmap [scope noCheckSelImg] \
	    -foreground #fff \
	    -data {
#define nocsel_width 8
#define nocsel_height 8
static unsigned char nocsel_bits[] = {
   0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7e};
    }]

    set noRadioSelImg [image create bitmap [scope noRadioSelImg] \
	    -foreground #fff \
	    -data {
#define norsel_width 8
#define norsel_height 8
static unsigned char norsel_bits[] = {
   0x18, 0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x3c, 0x18};
    }]

    set videoSelImg [image create bitmap [scope videoSelImg] \
	    -foreground red \
	    -data {
#define video_width 8
#define video_height 8
static unsigned char video_bits[] = {
   0x18, 0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x3c, 0x18};
    }]

    set audioSelImg [image create bitmap [scope audioSelImg] \
	    -foreground darkgreen \
	    -data {
#define audio_width 8
#define audio_height 8
static unsigned char audio_bits[] = {
   0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7e};
    }]

    set ctrlSelImg [image create bitmap [scope ctrlSelImg] \
	    -foreground green \
	    -data {
#define ctrl_width 8
#define ctrl_height 8
static unsigned char ctrl_bits[] = {  
   0x18, 0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x3c, 0x18};
    }]

    destructor {}

    # Makes the GUI
    private method MakeGUI {}

    # Declare a site
    private method declare {site_id descr {regrid 1}} 

    # Rebuild the grid
    private method regrid {}

    # refresh the site list.
    public method Refresh {{site_id ""}}    

    # add a site.
    private method add {site_id}    

    # remove a site.
    private method rem {site_id}    

    # Only one vbutton can be activated in the same row or column
    private method updateVbutton {pos}

    # Transform the given siteidin an only alphabetic string. 
    private method alphabeticesiteid {site_id}
}

#---------------------------------------------------------------

body TC_PartnerList::constructor {w} {

    set topw $w

    set connected_sites ""

    MakeGUI

    #regrid

    for {set i 1} {$i < 12} {incr i} {
        set vbutton($i) 0
        set lastvbutton($i) 0
    }

    Refresh
}

#---------------------------------------------------------------

body TC_PartnerList::destructor {} {

    catch {bind $topw <Destroy> ""}
    if {[winfo exists $topw]} {
        destroy $topw
    }
}

#---------------------------------------------------------------

body TC_PartnerList::MakeGUI {} {

    button .bbbtcf
    set bg [lindex [.bbbtcf config -bg] 4]
    set fg [lindex [.bbbtcf config -fg] 4]
    destroy .bbbtcf

    frame $topw \
	-relief groove \
	-borderwidth 1
    iwidgets::scrolledframe $topw.sf \
	-relief flat \
	-sbwidth 10 \
	-vscrollmode dynamic \
	-hscrollmode none

#	-labeltext "Partners" \
#	-width 20 \
#	-height 1 \
    
    pack $topw.sf -side top -expand 1 -fill both


    set topw [$topw.sf childsite]

    #--- Partners -----------
    
    frame $topw.partners 

#\
#	    -width 500 \
#	    -height 100

    pack  $topw.partners -side top -padx 0


    label $topw.partners.acr0 \
	-font [IsabelFontsC::GetFont medium medium] \
	-anchor w \
	-padx 0 -bd 0 \
	-text "Sites" 
    grid $topw.partners.acr0 \
	-sticky w \
	-row 0 \
	-column 0
    grid columnconfigure $topw.partners 0 -weight 70
    
    for {set i 1} {$i<12} {incr i} {
	label $topw.partners.l${i}_0 \
	    -font [IsabelFontsC::GetFont small medium] \
	    -bd 0 \
	    -padx 1  
	grid $topw.partners.l${i}_0 \
	    -column $i \
	    -row 0 
	grid columnconfigure $topw.partners $i -weight 1
    }

    label $topw.partners.audio \
	-font [IsabelFontsC::GetFont small medium] \
	-bd 0 \
	-padx 1 \
	-text "Au"
    balloon add $topw.partners.audio Audio
    grid $topw.partners.audio \
	-row 0 \
	-column 13 
    grid columnconfigure $topw.partners 13 -weight 20
    
    label $topw.partners.control \
	-font [IsabelFontsC::GetFont small medium] \
	-bd 0 \
	-padx 1 \
	-text "Ct"
    balloon add $topw.partners.control Control
    grid $topw.partners.control \
	    -row 0 \
	    -column 14 
    grid columnconfigure $topw.partners 14 -weight 20

    bind $topw <Destroy> "delete object $this"
}

#---------------------------------------------------------------

body TC_PartnerList::declare {site_id descr {regrid 1}} {

    set ask [alphabeticesiteid $site_id]

    set userDescr($site_id)   $descr
    set alpha($site_id) $ask

    if {[lsearch -exact $declared $site_id] == -1} {

	lappend declared $site_id

	#--- site_id ---
	canvas $topw.partners.acro$ask \
	    -width 40 \
	    -height 8
	
	$topw.partners.acro$ask create text 1 5 \
	    -text $site_id \
	    -font [IsabelFontsC::GetFont small medium] \
	    -anchor w \
	    -fill red \
	    -tag acro$ask
	
	#--- Audio ---------
	checkbutton $topw.partners.aud$ask \
	    -variable [scope abutton($site_id)] \
	    -font [IsabelFontsC::GetFont small medium] \
	    -width 14 \
	    -padx 1 -pady 0 \
	    -bd 0 \
	    -highlightthickness 0   \
	    -onvalue  1 \
	    -offvalue 0 \
	    -indicatoron 0 \
	    -image [scope noCheckSelImg] \
	    -selectimage [scope audioSelImg] \
	    -selectcolor $bg \
	    -bg $bg \
	    -activebackground $bg

	#--- Control ------------
	checkbutton $topw.partners.ctrl$ask \
	    -variable [scope cbutton($site_id)] \
	    -font [IsabelFontsC::GetFont small medium] \
	    -width 14 \
	    -padx 1 -pady 0 \
	    -bd 0 \
	    -highlightthickness 0   \
	    -onvalue  1 \
	    -offvalue 0 \
	    -indicatoron 0 \
	    -image [scope noRadioSelImg] \
	    -selectimage [scope ctrlSelImg] \
	    -selectcolor $bg \
	    -bg $bg \
	    -activebackground $bg 

	#--- Video ------------
	for {set i 1} {$i < 12} {incr i} {

	    if {$i%2 == 0} {
		#set cbg grey70
		set cbg [IsabelColorsC::GetResource background Background red]
	    } else {
		#set cbg grey60
		set cbg [IsabelColorsC::GetResource troughColor Background red]
	    } 
	    checkbutton $topw.partners.vid${i}_$ask \
		-variable [scope vbutton($i)] \
		-font [IsabelFontsC::GetFont small medium] \
		-width 14 \
		-padx 1 -pady 0 \
		-bd 0 \
		-highlightthickness 0       \
		-onvalue $site_id \
		-offvalue 0 \
		-command [code $this updateVbutton $i] \
		-indicatoron 0 \
		-image [scope noRadioSelImg] \
		-selectimage [scope videoSelImg] \
		-selectcolor $cbg \
		-activebackground $cbg \
		-bg $cbg
	}

	#--- make grid
	if {$regrid} {
	    regrid
	}
    }
    
    balloon add $topw.partners.acro$ask $descr
    
    return $ask
}

#---------------------------------------------------------------

body TC_PartnerList::regrid {} {

    set declared [lsort -dictionary $declared]
    
    set row 1
    foreach sd $declared {
	grid  $topw.partners.acro$alpha($sd) \
	    -row $row \
	    -column 0 
	
	grid $topw.partners.aud$alpha($sd) \
	    -row $row \
	    -sticky nswe \
	    -column 13
	
	grid $topw.partners.ctrl$alpha($sd) \
	    -row $row \
	    -sticky nswe \
	    -column 14
	
	for {set i 1} {$i < 12} {incr i} {
	    grid $topw.partners.vid${i}_$alpha($sd) \
		-row $row \
		-sticky nswe \
		-column $i
	}
	
	incr row
    }
}

#---------------------------------------------------------------

body TC_PartnerList::add {site_id} {

    if {[lsearch -exact $connected_sites $site_id] != -1} return

    set name    [SiteManager::GetSiteInfo $site_id name]

    set ask [declare $site_id $name 1]

#
#
#
#
#    $topw.partners.acro$ask configure -foreground $fg

    $topw.partners.acro$ask itemconfigure acro$ask -fill $fg

    lappend connected_sites $site_id
}

#---------------------------------------------------------------

body TC_PartnerList::rem {site_id} {

    if {[lsearch -exact $connected_sites $site_id] == -1} return
    
    set ask [alphabeticesiteid $site_id]
    
    if {[winfo exists $topw.partners.acro$ask]} {
	#
	#
	#       $topw.partners.acro$ask configure -foreground red
	$topw.partners.acro$ask itemconfigure acro$ask -fill red
    }
    set connected_sites [ldelete $connected_sites $site_id]
}

#---------------------------------------------------------------

body TC_PartnerList::updateVbutton {pos} {

    # He activado una casilla en la columna $pos
    if {$vbutton($pos) != "0"} {
	
	# Miro si antes estaba marcada una casilla dentro de la misma fila en 
	# la que acabo de activar, y si es asi actualizo cosas.
	for {set i 1} {$i < 12} {incr i} {
	    if {$i != $pos && $vbutton($i) == $vbutton($pos)} {
		set vbutton($i) 0
		set lastvbutton($i) 0
	    }
	}

	# Antes estaba marcada una casilla dentro de la misma columna que acabo de activar.
	if {$lastvbutton($pos) != "0"} {
	    set abutton($lastvbutton($pos)) 0
	    set cbutton($lastvbutton($pos)) 0
	}

	# Actualizo audio y ctrl.
	set abutton($vbutton($pos)) 1
	set cbutton($vbutton($pos)) 1 

	# Para la vez siguiente, indico que casilla estaba marcada en la columna.
	set lastvbutton($pos) $vbutton($pos)
    } else { 
	# Acabo de apagar todas las casillas de la columna $pos.
	
	# Desactivo el audio y control de la fila desactivada.
	set abutton($lastvbutton($pos)) 0
	set cbutton($lastvbutton($pos)) 0
	
	# Para la vez siguiente, indico que nada estaba marcado en la columna.
	set lastvbutton($pos) 0
    }
}

#---------------------------------------------------------------

body TC_PartnerList::alphabeticesiteid {siteid} {

    regsub -all {\.|:} $siteid {_} ask
    return $ask
}

#---------------------------------------------------------------

body TC_PartnerList::Refresh {{site_id ""}} {

    set all [SiteManager::GetSites] 

    if {$site_id == ""} {
	foreach _sid $all {
	    add $_sid
	}
	
	foreach _sid $connected_sites {
	    if {[lsearch -exact $all $site_id] == -1} {
		rem $site_id
	    }
	}
    } else {
	if {[lsearch -exact $all $site_id] == -1} {
	    rem $site_id
	} else {
	    add $site_id
	}
    }
}

#---------------------------------------------------------------

body TC_PartnerList::GetVideoSites {{lastcol 11}} {
    
    set res {}
    for {set i 1} {$i <= $lastcol} {incr i} {
	if {$vbutton($i) == 0} {
	    lappend res ""
	} else {
	    lappend res $vbutton($i)
	}
    }
    return $res
}

#---------------------------------------------------------------

body TC_PartnerList::SetVideoSites {site_idList} {

    for {set i 1} {$i < 12} {incr i} {
	set vbutton($i) 0
	set lastvbutton($i) 0
	set site_id [lindex $site_idList [expr $i-1]]
	set vbutton($i) $site_id
	set lastvbutton($i) $site_id
    }
}   

#---------------------------------------------------------------

body TC_PartnerList::GetAudioSites {} {

    set res {}
    foreach sid $declared {
	if {$abutton($sid) == 1} {
	    lappend res $sid
	}
    }
    return $res
}

#---------------------------------------------------------------

body TC_PartnerList::SetAudioSites {site_idList} {

    foreach sid $declared {
	if {[lsearch -exact $site_idList $sid] == -1} {
	    set abutton($sid) 0
	} else {
	    set abutton($sid) 1
	}
    }
}

#---------------------------------------------------------------

body TC_PartnerList::GetCtrlSites {} {

    set res {}
    foreach sid $declared {
	if {$cbutton($sid) == 1} {
	    lappend res $sid
	}
    }
    return $res
}

#---------------------------------------------------------------

body TC_PartnerList::SetCtrlSites {site_idList} {

    foreach sid $declared {
	if {[lsearch -exact $site_idList $sid] == -1} {
	    set cbutton($sid) 0
	} else {
	    set cbutton($sid) 1
	}
    }
}

#---------------------------------------------------------------

body TC_PartnerList::SetVideoLabels {ll} {

    set n [llength $ll]
    for {set i 11} {$i>=[expr $n+1]} {incr i -1} {
	$topw.partners.l${i}_0 configure -text ""
	catch {balloon del $topw.partners.l${i}_0}
    }

    for {set i 1} {$i<=$n} {incr i} {
	set tit_bal [lindex $ll [expr $i-1]]
	set title   [lindex $tit_bal 0]
	set balloon [lindex $tit_bal 1]
	$topw.partners.l${i}_0 configure \
	    -text "[string range $title 0 1]" \
	    -anchor w
	if {$balloon != ""} {
	    balloon add $topw.partners.l${i}_0 $balloon
	} else {
	    if {[string length $title] > 2} {
		balloon add $topw.partners.l${i}_0 $title
	    }
	}
    }
}

#---------------------------------------------------------------

