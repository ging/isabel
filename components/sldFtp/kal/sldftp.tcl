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

##############################################################
#
#   Description:
#     slide ftper
#
################################################################


#===============================================================
#
# SldFtpSenderClass
#
#===============================================================


class SldFtpSenderClass {
    
    # Sites added to the component
    private variable sites_added ""

    # SITE_ID of the added or defined sites
    private variable defined_sites ""

    # ftp sender task name.
    private variable ftpTask -1

    # list of the slide filenames to send.
    private variable filenames ""

    # maximun time waiting for ACK's
    private common timeout

    # maximun ftp bw
    private variable maxbw 500

    # BW factor to apply to the video component while sending slides.
    private variable videobwfactor 0.5

    # Length of filenames list.
    private common total 0

    # Number of the current slide.
    private common current 0

    # Message to display in the state bar.
    private common state idle

    # Must send or not slides to $to($siteid).
    private common to 
    
    # list with the sites selected to send slides.
    private variable to_marked ""

    # auto select new sites.
    private common autoselect 1

    # received($siteid) indicates if this site has receiced the current slide.
    private common received

    # bad($siteid) indicates the number of failed transmitions.
    private common bad

    # ok($siteid) indicates the number of successful transmitions.
    private common ok

    # name of the browser object.
    private variable browser ""

    # name of the control window.
    private variable guiname .tSldFtp

    # Site sending slides now.
    # In the stopped state, sender is "".
    private variable sender ""

    # this counter is used to identify each file transmition with a different
    # value, and it is also used in the ack, nack, done and timeouts callbacks 
    # in order to match transmittions and callbacks.
    private common contID 0

    private variable flag_synced 0
    private variable first_site  0
    private variable created 0

    constructor {} {}

    destructor  {} 

    public method SetCreated {}
    public method Created {}

    public method FirstSite {}

    public method AddSite {siteid new}
    public method DelSite {siteid}

    public method Sync {_sender _maxbw _videobwfactor}

    # Create my GUI, or open it if it already exists.
    private method CreateGUI {}

    # Toggles the state of the gui.
    public method ToggleGUI {}

    private method SetWidgetsSendingMode {}    
    private method SetWidgetsStoppedMode {}    

    # Select all the sites.
    private method AutoSelect {} 

    #----

    # Send a message to all sites informing that I want to send new files. 
    public method RequestSendSlides {} 

    # Site $_sender is going to send slides
    public method SendSlides {_sender} 
    
    #----

    # Send a Stop command to all sites.
    # A state line message is given as parameter.
    public method RequestStop {msg} 

    # Destroy the FTP tasks, and set the widget states into the stopped mode.
    # Also, if I am the sender site, then I reset the video bw factor.
    public method Stop {msg} 

    #----

    private method SetTimeout {} 

    public method SetMaxBW {bw} 
    public method SetVideoBWFactor {f} 
    private method ApplyVideoBWFactor {} 
    private method ResetVideoBWFactor {} 
    
    private method GetToSitesList {}
    
    private method SetState {w m}
    private method SendNext {}
    
    private method ACK     {cid u}
    private method NACK    {cid siteid {msg ""}}
    private method TIMEOUT {cid}
    private method DONE    {cid}

    private method TooMuchBig {f}
    
    #--- remote check procedure --------
    #
    # This procedure is invoked in the receiver site foreach slide to
    # send. 
    # This procedure will see if I can write a file in the target directory. 
    # I can write it if the directory contains remote files, i.e.
    # files received previously using sldftp. I know that a directory
    # contains remote files because there exists a file called .remote.
    # It the target directory doesn't exist, it can be created and marked
    # with the .remote file.
    # I cann't write in a directory if it doesn't contains remote files, i.e.
    # the .remote file doesn't exist.
    #
    # There is an activity sldftp resource, called forcewrite, which value is a
    # list of site roles, that indicates which sites can write in no-remote
    # directories (without the .remote file). This resource should be assigned
    # to the sites that distributes slides to everybody (coordinators).
    # This resource determine the value of the force parameter.
    #-----------------------------------
    public proc RemoteCheck {sld crc force}

    public method Nop {} {}
}


#---------------------------------------------------------------

body SldFtpSenderClass::constructor {} {

    CreateGUI
}

#---------------------------------------------------------------

