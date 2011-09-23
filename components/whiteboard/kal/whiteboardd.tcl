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
# $Id: whiteboardd.tcl 22653 2011-06-14 08:58:06Z santiago $ 
#
# Description
#
#########################################################################


set ISABEL_DIR .
catch {set ISABEL_DIR $env(ISABEL_DIR)}

set ISABEL_WORKDIR .
catch {set ISABEL_WORKDIR $env(ISABEL_WORKDIR)}

set ISABEL_TMP_DIR .
catch {set ISABEL_TMP_DIR $env(ISABEL_TMP_DIR)}

set WB_TMP_DIR .
catch {set WB_TMP_DIR $ISABEL_TMP_DIR/isabel_whiteboard}

#===============================================================

global ISABEL_DEBUG

if {[info exists env(ISABEL_DEBUG)]} {
   switch -- $env(ISABEL_DEBUG) {
       0 {set ISABEL_DEBUG 0}
       1 {set ISABEL_DEBUG 1}
       2 {set ISABEL_DEBUG 2}
       3 {set ISABEL_DEBUG 3}
       default {set ISABEL_DEBUG 0}
   }
} else {
   set ISABEL_DEBUG 0
}

LogsMessagesC::Initialice Whiteboard
 
#===============================================================

set PRINTER lp
catch {set PRINTER $env(PRINTER)}


###############################################################
###############################################################
#
#   Funciones para la compatibilidad con la parte de 
#   comunicaciones anterios a SeCo
#
###############################################################
###############################################################

proc ISA_RDOgroup {args} {

    network RDOGroup whiteboard $args
}

proc ISA_RDOnode {userid args} {

    foreach _site_id [SiteManager::GetSites] {
	if {$userid == [SiteManager::GetSiteInfo $_siteid chid]} { 
	    eval network SiteRequest $_site_id COMP Whiteboard Cmd $args
	}
    }
}


###############################################################
###############################################################
#
#   Miscellaneous functions
#
###############################################################
###############################################################

# -------------------------------------------------------------
# WhiteboardAddPartner
# -------------------------------------------------------------

proc WhiteboardAddPartner {userid} {
    
    DebugTraceMsg "- Whiteboard: WhiteboardAddPartner USERID=$userid" 3
}


# -------------------------------------------------------------
# WhiteboardRemovePartner
# -------------------------------------------------------------

proc WhiteboardRemovePartner {userid} {
    Wb_del_user Whiteboard $userid
}


# -------------------------------------------------------------
# WhiteboardSync
#    Lo ejecutan todos los usuarios que estaban conectados
#    Envia una lista de todas las paginas 
# -------------------------------------------------------------

proc WhiteboardSync {userid} {
   global all_pages myuserid page 

   DebugTraceMsg "- Whiteboard: Enviando lista pag  $userid $all_pages " 3

    foreach _site_id [SiteManager::GetSites] {
	if {$userid == [SiteManager::GetSiteInfo $_siteid chid]} { 

	    network SiteRequest $_site_id COMP Whiteboard Cmd create_list_of_pages $all_pages

	    DebugTraceMsg "- Whiteboard: ESTADO MAP= [wm state .wbtl]" 3
	    if {[wm state .wbtl] == "normal" } {
		network SiteRequest $_site_id COMP Whiteboard Cmd pizarra_map
	    }
	}
    }
}


# -------------------------------------------------------------
# StartAsCoordinator
#    El primer usuario crea las n paginas iniciales
# -------------------------------------------------------------

proc StartAsCoordinator {n_pag} {
    for {set i 1} {$i <= $n_pag} {incr i} {
        new_page
    }
}



proc InitWhiteboard {} {

    InitPizarraGUI

    PutResize [session Activity GetResource Whiteboard resizable 0]

    update

}



###############################################################
#
# Main.
#
###############################################################

proc ISABEL_CPizarra {} {
    global myuserid myhost mysite_id \
	geometry ISABEL_DIR WB_TMP_DIR \
	env uniqueMaster uniqueView
    
    DebugTraceMsg "- Whiteboard: ISABEL_CPizarra" 3
    
    set myuserid         [SiteManager::MyChId]
    set mysite_id         $env(ISABEL_SITE_ID)
    
    set layout   [session Activity GetResource Whiteboard layout "700x600"]
    set geometry [dvirtual V2R_Layout $layout]
    
    set _display [session Environ cget -scenary_display]

    set myhost      [exec hostname]

    init_var

    toplevel .wbtl -screen $_display

    wm protocol .wbtl WM_DELETE_WINDOW "wm withdraw .wbtl"

    wm iconname   .wbtl Whiteboard
    wm iconbitmap .wbtl @$ISABEL_DIR/lib/images/xbm/apple_drawing.xbm
    wm withdraw .wbtl

    # Opciones del fichero de actividad
    set uniqueMaster [session Activity GetResource Whiteboard uniqueMaster 0]
    set uniqueView   [session Activity GetResource Whiteboard uniqueView   0]

    # Arrancando el objeto scanner
    check_tmp_whiteboard_dir

    ScannerDialogWindowC wb_scanner $WB_TMP_DIR done_scanner \
        close_scanner error_scanner 1

    
    # Titulo de la ventana
    set wb_title [session Activity GetResource Whiteboard title ""]
    wm title .wbtl $wb_title

    # Creando la interfaz
    DebugTraceMsg "- Whiteboard: Initializing interface..." 3
    InitWhiteboard
    DebugTraceMsg "- Whiteboard: Interface initialized" 3

    network join_group whiteboard
}




