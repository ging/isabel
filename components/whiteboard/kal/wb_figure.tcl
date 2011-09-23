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
# $Id: wb_figure.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Esta clase esta asociada a cualquier figura, de esta se 
#   heredaran, cada tipo de figura, sus variables y metodos
#
#########################################################################
 

class Figure {

# Tag que identifica a la figura
   public variable numfig ""      
   public variable father ""
   public variable cobj ""
   public variable type ""
   public variable figlist ""
   public variable undo ""
   public variable exist 1
   public variable selec 0
 
 
# Canvas de la figura
   protected variable canvas ""   
# Pagina de la figura
   protected variable mypag ""    


# Caracteristicas de las figuras
   protected variable _opciones ""
   protected variable _coords ""
   public method _exec_show_apply {new_coords options} 
   protected method _show_cancel {} 
   protected method _show_apply {} 
   protected method _show_change {} 
   protected method _show_calculate_tags {} {}
   protected method _show_calculate_coords {} {}


   public method move {difx dify}
   public method delete_selection {}
   public method delete {}
   public method scale {factor centro_ref_x centro_ref_y}
   public method rotate {center_ref_x center_ref_y}
   public method new_center {center_fig center_ref_x center_ref_y } 
   public method align {align}
   public method calculate_coords_symmetry_hor {posx posy}
   public method calculate_coords_symmetry_ver {posx posy}
   public method above_level {}
   public method redo {} 
   public method change_coords {coods} 

   public method symmetry_horizontal {posx posy}
   public method symmetry_vertical {posx posy}

   public method move_points {prop posx posy} {}
   public method show_info {} {}
   public method copy {replicated} {}
   public method select {} {}

   public method saveFile {fileId swapFileId}

   protected method _create_point_sel {x y prop tag}

   private method _rotate_coords {coords}
   private method _sel_coords_align {coords coord op_logica } 
}



# ----------------------------------------------------------------------
# METHOD: move
#
#   Mover una figura
# ----------------------------------------------------------------------

body Figure::move {difx dify } {
   global page

   if {!$page($mypag,enable)} {return}
   $canvas move $numfig $difx $dify
}



#---------------------------------------------------------------
# METHOD: _create_point_sel 
#
#   Pinta el cuadradito rojo de seleccion de figura en las 
#   coordenadas (x,y)
#---------------------------------------------------------------

body Figure::_create_point_sel {x y prop tag} {
    global page
    set list [list sel$tag $prop]
    set page($mypag,id) [eval $canvas create rectangle [expr $x - 3] \
       [expr $y -3] [expr $x +3] [expr $y + 3] -fill red -tag {$list}]
}
 


# ----------------------------------------------------------------------
# METHOD: delete_selection
# 
#   Borrar los cuadraditos rojos de seleccion
# ----------------------------------------------------------------------

body Figure::delete_selection {} {

  global page
  if {!$page($mypag,enable)} { return }

  configure -selec 0
  $canvas delete sel$numfig
  $canvas dtag $numfig "resaltado"
  set ind [lsearch -glob $page($mypag,sel) $numfig]
  if {$ind != -1} {
     set page($mypag,sel) [lreplace $page($mypag,sel) $ind $ind ]
  } else  {
     puts "- isabel_whiteboard Error: The figure=$numfig should be selected"
  }
}





# ----------------------------------------------------------------------
# METHOD: delete
#
#   Borrar una figura
# ----------------------------------------------------------------------

body Figure::delete {} {
   global page

   if {!$page($mypag,enable)} {return}

   if {[cget -selec]} { delete_selection }

   configure -exist 0
   $page($mypag,canvas) delete $numfig
   set ind [lsearch -glob $page($mypag,FigList) $numfig]

   if {$ind != -1} {
      set page($mypag,FigList) [lreplace $page($mypag,FigList) $ind $ind]
   }
}






# ----------------------------------------------------------------------
# METHOD: scale
#
#   Escalar una figura, mantiene la forma
# ----------------------------------------------------------------------

