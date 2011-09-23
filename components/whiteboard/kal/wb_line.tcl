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
# $Id: wb_line.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Esta clase esta asociada a la figura de linea
#   Los objetos de esta clase se nombran de la siguiente forma:
#      ${pag}fig$i
#   donde pag es la pagina a la que pertenecen e i es un entero
#
#########################################################################
 

 
class Line {
 
   inherit Figure

   constructor {pag fig clase coords outline width arrow arrowshape} {}

   method show_info {}
   method copy {replicated}
   method select {}
   method move_points {prop posx posy}

 
   # ###################
   # private interface
   # ###################
 
   private variable _posi
 
   private method _show_calculate_tags {}
   private method _show_calculate_coords {}
}
 
 
 
# ----------------------------------------------------------------------
#                             CONSTRUCTOR
# ----------------------------------------------------------------------
 
body Line::constructor {pag fig clase coords outline width arrow arrowshape} {
 
   global page 
 
   set numfig fig$fig
   set canvas $page($pag,canvas)
   set type "line"
   set mypag $pag

   switch $clase {
      line {
         set page($pag,id) [eval $canvas create line $coords -fill $outline \
     		-width $width -tags fig$fig]
      }
      linea_abierta {
         set page($pag,id) [eval $canvas create line $coords -width $width \
           -fill $outline -smooth off  \
           -tags fig$fig]
      }
      curva_abierta {
         set page($pag,id) [eval $canvas create line $coords -width $width \
           -fill $outline -smooth on  \
           -tags fig$fig]
      }
   }
   $canvas itemconfigure $page($pag,id) -arrow $arrow \
      -arrowshape $arrowshape
}



 
 
 
# ----------------------------------------------------------------------
# METHOD: show_info
# ----------------------------------------------------------------------
 
body Line::show_info {} {

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
 
   label .wbtl.info.etiq2.r1 -text "Color" \
       -justify left -anchor w
   set _fig_color [$canvas itemcget $numfig -fill]
   label .wbtl.info.etiq2.e1 -background $_fig_color -width 7
   pack .wbtl.info.etiq2.r1 .wbtl.info.etiq2.e1  -side left
 
 
 
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
               -width 8m -highlightthickness 0 \
               -bd 1 -bg "#${red}${green}${blue}"
            pack $f.blue${red}${green}${blue} \
               -side top -in $f.green_row${red}${green} \
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
 
 
 
 
   # #### WIDTH ####

   frame .wbtl.info.etiq4
   pack .wbtl.info.etiq4
   frame .wbtl.info.etiq4.f1
   pack .wbtl.info.etiq4.f1 -side left

   set _fig_gr [$canvas itemcget $numfig -width]

   ::iwidgets::spinint .wbtl.info.etiq4.f1.sp1 -labeltext \
        "Width:" -width 2 -range {0 99}
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
 


   # #### ARROW ####

   set flecha [$canvas itemcget $numfig -arrowshape]

   set _fig_e1 [lindex $flecha 0]
   set _fig_e2 [lindex $flecha 1]
   set _fig_e3 [lindex $flecha 2]

   frame .wbtl.info.flecha2
   pack .wbtl.info.flecha2

   set _fig_flecha [$canvas itemcget $numfig -arrow]
   if {$_fig_flecha == ""} {set $_fig_flecha none}


   ::iwidgets::optionmenu .wbtl.info.flecha2.cb1 -labeltext "Arrow:"
   .wbtl.info.flecha2.cb1 insert end none first last both
 
   .wbtl.info.flecha2.cb1 select $_fig_flecha
 

   pack .wbtl.info.flecha2.cb1
 


   frame .wbtl.info.flecha1
   pack .wbtl.info.flecha1

   set f .wbtl.info.flecha1
   canvas $f.c1 -height 100 -width 100
   pack $f.c1 -side left
   image create bitmap im1 -file ${dir_iconos}conf_flecha.xbm
   $f.c1 create image 50 50 -image im1

   frame $f.f
   pack $f.f -side left
 


   ::iwidgets::spinint $f.f.sp1 -width 3 -labeltext "d1:"
   $f.f.sp1 delete 0 end
   $f.f.sp1 insert 0 $_fig_e1

   ::iwidgets::spinint $f.f.sp2 -width 3 -labeltext "d2:"
   $f.f.sp2 delete 0 end
   $f.f.sp2 insert 0 $_fig_e2

   ::iwidgets::spinint $f.f.sp3 -width 3 -labeltext "d3:"
   $f.f.sp3 delete 0 end
   $f.f.sp3 insert 0 $_fig_e3

   pack $f.f.sp1 $f.f.sp2 $f.f.sp3

 
 
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
 

body Line::_show_calculate_coords {} {
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


 
body Line::_show_calculate_tags {} {

   set _fig_color [.wbtl.info.etiq2.e1 cget -background]
   set _fig_gr [.wbtl.info.etiq4.f1.sp1 get]
   set _fig_flecha [.wbtl.info.flecha2.cb1 get]
   set _fig_e1 [.wbtl.info.flecha1.f.sp1 get]
   set _fig_e2 [.wbtl.info.flecha1.f.sp2 get]
   set _fig_e3 [.wbtl.info.flecha1.f.sp3 get]

   return [list -arrow $_fig_flecha \
          -arrowshape [list $_fig_e1 $_fig_e2 $_fig_e3] \
          -fill $_fig_color -width $_fig_gr]
} 


 
# ----------------------------------------------------------------------
# METHOD: copy
# ----------------------------------------------------------------------
 
body Line::copy {replicated} {

   set coords [$canvas coords $numfig]
   set outline [$canvas itemcget $numfig -fill]
   set smooth [$canvas itemcget $numfig -smooth]
   set arrow [$canvas itemcget $numfig -arrow]
   set arrowshape [$canvas itemcget $numfig -arrowshape]
   set width [$canvas itemcget $numfig -width]

   set l [llength $coords]
   for {set i 0} {$i < $l} {incr i} {
       lappend newCoords [expr [lindex $coords $i] + 10]
   }

   create_figure ${mypag}fig$replicated \
      $mypag $replicated line $newCoords \
      "" \"$outline\" "" $width  $arrow $arrowshape "" "" "" "" "" ""

   $canvas itemconfigure fig$replicated -smooth $smooth
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: select
# ----------------------------------------------------------------------
 
body Line::select {} {
 
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
 
body Line::move_points {prop posx posy} {
 
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



 

