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
############## Description #####################################
#
#   Balloon class.
#
############## Summary of Data Structures ######################
#
############## External Functions ##############################
#
#
################################################################



class BalloonClass {

    private common widgetname ""
    private common thislist ""
    private common timer   500
    private common msgs
    private common afterid

    private common contdisplays 0
    private common tl_balloon_name

    constructor {} {}

    destructor {}

    # assigns the message $msg to the widget $w
    public method add {w msg}

    # deletes ballon associated to widget $w
    public method del {w}

    # returns the message associated to the widget $w
    public method get {w}

    private method show {w}
    private method hide {w}


    # Starts waiting to pop up a balloon id
    private method startDelay {w}

    # Stops the timer for a balloon popup if one is in progress,
    # else if there is already a balloon window up it is hidden.
    private method stopDelay  {w}
}



body BalloonClass::constructor {} {

    lappend thislist $this

return

    if {$widgetname != ""} return

    set widgetname \
	    [menu .isa_userviews_help$this \
	    -tearoff no \
	    -foreground black \
	    -background yellow \
	    -borderwidth 1 \
	    -activeforeground black \
	    -activebackground yellow \
	    -activeborderwidth 0 \
		 -font [IsabelFontsC::GetFont small medium] ]
    $widgetname add command
}
    

body BalloonClass::destructor {} {

    set pos [lsearch -exact $thislist $this]
    set thislist [lreplace $thislist $pos $pos]

    if {$thislist == ""} {
	foreach tl [array names tl_balloon_name] {
	    destroy $tl
	}
	
	foreach w [array names msgs] {
	    del $w
	}
    } 
}


body BalloonClass::add {w msg} {

    set screen [winfo screen $w]
    if {![info exists tl_balloon_name($screen)]} {
	set afterid($screen) 0
	incr contdisplays
	set tl_balloon_name($screen) .tl_balloon_$contdisplays
	toplevel $tl_balloon_name($screen) -screen $screen
	wm withdraw $tl_balloon_name($screen)

	menu $tl_balloon_name($screen).menu \
		-tearoff no \
		-foreground black \
		-background yellow \
		-borderwidth 1 \
		-activeforeground black \
		-activebackground yellow \
		-activeborderwidth 0 \
		-font [IsabelFontsC::GetFont small medium]
	$tl_balloon_name($screen).menu add command
    }
    if 0 {
	if {[lsearch -exact [array names msgs] $w] == -1} {
	    bind $w <Enter>    "+[code $this startDelay %W]"
	    bind $w <Leave>    "+[code $this stopDelay %W]"
	    bind $w <Button-1> "+[code $this stopDelay %W]"
	}
    }

    del $w
    bind $w <Enter>    "+[code $this startDelay %W]"
    bind $w <Leave>    "+[code $this stopDelay %W]"
    bind $w <Button-1> "+[code $this stopDelay %W]"
    
    set msgs($w) $msg
}


body BalloonClass::del {w} {

    if {[lsearch -exact [array names msgs] $w] != -1} {
	stopDelay $w
	unset msgs($w)

	set bl [bind $w <Enter>]
	regsub [code $this startDelay %W]      $bl "" bl
	bind $w <Enter>    $bl
	
	set bl [bind $w <Leave>]
	regsub [code $this stopDelay %W] $bl "" bl
	bind $w <Leave>    $bl
	
	set bl [bind $w <Button-1>]
	regsub [code $this stopDelay %W]  $bl "" bl
	bind $w <Button-1> $bl
    }
}


body BalloonClass::get {w} {

    if {[lsearch -exact [array names msgs] $w] != -1} {
	return $msgs($w)
    }
    return ""
}


body BalloonClass::show {w} {

    set screen [winfo screen $w]

    set afterid($screen) 0
    # Are we still inside the window?
    set px [winfo pointerx $tl_balloon_name($screen)]
    set py [winfo pointery $tl_balloon_name($screen)]
    set mouseWindow [winfo containing -displayof $tl_balloon_name($screen) $px $py]

    # Is in button?
    if {! [string match $w* $mouseWindow] } return

    $tl_balloon_name($screen).menu entryconfigure 0 -label $msgs($w)
    
    # Coordinates of the balloon
    # set left [expr [winfo rootx $w] + round(([winfo width $w]/2.0))]
    set left [expr $px+10]
    # set top  [expr [winfo rooty $w] + [winfo height $w]]
    set top  [expr $py-22]
    
    # put up balloon window
    $tl_balloon_name($screen).menu post $left $top
}


body BalloonClass::hide {w} {
    set screen [winfo screen $w]
    $tl_balloon_name($screen).menu unpost
}


body BalloonClass::startDelay {w} {
    set screen [winfo screen $w]
    set afterid($screen) [after $timer [code $this show $w]]
}


body BalloonClass::stopDelay {w} {
    
    set screen [winfo screen $w]
    if { $afterid($screen) != 0 } {
	after cancel $afterid($screen)
	set afterid($screen) 0
    } else {
	hide $w
    }    
}







