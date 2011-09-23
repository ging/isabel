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
# $Id: wb_oval.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Esta clase esta asociada a la figura de tipo elipse
#   Los objetos de esta clase se nombran de la siguiente forma:
#      ${pag}fig$i
#   donde pag es la pagina a la que pertenecen e i es un entero
#
#########################################################################
 

 
class Oval {
 
   inherit Figure
 
   constructor {pag fig clase coords color outline stipple width} {}

   method show_info {}
   method copy {replicated}
   method select {}
   method move_points {prop posx posy}
   method change_coords {coords}

 
   # ###################
   # private interface
   # ###################
 
   private variable _fig_modo ""
   private variable _fig_pattern ""
   private variable _posi
   private variable _clase ""
 
   private method _show_calculate_tags {}
   private method _show_calculate_coords {}
   public method _show_filled {color}
   public method _change_info_pattern {pattern} 
}
 
 
 
# ----------------------------------------------------------------------
#                             CONSTRUCTOR
# ----------------------------------------------------------------------
 
body Oval::constructor {pag fig clase coords color outline stipple width} {
 
    global page dir_iconos

    set numfig fig$fig
    set canvas $page($pag,canvas)
    set type "oval"
    set mypag $pag
    set _clase $clase

    set page($pag,id) [eval $canvas create oval $coords \
        -fill $color -width $width -outline $outline -tags fig$fig]

    if {($stipple!="") && ($stipple!="\"\"")} {
        $canvas itemconfigure $page($pag,id) -stipple @${dir_iconos}$stipple
    }

    if {$clase == "oval"} {return}
    change_coords $coords
}


