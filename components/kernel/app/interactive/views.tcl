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


#---------------------------------------------------------------

#
# CreateJoinHideShowComp 
#
# If the component "compName" does not exist, then it is created.
#
# It the component exists but site "site_id" is not joined to it, then 
# he is joined.
#
# It the component has been created and site "site_d" is joined to it,
# the control user interface is hided or displayed.
#
proc CreateJoinHideShowComp {site_id compName} {

    if {![GlobalState::ComponentExists $compName]} {
	ComponentManager::StartComponent $site_id $compName
    } else {
	if {[lsearch -exact [GlobalState::GetComponentsByState $site_id "Waiting Running"] $compName] == -1} {
	    ComponentManager::StartComponent $site_id $compName
	} else {
	    network SiteRequest $site_id COMP $compName ShowHideApplToplevel withdraw
	}
    }
}

#---------------------------------------------------------------

#
# SetIMode
#
# Request to master to apply an interaction mode.
#
proc SetIMode {scenaryname {order ""} {site_id_list ""} args} {
    
    set _my_site_id [SiteManager::MySiteId] 
    set _my_name [SiteManager::GetSiteInfo $_my_site_id name] 

    uplevel #0 network MasterRequest CM InteractionModeRequest \
	[list $scenaryname] [list $site_id_list] $args \
	[list -scene [list \
			  -conference TeleMeeting \
			  -session  Session \
			  -presentation [session Activity GetResource $scenaryname scene.name $scenaryname] \
			  -site ${_my_site_id} \
			  -participant ${_my_name} \
			 ] \
	    ]
}

#---------------------------------------------------------------

#
# QuitSite.
#
# Quit my site.
#

proc QuitSite {} {

   if {[SiteManager::MyChId] == ""} {
       exit 1
   }
   if [tke_confirm "Are you sure you want to quit?" "quitsite" 10 0] {
      if {[SiteManager::MyChId] != -1} {
	  if {[session Environ cget -is_master] == "0"} {

	    set _my_site_id [SiteManager::MySiteId] 

	      SiteManager::SiteDisconnection $_my_site_id
	  } else {
	      network MasterRequest UM MasterDisconnection
	  }
	  after 5000 AtExit::ExitMsg 1 {"Master timeout expired."} {"Isabel exited."} 
      } else {
         exit 0
      }
   }
}
   
#---------------------------------------------------------------
