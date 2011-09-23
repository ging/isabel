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
# $Id: wb_polygon.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Esta clase esta asociada a la figura de tipo poligono
#   Los objetos de esta clase se nombran de la siguiente forma:
#      ${pag}fig$i
#   donde pag es la pagina a la que pertenecen e i es un entero
#
#########################################################################
 


class Polygon {
 
   inherit Figure
 
   constructor {pag fig clase coords color outline stipple width} {}
 
   method show_info {}
   method copy {replicated}
   method select {}
   method move_points {prop posx posy}
   method change_coords {coords}

   private method __calculate_arrow_coords {coords}

 
 
   # ###################
   # private interface
   # ###################
 
   private variable _fig_modo ""
   private variable _fig_pattern ""
   private variable _posi
 
   private method _show_calculate_tags {}
   private method _show_calculate_coords {}
   public method _show_filled {color}
   public method _change_info_pattern {pattern}
}
 
 
 
# ----------------------------------------------------------------------
#                             CONSTRUCTOR
# ----------------------------------------------------------------------
 
body Polygon::constructor {pag fig clase coords color outline stipple \
  width} {
 
   global page dir_iconos

   set numfig fig$fig
   set canvas $page($pag,canvas)
   set type "polygon"
   set mypag $pag

   switch $clase {

      "polygon" -

      "linea_cerrada"  {
         set page($pag,id) [eval $canvas create polygon $coords \
           -width $width -smooth off \
           -fill $color -outline $outline -tags fig$fig]
      }
      "curva_cerrada" {
         set page($pag,id) [eval $canvas create polygon $coords \
           -width $width -smooth on -fill $color -outline $outline \
           -tags fig$fig]
      }
      "arrow" {
         set page($pag,id) [eval $canvas create polygon \
             [__calculate_arrow_coords $coords] \
             -fill $color -width $width -tags fig$fig -outline $outline]
      }
   }

   if {($stipple!="") && ($stipple!="\"\"")} {
        $canvas itemconfigure $page($pag,id) -stipple @${dir_iconos}$stipple
   }
}
 

