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
#   Communication with the coordinator manager
#
################################################################


#--------------------------------------------------------------
# Notepad_set_coordinator
#     Al crear una pagina, me pongo de coordinador
#--------------------------------------------------------------
process Notepad_set_coordinator {applid coordid pagid notify} {

    network MasterRequest XS CoordManager SetCoord $applid $coordid $pagid $notify
}


#--------------------------------------------------------------
# Notepad_change_coordinator
#     Me pongo de coordinador (GET FLOOR)
#--------------------------------------------------------------
process Notepad_change_coordinator {applid coordid pagid} {

    network MasterRequest XS CoordManager ChangeCoord $applid $coordid $pagid
}
 

#--------------------------------------------------------------
# Notepad_add_sync_user
#     A~ado a la lista de candidatos actualizados
#--------------------------------------------------------------
process Notepad_add_sync_user {applid userid pagid} {

    network MasterRequest XS CoordManager AddSyncUser $applid $userid $pagid
}


#--------------------------------------------------------------
# Notepad_del_sync_user
#     Elimino de la lista de candidatos actualizados
#--------------------------------------------------------------
process Notepad_del_sync_user {applid userid pagid} {

    network MasterRequest XS CoordManager DelSyncUser $applid $userid $pagid
}



#--------------------------------------------------------------
# Notepad_del_user
#     Aviso de que un usuario se ha ido, se borra su info
#     y es posible que fuera master de alguna pagina, eleccion
#     de coord para esas paginas
#--------------------------------------------------------------
process Notepad_del_user {applid userid} {

    network MasterRequest XS CoordManager DelUser $applid $userid
}



#--------------------------------------------------------------
# Notepad_unset_pag
#     Se cierra una pagina, borrar toda su info
#--------------------------------------------------------------
process Notepad_unset_pag {applid pagid} {

    network MasterRequest XS CoordManager UnsetPag $applid $pagid
}



#-------------------------------------------------------
# notepad_notify_new_coordinator:
#     Se ejecuta cuando un usuario se ha muerto y se
#     elige nuevo coordinador de una pag suya
#-------------------------------------------------------
process notepad_notify_new_coordinator {userid namepage} {
    global myuserid coorduserid writemode wroot
    
    if {$userid == $myuserid} {
	set coorduserid $myuserid
	set writemode 1
	$wroot.f.f2.write select
	focus_notepad
	NotepadSetState normal
	
    } else {
	write_mode_off
	set coorduserid $userid
    }
}

