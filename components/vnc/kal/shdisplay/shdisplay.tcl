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
# shdisplay.tcl
#
# Shared Display component.
#
# This component only support one channel at a time: one
# site is the sender and the other can be receivers.
# One site can only send one channel, and its channel
# identifier is the site userid.
#
#########################################################################

class ShDisplayComponentC {

    #------------------
    # PUBLIC INTERFACE
    #------------------

    constructor {} {}

    public method ExecDaemonCmd {cmd {cb ""}}

    # Activate or deactivate monitor statistics
    public method MonitorStats {activate}

    public method Nop {} {}

    # Configure the display sharing to transmit and receive a window.
    # This function is local, that is it must be executed in all sites, so that each site will
    # start a source channel, a sink channel, or do nothing.
    # 
    # $_sender_site is the site_id of the site that will start the source channel.
    #
    # Sites that will start a sync channel are $_recv_sites, excluding $_excluded_sites.
    # The wildcard "*", "" or site_idid list are uniq valid values for $_recv_sites and $_excluded_sites.
    #
    # If $_grabmode is wintitle, the window to capture is identified by title ($source_win_title).
    # If $_grabmode is shmem, the window to capture stores its content in shared memory, and this shared
    # memory is read to get the image to send.
    #
    # $_viewerlayout is the (virtual) layout of the receiver ShDisplay window.
    # Note that a window layout is a list like this: "geometry scalesize scalepos anchor".
    public method Start {_sender_site _recv_sites _excluded_sites _grabmode source_win_title _viewerlayout}
 
    # Stop transmition
    public method Stop {}

    # Sets the local bw usage (kbps).
    public method SetBW {_bw}

    # Sets the local grabsize value.
    # $_gs: is the desired grab size
    public method SetGrabSize {_gs}

    # Sets packet duplication value.
    # All packets sent to the network are repeated $_pktdup times,
    # but the used bandwidth is not modified, so it is been divided by
    # the number of repetitions.
    public method SetPktDuplication {_pktdup}

    # Select the codec to send images:
    # $codec is a codec name and a list of -option/values items.
    public method SetCodec {_codec}

    # Set the codec quality.
    # It value meaning depends of the current codec.
    public method SetQuality {_quality}


    #-------------------
    # PRIVATE INTERFACE
    #-------------------

    # Local scenary display
    private variable display

    # shdisplay daemon object
    private variable shdisdobj

    # Indicates if the monitor statistics are active or not
    private variable doMonitorStats 0

    destructor {}

    #-- Config parameters ----

    # Current bandwidth usage (kbps).
    private variable bw

    # Current packet duplication value.
    private variable pktduplication

    # Current grab size
    private variable grabsize

    # Current codec.
    # It is a list with the codec name and the codec options.
    private variable codec

    # Current codec quality.
    # This value is set from the codec value, or from the config window (grifo). 
    private variable quality

    #-- Transmittion ------

    # site_id of the sender site
    private variable sender_site

    # reception (sync) sites.
    # It is a site_id list, a "*" or a empty string "".
    private variable recv_sites

    # sites to exclude from $recv_sites.
    # It is a site_id list, a "*" or a empty string "".
    private variable excluded_sites

    # Grab mode: how window images are captured.
    #   - wintitle -> X Get Image
    #   - shmem    -> Shared memory
    private variable grabmode

    # name or prefix name of the transmitted window
    private variable windowId

    # Current viewerlayout, i.e. virtual layout of the receiver ShDisplay window.
    private variable viewerlayout

    # Create the local source channel, i.e. the sender.
    private method CreateLocalChannel {}

    # Create the local sync channel to receive images from $_sender_site.
    private method CreateRemoteChannel {}

    #-- Source ------

    # After code used when selecting a window.
    private variable afterId ""

    # Say to the shdisplay daemon what the window to send is.
    # The window title prefix is $title.
    # The window identifier is $wid.
    private method SelectWindow   {title} 
    private method SelectWindowCB {title wid res}

     # Say to the shdisplay daemon what the shared memory to send is.
    private method SelectShMem {}

    #-- GW ----------------------

    # Gateway Configuration: creates an additional channel to send the desktop.
    
    private method GatewayMode {}

    #------------------------

    # Return one value option from a option-value list.
    # $list is the option-value list.
    # $optname is option name to search.
    # $default is the value to return if $optname option is not found.
    private method GetOptionListValue {list optname {default ""}}
}

#--------------------------------------------------------------