body SldFtpSenderClass::destructor {} {

    catch {destroy $guiname}
    
    if {$ftpTask != -1} {
	network RDOLocalGroup local_isabel_comp_Ftp "Eval delete object $ftpTask"
    }
    
    if {$browser != ""} {
	catch {delete object $browser}
    }
}

#--------------------------------------------------------------

body SldFtpSenderClass::SetCreated {} {
    
    set created 1
}

#--------------------------------------------------------------

body SldFtpSenderClass::Created {} {
    
    return $created
}

#--------------------------------------------------------------

body SldFtpSenderClass::FirstSite {} {

    set flag_synced 1
    set first_site  1
	
}

#--------------------------------------------------------------

body SldFtpSenderClass::Sync {_sender _maxbw _videobwfactor} {

    if {$flag_synced} return

    set sender $_sender
    if {$sender != ""} {
	SetWidgetsSendingMode
    } else {
	SetWidgetsStoppedMode
    }
    SetMaxBW         $_maxbw
    SetVideoBWFactor $_videobwfactor
    
    set flag_synced 1

    set state synchronized
}

#--------------------------------------------------------------

body SldFtpSenderClass::SetMaxBW {bw} {

    set maxbw $bw

    $guiname.others.maxbw conf -text "BW = $maxbw kbps"

    if {$ftpTask != -1} {
	network RDOLocalGroup local_isabel_comp_Ftp "Eval $ftpTask setBW $bw"
    }
}

#--------------------------------------------------------------

body SldFtpSenderClass::SetVideoBWFactor {f} {

    set videobwfactor $f

    if {$sender == [SiteManager::MySiteId]} {
	ApplyVideoBWFactor
    }
}

#--------------------------------------------------------------

body SldFtpSenderClass::ApplyVideoBWFactor {} {

    sms forceState Video_Cameras_factors add -bw_factor $videobwfactor
    sms forceState Video_Vga_factors     add -bw_factor $videobwfactor
}


#--------------------------------------------------------------

body SldFtpSenderClass::ResetVideoBWFactor {} {

    sms forceState Video_Cameras_factors add -bw_factor 1
    sms forceState Video_Vga_factors     add -bw_factor 1
}

#---------------------------------------------------------------

body SldFtpSenderClass::CreateGUI {} {
    global ISABEL_DIR env

    if {[winfo exists $guiname]} {
	destroy $guiname
    }    

    toplevel $guiname -screen [session Environ cget -ctrl_display]

    wm protocol $guiname WM_DELETE_WINDOW "wm withdraw $guiname"
    
    wm geometry $guiname [session Activity GetResource SldFtp geometry]
    
    wm title        $guiname "Slides Ftp"
    wm iconname     $guiname "Slides Ftp"
    wm iconbitmap   $guiname @$ISABEL_DIR/lib/images/xbm/slides2.xbm
    
    #---------------------------------------------------
    # Frame for the status line.
    #---------------------------------------------------
    
    frame $guiname.last
    pack  $guiname.last -side bottom -fill x
    
    label  $guiname.last.state \
	-textvariable [scope state] \
	-anchor w \
	-relief sunken
    
    label  $guiname.last.total \
	-textvariable [scope total] \
	-width 4 \
	-relief sunken \
	-anchor c
    
    label  $guiname.last.current \
	-textvariable [scope current] \
	-width 4 \
	-relief sunken \
	-anchor c

    grid columnconfigure $guiname.last 0 -weight 1

    grid $guiname.last.state \
	-row 0 -column 0 \
	-padx 3 \
	-pady 3 \
	-sticky we
    grid $guiname.last.current \
	-row 0 -column 1 \
	-padx 3 \
	-pady 3 \
	-sticky we
    grid $guiname.last.total \
	-row 0 -column 2 \
	-padx 3 \
	-pady 3 \
	-sticky we
    
    #---------------------------------------------------
    # Frame to select destination partners
    #---------------------------------------------------
    
    frame $guiname.to -relief groove -borderwidth 3
    pack  $guiname.to -side right -padx 4 -pady 4 \
	-ipadx 2 -ipady 2 \
	-expand 0 -fill both
    
    label $guiname.to.title -text "Send To:"
    pack  $guiname.to.title  -side top -fill x -padx 5


    checkbutton $guiname.to.auto \
	-text "Auto select" \
	-variable [scope autoselect] \
	-onvalue 1 \
	-offvalue 0 \
	-command [code $this AutoSelect]
    pack   $guiname.to.auto -side bottom 


    frame $guiname.to.sites 
    pack $guiname.to.sites -side top -expand 0 -fill x
    
    
    #---------------------------------------------------
    # Frame for the commands.
    #---------------------------------------------------
    
    frame $guiname.others -relief flat
    pack  $guiname.others -side right -expand 0 -fill both
    
    button $guiname.others.send \
	-relief raised \
	-text Send \
	-anchor c \
	-bd 6 \
	-command "$this RequestSendSlides"
    pack   $guiname.others.send \
	-side top \
	-expand 1 \
	-fill x \
	-pady 1 \
	-padx 5
    
    global env
    button $guiname.others.abort \
	-relief raised \
	-text Abort \
	-anchor c \
	-bd 6 \
	-command "$this RequestStop \"Aborted by $env(ISABEL_SITE_ID).\""
    pack   $guiname.others.abort \
	-side top \
	-expand 1 \
	-fill x \
	-pady 1 \
	-padx 5
    
    ::iwidgets::optionmenu $guiname.others.timeout \
	-labelpos nw \
	-labeltext "Timeout:" \
	-width 140 \
	-command [code $this SetTimeout]
    
    $guiname.others.timeout insert end "5 seconds" "10 seconds" "30 seconds" "2 minutes" \
	"10 minutes" "30 minutes"  "1 hour" "10 hours"
    
    $guiname.others.timeout select 2
    SetTimeout
    
    pack   $guiname.others.timeout \
	-side top \
	-expand 1 \
	-fill x \
	-pady 1 \
	-padx 5
    
    label $guiname.others.maxbw -text "BW = $maxbw bkps"
    pack   $guiname.others.maxbw \
	-side top \
	-expand 1 \
	-fill x \
	-pady 1 \
	-padx 5
    

    #---------------------------------------------------
    # Explorer
    #---------------------------------------------------
    
    if {$browser != ""} {
	catch {delete object $browser}
    }
    
    set browser [SldBrowserClass "#auto" $guiname \
		     -root $env(ISABEL_WORKDIR)/pres \
		     -title "Select Slides:"]
    
    #---------------------------------------------------
    
    wm withdraw $guiname
}  

