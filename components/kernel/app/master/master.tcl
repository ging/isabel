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
# master.tcl.version
#
# The Master of the ISABEL conference application.
#
#########################################################################

#===============================================================
# ISABEL Version and Release. 
#===============================================================
set ISABEL_Version $env(ISABEL_VERSION)
set ISABEL_Release $env(ISABEL_RELEASE)


#===============================================================
#  EXIT CLEANUP
#===============================================================


#===============================================================
# MAIN
#===============================================================

process ISABEL_Main {} {
    global env argv ISABEL_Version ISABEL_Release

    set siteConfigFile [lindex $argv 0]

    LogsMessagesC::Initialice Master
    WriteTraceMsg "Starting Master $ISABEL_Version-$ISABEL_Release"

    # Create the customer configuration store
    CustomerCfgC customercfg
    customercfg Dump

    # Create the user interface object.
    UserInterfaceC interface
    interface OpenLogo "Master Site - $ISABEL_Version-$ISABEL_Release"
    
    # Create the session info object.
    WriteTraceMsg "Site configuration file: $siteConfigFile"
    SessionInfo session $siteConfigFile
    
    # Save master isabel version.
    session SetMasterInfo isabel_version $ISABEL_Version	 	
	
    #  Shortcuts to some environemnt variables.
    #   - ISABEL_DIR:         isabel directory.
    #   - ISABEL_WORKDIR:     work directory.
    #   - ISABEL_DEBUG:       debug mode.
    #   - ISABEL_ITCL:        use the itcl interpreter
    #   - etc ...
    global ISABEL_DIR ISABEL_WORKDIR ISABEL_DEBUG ISABEL_TCL
    set ISABEL_DIR             [session Environ cget -install_dir]
    set ISABEL_WORKDIR         [session Environ cget -workdir]
    set ISABEL_DEBUG           [session Environ cget -debug_level]
    set ISABEL_TCL             [session Environ cget -tcl]
    
    # Create the services object.
    interface LogoMsg "Creating SW services"
    ServicesC services
    interface LogoMsg ""

    # Load the activity.
    interface LogoMsg "Loading Service"
    session Activity Load $env(ISABEL_SETUP_DIR)/[session Environ cget -session_id]/service
    interface LogoMsg ""

    # Create the SMS Manager
    SmsManagerMaster smsm

    # SMS client
    SmsManagerClient sms

    # Create the network object.
    interface LogoMsg "Starting network"
    NetworkC network 1 Master
    interface LogoMsg ""

    # Publish the master environment in the "isabel_master_event" group of SeCo
    session PublishVersion
    session PublishMasterEnviron
    session PublishEventData

    # Publish the service in the "service" group of SeCo
    session Activity PublishService

    # Auto Quit
    SiteManager::ConnectionCB Add AutoQuitC::refresh    
    SiteManager::DisconnectionCB Add AutoQuitC::refresh    

    # Load master plugins:
    foreach _mp [glob -directory $ISABEL_DIR/lib/isatcl/masterplugins -tails -nocomplain *] {
	WriteTraceMsg "Master - Loading pluging \"$_mp\""
	if {[catch {source $ISABEL_DIR/lib/isatcl/masterplugins/$_mp} _errmsg]} {
	    ErrorTraceMsg "Master - pluging \"$_mp\" produces: $_errmsg"
	}
    }

    # Default/initial interaction mode
    set _actname [session Activity GetName]
    set _defim   [session Activity GetResource $_actname imode ""]
    if {$_defim != ""} {
	catch "ComponentManager::InteractionModeRequest $_defim"
    }

    update idletasks	  

    DVirtualC dvirtual [session Environ cget -scenary_display]

    interface DestroyLogo
    interface Open
}   

#---------------------------------------------------------------

after 1 ISABEL_Main

#---------------------------------------------------------------


