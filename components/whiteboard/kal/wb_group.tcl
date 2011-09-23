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
# $Id: wb_group.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Esta clase esta asociada a los grupos de figuras
#   Los objetos de esta clase se nombran de la siguiente forma:
#      ${pag}fig$i
#   donde pag es la pagina a la que pertenecen e i es un entero
#
#########################################################################
 
 
 
class Group {
 
   inherit Figure

   constructor {pag fig clase} {}
 
   method create {pag fig}
   method move {difx dify}
   method copy {replicated}
   method select {}
   method delete {}
   method scale {factor centro_ref_x centro_ref_y}
   method rotate {center_ref_x center_ref_y}
   method symmetry_horizontal {posx posy}
   method symmetry_vertical {posx posy}
   method move_points {prop posx posy}

}
 
 
 
# ----------------------------------------------------------------------
#                             CONSTRUCTOR
# ----------------------------------------------------------------------
 
body Group::constructor {pag fig clase} {
 
   global page

   set numfig fig$fig
   set mypag $pag
   set canvas $page($pag,canvas)
   set type "group"

   # creo el grupo por peticion explicita del usuario y no por
   # gestion heredada de otros cmds
   if {$clase == "group"} { create $pag $fig }
}
 
 
 
 
# ----------------------------------------------------------------------
# METHOD: create
# ----------------------------------------------------------------------
 
body Group::create {pag fig} { 

   # existe seleccion
   global page 

   set list $page($pag,sel)
   set lista ""
   foreach elemen $page($pag,sel) {
      eval lappend lista [get_all_fig_from_group $pag $elemen]
   }
   delete_selection_cmd $pag

   # lista tiene todos los fig seleccionados

   foreach el $lista {
      $canvas addtag above withtag $el
   }

   set above [get_fig $canvas [$canvas find above above]]
   save_undo_group $pag $list

   if {$above != ""} {
      $canvas raise above $above
      foreach lem [$canvas find withtag above] {
         $canvas raise $above $lem
      }
   
   } else {
      $canvas raise above
   }

   $canvas dtag above above
   $canvas dtag bloque bloque

   set fig_list ""
   foreach el $list {
      $pag$el configure -father fig$page($pag,FigNumber)
      set ind [lsearch -glob $page($pag,FigList) $el ]
      set page($pag,FigList) [lreplace $page($pag,FigList) $ind $ind ]
   }

   ${pag}fig$page($pag,FigNumber) configure -figlist $list 
   lappend page($pag,FigList) fig$page($pag,FigNumber)

   ${pag}fig$page($pag,FigNumber) select 

   incr page($pag,FigNumber)
}

 
 
 

# ----------------------------------------------------------------------
# METHOD: move
# ----------------------------------------------------------------------
 
body Group::move {difx dify} {

   foreach el [cget -figlist] { $mypag$el move $difx $dify }
}




# ----------------------------------------------------------------------
# METHOD: copy
# ----------------------------------------------------------------------
 
body Group::copy {replicated} {

   # elemento es el original y replicated es la copia
   # como es un grupo por cada uno de los componentes hay que llamar a copy
   # de nuevo

   global page

   set fig_list_group [cget -figlist]
   set fig_list_group [ordena_lista $mypag $fig_list_group $numfig]

   set fig_list ""

   foreach el $fig_list_group {
      # crea el nuevo objeto de clase figura

      set replic $page($mypag,FigNumber)
      incr page($mypag,FigNumber)

      if {[$mypag$el cget -type] == "group"} {
         uplevel #0 "Group ${mypag}fig$replic $mypag $replic \"\""
      }

      # hace la copia
      $mypag$el copy $replic

      # actualiza el valor del padre en los hijos
      ${mypag}fig$replic configure -father fig$replicated

      # actualiza la lista de los hijos 
      lappend fig_list fig$replic
   }
   
   ${mypag}fig$replicated configure -figlist $fig_list
}


# ----------------------------------------------------------------------
# METHOD: select
# ----------------------------------------------------------------------
 
