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
###############################################################
#
#   Description:
#     Questions manager.
#
################################################################


#===============================================================
#
#  Class: question object.
#
#===============================================================

class QuestionsC {

    #------------------
    # PUBLIC INTERFACE
    #------------------

    constructor {} {}
    destructor {}

    public method AddMe   {}
    public method AddSite {site_id new}
    public method DelSite {site_id}

    private method SetModerator     {site_id}
    private method SetSpeakers      {site_idlist}
    private method SetQuestioner    {site_id}
    private method SetCtrlSites     {site_idlist}
    private method SetResetSites    {site_idlist}
    private method SetAudioSites {site_idlist}
    private method SetSpeakerCanAsk {bool}

    public method SmsQuestionsFrom {site_id}

    public  method SelectSite    {site_id}
    public  method DeselectSite  {site_id}
    public  method DeselectSites {}

    # Send a SMS message to toggle the selection status of $site_id
    public  method SmsToggleSite    {site_id}

    # Video layout to use
    public method SetVideoLayouts {ml ol om rml rol rom}

    # Set setaudio variable.
    public method SetSetAudio {sa}

    # Set Audio/Video layout.
    public method SetAVLayout {}
    public method SetAResetLayout {}

    public method Reset {}

    public method SetCtrlLayout {_ctrllayout}

    public method Iconify   {}
    public method Deiconify {}

    # Is the local question widget iconified?
    public method Iconified {}

    public method Nop {} {}

    #-------------------
    # PRIVATE INTERFACE
    #-------------------

    private variable moderatorSiteId    ""
    private variable speakerSiteIdList  ""
    private variable questionerSiteId   ""

    # list of site_id of the control sites, or *
    private variable ctrlSiteIdList  ""

    private variable resetSiteIdList ""
    private variable audioSiteIdList ""

    # If $setaudio is 1:
    #   - speaker and questioner audio channels are opened.
    #   - other channels are closed.
    # If $setaudio is 0:
    #   - audio channel states are not changed.
    private variable setaudio 1

    # local Question widget
    private variable qw

    # Video configuration
    private variable videoMainLayout  ""
    private variable videoOtherLayout ""
    private variable videoOtherMap     0
    private variable videoResetMainLayout  ""
    private variable videoResetOtherLayout ""
    private variable videoResetOtherMap     0

    # Layout of the control window
    private variable ctrllayout ""

    # list of sites with pending requests.
    private variable pendingRequests ""

    # Enable Speaker as questioner, i.e., the video of a speaker site must be removed form its position in the display, and
    # it must be positioned in the place of the questioner. 
    # This must be done when the site is selected in the question control window.
    private common speakerCanAsk 0

    # Processes SMS messages:
    private method processQuestionsState {args}
    private method processPanelState {args}
    private method processSiteState  {args}

    # Return one value option from a option-value list.
    # $list is the option-value list.
    # $optname is option name to search.
    # $default is the value to return if $optname option is not found.
    private method getOptionListValue {list optname {default ""}}
}

#---------------------------------------------------------------