#---------------------------------------------------------------

body SldFtpSenderClass::SetWidgetsSendingMode {} {

    SetState $guiname.to.sites  disabled
    $guiname.others.send  conf -state disabled
    $browser setState disabled
}

#---------------------------------------------------------------

body SldFtpSenderClass::SetWidgetsStoppedMode {} {

    SetState $guiname.to.sites normal
    catch {
	$guiname.others.send  conf -state  normal
    }
    $browser setState normal
}

#---------------------------------------------------------------

body SldFtpSenderClass::ToggleGUI {} {
    
    if {[wm state $guiname] == "normal"} {
	wm withdraw $guiname
    } else {
	wm deiconify $guiname
    }
}


#---------------------------------------------------------------
# SldFtpSenderClass::AutoSelect 
#
#---------------------------------------------------------------
body SldFtpSenderClass::AutoSelect {} {
    
    if {$autoselect} {
	foreach siteid $sites_added {
	    set to($siteid) 1
	}
    }
}


#---------------------------------------------------------------
# SldFtpSenderClass::SendSlides
#
# Start the transmition of the slides.
#---------------------------------------------------------------
body SldFtpSenderClass::RequestSendSlides {} {

    network MasterRequest NET SiteBroadcast COMP SldFtp Cmd SendSlides [SiteManager::MySiteId]
}

#---------------------------------------------------------------

body SldFtpSenderClass::SendSlides {_sender} {

    if {$sender != ""} {
	return
    } else {

	SetWidgetsSendingMode
	
	set sender $_sender

	set state "[SiteManager::GetSiteInfo $sender name] is sending slides" 

	if {$sender == [SiteManager::MySiteId]} {

	    set filenames [$browser getAllSlds]
	    set total     [llength $filenames]
	    set current   -1
	    
	    foreach s $sites_added {
		set received($s) 0
		set ok($s)       0
		set bad($s)      0
	    }
	    
	    set state "Sending slides."
	    
	    # get initially selected sites.
	    set to_marked [GetToSitesList]

	    if {$to_marked == ""} {
		ErrorTraceMsg "SldFtp: Select destination sites."
		RequestStop "No destination sites selected."
		return 
	    }
	    
	    if {$filenames == ""} {
		ErrorTraceMsg "SldFtp: Select slides to distribute."
		RequestStop "No slides selected to distribute."
		return
	    }
	    
	    ApplyVideoBWFactor
	    
	    SendNext
	}
    }
}


