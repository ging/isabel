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
# $Id: wb_ftp.tcl 22589 2011-06-08 06:43:49Z santiago $ 
#
# Description
#
#########################################################################


################################################################
# COMMANDS FROM MASTER: SYNC 
################################################################
# ack_ftp_read_master
# timeout_ftp_read_master
# done_ftp_read_master

################################################################
# COMMANDS FROM MASTER: OPEN IMAGE
################################################################
# ack_ftp_image

################################################################
# COMMANDS FROM MASTER: OPEN FILE
################################################################
# ack_ftp_open_file
# nack_ftp_open_file
# done_ftp_open_file
# timeout_ftp_open_file
#
################################################################


# ##############################################################
#
# Commands from master
#
# ##############################################################

#---------------------------------------------------------------
# ack_ftp_read_master
#
#   ordena al usuario que ha recibido correctamente el fichero
#   de sincronizacion que lo lea
#---------------------------------------------------------------

proc ack_ftp_read_master {name pag siteid} {
   global recibidos page

    set userid [SiteManager::GetSiteInfo $siteid chid]

   if {[$page($pag,canvas) find withtag sendingWindow]== ""} { 
       # El usuario ha abortado
       return 
   }

   set recibIdos [concat $recibidos $userid]
   ISA_RDOnode $userid read_file_from_master $pag
   ISA_RDOnode $userid done $pag
}

#---------------------------------------------------------------
# timeout_ftp_read_master
#
#   Se ha cumplido el timeout, los participantes que no hayan
#   recibido el fichero de sincronizacion seran avisados para
#   que si lo desean sincronicen de nuevo
#---------------------------------------------------------------

proc timeout_ftp_read_master {pag siteid {msg_err ""}} {
   global sent_done_user page

    set userid [SiteManager::GetSiteInfo $siteid chid]

   if {[$page($pag,canvas) find withtag sendingWindow]== ""} { 
       # El usuario ha abortado
       return 
   }

   .wbtl.state.f.info configure -text ""
   set sent_done_user($pag,$userid) 1
   ISA_RDOnode $userid msg_send_error $pag
}


#---------------------------------------------------------------
# done_ftp_read_master
#
#   Develve el control al maestro porque ya ha terminado la tx
#   del fichero de sincronizacion. El maestro podra operar
#   con la pagina otra vez.
#---------------------------------------------------------------

proc done_ftp_read_master {name siteid pag} {
   global sent_done_user page

    set userid [SiteManager::GetSiteInfo $siteid chid]

   if {[$page($pag,canvas) find withtag sendingWindow]== ""} { 
       # El usuario ha abortado
       return 
   }

   # --------------------------------------------
   # nadie ha abortado, el FTP ha terminado bien
   # --------------------------------------------
   set sent_done_user($pag,$userid) 1
}




################################################################
#
#   Commands from master when 'Open image' is invoked
#
################################################################



#---------------------------------------------------------------
# ack_ftp_image
#
#   Da la orden de crear el nuevo objeto Image en el nodo que
#   ha recibido el fichero
#---------------------------------------------------------------
 
proc ack_ftp_image {name pag posx posy siteid} {
    # proc que se ejecuta en el master cuando llega la conf de un usuario
    # ojo! porque del ultimo no le llega
    global recibidos general page send_done

    set userid [SiteManager::GetSiteInfo $siteid chid]

    trace_busy "ack_ftp_image:: ACK FTP IMAGE: $userid"
    if {$send_done == -1} { return }

    ISA_RDOnode $userid create_file_image $pag image \
        [list $posx $posy] \"$general(Color)\" \
        \"$general(FillColor)\" \"$general(Stipple)\" $general(Width) \
        $general(Arrow) $general(ArrowShape) $general(Font) \
        $general(Point) $general(Weight) $general(Anchor) \
        $general(Justif) $name
    set recibidos [concat $recibidos $userid]
}


