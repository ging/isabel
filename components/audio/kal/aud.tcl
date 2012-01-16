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
# Description
#
#   Audio component
#
#
#########################################################################

class AudioComponentC {

    #------------------
    # PUBLIC INTERFACE
    #------------------

    # The Sms State object
    public variable smsobj

    constructor {} {}
    destructor  {} 

    # I am joined to audio component.
    public method AddMe {} {}

    # I leave the audio component.
    public method DelMe {} {}
    
    # Remote site joins to audio component.
    public method AddRemoteSite {site_id new} {}

    # Remote site leaves the audio component.
    public method DelRemoteSite {site_id} {}
    
    # Inform about the microphone gain of site $siteid.
    # If $site_id is equal to [SiteManager::MySiteId] it also saves the value $n to disk.
    public method SetGain {site_id n}

    # Method to manage mute switches.
    public method SetMute {site_id_list on}

    # Switch on/off the input tone generator.
    public method SetInputTone  {site_id on}

    # Switch on/off the output Emilio locution.
    public method SetOutputTone {site_id on}

    # Show or hide to tone buttons.
    public method ShowToneButtons {b}

    # Save value $v to file $filename
    public method Save {filename v}
    
    # Load value from $filename.
    # Return $default on error.
    public method Load {filename default}

    public method ExecDaemonCmd {cmd {cb ""}}

    public method Nop {} {}    

    #-------------------

    # Testing capture device:
    #
    # Methods used to configure the audio daemon to test the capture device.
    #
    # - The captured audio is sent to 127.0.0.1/$port, where a RTP Echo program is running.
    # - The RTP Echo program changes the SSRC of the received packets, before returning them to the sender.
    #

    public method StartCaptureDeviceTesting {port}
    public method StopCaptureDeviceTesting {}
    
    private variable captureDeviceTesting_SSRC 4
    private variable captureDeviceTesting_BindId ""

    #-------------------
    # PRIVATE INTERFACE
    #-------------------

    # Toplevel widget root
    private variable wroot

    # Directory to save configuration files
    private variable cfg_dir      

    # Control window display.
    private variable display

    # Audio and Vumeter daemon objects
    private variable acdobj
    private variable vcdobj  

    # Internal objects:
    #    miobj - Mixer object
    private variable miobj

    # Indicates if the monitor statistics are active or not
    private variable doMonitorStats 0

    #------------------------

    # Create the control window
    private method CreateMainWindow {}

    #------------------------

    # Wait until audio daemon is ready
    private method WaitUntilReady {}

    #------------------------

    # Activate or deactivate monitor statistics
    public method MonitorStats {activate}
}

#---------------------------------------------------------

body AudioComponentC::constructor {} {
    global env

    set wroot .tAudio
    
    set cfg_dir [session Environ cget -config_dir]/audio
    file mkdir $cfg_dir
    
    set display [session Environ cget -audio_ctrl_display]

    # Start vumeter daemon
    # OJO. Falta meter CB para actualizar el audio cuando se muera el vumetro.
    set vcdobj [DVumeterC "#auto" 30000 "" $display]
    if {[catch {$vcdobj Start} res]} {
	ErrorTraceMsg "Vumeter Daemon can not be created.\n$res"
	error $res
    }

    # Start audio daemon
    set no_cc_cb "network MasterRequest CM KillComponent [SiteManager::MySiteId] Audio"
    set acdobj [DAudioC "#auto" 50000 $no_cc_cb]
    if {[catch {$acdobj Start} res]} {
	ErrorTraceMsg "Audio Daemon can not be created.\n$res"
	error $res
    }

    #-------------------

    WaitUntilReady

    #-------------------

    # Initialize the configuration class
    AudioConfigC::Initialize [code $acdobj]

    #-------------------

    # Create control window
    CreateMainWindow

    set smsobj [AudioSmsStateC "#auto" $this]

    set miobj  [AudioMixerC  "#auto" $this [code $acdobj] [code $vcdobj] [code agui]]

    #-------------------

    # Restore monitor statistics state:
    $acdobj RDO audio_do_stats($doMonitorStats)

}

#---------------------------------------------------------

body AudioComponentC::destructor {} {

    catch {destroy $wroot}
    catch {delete object $acdobj}
    catch {delete object $vcdobj}
    catch {delete object $miobj}
    catch {delete object $smsobj}
}

#---------------------------------------------------------

body AudioComponentC::WaitUntilReady {} {
    
    set tries 60
    while {$tries > 0} {
	incr tries -1
	set res [$acdobj RPC audio_ready()]
	switch -- $res {
	    "" {
		break
	    } 
	    "OK\n" {
		return
	    }
	    default {
		after 500
	    }
	}
    }
    set msg "Audio daemon can't not get READY state."
    ErrorTraceMsg $msg
    error $msg
}

#---------------------------------------------------------