body QuestionsC::constructor {} {

    set qw [QuestionsWidget #auto]

    $qw SetResetCmd    [code $this Reset]
    $qw SetRequestCmd  "$this SmsToggleSite [SiteManager::MySiteId]"
    $qw DisableReset
    $qw DisableRequest

    # Global SMS messages which I want to receive:
    sms joinKey Questions       [code $this processQuestionsState]
    sms joinKey Questions_panel [code $this processPanelState]

    sms joinKey Video_Cameras
   
   # Send an empty messages to get the states
    #sms setState Questions    add 
    #sms setState Questions_panel       add 

}

#---------------------------------------------------------------

body QuestionsC::destructor {} {

    delete object $qw
}

#---------------------------------------------------------------

body QuestionsC::AddMe {} {

    set me [SiteManager::MySiteId]

    $qw AddSite $me [code $this SmsQuestionsFrom $me]

    SetCtrlSites $ctrlSiteIdList

    #SelectSite $me


    # SMS messages (related with me) which I want to receive:
    sms joinKey Questions_site_$me  [code $this processSiteState]
    
    # I send my site state
    sms forceState Questions_site_$me add \
	-site_id $me \
	-request 0

}

#---------------------------------------------------------------

body QuestionsC::AddSite {site_id new} {

    $qw AddSite $site_id [code $this SmsQuestionsFrom $site_id]

    SetCtrlSites $ctrlSiteIdList

    # SMS messages (related with this site) which I want to receive:
    sms joinKey Questions_site_$site_id  [code $this processSiteState]
}

#---------------------------------------------------------------

body QuestionsC::DelSite {site_id} {

    if {[SiteManager::MySiteId] != $site_id} {
	$qw DelSite $site_id
	
	# if {$moderatorSiteId == $site_id} {
	#    SetModerator "" 1
	# }
	
	set pos [lsearch -exact $pendingRequests $site_id]
	if {$pos != -1} {
	    set pendingRequests [lreplace $pendingRequests $pos $pos]
	}
    }

    # Do not receive SMS messages about that site:
    sms leaveKey Questions_site_$site_id

    # Deselect
    sms setState Questions_site_$site_id add \
	-site_id $site_id \
	-request 0
    
}

#---------------------------------------------------------------

body QuestionsC::SetModerator {site_id} {

    set moderatorSiteId $site_id
}

#---------------------------------------------------------------

body QuestionsC::SetSpeakers {site_idlist} {

    set speakerSiteIdList $site_idlist
}

#---------------------------------------------------------------

body QuestionsC::SetQuestioner {site_id} {

    set questionerSiteId $site_id
}

#---------------------------------------------------------------

body QuestionsC::SetCtrlSites {site_idlist} {

    set ctrlSiteIdList $site_idlist
    
    set _my_site_id [SiteManager::MySiteId]
    set _tool [SiteManager::GetSiteInfo $_my_site_id tool]

    if {$site_idlist == "*" ||
	[lsearch -exact $site_idlist [SiteManager::MySiteId]] != -1 ||
	[lsearch -exact "FlashGateway" $_tool] != -1} {
	$qw EnableReset
	$qw EnableRequest
	$qw EnableSites 
    } else {
	$qw DisableReset
	$qw DisableSites 
	if {$site_idlist == ""} {
	    $qw DisableRequest
	} else {
	    $qw EnableRequest
	}
    }
}

#---------------------------------------------------------------

body QuestionsC::SetResetSites {site_idlist} {

    set resetSiteIdList $site_idlist
}

#---------------------------------------------------------------

body QuestionsC::SetAudioSites {site_idlist} {

    set audioSiteIdList $site_idlist
}

#---------------------------------------------------------------

body QuestionsC::SetSpeakerCanAsk {bool} {

	set speakerCanAsk $bool
}

#---------------------------------------------------------------

body QuestionsC::SelectSite {site_id} {

    $qw AddRequestingSite $site_id
    
    set pos [lsearch -exact $pendingRequests $site_id]
    if {$pos == -1} {
	lappend pendingRequests $site_id
    }
}

#---------------------------------------------------------------

body QuestionsC::DeselectSite {site_id} {

    $qw DelRequestingSite $site_id
    
    set pos [lsearch -exact $pendingRequests $site_id]
    if {$pos != -1} {
	set pendingRequests [lreplace $pendingRequests $pos $pos]
    }
}

#---------------------------------------------------------------

body QuestionsC::DeselectSites {} {

    $qw NoRequestingSites

    set pendingRequests ""
}

#---------------------------------------------------------------

body QuestionsC::SmsToggleSite {site_id} {

    set pos [lsearch -exact $pendingRequests $site_id]
    if {$pos == -1} {
	sms setState Questions_site_$site_id add \
	    -site_id $site_id \
	    -request 1
    } else {
	sms setState Questions_site_$site_id add \
	    -site_id $site_id \
	    -request 0
    }
}

#---------------------------------------------------------------

body QuestionsC::SetCtrlLayout {_ctrllayout} {

	set ctrllayout $_ctrllayout
	$qw SetLayout $_ctrllayout
}

#---------------------------------------------------------------

body QuestionsC::Iconify {} {

	$qw Iconify

	set moderatorSiteId   ""
	set speakerSiteIdList ""
	set questionerSiteId  ""
        set ctrlSiteIdList    ""
	set resetSiteIdList   ""
	set audioSiteIdList   ""
	#DeselectSites
}

#---------------------------------------------------------------

body QuestionsC::Deiconify {} {

	$qw Deiconify
}

#---------------------------------------------------------------

body QuestionsC::Iconified {} {

    if {[$qw Iconified]} {
	return 1
    } else {
	return 0
    }
}

#---------------------------------------------------------------

body QuestionsC::SmsQuestionsFrom {site_id} {

    DeselectSite $site_id

    sms setState Questions add \
	-question_from            $site_id \
	-reset_mode               0 \
        -nsec [clock seconds]
    
    sms setState Questions_site_$site_id add \
	-site_id $site_id \
	-request 0
}

#---------------------------------------------------------------

body QuestionsC::SetVideoLayouts {ml ol om rml rol rom} {

	set videoMainLayout       $ml
	set videoOtherLayout      $ol
	set videoOtherMap         $om
	set videoResetMainLayout  $rml
	set videoResetOtherLayout $rol
	set videoResetOtherMap    $rom
}

#---------------------------------------------------------------

body QuestionsC::SetSetAudio {sa} {

	set setaudio $sa
}

#---------------------------------------------------------------

body QuestionsC::SetAResetLayout {} {

    sms setState Video_Cameras add \
	-sites $resetSiteIdList \
	-main_layouts $videoResetMainLayout \
	-others_layouts $videoResetOtherLayout \
	-map_others $videoResetOtherMap
    
    if {$setaudio} {
	if {$audioSiteIdList != ""} {
	    set _sids $audioSiteIdList
	} else {
	    set _sids $resetSiteIdList
	}
	network MasterRequest AudioBypassTranslator::setCaptures $_sids
   }

    $qw SetSpeakerSites $resetSiteIdList
    $qw SetQuestionerSite ""
}

#---------------------------------------------------------------

body QuestionsC::SetAVLayout {} {
    
    if {$questionerSiteId == ""} {
	set _sids [linsert $speakerSiteIdList 0 ""]
    } else {
	set pos [lsearch -exact $speakerSiteIdList $questionerSiteId]
	if {$pos == -1} {
	    set _sids [linsert $speakerSiteIdList 0 $questionerSiteId] 
	} else {
	    if {$speakerCanAsk} {
		set _sids [linsert [lreplace $speakerSiteIdList $pos $pos ""] 0 $questionerSiteId] 
	    } else {
		set _sids [linsert $speakerSiteIdList 0 ""]
	    }
	}
    }

    $qw SetSpeakerSites $_sids
    $qw SetQuestionerSite [lindex $_sids 0]
    
    sms setState Video_Cameras add \
	-sites $_sids \
	-main_layouts $videoMainLayout \
	-others_layouts $videoOtherLayout \
	-map_others $videoOtherMap
    
    if {$setaudio} {
	if {$audioSiteIdList != ""} {
	    set _sids [eval concat $moderatorSiteId $questionerSiteId $audioSiteIdList]
	} else {
	    set _sids [eval concat $moderatorSiteId $questionerSiteId $speakerSiteIdList]
	}
	network MasterRequest AudioBypassTranslator::setCaptures $_sids
    }
}

#---------------------------------------------------------------

body QuestionsC::Reset {} {

    # DeselectSites
    foreach _sid [SiteManager::GetSites] {
	sms setState Questions_site_$_sid add \
	    -site_id $_sid \
	    -request 0
    }

    if {[lsearch -exact $speakerSiteIdList $moderatorSiteId] != -1} {
	sms setState Questions add \
	    -question_from "" \
            -reset_mode     1 \
            -nsec [clock seconds]
    } else {
	sms setState Questions add \
	    -question_from  $moderatorSiteId \
            -reset_mode     1 \
            -nsec [clock seconds]
    }
}

#-----------------------------------------------------------------

body QuestionsC::processQuestionsState {args} {

    set _active [getOptionListValue $args -active "0"]

    set _ml   [getOptionListValue $args -video_main_layout ""]
    set _ol   [getOptionListValue $args -video_other_layout ""]
    set _om   [getOptionListValue $args -video_other_map ""]
    set _rml  [getOptionListValue $args -video_reset_main_layout ""]
    set _rol  [getOptionListValue $args -video_reset_other_layout ""]
    set _rom  [getOptionListValue $args -video_reset_other_map ""]

    set _sa   [getOptionListValue $args -audio_set "1"]

    set _chair       [getOptionListValue $args -chair ""]
    set _speakers    [getOptionListValue $args -speakers ""]
    set _controllers [getOptionListValue $args -controllers ""]
    set _reset_sites [getOptionListValue $args -reset_sites ""]
    set _audio_sites [getOptionListValue $args -audio_sites $_reset_sites]
    set _spk_can_ask [getOptionListValue $args -speaker_can_ask 0]
    
    set _from [getOptionListValue $args -question_from ""]

    set _reset [getOptionListValue $args -reset_mode "1"]

    if {$_active != "1"} {
	Iconify
	return
    }
    
    SetVideoLayouts $_ml $_ol $_om $_rml $_rol $_rom
    SetSetAudio     $_sa

    SetModerator     $_chair
    SetSpeakers      $_speakers
    SetCtrlSites     $_controllers
    SetResetSites    $_reset_sites
    SetAudioSites    $_audio_sites
    SetSpeakerCanAsk $_spk_can_ask

    SetQuestioner $_from

    Deiconify

    if {$_reset == "1"} {
	SetAResetLayout
    } else {
	SetAVLayout
    }
}

#-----------------------------------------------------------------

body QuestionsC::processPanelState {args} {

    set _layout  [getOptionListValue $args -layout "100x100+0+0"]

    SetCtrlLayout $_layout
}

#---------------------------------------------------------------

body QuestionsC::processSiteState {args} {

    set _site_id [getOptionListValue $args -site_id ""]
    
    set _request [getOptionListValue $args -request 0]
    if {$_request == "0"} {
	DeselectSite $_site_id
    } else {
	SelectSite $_site_id
    }
}

#---------------------------------------------------------------

body QuestionsC::getOptionListValue {list optname {default ""}} {
    
    set pos [lsearch -exact $list $optname]
    if {$pos == -1} {
	return $default	
    } else {
	return [lindex $list [expr $pos+1]]
    }
}

#---------------------------------------------------------------
