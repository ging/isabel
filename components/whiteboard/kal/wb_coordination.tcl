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
# $Id: wb_coordination.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#           GESTION DEL TESTIGO O PERMISO DE ESCRITURA
#
#########################################################################


############################################################################
#			FLOOR MANAGEMENT
############################################################################

############### External Functions #########################################
# 
# ask_floor
# lock_floor
# write_mode_on
# write_mode_off
#
############################################################################


proc changeCoordinator {} {
    global myuserid page all_pages mysite_id

    # Compruebo que estoy sync con todas las paginas
    foreach pag $all_pages {
        if {(!$page($pag,enable)) || ($page($pag,lock))} { 
           wb_tk_msg .wbtl.alert  Alert \
               "The page $pag is not synchnonized" Continue 0
           return
        }
    }

    # Estoy sync con todas las paginas 
    foreach pag $all_pages {
        ISA_RDOgroup Cmd only_if_enabled_page $pag \
           get_floor $pag $myuserid $mysite_id
    }
}


# --------------------------------------------------------------------------
# ask_floor
#     Pide el testigo al coordinador de la pagina
# --------------------------------------------------------------------------
proc ask_floor {} {
   global page active_pag myuserid mysite_id uniqueMaster

   if {![info exist active_pag]} {
        puts "- isabel_whiteboard Error:: There is no active page"
        return
   }

   if {$page($active_pag,write)} {
      if {!$uniqueMaster} { 
          .wbtl.barra.floor.floor select
      }
      return
   } 
   if {!$uniqueMaster} {
      .wbtl.barra.floor.floor deselect
   }

   if {($page($active_pag,enable)) && (!$page($active_pag,lock)) } {
       ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
          get_floor $active_pag $myuserid $mysite_id

   } elseif {$page($active_pag,lock)} {
       wb_tk_msg .wbtl.alert  Alert "The page $active_pag is locked" Continue 0

   } else {
       wb_tk_msg .wbtl.warning Warning "Your page is inconsistent.\n \
            Press 'Read Master' in File Menu." OK 0
   } 
}

# --------------------------------------------------------------------------
# get_floor
#     Si soy el coordinador y la pagina no esta bloqueada cedo el
#     testigo tarde o temprano (cuando este desocupado)
# --------------------------------------------------------------------------

proc get_floor {request_pag request_userid name} {
   global page busy peticion min_peticion max_peticion wait_peticion
   
   if {!$page($request_pag,write)} { return }

   if {$page($request_pag,lock)} {
       # pagina bloqueada
       ISA_RDOnode Cmd only_if_enabled_page $request_pag \
           $request_userid wb_tk_msg \
           .wbtl.alert Alert "The page $request_pag is locked" Continue 0
       return

   } elseif {(($busy >0) || ($page($request_pag,sending) == 1))} {
       # encolo la peticion
       while {$wait_peticion} {
           tkwait variable $wait_peticion
       }
       set wait_peticion 1
       for {set i $min_peticion} {$i < $max_peticion} {incr i} {
           if {(($peticion($i,cmd) == "floor") && \
                ($peticion($i,pag) == $request_pag) && \
                ($peticion($i,user) == $request_userid) && \
                ($peticion($i,name) == $name))} {
              set wait_peticion 0
              return
           }
       }
       set peticion($max_peticion,cmd) "floor"
       set peticion($max_peticion,pag) $request_pag
       set peticion($max_peticion,user) $request_userid
       set peticion($max_peticion,name) $name

       incr max_peticion
       set wait_peticion 0
       return -1
    } 

    # ---------------------------
    # atiendo la peticion
    # ---------------------------
    incr busy

    change_floor_variables $request_pag $name $request_userid

    incr busy -1
    examine_peticion_queue
}


# --------------------------------------------------------------------------
# no_get_floor
#     No me ceden el testigo y en la pantalla me avisan con el siguiente msg
# --------------------------------------------------------------------------
proc no_get_floor { } {
    wb_tk_msg .wbtl.warning Warning "Your must wait for coordinator's pictures \
         before editing." OK 0
}