#---------------------------------------------------------------
# SldFtpSenderClass::SendNext
#
# Send the next slide.
#---------------------------------------------------------------
body SldFtpSenderClass::SendNext {} {
    global env

    if {$sender == ""} {
	return
    }

    incr current
    set f [lindex $filenames $current]
    
    # update to_marked sites
    set to_marked [GetToSitesList]
    
    if {$to_marked == "" || $f == ""} {

	RequestStop done
	
	return
    }
    
    set state "Sending $f"
    
    if {[string range $f 0 0] != "/"} {set f /$f}
    
    foreach s $to_marked {
	set received($s) 0
    }

    if 0 {
	if {![catch {file size $env(ISABEL_WORKDIR)/pres/$f} res]} {
	    if {$res > 17000000} {
		TooMuchBig $f
		return
	    }
	}
    }

    incr contID
    
    set ackcb     "Eval [code $this ACK $contID]"
    set nackcb    "Eval [code $this NACK $contID]"
    set donecb    "Eval [code $this DONE $contID]"
    set timeoutcb "Eval [code $this TIMEOUT $contID]"

    set force 0
    set forceroles [session Activity GetResource SldFtp forcewrite ""]
    foreach fr $forceroles {
	if {[lsearch -exact [SiteManager::GetSiteInfo [SiteManager::MySiteId] roles] $fr] != -1} {
	    set force 1
	    break
	}
    }
    set crc [exec isabel_sum $env(ISABEL_WORKDIR)/pres/$f]
    set ftpTask [network RPCLocalGroup local_isabel_comp_Ftp "Eval \
		     FtpSendFile \
                     [list $env(ISABEL_WORKDIR)] \
                     [list pres/$f] \
                     ISABEL_WORKDIR \
                     [list pres/$f] \
		     [list $to_marked] \
                     local_isabel_comp_SldFtp \
                     $timeout \
		     [list $ackcb] \
                     [list $nackcb] \
                     [list $donecb] \
                     [list $timeoutcb] \
		     $maxbw \
		     [list "RemoteCheck pres/$f $crc $force"]"]
    if {$ftpTask == -1} {
	set state "Skiping $f"
	ErrorTraceMsg "SldFtp: can not start sender task."
	foreach siteid $to_marked {
	    incr bad($siteid)
	}
	SendNext
    } 
} 


#---------------------------------------------------------------
# SldFtpSenderClass::TooMuchBig
#
# File can't be sent because is too much big.
#---------------------------------------------------------------
body SldFtpSenderClass::TooMuchBig {f} {

    ErrorTraceMsg "SldFtp: I can't send slide \"...$f\". It is too big."

    ErrorMsgC "#auto" close -timeout 10000  \
	"SldFtp:" \
	"I can't send slide \"...$f\"." \
	"It is too big."
    
    foreach s $to_marked {
	if {$received($s) == 0} {
	    incr bad($s)
	}
    }
    SendNext
}


#---------------------------------------------------------------
# SldFtpSenderClass::SetTimeout
#
# Set the timeout value.
#---------------------------------------------------------------
body SldFtpSenderClass::SetTimeout  {} {

    set t [$guiname.others.timeout get]
    switch -- $t {
	"5 seconds" {
	    set timeout 5000
	}
	"10 seconds" {
	    set timeout 10000
	}
	"30 seconds" {
	    set timeout 30000
	}
	"2 minutes" {
	    set timeout 120000
	}
	"10 minutes" {
	    set timeout 600000
	}
	"30 minutes" {
	    set timeout 1800000
	}
	"1 hour" {
	    set timeout 3600000
	}
	"10 hours" {
	    set timeout 36000000
	}
    }

    if {$ftpTask != -1} {
	network RDOLocalGroup local_isabel_comp_Ftp "Eval $ftpTask setLiveTime $timeout"
    }

}


#---------------------------------------------------------------
# SldFtpSenderClass::ACK
#
# Callback to execute when siteid receives the current file.
#---------------------------------------------------------------
body SldFtpSenderClass::ACK  {cid siteid} {

    if {$cid != $contID} return
    
    DebugTraceMsg "- SldFtp: ACK from [SiteManager::GetSiteInfo $siteid name]" 2
    set received($siteid) 1
    incr ok($siteid)
}


