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
# $Id: wb_cmanager.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#    COORDINATOR CENTER COMMUNICATION
#
#########################################################################

# --------------------------------------------------------------
# Wb_set_coordinator
#     Al crear una pagina, me pongo de coordinador
# --------------------------------------------------------------

process Wb_set_coordinator {applid coordid pagid notify nobodysync} {

   network MasterRequest XS CoordManager SetCoord $applid $coordid $pagid \
      $notify $nobodysync
}


# --------------------------------------------------------------
# Wb_change_coordinator
#     Me pongo de coordinador (GET FLOOR)
# --------------------------------------------------------------

process Wb_change_coordinator {applid coordid pagid} {

    network MasterRequest XS CoordManager ChangeCoord $applid $coordid $pagid
}


# --------------------------------------------------------------
# Wb_add_sync_user
#     A~ado a la lista de candidatos actualizados
# --------------------------------------------------------------

process Wb_add_sync_user {applid userid pagid} {

    network MasterRequest XS CoordManager AddSyncUser $applid $userid $pagid
}


# --------------------------------------------------------------
# Wb_del_sync_user
#     Elimino de la lista de candidatos actualizados
# --------------------------------------------------------------

process Wb_del_sync_user {applid userid pagid} {

   network MasterRequest XS CoordManager DelSyncUser $applid $userid $pagid
}


# --------------------------------------------------------------
# Wb_del_user
#     Aviso de que un usuario se ha ido, se borra su info
#     y es posible que fuera master de alguna pagina, eleccion
#     de coord para esas paginas
# --------------------------------------------------------------

process Wb_del_user {applid userid} {

    network MasterRequest XS CoordManager DelUser $applid $userid
}


# --------------------------------------------------------------
# Wb_unset_pag
#     Se cierra una pagina, borrar toda su info
# --------------------------------------------------------------

process Wb_unset_pag {applid pagid} {

    network MasterRequest XS CoordManager UnsetPag $applid $pagid
}


# --------------------------------------------------------------
# wb_notify_new_coordinator
#     El CManager designa un nuevo coordinador de la pagina 
#
# --------------------------------------------------------------

process wb_notify_new_coordinator {userid pag} {
    global myuserid page mysite_id active_pag latestAction all_pages \
           uniqueMaster

    # Soy el nuevo coordinador designado
    if {$userid == $myuserid} {
        set page($pag,write) 1
        set page($pag,master_id) $myuserid
        set page($pag,master_name) $mysite_id

        if {$active_pag == $pag} {
            if {!$uniqueMaster} {
                .wbtl.barra.floor.floor select
            }
            activa_eventos
            eval $latestAction
            change_interface normal
        }
        ISA_RDOgroup Cmd only_if_enabled_page $pag \
            set page($pag,master_name) $mysite_id

    } else {
        if {[info exist page($pag,canvas)]} {
            write_mode_off $pag
            set page($pag,master_id) $userid
        }
    }
}

process wb_nobody_is_synch {userid pag} {
    # Sacar un mensaje 
    global page dir_iconos
    frame $page($pag,canvas).f1 -bd 4 -relief groove
    pack $page($pag,canvas).f1 -padx 1c -pady 1c

    label $page($pag,canvas).f1.bitmap -bitmap @$dir_iconos/adm.xbm \
        -foreground red
    frame $page($pag,canvas).f1.f2
    pack $page($pag,canvas).f1.bitmap -side left -padx 0.5c -pady 0.5c
    pack $page($pag,canvas).f1.f2

    label $page($pag,canvas).f1.f2.label \
        -font -*-times-bold-R-Normal--*-160-*-*-*-*-*-* \
        -text "Nobody is the page master of $pag"

    button $page($pag,canvas).f1.f2.button -text "Close $pag page" \
        -command "nobodyIsMaster $pag"

    pack $page($pag,canvas).f1.f2.label \
        -padx 0.5c -pady 0.5c
    pack $page($pag,canvas).f1.f2.button \
        -padx 0.5c -pady 0.5c
}

process nobodyIsMaster {pag} {
    global all_pages
    if {[lsearch -glob $all_pages $pag] == -1} {
        puts "- isabel_whiteboard Error:: \
              The page $active_page does not seem to exist"
        return
    }

    libera $pag
    set len [llength $all_pages]
    if {$len > 0} {
        incr len -1
        set newPage [lindex $all_pages $len]
        .wbtl.f1.note select $newPage

    } else {
        # vamos a cerrar la unica paginaabierta que habia, abrir otra
        new_page
        ISA_RDOgroup Cmd .wbtl.f1.note select $active_pag
    }

    delete_page $pag
}