proc wb_tk_msg {window title msg button_msg activa_eventos} {
   catch {destroy $window}

    desactiva_eventos  
    toplevel $window 
    update
    after idle grab $window
    wm title $window $title
 
    frame $window.top -relief groove -bd 2
    label $window.top.label -text $msg \
         -font -*-times-medium-R-Normal--*-180-*-*-*-*-*-*
 
    frame $window.bottom -bd 2
    if {$activa_eventos} {
        button $window.bottom.button -text $button_msg  \
		-command "activa_eventos; destroy $window"
    } else {
        button $window.bottom.button -text $button_msg  \
		-command "desactiva_eventos; destroy $window"
    }
 
    bind $window <Key-Return> "$window.bottom.button invoke"

    pack $window.top $window.bottom -side top -expand 1 -fill x
    pack $window.top.label $window.bottom.button -expand 1

    tkwait window $window
}



# --------------------------------------------------------------------------
# lock_floor
#    Bloquear una pagina de la que soy coordinador
# --------------------------------------------------------------------------

proc lock_floor { } {
   global page active_pag balloon 
   if {!$page($active_pag,write)} { return }

   if {$page($active_pag,lock)} {
      set page($active_pag,lock) 0
      .wbtl.barra.floor.lock configure -image unlock
      balloon add .wbtl.barra.floor.lock "Lock Floor"
      ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
          remote_unlock_floor $active_pag
      return
   } 
   # ---------------------------------
   # Pagina no bloqueada, la bloqueo
   # ---------------------------------
   set page($active_pag,lock) 1
   .wbtl.barra.floor.lock configure -image lock
   balloon add .wbtl.barra.floor.lock "Unlock Floor"
   ISA_RDOgroup Cmd only_if_enabled_page $active_pag remote_lock_floor $active_pag
}

proc remote_unlock_floor {pag} {
    global active_pag page 
    set page($pag,lock) 0
    if {$pag == $active_pag} {
        .wbtl.barra.floor.lock configure -image unlock
        balloon add .wbtl.barra.floor.lock "Lock Floor"
    } 
}

proc remote_lock_floor {pag} {
    global active_pag page 
    set page($pag,lock) 1
    if {$pag == $active_pag} {
       .wbtl.barra.floor.lock configure -image lock
        balloon add .wbtl.barra.floor.lock "Unlock Floor"
    }
}

# --------------------------------------------------------------------------
# write_mode_on
#    Si se me permite ser coordinador habilito la interfaz y le digo al
#    resto de participantes que yo soy el nuevo coordinador
# --------------------------------------------------------------------------
proc write_mode_on {pag} {
    global active_pag page uniqueMaster

   if {!$page($pag,enable)} {
       # WriteTraceMsg "You must wait for master's pictures before editing"
        wb_tk_msg .wbtl.msg  \
            Warning "You must wait for master's pictures before editing" OK 0
        if {!$uniqueMaster} {
            .wbtl.barra.floor.floor deselect
        }
	return
    }
    ISA_RDOgroup Cmd only_if_enabled_page $pag write_mode_off $pag
    set page($pag,write) 1
    if {$active_pag == $pag} {
        if {!$uniqueMaster} {
            .wbtl.barra.floor.floor select
        }
        activa_eventos
        change_interface normal
    }
}


# --------------------------------------------------------------------------
# write_mode_off
#    Deshabilito la interfaz porque yo no soy el coordinador
# --------------------------------------------------------------------------
proc write_mode_off {pag} {

    global active_pag page uniqueMaster

    if {![info exist active_pag]} {return}
    set page($pag,write) 0
    if {$pag == $active_pag} {
       if {!$uniqueMaster} {
           .wbtl.barra.floor.floor deselect
       }
       desactiva_eventos  
       change_interface disabled
    }
}