body Oval::change_coords {coords} {
    switch $_clase {
        circunf_diam {
           set posx [lindex $coords 0]
           set posy [lindex $coords 1]
           set posix [lindex $coords 2]
           set posiy [lindex $coords 3]

           set ladox  [expr {abs ([expr $posx -$posix])} ]
           set ladoy [expr {abs ([expr $posy - $posiy])}]
           set radio [expr (hypot ($ladox, $ladoy)) /2]
           if {$posx > $posix} { set centrox [expr $posx - ($ladox /2)]
           } else { set centrox [expr $posx + ($ladox /2)]}
           if {$posy > $posiy} { set centroy [expr $posy - ($ladoy /2)]
           } else { set centroy [expr $posy + ($ladoy /2)]}

           set coords [concat [expr $centrox-$radio] [expr $centroy-$radio] \
              [expr $centrox + $radio] [expr $centroy + $radio]]
        }
        circunf_radio {
           set posx [lindex $coords 0]
           set posy [lindex $coords 1]
           set posix [lindex $coords 2]
           set posiy [lindex $coords 3]
 
           set ladox  [expr {abs ([expr $posx - $posix])} ]
           set ladoy [expr {abs ([expr $posy - $posiy])}]
           set radio [expr hypot ($ladox, $ladoy)]
           set coords [concat [expr $posx-$radio] [expr $posy-$radio] \
               [expr $posx + $radio] [expr $posy + $radio]]
         }
    }
    eval $canvas coords $numfig $coords
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: show_info
# ----------------------------------------------------------------------
 
body Oval::show_info {} {
 
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
               -width 8m -highlightthickness 0 -bd 1 -bg "#${red}${green}${blue}"
            pack $f.blue${red}${green}${blue} -side top \
               -in $f.green_row${red}${green} \
               -fill both -expand 1
            bind $f.blue${red}${green}${blue} <1> {
               # set _fig_color [%W cget -bg]
 
            }
            bind $f.blue${red}${green}${blue} <ButtonRelease-1> \
               [code $this _show_filled  ${red}${green}${blue} ]
         }
         pack $f.green_row${red}${green} -side left \
            -in $f.red_col${red} -fill both \
            -expand 1
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
          [code $this _change_info_pattern none ]
   pack $f.none -side left

 
   radiobutton $f.solid  -width 2 -height 1 -variable pattern -value \
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
 
   ::iwidgets::spinint .wbtl.info.etiq4.f1.sp1 -labeltext "Width:" \
       -width 2 -range {0 99}
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
 

body Oval::_change_info_pattern {pattern} {

   if {$pattern == "solid"} { set _fig_pattern ""
   } else { set _fig_pattern $pattern }

}

 
body Oval::_show_filled {color} {
   global fill
   if {$fill} {
       # modifico relleno de la figura
       if {($_fig_modo != "hueca") || ($_fig_pattern != "")} {
           .wbtl.info.etiq2.e1 configure -background "#$color"
       }
   } else {
       # modifico el borde de la figura
       .wbtl.info.etiq2.e2 configure -background "#$color"
   }
}


 
body Oval::_show_calculate_coords {} {
 
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
 
 
 
body Oval::_show_calculate_tags {} {
 
   global dir_iconos
 
   set _fig_gr [.wbtl.info.etiq4.f1.sp1 get]
   set _fig_pattern [file tail $_fig_pattern]
   set _fig_color [.wbtl.info.etiq2.e1 cget -background]
   set _fig_borde [.wbtl.info.etiq2.e2 cget -background]

   if {$_fig_pattern == "none"} {
      set new_options [list -outline $_fig_borde -fill "" -width $_fig_gr]
   } elseif {$_fig_pattern == ""} {
      set new_options [list -outline $_fig_borde -fill \
                       $_fig_color -width $_fig_gr -stipple ""]
   } else {
      set new_options [list -outline $_fig_borde -fill \
                       $_fig_color -width $_fig_gr -stipple \
                       @$dir_iconos$_fig_pattern]
   }
   return $new_options
}
 
 
 
 
# ----------------------------------------------------------------------
# METHOD: copy
# ----------------------------------------------------------------------
 
body Oval::copy {replicated} {

   set coords [$canvas coords $numfig]
   set color [$canvas itemcget $numfig -fill]
   set outline [$canvas itemcget $numfig -outline]
   set stipple [$canvas itemcget $numfig -stipple]
   set width [$canvas itemcget $numfig -width]

   set l [llength $coords]
   for {set i 0} {$i < $l} {incr i} {
       lappend newCoords [expr [lindex $coords $i] + 10]
   }
 
   create_figure ${mypag}fig$replicated \
      $mypag $replicated \
      oval $newCoords  \"$color\" \
      \"$outline\" \"[file tail $stipple]\" $width  "" "" "" "" "" "" "" ""
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: select
# ----------------------------------------------------------------------
 
body Oval::select {} {
 
   global page

   lappend page($mypag,sel) $numfig
   $canvas addtag "resaltado" withtag $numfig
   append namefig $mypag $numfig
   $namefig configure -selec 1
 
   # pinta los cuadraditos de la seleccion
   set coords [$canvas coords $numfig]
 
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
# METHOD: move_points
# ----------------------------------------------------------------------
 
body Oval::move_points {prop posx posy} {
 
   global page
   if {[$canvas find withtag resaltado] != ""} {
      foreach el $page($mypag,sel) {
          $mypag$el delete_selection 
      }
   }
   set coords [$canvas coords $numfig]

   switch $prop {
      punto1 {
         if {([lindex $coords 2] > $posx) && \
           ([lindex $coords 3] > $posy)} {
            set coords [lreplace $coords 0 1 $posx $posy]
         }
      }
      punto2 {
         if {([lindex $coords 3] > $posy)} {
            set coords [lreplace $coords 1 1 $posy]
         }
      }
      punto3 {
         if {([lindex $coords 0] < $posx) && \
           ([lindex $coords 3] > $posy)} {
            set coords [lreplace $coords 1 2 $posy $posx]
         }
      }
      punto4 {
         if {([lindex $coords 0] < $posx)} {
            set coords [lreplace $coords 2 2 $posx]
         }
      }
      punto5 {
         if {([lindex $coords 0] < $posx) && \
           ([lindex $coords 1] < $posy)} {
            set coords [lreplace $coords 2 3 $posx $posy]
         }
      }
      punto6 {
         if {([lindex $coords 1] < $posy)} {
            set coords [lreplace $coords 3 3 $posy]
         }
      }
      punto7 {
         if {([lindex $coords 2] > $posx) && \
           ([lindex $coords 1] < $posy)} {
            set coords [lreplace $coords 0 0 $posx]
            set coords [lreplace $coords 3 3 $posy]
         }
      }
      punto8 {
         if {([lindex $coords 2] > $posx)} {
            set coords [lreplace $coords 0 0 $posx]
         }
      }
   }

   eval $canvas coords $numfig $coords
   $this select

}




