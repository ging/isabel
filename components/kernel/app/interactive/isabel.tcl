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
# isabel.itcl
#
# Starts an Isabel site.
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
    global env argc argv ISABEL_Version ISABEL_Release ISABEL_ROLE		

    set tool           [lindex $argv 0]
    set siteConfigFile [lindex $argv 1]

    LogsMessagesC::Initialice $tool

    WriteTraceMsg "Site role/tool: $tool"


    # Create the customer configuration store
    CustomerCfgC customercfg
    customercfg Dump

    BalloonClass balloon	

    # Create the session info object.
    WriteTraceMsg "Site configuration file: $siteConfigFile"
    SessionInfo session $siteConfigFile

    # Create the user interface object.
    UserInterfaceC interface "ISABEL [session Environ cget -site_id]"

    interface OpenLogo \
	    "$tool Site: $ISABEL_Version-$ISABEL_Release"

    #  Shortcuts to some environemnt variables.
    #   - ISABEL_DIR:         isabel directory.
    #   - ISABEL_WORKDIR:     work directory.
    #   - ISABEL_DEBUG:       debug mode.
    #   - ISABEL_ITCL:        use the itcl interpreter
    #   etc ...
    global ISABEL_DIR ISABEL_WORKDIR ISABEL_DEBUG ISABEL_TCL
    set ISABEL_DIR             [session Environ cget -install_dir]
    set ISABEL_WORKDIR         [session Environ cget -workdir]
    set ISABEL_DEBUG           [session Environ cget -debug_level]
    set ISABEL_TCL             [session Environ cget -tcl]

    interface LogoMsg "Waiting for SeCo network"
    NetworkC network 0 IsabelSite

    ServicesC services

    # SMS client
    SmsManagerClient sms

    # Connect with master	
    #network MasterConnect

    # Get event values defined by the master site.    
    network join_group isabel_master_event

    interface LogoMsg "Waiting for version number"
    session WaitForVersion
    interface LogoMsg ""

    interface LogoMsg "Waiting for event data"
    session WaitForEventData
    interface LogoMsg ""

    interface LogoMsg "Waiting for master environment"
    session Environ WaitForMasterEnviron
    interface LogoMsg ""

    # Wait for service definition
    network join_group isabel_master_service
    interface LogoMsg "Waiting for service definition"
    session Activity WaitForServiceDefinition
    interface LogoMsg ""

    interface DestroyLogo
    interface OpenUserActions

    SiteManager::SiteConnection

    if {[SiteManager::MyChId] == -1} {
	set w [SiteManager::GeMyUserInfo site_id]@[SiteManager::GeMyUserInfo host]
	ErrorTraceMsg "\"$w\" connection permission denied."
	exit 1
    }

    # Create Irouter task
    IrouterTaskC ::irouter
    IrouterConfigC::Start
    irouter AddListener IrouterConfigC::SetState
    #IrouterTrafficLogsC::Initialice

    network join_group isastate
    
    update idletasks	  
    DVirtualC dvirtual [session Environ cget -scenary_display]

    network join_group components

    network join_local_group local_isabel_client
    network join_group isabel_client

    network join_local_group isabel_snmp
}

#---------------------------------------------------------------

after 1 ISABEL_Main

#---------------------------------------------------------------

