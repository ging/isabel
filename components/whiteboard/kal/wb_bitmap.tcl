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
# $Id: wb_bitmap.tcl 20206 2010-04-08 10:55:00Z gabriel $
#
# Description
#
#   Esta clase esta asociada a la figura de tipo bitmap
#   Los objetos de esta clase se nombran de la siguiente forma:
#      ${pag}fig$i
#   donde pag es la pagina a la que pertenecen e i es un entero
#
################################################################
 

class Bitmap {
 
   inherit Figure 

   constructor {args} {}

   private method _constructor           {pag fig coords color file}
   private method _constructor_from_file {pag fig coords opciones} 

   method show_info {}
   method copy {replicated}
   method select {}
   method scale {factor centro_ref_x centro_ref_y} {}
   method rotate {center_ref_x center_ref_y}


   # ###################
   # private interface
   # ###################
 
   private variable _new_options ""
   private variable _posi
 
   private method _show_calculate_coords {}
   private method _show_calculate_tags {}
}
 
 
# ----------------------------------------------------------------------
#                             CONSTRUCTOR
# ----------------------------------------------------------------------
 

body Bitmap::constructor {args} {
    set len [llength $args]
    if {$len==5} {
        eval _constructor $args
    } elseif {$len ==4} {
        eval _constructor_from_file $args
    } else {
        puts "- isabel_whiteboard:: Bitmap::constructor there was a problem \
              building the Bitmap object with $len parameters!"
    }
}

body Bitmap::_constructor {pag fig coords color file} {
   global page general dir_iconos

   set numfig fig$fig
   set canvas $page($pag,canvas)
   set type "bitmap"
   set mypag $pag
 
   set page($pag,id) [eval $canvas create bitmap $coords \
      -bitmap @$dir_iconos$file \
      -foreground $color -tags fig$fig]
}


body Bitmap::_constructor_from_file {pag fig coords opciones} {
    global page general dir_iconos

    set numfig fig$fig
    set canvas $page($pag,canvas)
    set type "bitmap"
    set mypag $pag

    set ind [lsearch -exact $opciones "-bitmap"]
    incr ind

    set file [file tail [lindex $opciones $ind]]
    set page($pag,id) [eval $canvas create bitmap $coords \
      -bitmap @$dir_iconos$file \
      -foreground black -tags fig$fig]

    set opciones [lreplace $opciones [expr $ind -1] $ind]
    eval $page($pag,canvas) itemconfigure fig$fig $opciones
}
 

# ----------------------------------------------------------------------
# METHOD: show_info
# ----------------------------------------------------------------------
 