body Figure::scale {factor centro_ref_x centro_ref_y } {
   global page

   if {!$page($mypag,enable)} { return }

   set coords [$canvas coords $numfig]
   set center [calculate_center $coords]
   set centro_x [lindex $center 0]
   set centro_y [lindex $center 1]
   set dist_x [expr ($factor - 1)* ($centro_x - $centro_ref_x)]
   set dist_y [expr ($factor - 1) * ($centro_y - $centro_ref_y)]
   $canvas scale $numfig $centro_x $centro_y $factor $factor
   $canvas move $numfig $dist_x $dist_y
}





# ----------------------------------------------------------------------
# METHOD: rotate
#
#   Girar una figura noventa grados
# ----------------------------------------------------------------------
 
body Figure::rotate {center_ref_x center_ref_y} {
 
     global page

     set coords [$canvas coords $numfig]
     set coords_undo $coords
 
     set coords [_rotate_coords $coords]
     set center_fig [calculate_center $coords_undo]
     set new_center [new_center $center_fig $center_ref_x \
         $center_ref_y]
     eval $canvas coords $numfig $coords
 
     eval $canvas move $numfig $new_center
}



body Figure::_rotate_coords {coords} {

   set centro [calculate_center $coords]
   set centro_x [lindex $centro 0]
   set centro_y [lindex $centro 1]

   for {set i 0} { $i< [llength $coords ] }  {incr i 2  } {
      set pto_x [lindex $coords $i]
      set pto_y [lindex $coords [expr $i +1]]
      set dist_x [expr abs ($centro_x - $pto_x ) ]
      set dist_y [expr abs ($centro_y - $pto_y )]
      set dist [expr (hypot ($dist_x, $dist_y)) ]

      if {$dist_x != 0} {
         set angulo [expr (atan ($dist_y/ $dist_x))]
      } else { set angulo 3.1416}

      if  {($centro_x <= $pto_x) && ($centro_y <= $pto_y)} {
          set coord_x [expr $centro_x  + ($dist * sin($angulo))]
          set coord_y [expr $centro_y  - ($dist * cos($angulo))]

      } elseif  {($centro_x <= $pto_x) && ($centro_y  >= $pto_y)} {
          set coord_x [expr $centro_x  - ($dist * sin($angulo))]
          set coord_y [expr $centro_y  - ($dist * cos($angulo))]

      } elseif  {($centro_x >= $pto_x) && ($centro_y >= $pto_y)} {
          set coord_x [expr $centro_x  - ($dist * sin($angulo))]
          set coord_y [expr $centro_y  + ($dist * cos($angulo))]

      } elseif  {($centro_x >= $pto_x) && ($centro_y <= $pto_y)} {
          set coord_x [expr $centro_x  + ($dist * sin($angulo))]
          set coord_y [expr $centro_y  + ($dist * cos($angulo))]
      }

      set coords [lreplace $coords $i $i $coord_x]
      set coords [lreplace $coords [expr $i +1] [expr $i +1] $coord_y]

   }

   return $coords

}



# ----------------------------------------------------------------------
# METHOD: new_center
#
#   Calcula el centro de una figura
# ----------------------------------------------------------------------

body Figure::new_center {center_fig center_ref_x center_ref_y } {
 
   set center_fig_x [lindex $center_fig 0]
   set center_fig_y [lindex $center_fig 1]
   set dif_x [expr $center_ref_x - $center_fig_x ]
   set dif_y [expr $center_ref_y - $center_fig_y ]

   if {$center_ref_x > $center_fig_x} {
        set eje_x [expr   $dif_x - $dif_y]
        set eje_y [expr $dif_y + $dif_x]

   } else {
        set eje_x [expr   $dif_x - $dif_y]
        set eje_y [expr   $dif_y + $dif_x]
   }

   return [concat $eje_x $eje_y]
}



# ----------------------------------------------------------------------
# METHOD: align
#
#   Alinea una figura, repecto de la seleccion de ese momento
# ----------------------------------------------------------------------