body Group::select {} {

   global page

   while {[cget -father] != ""} { set numfig [cget -father] }
	  
   set coords [eval $canvas bbox [get_all_fig_from_group $mypag $numfig]]

   lappend page($mypag,sel) $numfig
   configure -selec 1

   set arriba_x [lindex $coords 0]
   set arriba_y [lindex $coords 1]
   set abajo_x [lindex $coords 2]
   set abajo_y [lindex $coords 3]

   _create_point_sel $arriba_x $arriba_y punto1 $numfig
   _create_point_sel $abajo_x  $abajo_y  punto5 $numfig
   _create_point_sel $abajo_x  $arriba_y punto3 $numfig
   _create_point_sel $arriba_x $abajo_y  punto7 $numfig

   set medio_x [expr (abs($abajo_x - $arriba_x)/2)]
   set medio_y [expr (abs($abajo_y - $arriba_y)/2)]

   if {$arriba_x > $abajo_x} { set medio_x [expr $medio_x + $abajo+x]
   } else { set medio_x [expr $medio_x + $arriba_x]}

   if {$arriba_y > $abajo_y} { set medio_y [expr $medio_y + $abajo+y]
   } else { set medio_y [expr $medio_y + $arriba_y]}

   _create_point_sel $medio_x  $arriba_y punto2 $numfig
   _create_point_sel $medio_x  $abajo_y  punto6 $numfig
   _create_point_sel $arriba_x $medio_y  punto8 $numfig
   _create_point_sel $abajo_x  $medio_y  punto4 $numfig
}




 
# ----------------------------------------------------------------------
# METHOD: delete
# ----------------------------------------------------------------------
 
body Group::delete {} {

   global page

   foreach el [cget -figlist] { $mypag$el delete }

   if {[cget -selec]} {delete_selection}

   set ind [lsearch -glob $page($mypag,FigList) $numfig]

   if {$ind != -1} {
        set page($mypag,FigList) [lreplace $page($mypag,FigList) $ind $ind]
   }

   $this configure -exist 0
}




# ----------------------------------------------------------------------
# METHOD: scale
# ----------------------------------------------------------------------
 
body Group::scale {factor centro_ref_x centro_ref_y} {

   foreach el [cget -figlist] {
      $mypag$el scale $factor $centro_ref_x $centro_ref_y
   }
}


 
# ----------------------------------------------------------------------
# METHOD: rotate
# ----------------------------------------------------------------------
 
body Group::rotate {center_ref_x center_ref_y} {

   foreach el [cget -figlist] {
	  $mypag$el rotate $center_ref_x $center_ref_y
   }
}


# ----------------------------------------------------------------------
# METHOD: symmetry_horizontal
# ----------------------------------------------------------------------
 
body Group::symmetry_horizontal {posx posy} {

   global page
   set fig_list_group [cget -figlist ]
   foreach el $fig_list_group {
      uplevel #0 "$mypag$el symmetry_horizontal $posx $posy"
   }
}



# ----------------------------------------------------------------------
# METHOD: symmetry_vertical
# ----------------------------------------------------------------------
 
body Group::symmetry_vertical {posx posy} {

   global page
   set fig_list_group [cget -figlist ]
   foreach el $fig_list_group {
      uplevel #0 " $mypag$el symmetry_vertical $posx $posy "
   }
}




# ----------------------------------------------------------------------
# METHOD: move_points
# ----------------------------------------------------------------------
 
body Group::move_points {prop posx posy} {

   global centro_x centro_y punto_sel page

   set tipo [$this cget -type]
   if {$page($mypag,sel) != $numfig} {
      set list $page($mypag,sel)
      set ind [lsearch -glob $list $numfig]
      set list [lreplace $list $ind $ind]
      foreach el $list { $mypag$el delete_selection }
   }
   set factor [calculate_scale_factor_x $mypag $numfig $tipo \
        $punto_sel $centro_x $centro_y $posx $posy]
   exec_scale $mypag $factor $centro_x $centro_y
   $this select 
}





