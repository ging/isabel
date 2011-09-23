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
# $Id: wb_scanner.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#########################################################################

# --------------------------------------------------------
# scanner
#     Llama a la funcion de scanner para generar una 
#     imagen GIF en la pagina.
# --------------------------------------------------------
proc scanner {} {
    global busy 
    desactiva_eventos
    incr busy
    wb_scanner Open
}

# --------------------------------------------------------
# done_scanner
#     Fichero listo para leerlo y enviarselo al resto
#     de participantes
# --------------------------------------------------------
proc done_scanner {filename} {
    global busy active_pag page
    incr busy
    $page($active_pag,canvas) configure -cursor clock
    wb_scanner Close
    desactiva_eventos
    place_image $filename
    $page($active_pag,canvas) configure -cursor left_ptr
}

# --------------------------------------------------------
# error_scanner
#     Ha habido algun error al escanear, saco mensaje
#     de error
# --------------------------------------------------------
proc error_scanner {error_msg=""} {
    global busy
    # -------------------------
    # saco mensaje de error
    # -------------------------
    wb_tk_msg .wbtl.errorScan "Scanner Error" "Scanner Error: $error_msg" \
        Continue 0
    incr busy -1
    examine_peticion_queue
}

proc close_scanner {} {
    global busy latestAction
    # -------------------------
    # saco mensaje de error
    # -------------------------
    incr busy -1
    eval $latestAction
}