body Bitmap::show_info {} {
 
   global normalbg entry_colour dir_iconos 

   catch {destroy .wbtl.info}

   toplevel .wbtl.info

   update
   after idle grab .wbtl.info

   frame .wbtl.info.etiq1
   pack .wbtl.info.etiq1

   label .wbtl.info.etiq1.e1 -text "Change an object via edit pane: " \
       -foreground black -background $normalbg \
       -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*
   pack .wbtl.info.etiq1.e1 -side left
 
   set _opciones [get_tags [$canvas itemconfigure $numfig] ""]
   set _new_options $_opciones


   # #### FILL ####

   frame .wbtl.info.etiq2
   pack .wbtl.info.etiq2


   # opciones de relleno

   label .wbtl.info.etiq2.r1 -text "Color" -justify left -anchor w
   set _fig_color [$canvas itemcget $numfig -foreground]
   label .wbtl.info.etiq2.e1 -background $_fig_color -width 7
   pack .wbtl.info.etiq2.r1 .wbtl.info.etiq2.e1 -side left
 

   # pinta mapa de colores
 
   frame .wbtl.info.colors
   pack .wbtl.info.colors
   set f .wbtl.info.colors
 
   foreach red {00 80 ff} {
      frame $f.red_col${red}
 
      foreach green {00 80 ff} {
         frame $f.green_row${red}${green}
 
         foreach blue {00 80 ff} {
            frame $f.blue${red}${green}${blue} -relief raised -height 8m \
               -width 8m -highlightthickness 0 -bd 1 \
               -bg "#${red}${green}${blue}"
            pack $f.blue${red}${green}${blue} -side top \
               -in $f.green_row${red}${green} \
               -fill both -expand 1
            bind $f.blue${red}${green}${blue} <ButtonRelease-1> \
               ".wbtl.info.etiq2.e1 configure \
                   -background \"#${red}${green}${blue}\""
         }
         pack $f.green_row${red}${green} -side left \
            -in $f.red_col${red} -fill both -expand 1
      }
      pack $f.red_col${red} -side left -fill both -expand 1 -in $f
   }
 

   # #### COORDS ####

   set _coords [$canvas coords $numfig]
   for {set j 0} {$j < [expr [llength $_coords] -1]} {incr j 2} {
      set _posi(x,$j) [expr {int ([list_range $_coords $j])}]
      set _posi(y,$j) [expr {int ([list_range $_coords [expr $j + 1]]) }]
      trace_busy "Bitmap::show_info::COORDS X,Y de $j son i \
                       $_posi(x,$j) $_posi(y,$j)"
   }
 
   set j 1
   for {set i 0} {$i< [llength $_coords]} {incr i 2} {
      frame .wbtl.info.coord$j
      pack .wbtl.info.coord$j
      label .wbtl.info.coord$j.e1 -text "Coordinate $j (x,y): " \
         -foreground black -background $normalbg \
         -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*

      ::iwidgets::spinint .wbtl.info.coord$j.e2 -width 4 -range {0 9999}
      .wbtl.info.coord$j.e2 delete 0 end
      .wbtl.info.coord$j.e2 insert 0 $_posi(x,$i)

      ::iwidgets::spinint .wbtl.info.coord$j.e3 -width 4 -range {0 9999}
      .wbtl.info.coord$j.e3 delete 0 end
      .wbtl.info.coord$j.e3 insert 0 $_posi(y,$i)

      pack .wbtl.info.coord$j.e1 .wbtl.info.coord$j.e2 .wbtl.info.coord$j.e3 -side left
      incr j
   }
 
 
   # #### BUTTONS ####
 
   frame .wbtl.info.etiqboton
   pack .wbtl.info.etiqboton
	
   button .wbtl.info.etiqboton.cancelar -text Cancel -command \
      [code $this _show_cancel]

   button .wbtl.info.etiqboton.prueba -text Apply -command \
      [code $this _show_apply]
 
   button .wbtl.info.etiqboton.modificar -text Change -command \
      [code $this _show_change]

   pack .wbtl.info.etiqboton.cancelar .wbtl.info.etiqboton.prueba \
      .wbtl.info.etiqboton.modificar -side left
   $canvas configure -cursor left_ptr
   tkwait window .wbtl.info
}



body Bitmap::_show_calculate_coords {} {
   set new_coords ""
   set j 1
   for {set i 0} {$i< [llength $_coords] } {incr i 2} {
      set _posi(x,$i) [.wbtl.info.coord$j.e2 get]
      set _posi(y,$i) [.wbtl.info.coord$j.e3 get]
      set new_coords [concat $new_coords $_posi(x,$i) $_posi(y,$i)]
      incr j
   }
   return $new_coords
}



body Bitmap::_show_calculate_tags {} {
    set color [.wbtl.info.etiq2.e1 cget -background]
    return [concat -foreground $color]
}

 
 
# ----------------------------------------------------------------------
# METHOD: copy
# ----------------------------------------------------------------------
 
body Bitmap::copy {replicated} {

   set coords [$canvas coords $numfig]
   set color [$canvas itemcget $numfig -foreground]
   set bitmap [$canvas itemcget $numfig -bitmap] 
   set n [split $bitmap "/"]
   set bitmap [lindex $n end]

   set l [llength $coords]
   for {set i 0} {$i < $l} {incr i} {
       lappend newCoords [expr [lindex $coords $i] + 10]
   }

   create_figure ${mypag}fig$replicated \
      $mypag $replicated bitmap $newCoords \
      $color "" "" "" "" "" "" "" "" "" "" $bitmap 
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: select
# ----------------------------------------------------------------------
 
body Bitmap::select {} {

   global page

   lappend page($mypag,sel) $numfig
   $canvas addtag "resaltado" withtag $numfig
   append namefig $mypag $numfig
   $namefig configure -selec 1

   # pinta los cuadraditos de la seleccion
   set coords [$canvas bbox $numfig]

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
# METHOD: rotate
# ----------------------------------------------------------------------
 
body Bitmap::rotate {center_ref_x center_ref_y} {

    set bbox [$canvas bbox $numfig]
    set center_fig [calculate_center $bbox]
    set new_center [new_center $center_fig $center_ref_x \
         $center_ref_y]
 
    eval $canvas move $numfig $new_center
}
 