body ShDisplayComponentC::constructor {} {


    set bw             500
    set pktduplication 1
    set codec          "MPEG-4 -qlty 50"
    set quality        50

    set no_cc_cb "network MasterRequest CM KillComponent [SiteManager::MySiteId] VNC"
    set shdisdobj [DShDispC #auto 30000 $no_cc_cb]
    if {[catch {$shdisdobj Start} res]} {
	delete object $shdisdobj
	ErrorTraceMsg "Shared Display: isabel_shdisplay Daemon can not be created."
	error $res
    }

    set sender_site    ""
    set recv_sites     ""
    set excluded_sites ""
    set grabmode       wintitle
    set windowId       ""
    set viewerlayout   ""

    set display      [session Environ cget -scenary_display]

    #-------------------

    # Restore monitor statistics state:
    $shdisdobj RDO shdis_do_stats($doMonitorStats)
    
    #-------------------
    
    GatewayMode
    
    #-------------------
}



#--------------------------------------------------------------

body ShDisplayComponentC::destructor {} {
    
    after cancel $afterId
    catch {delete object $shdisdobj}
}

#--------------------------------------------------------------

body ShDisplayComponentC::Start {_sender_site _recv_sites _excluded_sites _grabmode source_win_title _viewerlayout} {
    
    Stop
    
    set sender_site    $_sender_site
    set recv_sites     $_recv_sites
    set excluded_sites $_excluded_sites
    set grabmode       $_grabmode
    set windowId       $source_win_title
    set viewerlayout   $_viewerlayout
    
    set me [SiteManager::MySiteId]
    
    if {$_sender_site == $me} {
	CreateLocalChannel
    } else {
	if {$_sender_site != ""} {
	    if {$_recv_sites == "*" || [lsearch -exact $_recv_sites $me] != -1} {
		if {$_excluded_sites != "*" && [lsearch -exact $_excluded_sites $me] == -1} {
		    CreateRemoteChannel
		}
	    }
	}
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::CreateLocalChannel {} {

    set _chid [SiteManager::MyChId]

    if {$_chid == ""} return

    $shdisdobj RDO shdis_create_source_channel($_chid)

    set pdusize [session Activity GetResource ShDisplay pdusize 1024]
    $shdisdobj RDO shdis_bucket_size($pdusize)

    $shdisdobj RDO shdis_send($_chid,1)

    set _grabsize [session Activity GetResource ShDisplay grabsize 800x600]
    SetGrabSize       $_grabsize

    SetBW             $bw
    SetPktDuplication $pktduplication
    SetCodec          $codec

    $shdisdobj Bind $_chid

    if {$grabmode == "wintitle"} {
	SelectWindow $windowId
    } else {
	SelectShMem
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::CreateRemoteChannel {} {

    set _chid [SiteManager::GetSiteInfo $sender_site chid]

    if {$_chid == ""} return

    $shdisdobj RDO shdis_create_sink_channel($_chid)

    set scalesize [lindex $viewerlayout 1]
    if {$scalesize != 1} {
	set scalesize 0
    }

    set rremotegeom [dvirtual V2R_Layout $viewerlayout]
    
    $shdisdobj RDO video_window_geometry($_chid,$rremotegeom)

    $shdisdobj RDO shdis_follow_size($_chid,[expr 1-$scalesize])
    
    $shdisdobj RDO shdis_map_channel($_chid)
}

#--------------------------------------------------------------

body ShDisplayComponentC::SelectShMem {} {

    set _shMemId 1250
    set _shMemSize 12582912

    set _chid [SiteManager::MyChId]
    $shdisdobj RDO "shdis_select_source($_chid,SHM grabber;$_shMemId $_shMemSize)"
}

#--------------------------------------------------------------

body ShDisplayComponentC::SelectWindow {title} {

    after cancel $afterId
    set afterId ""

    if {$windowId != $title} return

    if {[catch {xutils GetWinIdByPrefixName $display $title} wid]} {
	ErrorTraceMsg "Shared Display: $wid" 
	if {$windowId != $title} return
	set afterId [after 1000 [code $this SelectWindow [list $title]] ]
    } else {
	set _chid [SiteManager::MyChId]
	$shdisdobj RCB shdis_select_by_winid($_chid,$wid) \
		[code $this SelectWindowCB [list $title] $wid]
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::SelectWindowCB {title wid res} {

    if {$windowId != $title} return

    if {$res != "OK\n"} {
	set afterId [after 1000 [code $this SelectWindow [list $title]] ]
    } else {
	set afterId ""

	if {$viewerlayout != ""} {

	    set dvirtualobj [DVirtualC "#auto" $display]
	    set rsourcegeom [$dvirtualobj V2R_Layout $viewerlayout]
            delete object $dvirtualobj

	    xutils SetGeometry $display $wid $rsourcegeom

	} else {

	    WarningTraceMsg "Shared Display: No layout specified for source window \"$title\"." 
	}
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::Stop {} {

    after cancel $afterId

    set recv_site       ""
    set excluded_sites  ""
    set grabmode        ""
    set windowId        ""
    set viewerlayout    ""

    if {$sender_site != ""} {
	set _chid [SiteManager::GetSiteInfo $sender_site chid]
	$shdisdobj RDO shdis_delete_channel($_chid)
    }
    
    set sender_site ""
}


#--------------------------------------------------------------

body ShDisplayComponentC::SetBW {_bw} {
    
    set bw $_bw

    ShDisplayConfigC::SetBW $_bw
    
    if {$sender_site == [SiteManager::MySiteId]} {
	set _chid [SiteManager::MyChId]
	$shdisdobj RDO shdis_channel_bandwidth($_chid,$_bw)
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::SetGrabSize {_gs} {
    
    set grabsize $_gs

    if {$sender_site == [SiteManager::MySiteId]} {
	set _chid [SiteManager::MyChId]
	$shdisdobj RDO video_grab_size($_chid,$_gs)
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::SetPktDuplication {_pktdup} {
    
    set pktduplication $_pktdup

    ShDisplayConfigC::SetPktDuplication $_pktdup
    
    if {$sender_site == [SiteManager::MySiteId]} {
	set _chid [SiteManager::MyChId]
	$shdisdobj RDO shdis_set_retrans($_chid,$_pktdup)
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::SetCodec {_codec} {

    set codec $_codec
    set codecname [lindex $_codec 0]

    ShDisplayConfigC::SelectCodec $codecname
    
    if {$sender_site != [SiteManager::MySiteId]} return

    set _chid [SiteManager::MyChId]

    switch -- $codecname {
	MPEG-4 {
	    $shdisdobj RDO shdis_select_codec($_chid,MPEG-4)
	}
	XVID {
	    $shdisdobj RDO shdis_select_codec($_chid,XVID)
	}
	MPEG-1 {
	    $shdisdobj RDO shdis_select_codec($_chid,MPEG-1)
	}
	MJPEG {
	    $shdisdobj RDO shdis_select_codec($_chid,MJPEG)
	}
	H.263 {
	    $shdisdobj RDO shdis_select_codec($_chid,H263)
	}
	H.264 {
	    $shdisdobj RDO shdis_select_codec($_chid,H264)
	}
	default {
	    ErrorTraceMsg "Shared Display: Unsupported codec $codecname"	    
	}
    }
    
    set quality [GetOptionListValue $codec -qlty 50]
    SetQuality $quality
    
}

#--------------------------------------------------------------

body ShDisplayComponentC::SetQuality {_quality} {
    
    set quality $_quality

    ShDisplayConfigC::SetQuality $_quality

    if {$sender_site == [SiteManager::MySiteId]} {
	set _chid [SiteManager::MyChId]
	$shdisdobj RDO shdis_codec_quality($_chid,$_quality)
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::ExecDaemonCmd {cmd {cb ""}} {

    if {$cb == ""} {
	$shdisdobj RDO $cmd
    } else {
	$shdisdobj RCB $cmd $cb
    }
}

#--------------------------------------------------------------

body ShDisplayComponentC::MonitorStats {activate} {

    set doMonitorStats $activate
    ExecDaemonCmd shdis_do_stats($activate)
}

#------------------------------------------------------------

body ShDisplayComponentC::GatewayMode {} {
    global env
    
    if {$env(ISABEL_ROLE) == "SipGateway"} {
    
	    $shdisdobj RDO "shdis_create_source_channel(1)"
	    $shdisdobj RDO "shdis_select_source(1,SHM grabber;1350 12582912)"
	    $shdisdobj RDO "video_grab_size(1,CIF)"
	    $shdisdobj RDO "shdis_select_codec(1,H263)"
	    $shdisdobj RDO "shdis_bind(1,127.0.0.1,61017)"
	    $shdisdobj RDO "shdis_channel_bandwidth(1,512)"
	    $shdisdobj RDO "shdis_send(1,1)"
    }

    if {$env(ISABEL_ROLE) == "FlashGateway"} {
    
    }  
}

#--------------------------------------------------------------

body ShDisplayComponentC::GetOptionListValue {list optname {default ""}} {

    set pos [lsearch -exact $list $optname]
    if {$pos == -1} {
	return $default	
    } else {
	return [lindex $list [expr $pos+1]]
    }
}

#--------------------------------------------------------------