body AudioComponentC::CreateMainWindow {} {
    global ISABEL_DIR

    toplevel  $wroot -screen $display

    wm protocol $wroot WM_DELETE_WINDOW "wm withdraw $wroot"
    
    wm geometry $wroot [session Activity GetResource Audio geometry]
    
    wm title      $wroot Audio
    wm iconname   $wroot Audio
    wm iconbitmap $wroot @$ISABEL_DIR/lib/images/xbm/boombox.xbm

    wm withdraw $wroot

    wm resizable $wroot 1 0

    #- Create the widget --------------------------------------

    set waui $wroot.aui 

    catch {delete object agui}

    AudioGuiC agui $waui $display
    pack $waui

}
    
#---------------------------------------------------------

body AudioComponentC::AddMe {} {

    set _mysiteid [SiteManager::MySiteId]
    
    $miobj AddChannel $_mysiteid

    #-------------------

    # Now the local channel has been created.
    
    #-------------------

    # Set codec again

    set _codec [AudioConfigC::CodecCmd GetCurrentCodec]
    $acdobj RDO audio_set_codec($_codec)
    
    #-------------------

    # SMS messages which I want to receive:
    sms joinKey Audio_site_$_mysiteid   [code $smsobj setSiteState]
    sms joinKey Audio_global            [code $smsobj setGlobalState]
    
    # I send my site state
    sms forceState Audio_site_$_mysiteid add \
	-site_id $_mysiteid \
	-gain [$miobj GetGain $_mysiteid] \
        -capture [$miobj GetMute $_mysiteid] \
	-input_tone [$miobj GetInputTone $_mysiteid] \
	-output_tone [$miobj GetOutputTone $_mysiteid]
    
    # Send an empty codec message
    # sms setState Audio_global add 
}

#---------------------------------------------------------

body AudioComponentC::AddRemoteSite {site_id new} {

    # Create the channel for the site
    $miobj AddChannel $site_id

    #-------------------
    
    # SMS messages which I want to receive:
    sms joinKey Audio_site_$site_id   [code $smsobj setSiteState]
}

#---------------------------------------------------------

body AudioComponentC::DelMe {} {

    set _mysiteid [SiteManager::MySiteId]

    $miobj DelChannel [SiteManager::MySiteId]

    # SMS messages which I do not want to receive:
    sms leaveKey Audio_site_$_mysiteid
    sms leaveKey Audio_global
    
}
   
#---------------------------------------------------------

body AudioComponentC::DelRemoteSite {site_id} {

    $miobj DelChannel $site_id

    # SMS messages which I do not want to receive:
    sms leaveKey Audio_site_$site_id
}
   
#---------------------------------------------------------

body AudioComponentC::SetGain {site_id n} {

    $miobj SetGain $site_id $n
}

#---------------------------------------------------------

body AudioComponentC::SetMute {site_id_list on} {
    
    $miobj SetMute $site_id_list $on
}

#---------------------------------------------------------

body AudioComponentC::SetInputTone {site_id on} {

    $miobj SetInputTone $site_id $on
}

#---------------------------------------------------------

body AudioComponentC::SetOutputTone {site_id on} {

    $miobj SetOutputTone $site_id $on
}

#---------------------------------------------------------

body AudioComponentC::ShowToneButtons {b} {

    $miobj ShowToneButtons $b
}

#---------------------------------------------------------

body AudioComponentC::Save {filename v} {

    if {[catch { 
	set fd [open $cfg_dir/$filename w]
	puts $fd $v
	close $fd
    } emsg]} {
	ErrorTraceMsg "Audio ($filename): $emsg"
    }
    return
    
}

#---------------------------------------------------------

body AudioComponentC::Load {filename default} {

    if {[catch {
	set fd [open $cfg_dir/$filename r]
	gets $fd v
	close $fd
    } emsg]} {
	return $default
    }
    if {$v == ""} {
	return $default
    } else {
	return $v
    }
}

#---------------------------------------------------------

body AudioComponentC::ExecDaemonCmd {cmd {cb ""}} {
    $acdobj RCB $cmd $cb
}

#---------------------------------------------------------

body AudioComponentC::MonitorStats {activate} {

    if {$activate == 1} {

	set doMonitorStats 1
	ExecDaemonCmd audio_do_stats(1)

    } else {

	set doMonitorStats 0
	ExecDaemonCmd audio_do_stats(0)
    }
}

#---------------------------------------------------------

body AudioComponentC::StartCaptureDeviceTesting {port} {

    $acdobj RDO audio_reset_devices()

    $acdobj RDO audio_new_channel($captureDeviceTesting_SSRC)
    set captureDeviceTesting_BindId [$acdobj RPC audio_bind(127.0.0.1,$port)]
    regsub "\\n" $captureDeviceTesting_BindId "" captureDeviceTesting_BindId

}

#---------------------------------------------------------

body AudioComponentC::StopCaptureDeviceTesting {} {
    
    $acdobj RDO audio_delete_channel($captureDeviceTesting_SSRC)
    $acdobj RDO audio_unbind($captureDeviceTesting_BindId)
    set captureDeviceTesting_BindId ""
    
}

#---------------------------------------------------------