#---------------------------------------------------------------
# SldFtpSenderClass::NACK
#
# Callback to execute when siteid can not receive the current file.
#---------------------------------------------------------------
body SldFtpSenderClass::NACK {cid siteid {msg ""}} {

    if {$cid != $contID} return
    
    set u [SiteManager::GetSiteInfo $siteid name]
    ErrorTraceMsg "SldFtp: $u can't receive slide:\n$msg."
    set received($siteid) 1
    incr bad($siteid)
}


#---------------------------------------------------------------
# SldFtpSenderClass::TIMEOUT
#
# Timeout expiration. Somebody has not sent ack or nack.
#---------------------------------------------------------------
body SldFtpSenderClass::TIMEOUT {cid} {

    if {$cid != $contID} return
    
    foreach s $to_marked {
	if {$received($s) == 0} {
	    set d [SiteManager::GetSiteInfo $s name]
	    set msg "timeout expired"
	    ErrorTraceMsg "SldFtp: $d can't receive slide:\n$msg."
	    incr bad($s)
	}
    }
    set state "done"
    network RDOLocalGroup local_isabel_comp_Ftp "Eval delete object $ftpTask"
    set ftpTask  -1
    SendNext
}

#---------------------------------------------------------------
# SldFtpSenderClass::DONE
#
# Evebody has sent ack or nack, so let go with the next slide.
#---------------------------------------------------------------
body SldFtpSenderClass::DONE {cid} {

    if {$cid != $contID} return

    set state "done"
    set ftpTask  -1
    SendNext
}

#---------------------------------------------------------------
body SldFtpSenderClass::RequestStop {msg} {

    network  MasterRequest NET SiteBroadcast COMP SldFtp Cmd Stop $msg
}

#---------------------------------------------------------------

body SldFtpSenderClass::Stop {msg} {
    
    if {$sender == ""} {
	return
    } else {
	
	WriteTraceMsg "- SldFtp stopped: $msg"
	
	set state $msg

	if {$ftpTask != -1} {
	    network RDOLocalGroup local_isabel_comp_Ftp "Eval delete object $ftpTask"
	    set ftpTask  -1
	}
	
	if {$sender == [SiteManager::MySiteId]} {
	    ResetVideoBWFactor
	}
	
	set sender ""
	
	SetWidgetsStoppedMode
    }
}

#---------------------------------------------------------------
# SldFtpSenderClass::SetState
#
# Set the state of widget to normal or disabled.
#---------------------------------------------------------------
body SldFtpSenderClass::SetState {widget state} {

    if {[winfo exists $widget]} {
	foreach w [pack slaves $widget] {
	    if {[string match "$guiname.to.sites.b*.r*" $w]} continue
	    if {$w == "$guiname.to.auto"} continue
	    SetState $w $state
	}
	catch {$widget configure -state $state}
    }
}


#---------------------------------------------------------------
# SldFtpSenderClass::GetToSitesList 
#
# Get a list with the selected sites.
#---------------------------------------------------------------
body SldFtpSenderClass::GetToSitesList {} {
    
    set res ""
    foreach siteid $sites_added {
	if {$to($siteid)} {
	    lappend res $siteid
	}
    }
    return $res
}