body Figure::align {align} {
   global page


   if {!$page($mypag,enable)} { return }

   set coords [eval $canvas bbox [get_all_fig_from_group \
      $mypag $numfig]]

   switch $align {

      "horizontal-inferior" {
         set diferencia [_sel_coords_align $coords 1 "<"]
         move 0 $diferencia
      }

      "horizontal-superior" {
          set diferencia [_sel_coords_align $coords 1 ">"]
          move 0 $diferencia
      }

      "vertical-right" {
          set diferencia [_sel_coords_align $coords 0 "<"]
          move $diferencia 0
      }

      "vertical-left" {
          set diferencia [_sel_coords_align $coords 0 ">"]
          move $diferencia 0
      }
   }
}



body Figure::_sel_coords_align {coords coord op_logica } {
   global page

   set coords_maestro [eval $canvas bbox \
      [get_all_fig_from_group $mypag $page($mypag,sel)]]
   set coord_sel_m [lindex $coords_maestro $coord]

   for {set i $coord} {$i< [llength $coords_maestro]} {incr i 2} {
      set coord_m [lindex $coords_maestro $i]
      if {[expr $coord_sel_m $op_logica $coord_m]} {
         set coord_sel_m $coord_m
      }
   }

   set coord_sel_e [lindex $coords $coord]

   for {set i $coord} {$i< [llength $coords] } {incr i 2} {
      set coord_e [lindex $coords $i]
      if {[expr $coord_sel_e $op_logica $coord_e]} {
         set coord_sel_e $coord_e
      }
   }

   set diferencia [eval expr $coord_sel_m - $coord_sel_e]
   
   return $diferencia
}



# ----------------------------------------------------------------------
# METHOD: calculate_coords_symmetry_hor
#
#   Calcula las coordenadas de la figura simetrica respecto de un eje
#   horizontal
# ----------------------------------------------------------------------

body Figure::calculate_coords_symmetry_hor {posx posy} {

   set coords [$canvas coords $numfig]
 
   for {set i 1} {$i< [llength $coords]} {incr i 2} {
      set coord_y [lindex $coords $i]
      set dist_y [expr $posy - $coord_y]
      set coords [lreplace $coords $i $i [expr $posy + $dist_y]]
   }

   return $coords
}



# ----------------------------------------------------------------------
# METHOD: calculate_coords_symmetry_ver
#
#   Calcula las coordenadas de la figura simetrica respecto de un eje
#   vertical
# ----------------------------------------------------------------------

body Figure::calculate_coords_symmetry_ver {posx posy} {

   set coords [$canvas coords $numfig]

   for {set i 0} {$i< [llength $coords]} {incr i 2} {
      set coord_x [lindex $coords $i]
      set dist_x [expr $posx - $coord_x]
      set coords [lreplace $coords $i $i [expr $posx + $dist_x]]
   }
   return $coords
}



# ----------------------------------------------------------------------
# METHOD: above_level
#
#   Sube de nivel una figura
# ----------------------------------------------------------------------

body Figure::above_level {} {
   global general page

   if {!$page($mypag,enable)} {return}

   set lista [get_all_fig_from_group $mypag $numfig]
   foreach el $lista {
      $canvas addtag above withtag $el
   }

   set coords [$canvas bbox above]
   set cadena [eval $canvas find overlapping $coords]
   set above [$canvas find above above]
   set aux ""

   if {$above != ""} {
       while {([lsearch -glob $cadena $above] == -1) && ($above != "")} {
          lappend aux [get_fig $canvas $above]
	  $canvas addtag above withtag $above
	  set above [$canvas find above $above]
       }

       # la variable above mantiene el elemento 
       # que esta encima superpuesto o ""
       $canvas dtag above above
       foreach el $lista {
          $canvas addtag above withtag $el
       }

       # solo los elementos que voy a mover tienen la tag above

       if {$above != ""} {
          set above [get_fig $canvas $above]
          while {[$mypag$above cget -father ] != ""} {
              set above [$mypag$above cget -father]
          }

          set list [get_all_fig_from_group $mypag $above]
          undo_above $mypag $lista [eval lappend aux $list] $lista_sel

          foreach el $list {
             $canvas addtag bloque withtag $el
          }

          $canvas raise above bloque

       } else { 
          if {$lista_sel != $numfig } {
             save_undo_group $mypag $lista_sel
          }
       }

    } else {

       if {$lista_sel != $numfig } {
          save_undo_group $mypag $lista_sel
       }

    }

    $canvas dtag bloque bloque
    $canvas dtag above above
    select
}



