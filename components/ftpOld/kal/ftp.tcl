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
#   Ftp sender and receiver.
#
################################################################


#===============================================================
#
#  FTP SENDER AND RECEIVER TASKS
#
#===============================================================

class SenderTask {

    # task id
    private common taskidcounter 0
    private variable taskid

    # path of the file to send.
    private variable path ""

    # file to send.
    private variable filename ""

    # ack table
    private variable ack

    # destination sites
    private variable destines ""

    # Callback to execute when an ack is received
    # Takes $site_id as argument
    private variable ack_cb ""

    # Callback to execute when a nack is received
    # Takes $site_id and $reason as argument.
    private variable nack_cb ""

    # callback to execute when everybody has responded (ack or nack)
    private variable done_cb ""

    # SeCo local group where the client application (in all the sites) receives SeCo Messages from the Ftp component.
    private variable localgroup

    constructor {p f erd f2 siteidlist _localgroup livetime ackcb nackcb donecb ltcb bw {remoteCheck ""}} {}

    destructor {}

    public method ack  {siteid} {}
    public method nack {siteid reason} {}

    public method remove {siteid} {}

    private method done {} {}

    # the command SEND is invoked in the cftp daemon with a little delay
    private variable aftersend  ""

    #-- LIVETIME ---------------------------------

    # afterlivetime
    private variable afterlivetime ""

    # callback to execute when livetime expires.
    private variable livetime_cb ""

    private method livetime {} {}

    # change the livetime to the new given value
    public method setLiveTime {lt}

    #-- BW -------------------------

    # change the current bw
    public method setBW {bw}
}



#---------------------------------------------------------------
# SenderTask::constructor
#
# Start a sender task.
# Arguments:
#   p - path of the file to send.
#   f - file to send.
#   erd - environment variable that contains the root directory where f2
#         will be written.
#   f2 - file to create in receiver ( after $env($erd) ).
#   siteidlist - site_id of the target sites.
#   _localgroup - SeCo local group where the client application (in all the sites) 
#                 receives SeCo Messages from the Ftp component.
#   livetime - livetime
#   ackcb - Callback to execute when an ack is received.
#           Takes $site_id as argument.
#   nackcb - callback to execute when a nack is received.
#            Takes $site_id and $reason as arguments.
#   donecd - callback to execute when all the sites have responded (ack or nack)
#   ltcb - callback to execute when ltime expires.
#   bw - Transmition bandwidth (kilobit per second)
#   remoteCheck - procedure to execute in the remote site before programming
#                 the cftp daemon to receive a file. This procedure will return
#                 "" if the file must be received, and a error message if not.
#                 This error message is returned to the sender using nackcb.
#---------------------------------------------------------------