body Polygon::__calculate_arrow_coords {coords} {
         set posx [lindex $coords 0]
         set posy [lindex $coords 1]
         set posix [lindex $coords 2]
         set posiy [lindex $coords 3]
         set dist_x [expr (abs($posix - $posx))]
         set dist_y [expr {int (abs($posiy - $posy))}]
         if {$posix >= $posx} {
             set coord_x_medio [expr $posx + [expr $dist_x /2]]
         } else {
             set coord_x_medio [expr $posx - [expr $dist_x /2]]}
         if {$posiy >= $posy} {
             set coord_y_medio [expr $posy + [expr $dist_y /2]]
             set coord_y_tercio1 [expr $posy + [expr $dist_y /3]]
             set coord_y_tercio2 [expr $posy + [expr 2 * $dist_y /3]]
         } else {
             set coord_y_medio [expr $posy - [expr $dist_y /2]]
             set coord_y_tercio1 [expr $posy - [expr $dist_y /3]]
             set coord_y_tercio2 [expr $posy - [expr 2 * $dist_y /3]]
         }
         return "$posx \
             $coord_y_tercio1 \
             $coord_x_medio $coord_y_tercio1  \
             $coord_x_medio $posy $posix $coord_y_medio $coord_x_medio $posiy \
             $coord_x_medio $coord_y_tercio2 $posx $coord_y_tercio2"

}

 
body Polygon::change_coords {coords} {
   # solo lo pueden llmar los poligonos que sean flechas!!
    eval $canvas coords $numfig [__calculate_arrow_coords $coords]
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: show_info
# ----------------------------------------------------------------------
 
body Polygon::show_info {} {

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

 

   # #### FILL ####

   frame .wbtl.info.etiq2
   pack .wbtl.info.etiq2



 
   # opciones de relleno
 
   radiobutton .wbtl.info.etiq2.r1 -text "Fill Color" -variable fill -value 1 \
       -selectcolor "#000000000" -justify left -anchor w
 
   set _fig_color [$canvas itemcget $numfig -fill]
   set _fig_modo ""
   if {$_fig_color == ""} { set _fig_modo hueca
      set _fig_color "#FFFFFFFFF"}
 
   label .wbtl.info.etiq2.e1 -background $_fig_color -width 7
 
 
 
 
   # opciones del borde
 
   radiobutton .wbtl.info.etiq2.r2 -text "Color" -variable fill -value 0 \
      -selectcolor "#000000000" -justify left -anchor w
 
 
   set _fig_borde [$canvas itemcget $numfig -outline]
   label .wbtl.info.etiq2.e2 -background $_fig_borde -width 7
 
   .wbtl.info.etiq2.r2 invoke
 
   pack .wbtl.info.etiq2.r2 .wbtl.info.etiq2.e2 .wbtl.info.etiq2.r1 .wbtl.info.etiq2.e1 -side left
 
 


 
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
               [code $this _show_filled  ${red}${green}${blue} ]
         }
         pack $f.green_row${red}${green} -side left \
            -in $f.red_col${red} -fill both -expand 1
      }
      pack $f.red_col${red} -side left -fill both -expand 1 -in $f
   }
 
 


   # #### STIPPLE ####


   frame .wbtl.info.patterns
   pack .wbtl.info.patterns
 
   set _fig_pattern [$canvas itemcget $numfig -stipple]
   set _fig_pattern [file tail $_fig_pattern]

   frame .wbtl.info.patterns.r1
   frame .wbtl.info.patterns.r2
   pack .wbtl.info.patterns.r1 .wbtl.info.patterns.r2
 
   set f .wbtl.info.patterns.r1
 
 
   radiobutton $f.none -height 1 -variable pattern -value none \
          -bg white -indicatoron 0 -text NONE -command \
          [code $this _change_info_pattern none]
   pack $f.none -side left
 
   radiobutton $f.solid -width 2 -height 1 -variable pattern -value \
          solid -bg white -indicatoron 0  -command \
          [code $this _change_info_pattern solid ]
   pack $f.solid -side left
 
   foreach ptrn {pattern1 pattern2 pattern3 pattern4 pattern5 \
     pattern6 pattern7} {
 
        radiobutton $f.$ptrn -bitmap @${dir_iconos}${ptrn}.xbm \
          -bg white -variable pattern -indicatoron 0 \
          -value ${ptrn}.xbm -command \
          [code $this _change_info_pattern ${ptrn}.xbm ]
        pack $f.$ptrn -side left
   }
 
   set f .wbtl.info.patterns.r2
 
   foreach ptrn {pattern8 pattern9 pattern10 pattern11 pattern12 \
     pattern13 pattern14 pattern15 pattern16} {
 
        radiobutton $f.$ptrn -bitmap @${dir_iconos}${ptrn}.xbm \
          -bg white -variable pattern -indicatoron 0 \
          -value ${ptrn}.xbm -command \
          [code $this _change_info_pattern ${ptrn}.xbm ]
        pack $f.$ptrn -side left
   }
 
   if {$_fig_modo == "hueca"} {.wbtl.info.patterns.r1.none invoke
   } elseif {$_fig_pattern == ""} {.wbtl.info.patterns.r1.solid invoke
   } else {
       set pattern [lindex [split $_fig_pattern "."] 0]
       if { [lsearch -glob [list pattern1 pattern2 pattern3 pattern4 pattern5 \
                      pattern6 pattern7] $pattern] != -1 } {
            .wbtl.info.patterns.r1.$pattern invoke
       } else {
            .wbtl.info.patterns.r2.$pattern invoke
       }
   }

 
 

   # #### WIDTH ####

   frame .wbtl.info.etiq4
   pack .wbtl.info.etiq4
   frame .wbtl.info.etiq4.f1
   pack .wbtl.info.etiq4.f1 -side left

   set _fig_gr [$canvas itemcget $numfig -width]
   ::iwidgets::spinint .wbtl.info.etiq4.f1.sp1 \
       -labeltext "Width:" -width 2 -range {0 99}
   .wbtl.info.etiq4.f1.sp1 delete 0 end
   .wbtl.info.etiq4.f1.sp1 insert 0 $_fig_gr

   pack .wbtl.info.etiq4.f1.sp1
 


   # #### COORDS ####

   set _coords [$canvas coords $numfig]
   for {set j 0} {$j < [expr [llength $_coords] -1]} {incr j 2} {
      set _posi(x,$j) [expr {int ([list_range $_coords $j])}]
      set _posi(y,$j) [expr {int ([list_range $_coords [expr $j + 1]]) }]
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
 


   # #### SMOOTH ####

   frame .wbtl.info.etiqs3
   pack .wbtl.info.etiqs3

   set _fig_smooth [$canvas itemcget $numfig -smooth]
   switch $_fig_smooth {
       0  {set _fig_smooth off}
       1  {set _fig_smooth on}
       "" {set _fig_smooth off}
   }

   ::iwidgets::optionmenu .wbtl.info.etiqs3.cb1 -labeltext "Smooth:"
   .wbtl.info.etiqs3.cb1 insert end on off
   .wbtl.info.etiqs3.cb1 select $_fig_smooth
   pack .wbtl.info.etiqs3.cb1
 
 

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


body Polygon::_change_info_pattern {pattern} {
 
   if {$pattern == "solid"} { set _fig_pattern ""
   } else { set _fig_pattern $pattern }
}
 


body Polygon::_show_filled {color} {
   global fill
   if {$fill} {
       # modifico relleno de la figura
       if {($_fig_modo != "hueca") || ($_fig_pattern!="")} {
           .wbtl.info.etiq2.e1 configure -background "#$color"
       }
   } else {
       # modifico el borde de la figura
       .wbtl.info.etiq2.e2 configure -background "#$color"
   }
}
 
 

body Polygon::_show_calculate_coords {} { 
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


body Polygon::_show_calculate_tags {} {
   global dir_iconos

   set _fig_gr [.wbtl.info.etiq4.f1.sp1 get]
   set _fig_smooth [.wbtl.info.etiqs3.cb1 get]
   set _fig_pattern [file tail $_fig_pattern]
   set _fig_color [.wbtl.info.etiq2.e1 cget -background]
   set _fig_borde [.wbtl.info.etiq2.e2 cget -background]

   if {$_fig_pattern == "none"} {
      set new_options [list -outline $_fig_borde \
          -fill "" -width $_fig_gr -smooth $_fig_smooth]
   } elseif {$_fig_pattern == ""} {
      set new_options [list -outline $_fig_borde \
          -fill $_fig_color -width $_fig_gr -stipple "" -smooth $_fig_smooth]
   } else {
      set new_options [list -outline $_fig_borde \
          -fill $_fig_color -width $_fig_gr -stipple \
          @$dir_iconos$_fig_pattern -smooth $_fig_smooth]
   }
   return $new_options
}




 
# ----------------------------------------------------------------------
# METHOD: copy
# ----------------------------------------------------------------------
 
body Polygon::copy {replicated} {

   set coords [$canvas coords $numfig]
   set color [$canvas itemcget $numfig -fill]
   set outline [$canvas itemcget $numfig -outline]
   set stipple [$canvas itemcget $numfig -stipple]
   set width [$canvas itemcget $numfig -width]
   set smooth [$canvas itemcget $numfig -smooth]
 
   set l [llength $coords]
   for {set i 0} {$i < $l} {incr i} {
       lappend newCoords [expr [lindex $coords $i] + 10]
   }

   create_figure ${mypag}fig$replicated \
      $mypag $replicated \
      linea_cerrada $newCoords  \"$color\" \"$outline\" \
      \"[file tail $stipple]\" $width  "" "" "" "" "" "" "" "" 

   $canvas itemconfigure fig$replicated -smooth $smooth
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: select
# ----------------------------------------------------------------------
 
body Polygon::select {} {

   global page

   lappend page($mypag,sel) $numfig
   $canvas addtag "resaltado" withtag $numfig
   append namefig $mypag $numfig
   $namefig configure -selec 1

   # pinta los cuadraditos de la seleccion
   set coords [$canvas coords $numfig]

   for {set i 0} {$i<=[expr [llength $coords] -1]} {incr i 2} {
        set coordx [list_range $coords $i]
        set coordy [list_range $coords [expr $i+1]]
        _create_point_sel $coordx $coordy punto$i $numfig
   }
}



 
 
 
 
 
# ----------------------------------------------------------------------
# METHOD: move_points
# ----------------------------------------------------------------------
 
body Polygon::move_points {prop posx posy} {

   global page

   if {[$canvas find withtag resaltado] != ""} {
      delete_selection_cmd $mypag
   }

   set coords [$canvas coords $numfig]

   set numero [string range $prop 5 end]
   set coords [lreplace $coords $numero \
      [expr $numero+1] $posx $posy]
   eval $canvas coords $numfig $coords
   $this select 

}


 
 