proc create_file_image {text type coords color outline stipple width \
 arrow arrowshape font point weight anchor justif name} {

    global page ISABEL_TMP_DIR

    set fg fig$page($text,FigNumber)
    lappend page($text,FigList) $fg
 
    create_figure $text$fg $text \
         $page($text,FigNumber) image $coords "" \
         "" "" "" "" "" "" "" "" $anchor \
         $justif ${ISABEL_TMP_DIR}/$name

    save_undo_paint_items $text fig$page($text,FigNumber)

    incr page($text,FigNumber)
}
 



# ##############################################################
#
#   Commands from master when 'Open file' is invoked
#
# ##############################################################
 


#---------------------------------------------------------------
# ack_ftp_open_file: 
#
#   Da la orden de abrir el fichero y cargar su contenido
#   en la pagina correspondiente, al nodo que ha recibido 
#   el fichero
#---------------------------------------------------------------

proc ack_ftp_open_file {name actual_page siteid} {
   global recibidos send_done

    set userid [SiteManager::GetSiteInfo $siteid chid]

   if {$send_done == -1} { return }
   ISA_RDOnode $userid read_file_node $name $actual_page
   set recibidos [concat $recibidos $userid]
}
 
proc read_file_node {name pag} {
   global ISABEL_TMP_DIR
   read_file $ISABEL_TMP_DIR/$name $pag
}
 




#---------------------------------------------------------------
# done_ftp_open_file:
#
#   Devuelve el control a la persona que dio la orden de abrir
#   el fichero para que siga realizando operaciones sobre la 
#   pagina. El envio del fichero ha terminado
#---------------------------------------------------------------

proc done_ftp_open_file {} {
   global send_done
   if {$send_done==-1} { return }
   set send_done 1
}
 



#---------------------------------------------------------------
# timeout_ftp_open_file:
#
#   Ha vencido el timeout. Los participantes que no hayan 
#   recibido el fichero seran avisados para que puedan 
#   sincronizar
#---------------------------------------------------------------

proc timeout_ftp_open_file { actual_page members } {
  # los que han mandado ack ya han cargado el fichero, los que han
  # mandado nack ya se les ha avisado, hay que manejar el resto
  global recibidos send_done
  if {$send_done == -1} { return }

  # Nadie ha abortado aun la transmision
  if {$members != ""} {
     send_msg_no_recibidos $actual_page $members
  }
  set send_done 1
}
 
 
proc send_msg_no_recibidos {pag members } {
    global recibidos send_done page
    # --------------------------------------------------
    # Envia a todos los que no han recibido el fichero
    # via FTP, un mensaje de error
    # --------------------------------------------------
    set us ""
    for {set i 0} {$i < [llength $members] } {incr i} {
        set el [lindex $members $i]
        if {[lsearch -exact $recibidos $el] == -1} {
           set us [concat $us $el]
        }
    }
    foreach m $us {
       ISA_RDOnode $m msg_send_error $pag
    }
}
 




#---------------------------------------------------------------
# nack_ftp_open_file
#
#   Avisa al participante de que ha habido un error en la tx
#   de su fichero para que sincronice con el maestro
#---------------------------------------------------------------

proc nack_ftp_open_file {pag siteid error_msg} {

   global recibidos page send_done


    set userid [SiteManager::GetSiteInfo $siteid chid]

   trace_busy "nack_ftp_open_file:: NACK $userid"
   if {$send_done == -1} { return }

   set recibidos [concat $recibidos $userid]
   ISA_RDOnode $userid msg_send_error $pag
   set send_done 1
}
 


proc msg_send_error {pag} {

  global page active_pag uniqueMaster
  set page($pag,enable) 0

  if {$pag == $active_pag} {
     if {!$uniqueMaster} {
        .wbtl.barra.floor.floor deselect
     }
     desactiva_eventos
     change_interface disabled
  }

  set page($pag,enable) 0
}
 