#---------------------------------------------------------------
# SldFtpSenderClass::AddSite
#
# Adds a new site.
#---------------------------------------------------------------
body SldFtpSenderClass::AddSite {siteid new} {

    WriteTraceMsg "- Adding site $siteid ([SiteManager::GetSiteInfo $siteid name])" SldFtp

    #
    # If I am a synced partner then I send the initial state to new partner.
    #
    if {$first_site || ($new && $flag_synced)} {
	network MasterRequest NET SiteRequest $siteid COMP SldFtp Cmd Sync $sender $maxbw $videobwfactor
    }
    
    if {[SiteManager::MySiteId] == $siteid} return
    
    set pos [lsearch -exact $sites_added $siteid]
    if {$pos == -1} {
	lappend sites_added $siteid
    } else {
	return
    }
    
    set received($siteid) 0
    set ok($siteid)       0
    set bad($siteid)      0
    
    set to($siteid) $autoselect
    
    set siteframe $guiname.to.sites.b$siteid 
    
    if {[winfo exists $siteframe]} {
	$siteframe.b configure \
	    -state normal \
	    -relief raised 
    } else {
	frame $siteframe
    
	checkbutton $siteframe.b \
	    -relief raised \
	    -text $siteid \
	    -anchor w \
	    -bd 1 \
	    -pady 0 \
	    -highlightthickness 0 \
	    -onvalue 1 \
	    -offvalue 0 \
	    -variable [scope to($siteid)]
	balloon add $siteframe.b [SiteManager::GetSiteInfo $siteid name]
	
	pack $siteframe.b -side left \
	    -expand 1 \
	    -fill x \
	    -pady 0 \
	    -padx 3
	
	radiobutton $siteframe.rr \
	    -variable [scope received($siteid)] \
	    -state disabled \
	    -selectcolor green \
	    -bd 1 \
	    -highlightthickness 0 \
	    -pady 0 \
	    -value 1
	pack $siteframe.rr -side left
	
	label  $siteframe.rbad \
	    -textvariable [scope bad($siteid)] \
	    -bd 0 \
	    -pady 0 \
	    -width 3 \
	    -anchor c \
	    -font [IsabelFontsC::GetFont medium medium] \
	    -foreground red
	pack   $siteframe.rbad -side right 
	
	label  $siteframe.rok \
	    -textvariable [scope ok($siteid)] \
	    -bd 0 \
	    -pady 0 \
	    -width 3 \
	    -font [IsabelFontsC::GetFont medium medium] \
	    -anchor c
	pack   $siteframe.rok -side right 
	
	set bg [$siteframe.rr cget -background]
	$siteframe.rr conf -activebackground $bg

	#---- griding

	lappend defined_sites $siteid
	set defined_sites [lsort -dictionary $defined_sites]
	
	set kont 0
	foreach sk $defined_sites {

	    set siteframe $guiname.to.sites.b$sk 
	    grid $siteframe -row $kont -column 0 -sticky nswe
	    grid rowconfigure $guiname.to.sites $kont -weight 1
	    
	    incr kont
	}
	
    }
}



#---------------------------------------------------------------
# SldFtpSenderClass::DelSite
#
# Removes a site.
#---------------------------------------------------------------
body SldFtpSenderClass::DelSite {siteid} {
    
    if {[SiteManager::MySiteId] == $siteid} return
    
    set pos [lsearch -exact $sites_added $siteid]
    if {$pos == -1} {
	return
    } else {
	set sites_added [lreplace $sites_added $pos $pos]
    }
    
    set siteframe $guiname.to.sites.b$siteid 
    
    $siteframe.b configure \
	-state disabled \
	-relief flat
    
    if {$ftpTask != -1} {
	set pos [lsearch -exact $to_marked $siteid]
	if {$pos != -1} {
	    set to_marked [lreplace $to_marked $pos $pos]
	    
	    network RDOLocalGroup local_isabel_comp_Ftp "Eval $ftpTask remove $siteid"
	}
    }
    
    set to($siteid) 0
    set received($siteid) 0
    set ok($siteid)  ""
    set bad($siteid) ""
}



#---------------------------------------------------------------
# SldFtpSenderClass::RemoteCheck
#
# Check if the target directory is remote or local.
#---------------------------------------------------------------
body SldFtpSenderClass::RemoteCheck {sld crc force} {
    global env
    
    set ISABEL_WORKDIR $env(ISABEL_WORKDIR)
    
    set slddir  [file dirname $sld]
    set sldfile [file tail $sld]
    
    if {[file isdirectory $ISABEL_WORKDIR/$slddir]} {
	if {![file isfile $ISABEL_WORKDIR/$slddir/.remote]} {
	    set crc2 ""
	    if {[file exists $ISABEL_WORKDIR/$sld]} {
		set crc2 [exec isabel_sum $ISABEL_WORKDIR/$sld]
	    }
	    if {$force} {
		if {$crc2 != ""  &&  $crc != $crc2} {
		    set date [clock format [clock seconds] -format %Y%m%d-%H%M%S]
		    file rename  $ISABEL_WORKDIR/$sld $ISABEL_WORKDIR/.$sld.bk.$date
		}
	    } else {
		set me [SiteManager::MySiteId]
		if {$crc2 != ""  &&  $crc != $crc2} {
		    set myname [SiteManager::GetSiteInfo $me name]
		    return "   Permission denied to ftp $slddir/$sldfile to $myname"
		}
	    }
	}
    } else {
	file mkdir $ISABEL_WORKDIR/$slddir
	close [open $ISABEL_WORKDIR/$slddir/.remote w]
    }
    return ""
}

#---------------------------------------------------------------