# ----------------------------------------------------------------------
# METHOD: redo
#
#   Redibuja una figura (viene del comando undo) atendiendo a la 
#   informacion almacenada en el objeto 
# ----------------------------------------------------------------------

body Figure::redo {} {
 
   global page

   if {!$page($mypag,enable)} {return}

   set undo [cget -undo]
   for {set i 0} {$i < [llength $undo]} {incr i} {
       set cmd [list_range $undo $i ]
       eval $cmd
   }

   # -----------------------------
   # Compruebo si estoy agrupado
   # -----------------------------
   if {[cget -father] == ""} {
       lappend page($mypag,FigList) $numfig
       configure -exist 1
       return

   } 

   # -----------------------------
   # Estoy agrupado
   # -----------------------------
   while {[cget -father] != ""} {
       set fig [cget -father]
   }

   set ind [lsearch -glob $page($mypag,FigList) $numfig]

   if {$ind == -1} {
       lappend page($mypag,FigList) $numfig
       configure -exist 1
   }
}
 


# ----------------------------------------------------------------------
# METHOD: symmetry_horizontal
# ----------------------------------------------------------------------
body Figure::symmetry_horizontal {posx posy} { 
   global page

   if {!$page($mypag,enable)} {return}

   set coords [calculate_coords_symmetry_hor $posx $posy]
   eval $page($mypag,canvas) coords $numfig $coords
}



# ----------------------------------------------------------------------
# METHOD: symmetry_vertical
# ----------------------------------------------------------------------
body Figure::symmetry_vertical {posx posy} { 
   global page

   if {!$page($mypag,enable)} { return }

   set coords [calculate_coords_symmetry_ver $posx $posy]
   eval $page($mypag,canvas) coords $numfig $coords
}



body Figure::change_coords {coords} { 
    eval $canvas coords $numfig $coords
}



# ----------------------------------------------------------------------
# METHOD: useful for show_info
# ----------------------------------------------------------------------

body Figure::_exec_show_apply {new_coords options} {
    global page
    eval $canvas coords $numfig $new_coords
    eval $canvas itemconfigure $numfig $options
    foreach el $page($mypag,sel) {
         $mypag$el delete_selection
         $mypag$el select
   }
}



body Figure::_show_apply {} {
   set new_coords [_show_calculate_coords]
   set new_options [_show_calculate_tags]
   _exec_show_apply "$new_coords" "$new_options"
}



body Figure::_show_change {} {
   set new_coords [_show_calculate_coords]
   set new_options [_show_calculate_tags]

   _exec_show_apply "$new_coords" "$new_options"
   ISA_RDOgroup Cmd only_if_enabled_page $mypag \
       $mypag$numfig _exec_show_apply "$new_coords" "$new_options"

   destroy .wbtl.info
   examine_peticion_queue
}



body Figure::_show_cancel {} {
   global page
   eval $canvas coords $numfig $_coords
   eval $canvas itemconfigure $numfig $_opciones
   foreach el $page($mypag,sel) {
      $mypag$el delete_selection
      $mypag$el select
   }
   destroy .wbtl.info
   examine_peticion_queue
}



# ----------------------------------------------------------------------
# METHOD: useful for show_info
# ----------------------------------------------------------------------

body Figure::saveFile {fileId swapFileId} {
   global page
   set canvas $page($mypag,canvas)
   set coords [$canvas coords $numfig]
   set opciones_t [$canvas itemconfigure $numfig]
   set opciones [get_tags $opciones_t ""]

   set ind [lsearch -exact $opciones "-stipple"]
   if {$ind != -1} {
       # el patron hace referencia al path absoluto, lo quito
       incr ind
       set pattern_file [file tail [lindex $opciones $ind]]
       set opciones [lreplace $opciones $ind $ind $pattern_file]
   }


   set figura   [list "%??%" $type $coords $opciones]
   puts $fileId $figura
   set l [list "%??%" $type $numfig -exist $exist]
   puts $swapFileId $l

}