body SenderTask::constructor {p f erd f2 siteidlist _localgroup livetime ackcb nackcb donecb ltcb bw {remoteCheck ""}} {
    global env 

    if {$taskidcounter == 0} {
	set taskidcounter [expr [SiteManager::MyChId]<<16]
    }
    
    incr taskidcounter
    set taskid $taskidcounter
    
    set path        $p
    set filename    $f
    set localgroup  $_localgroup
    set ack_cb      $ackcb    
    set nack_cb     $nackcb    
    set done_cb     $donecb    
    set livetime_cb $ltcb    
    
    if {$siteidlist == "*"} {
	set siteidlist [SiteManager::GetSites]
    }
    
    set destines ""
    foreach _site_id $siteidlist {
	if {[SiteManager::MySiteId] == $_site_id} continue
	if {[lsearch -exact [SiteManager::GetSites] $_site_id] == -1} {
	    set ack($_site_id) 1
	    set m1 "$_site_id is not connected to FTP."
	    set m2 "I can't send $f."
	    ErrorTraceMsg "FTP: $m1 $m2"
	    
	    if {$nack_cb != ""} {
		if {[catch {uplevel #0 $nack_cb $_site_id [list $m1]} res]} {
		    ErrorTraceMsg "FTP: callback error. $nack_cb produces:\n $res"
		}
	    }
	    
	} else {
	    set ack($_site_id) 0
	    lappend destines $_site_id
	}	
    }
    
    if {$destines == ""} {
	error "- No sites to send $p/$f"
    }
    
    DebugTraceMsg "- Ftp: request to send $f to $destines"
    
    component Cmd RDO cftp_create_channel($taskid)
    component Cmd RDO cftp_bw($taskid,$bw)
    
    # start the receiver tasks
    set checksum [exec isabel_sum $p/$f]
    foreach _site_id $destines {
	
	network SiteRequest $_site_id COMP Ftp Eval \
	    ReceiverTask recvtask$taskid $taskid $erd $f2 [SiteManager::MySiteId] $localgroup $this \
	    $livetime $checksum $remoteCheck
	
	set host 127.0.0.1
	
	set port 51014
	
	component Cmd RDO cftp_bind($taskid,$taskid,$host,$port)
	
    }

    component Cmd RDO cftp_send($taskid,$p,$f)

    set afterlivetime [after $livetime [code $this livetime]]

    return $taskid
}


#---------------------------------------------------------------
# SenderTask::destructor
#
#---------------------------------------------------------------

body SenderTask::destructor {} {
    
    after cancel $afterlivetime

    component Cmd RDO cftp_delete_channel($taskid)
}



#---------------------------------------------------------------
# SenderTask::ack
#
# $site_id has just received the file.
#---------------------------------------------------------------

body SenderTask::ack {siteid} {

    set ack($siteid) 1
    if {$ack_cb != ""} {
	if {[catch {network RDOLocalGroup $localgroup "$ack_cb $siteid"} res]} {
	    ErrorTraceMsg "FTP: callback error. $ack_cb produces:\n $res"
	}
    } else {
	DebugTraceMsg "FTP: ACK from $siteid" 2
    }

    done
}


#---------------------------------------------------------------
# SenderTask::nack
#
# $siteid can not received the file.
# "reason" explains the reason.
#---------------------------------------------------------------

body SenderTask::nack {siteid reason} {

    set ack($siteid) 1
    if {$nack_cb != ""} {
	if {[catch {network RDOLocalGroup $localgroup "$nack_cb $siteid [list $reason]"} res]} {
	    ErrorTraceMsg "FTP: callback error. $nack_cb produces:\n $res"
	}
    } else { 
	DebugTraceMsg "FTP: NACK from $siteid ($reason)"
    }

    done
}


#---------------------------------------------------------------
# SenderTask::done
#
# check if everybody has send ack or nack.
#---------------------------------------------------------------

body SenderTask::done {} {

    foreach _site_id [array names ack] {
	if {$ack($_site_id) == 0} {
	    return
	}
    }

    # All the sites have send ack or nack.

    set cb $done_cb

    delete object $this

    if {$cb != ""} {
	if {[catch {network RDOLocalGroup $localgroup $cb} res]} {
	    ErrorTraceMsg "FTP: callback error. $cb produces:\n $res"
	}
    } else {
	DebugTraceMsg "FTP: Finished transmition of $filename."
    }

}


#---------------------------------------------------------------
# SenderTask::remove
#
# removes $siteid from the destination site list.
#---------------------------------------------------------------

body SenderTask::remove {siteid} {

    set pos [lsearch -exact $destines $siteid]
    if {$pos != -1} {
	set destines [lreplace $destines $pos $pos]
	unset ack($siteid)

	component Cmd RDO cftp_unbind($taskid,$taskid)

	network SiteRequest $siteid COMP Ftp Eval delete object recvtask$taskid

	done
    }
}


#---------------------------------------------------------------
# SenderTask::livetime
#
# wait until livetime expired to execute livetime_cb and delete the task.
#---------------------------------------------------------------

body SenderTask::livetime {} {

    set cb $livetime_cb

    delete object $this

    if {$cb != ""} {
	if {[catch {network RDOLocalGroup $localgroup $cb} res]} {
	    ErrorTraceMsg "FTP: callback error. $cb produces:\n $res"
	}
    } else {
	DebugTraceMsg "FTP: Transmition livetime expired."
    }

}

#---------------------------------------------------------------

body SenderTask::setBW {bw} {

    component Cmd RDO cftp_bw($taskid,$bw)
}

#---------------------------------------------------------------

body SenderTask::setLiveTime {lt} {

    after cancel $afterlivetime

    set afterlivetime [after $lt [code $this livetime]]
}

#---------------------------------------------------------------

################################################################


class ReceiverTask {

    # task id
    private variable taskid 0

    # directory where filename will be written
    private variable rootdir

    # file to receive.
    private variable filename 

    # file checksum.
    private variable checksum 

    # name of the sender task object.
    private variable senderthis

    # site_id of sender site.
    private variable senderid

    # SeCo local group where the client application (in all the sites) receives SeCo Messages from the Ftp component.
    private variable localgroup

    # applid of the ftp component
    private variable ftpapplid -1

    # code of the after checking if the file has arrived
    private common accheckrecv 

    constructor {tid erd f sender _localgroup sthis livetime crc remoteCheck} {}

    destructor {}

    private proc checkrecv {mythis senderthis senderid rootdir filename} {}

    #-- LIVETIME ---------------------------------

    # afterlivetime
    private variable afterlivetime ""

    private method livetime {} {}
}


#---------------------------------------------------------------
# ReceiverTask::constructor
#
# Start a receiver task.
# Arguments:
#   tid - task id
#   erd - environment variable that contains the root directory where
#         $f will be written.
#   f - file to receive.
#   sender - siteid of sender site.
#   _localgroup - SeCo local group where the client application (in all the sites) 
#                 receives SeCo Messages from the Ftp component.
#   sthis - name of the sender task object.
#   crc - crc of the original file to send.
#   remoteCheck - the remoteCheck of the SenderTask.
#---------------------------------------------------------------

body ReceiverTask::constructor {tid erd f sender _localgroup sthis livetime crc remoteCheck} {
    global env 

    DebugTraceMsg "- Ftp: request to receive $f from $sender"

    set taskid   $tid
    set rootdir  $env($erd)
    set filename $f
    set checksum $crc
    set senderid $sender
    set senderthis $sthis
    set accheckrecv($this) "" 
    set localgroup  $_localgroup

    if {$remoteCheck != [list ""]} {
	if {[catch {uplevel "#0" $remoteCheck} res]} {
	    network SiteRequest $senderid COMP Ftp \
		    Eval $senderthis nack [SiteManager::MySiteId] $res
	    delete object $this
	    return
	} else {
	    if {$res != ""} {
		network SiteRequest $senderid COMP Ftp \
			Eval $senderthis nack [SiteManager::MySiteId] $res
		delete object $this
		return
	    }
	}
    }
    
    if {[file exist $rootdir/$f]} {
        set checksum2 [exec isabel_sum $rootdir/$f]
	if {$checksum == $checksum2} {
	    network SiteRequest $senderid COMP Ftp Eval $senderthis ack [SiteManager::MySiteId]
	    delete object $this
	    return
	}
	if {[catch {exec /bin/rm $rootdir/$f} emsg]} {
	    network SiteRequest $senderid COMP Ftp \
		    Eval $senderthis nack [SiteManager::MySiteId] $emsg
	    delete object $this
	    return
	}
    }

    set dirname [file dirname $rootdir/$f]
    catch {exec mkdir -p $dirname}
    if {[file writable $dirname] == 0} {
	set emsg "$dirname is not writable: permission denied."
	network SiteRequest $senderid COMP Ftp \
		Eval $senderthis nack [SiteManager::MySiteId] $emsg
	delete object $this
	return
    }

    component Cmd RDO cftp_create_channel($taskid)

    # start the receiver task
    set host 127.0.0.1
    set port 32112

    component Cmd RDO cftp_recv($taskid,$rootdir,$f)

    checkrecv $this $senderthis $senderid $rootdir $filename

    set afterlivetime [after $livetime [code $this livetime]]

    return    
}


#---------------------------------------------------------------
# ReceiverTask::destructor
#
#---------------------------------------------------------------

body ReceiverTask::destructor {} {
    
    component Cmd RDO cftp_end($taskid)
    component Cmd RDO cftp_delete_channel($taskid)
    catch {after cancel $accheckrecv($this)}
    if {[info exists accheckrecv($this)]} {
	unset accheckrecv($this)
    }
    after cancel $afterlivetime
}


#---------------------------------------------------------------
# ReceiverTask::checkrecv
#
# check if filename has been received.
#---------------------------------------------------------------

body ReceiverTask::checkrecv {mythis senderthis senderid rootdir filename} {

    if {[file exist $rootdir/$filename]} {
        DebugTraceMsg "- Ftp: $filename received."

	network SiteRequest $senderid COMP Ftp Eval $senderthis ack [SiteManager::MySiteId]
	delete object $mythis
    } else {
	set accheckrecv($mythis) [after 50 \
	    [code checkrecv $mythis $senderthis $senderid $rootdir $filename]]
    }
}


#---------------------------------------------------------------
# ReceiverTask::livetime
#
# wait until livetime expired to execute delete the task.
#---------------------------------------------------------------

body ReceiverTask::livetime {} {

    delete object $this
}


#===============================================================
#
#  SEND FILE
#
#===============================================================

#---------------------------------------------------------------
# FtpSendFile
#
# Create a task to Send a file
#---------------------------------------------------------------

process FtpSendFile {from_dirname from_filename to_erd to_filename to_sites lg_client livetime \
	ackcb nackcb donecb ltcb bw {remoteCheck ""}} {

    if {[catch {SenderTask "st#auto" $from_dirname $from_filename $to_erd $to_filename $to_sites $lg_client \
	    $livetime $ackcb $nackcb $donecb $ltcb $bw $remoteCheck\
	} res]} {
	ErrorTraceMsg "FTP: $res"
	return -1
    }
    
    return $res
}

#---------------------------------------------------------------
# Auxiliar method needed by the SldFtp component: RemoteCheck
#
# Check if the target directory is remote or local.
#---------------------------------------------------------------

process RemoteCheck {sld crc force} {
    global ISABEL_WORKDIR 
    
    set slddir  [file dirname $sld]
    set sldfile [file tail $sld]
    
    if { ! [file isdirectory $ISABEL_WORKDIR/$slddir]} {
	file mkdir $ISABEL_WORKDIR/$slddir
	close [open $ISABEL_WORKDIR/$slddir/.remote w]
	return ""
    }

    if { ! [file isfile $ISABEL_WORKDIR/$slddir/.remote]} {
	if {[file exists $ISABEL_WORKDIR/$sld]} {
	    set crc2 [exec isabel_sum $ISABEL_WORKDIR/$sld]
	    if {$crc != $crc2} {
		if {$force} {
		    set date [clock format [clock seconds] -format %Y%m%d-%H%M%S]
		    file rename  $ISABEL_WORKDIR/$sld $ISABEL_WORKDIR/.$sld.bk.$date
		} else {
		    set me [SiteManager::MySiteId]
		    set myname [SiteManager::GetSiteInfo $me name]
		    return "   Permission denied to ftp $slddir/$sldfile to $myname"
		}
	    }
	}
    }
    return ""
}

#---------------------------------------------------------------






