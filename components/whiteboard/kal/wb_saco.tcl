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
# $Id: wb_saco.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#    Fichero en vias de desaparecer :-)
#
#########################################################################
 

# ##############################################################
#
# Misc functions
#
# ##############################################################


#---------------------------------------------------------------
# get_tags: obtiene todas las opciones con su valor, preparadas
#		para poder crear la figura
#---------------------------------------------------------------
proc get_tags {opciones_t arg} {
    for {set m [expr [llength $opciones_t] -1]} {$m >= 0} {incr m -1} {
	# opcion_t mantiene la opcion y los 4 ultimos valores
        set opcion_t [list_range $opciones_t $m]
	# ultimo mantiene el ultimo valor de la opcion
        set ultimo [list_range $opcion_t 4]
        if {$ultimo != ""} {
            set opcion1 [list_range $opcion_t 0]
            set arg [concat $arg $opcion1]
            set opcion2 [list_range $opcion_t [expr [llength $opcion_t]-1]]
            if {$opcion1=="-text"} {
                set opcion2 [format "\"$opcion2\""]
            } else {
                set opcion2 [list $opcion2]
            }
 
            set arg [concat $arg $opcion2]
        }
    }
    return $arg
}



#---------------------------------------------------------------
# list_range: extrae el elemento numero num_lista de la cadena 
#		y le quita las llaves
#---------------------------------------------------------------
 
proc list_range {cadena num_lista} {
    set cadena [lindex $cadena $num_lista]
    set l_llaves [expr [string length $cadena] -2]
    set ltotal [expr [string length $cadena] - 1]
    set primer [string range $cadena 0 0]
    set ultimo [string range $cadena $ltotal $ltotal]
    if {($primer == "{") &&  ($ultimo == "}")}  {
        return  [string range $cadena 1 $l_llaves]
    } else {return $cadena }
}
 


proc trace_busy {msg} {
   global busy
   # puts stdout "$msg $busy"
}

 
proc trace_msg_error { msg} {
   # saca un mensaje de error real
   ErrorTraceMsg "- Whiteboard ERROR: $mgs"
}

 
