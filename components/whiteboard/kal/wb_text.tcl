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
# $Id: wb_text.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Esta clase esta asociada a la figura de tipo texto
#   Los objetos de esta clase se nombran de la siguiente forma:
#      ${pag}fig$i
#   donde pag es la pagina a la que pertenecen e i es un entero
#
#########################################################################
 


class Text {
 
   inherit Figure 

   constructor {args} {}

   private method _constructor {pag fig coords color font point weight \
                                anchor justify}
   private method _constructor_from_file {pag fig coords opciones}

   public method show_info {}
   public method copy {replicated}
   public method select {}
   public method rotate {center_ref_x center_ref_y}

   public method scale {factor centro_ref_x centro_ref_y} {}
   public method symmetry_horizontal {posx posy} {}
   public method symmetry_vertical {posx posy} {}

   public method saveFile {fileId swapFileId} 

 
   # ###################
   # private interface
   # ###################
 
   private variable _posi
 
   private method _show_calculate_coords {}
   private method _show_calculate_tags {}
}
 
 
 
# ----------------------------------------------------------------------
#                             CONSTRUCTOR
# ----------------------------------------------------------------------

body Text::constructor {args} { 
   set len [llength $args]
   if {$len == 4} {
       eval _constructor_from_file $args
   } elseif {$len==9} {
       eval _constructor $args
   } else {
        puts "- isabel_whiteboard:: Text::constructor there was a problem \
              building the Text object with $len parameters!"
   }   
}

body Text::_constructor_from_file {pag fig coords opciones} {
    global page

    set canvas $page($pag,canvas)
    set type "text"
    set numfig fig$fig
    set mypag $pag
    set posx [lindex $coords 0]
    set posy [lindex $coords 1]

    set ind [lsearch -exact $opciones "-text"]

    if {$ind == -1} { return }
    incr ind
    set texto [lindex $opciones $ind]
    set t ""

    for {set i 0} {$i < [string length $texto]} {incr i} {
        set char [string range $texto $i $i]
        if {$char == "%"} {
           set resto [string range $texto $i [expr $i+3]]
           if {$resto == "%??%"} {
               set t [append t "\n"]
               incr i 3
           } else {
               set t [append t $char]
           }
        } else { set t [append t $char] }
    }

    # set t [list_range $t 0]
    if {([string range $t 0 0]=="{") && ([string range $t end end]=="}")} {
        set long [expr [string length $t] -2]
        set t [string range $t 1 $long]
    }
    set opciones [lreplace $opciones $ind $ind $t]

    set page($pag,id) [eval $canvas create text $posx $posy ]
    $canvas addtag $numfig withtag $page($pag,id) 
 
    $canvas icursor $page($pag,id) @$posx,$posy
    $canvas focus $page($pag,id)
    focus $canvas

    eval $page($pag,canvas) itemconfigure $numfig $opciones
    set anchor [$canvas itemcget $numfig -anchor]
}

body Text::_constructor {pag fig coords color font point weight anchor justify} {

   global page

   set canvas $page($pag,canvas)
   set type "text"
   set numfig fig$fig
   set mypag $pag
   set posx [lindex $coords 0]
   set posy [lindex $coords 1]
   set anchor [change_pos $anchor]

   set page($pag,id) [eval $canvas create text $posx $posy -tag texto \
     -fill $color \
     -font -*-$font-$weight-R-Normal--*-[expr $point * 10]-*-*-*-*-*-* \
     -anchor $anchor -justify $justify ]
   $canvas addtag fig$fig withtag $page($pag,id)

   $canvas icursor $page($pag,id) @$posx,$posy
   $canvas focus $page($pag,id)
   focus $canvas
}

 
# ----------------------------------------------------------------------
# METHOD: show_info
# ----------------------------------------------------------------------
 
body Text::show_info {} {
 
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

   label .wbtl.info.etiq2.l1 -text "Color:" -background $normalbg \
         -foreground black \
         -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*
   set _fig_color [$canvas itemcget $numfig -fill]
   label .wbtl.info.etiq2.e1 -background $_fig_color -width 7



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
   set _posi(x,0) [expr {int ([list_range $_coords 0])}]
   set _posi(y,0) [expr {int ([list_range $_coords  1]) }]
 
   frame .wbtl.info.etiq3
   pack .wbtl.info.etiq3

   label .wbtl.info.etiq3.e1 -text "Coordinate (x,y): " -background \
         $normalbg -foreground black \
         -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*

   ::iwidgets::spinint .wbtl.info.etiq3.e2 -width 4 -range {0 9999}
   .wbtl.info.etiq3.e2 delete 0 end
   .wbtl.info.etiq3.e2 insert 0 $_posi(x,0)


   ::iwidgets::spinint .wbtl.info.etiq3.e3 -width 4 -range {0 9999}
   .wbtl.info.etiq3.e3 delete 0 end
   .wbtl.info.etiq3.e3 insert 0 $_posi(y,0)

   pack .wbtl.info.etiq3.e1 .wbtl.info.etiq3.e2 .wbtl.info.etiq3.e3 -side left
 
 

   # #### FONT ####

   frame .wbtl.info.etiq4
   pack .wbtl.info.etiq4

   set _fig_estilo [$canvas itemcget $numfig -font]
   set letra 3
   set char [string range $_fig_estilo $letra $letra]

   while { $char != "-"} {
        incr letra
        set char [string range $_fig_estilo $letra $letra]
   }

   set _fig_font [string range $_fig_estilo 3 [expr $letra -1]]
   ::swc::Optionmenu .wbtl.info.etiq4.cb1 -labeltext "Font:" \
         -labelmargin 0 -narrow 1 \
         -command { update; after idle grab .wbtl.info }
   .wbtl.info.etiq4.cb1 insert end courier helvetica times fixed clean

   .wbtl.info.etiq4.cb1 select $_fig_font

   set inic [expr $letra + 1]
   set letra [expr $letra + 1]
   set char [string range $_fig_estilo $letra $letra]
   while { $char != "-"} {
        incr letra
        set char [string range $_fig_estilo $letra $letra]
   }
 


   # #### WEIGHT ####

   set _fig_weight [string range $_fig_estilo $inic [expr $letra -1]]
   ::swc::Optionmenu .wbtl.info.etiq4.cb2 -labeltext "Weight:" \
         -labelmargin 0 -narrow 1 \
         -command { update; after idle grab .wbtl.info }
   .wbtl.info.etiq4.cb2 insert end bold medium
   .wbtl.info.etiq4.cb2 select $_fig_weight

   set inic [expr $letra + 13]
 
   pack .wbtl.info.etiq4.cb1 .wbtl.info.etiq4.cb2  -side left


   # #### POINT ####

   frame .wbtl.info.etiq5
   pack .wbtl.info.etiq5

   set _fig_point [string range $_fig_estilo $inic [expr $inic + 1 ]]
   ::swc::Optionmenu .wbtl.info.etiq5.cb1 -labeltext "Point:" \
         -labelmargin 0 -narrow 1 \
         -command { update; after idle grab .wbtl.info }
   .wbtl.info.etiq5.cb1 insert end \
	   2p 5p 7p 8p 9p 10p 12p 13p 14p 15p 16p 18p 19p \
	   20p 24p 25p 26p 30p 33p 34p 40p

   .wbtl.info.etiq5.cb1 select ${_fig_point}p

 
   # #### ANCHOR ####

   set _fig_pos [$canvas itemcget $numfig -anchor]
   switch $_fig_pos {
      n  {set _fig_pos "N" }
      ne {set _fig_pos "NE"}
      nw {set _fig_pos "NW"}
      s  {set _fig_pos "S" }
      se {set _fig_pos "SE"}
      sw {set _fig_pos "SW"}
      e  {set _fig_pos "E" }
      w  {set _fig_pos "W" }
      c  {set _fig_pos "C" }
      center  {set _fig_pos "C" }
      default { 
                 puts "- Whiteboard:: Text::show_info:: \
                                      unknown text anchor point\n"
                 set _fig_pos "C"
      }
   }

   ::swc::Optionmenu .wbtl.info.etiq5.cb2 -labeltext "Anchor:" \
         -labelmargin 0 -narrow 1 \
         -command { update; after idle grab .wbtl.info }
   .wbtl.info.etiq5.cb2 insert end NW N NE W C E SW S SE
   .wbtl.info.etiq5.cb2 select $_fig_pos
   pack .wbtl.info.etiq5.cb1 .wbtl.info.etiq5.cb2  -side left
 

   # #### JUSTIFY ####
   set _fig_justify [$canvas itemcget $numfig -justify]
   ::swc::Optionmenu .wbtl.info.etiq5.cb3 -labeltext "Justify:" \
         -labelmargin 0 -narrow 1 \
         -command {
             update
             after idle grab .wbtl.info 
             set justify [.wbtl.info.etiq5.cb3 get] 
             .wbtl.info.texto tag add all @0,0 end
             .wbtl.info.texto tag configure all -justify $justify
   }
   .wbtl.info.etiq5.cb3 insert end left center right
   .wbtl.info.etiq5.cb3 select $_fig_justify
   pack .wbtl.info.etiq5.cb3 .wbtl.info.etiq5.cb3  -side left



   # #### TEXTO ####

   global texto in_texto
   set texto [$canvas itemcget $numfig -text]
   set in_texto $texto
   text .wbtl.info.texto -width 50 -height 10
   .wbtl.info.texto insert end $texto
   .wbtl.info.texto tag add all @0,0 end
   .wbtl.info.texto tag configure all -justify $_fig_justify
   pack .wbtl.info.texto 


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
 


body Text::_show_calculate_coords {} {
   set new_coords ""
   for {set i 0} {$i< [llength $_coords] } {incr i 2} {
      set _posi(x,$i) [.wbtl.info.etiq3.e2 get]
      set _posi(y,$i) [.wbtl.info.etiq3.e3 get]
      set new_coords [concat $new_coords $_posi(x,$i) $_posi(y,$i)]
   }
   return $new_coords
}

 
body Text::_show_calculate_tags {} {
   set text [.wbtl.info.texto get @0,0 end]
   set texto [string range $text 0 [expr [string length $text] - 1]]

   set _fig_font [.wbtl.info.etiq4.cb1 get]
   set _fig_weight [.wbtl.info.etiq4.cb2 get]
   set point [.wbtl.info.etiq5.cb1 get]
   set _fig_point [string range $point 0 [expr [string length $point] -2]]
   set _fig_pos [.wbtl.info.etiq5.cb2 get]
   set pos [change_pos $_fig_pos]
   set _fig_justify [.wbtl.info.etiq5.cb3 get]
   set _fig_color [.wbtl.info.etiq2.e1 cget -background]

   set font \
     "-*-$_fig_font-$_fig_weight-R-Normal--*-[expr $_fig_point \
      * 10 ]-*-*-*-*-*-*"

   set new_options [list -fill $_fig_color  \
     -font $font -justify $_fig_justify -text "$texto" -anchor $pos]
   return $new_options
}


 
# ----------------------------------------------------------------------
# METHOD: copy
# ----------------------------------------------------------------------
 
body Text::copy {replicated} {

   set coords [$canvas coords $numfig]
   set color [$canvas itemcget $numfig -fill]
   set font [$canvas itemcget $numfig -font]
   set justify [$canvas itemcget $numfig -justif]
   set anchor [$canvas itemcget $numfig -anchor]
   set texto [$canvas itemcget $numfig -text]
 
   set l [llength $coords]
   for {set i 0} {$i < $l} {incr i} {
       lappend newCoords [expr [lindex $coords $i] + 10]
   }

   create_figure ${mypag}fig$replicated \
      $mypag $replicated \
      text $newCoords  \"$color\" \
      "" "" "" "" "" courier 12 bold $anchor $justify ""
   
   $canvas itemconfigure fig$replicated -font $font -text $texto
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: select
# ----------------------------------------------------------------------
 
body Text::select {} {
 
    global page

    lappend page($mypag,sel) $numfig
    $canvas addtag "resaltado" withtag $numfig
    append namefig $mypag $numfig
    $namefig configure -selec 1

    # pinta los cuadraditos de la seleccion
    set coords [$canvas coords $numfig]

    _create_point_sel [lindex $coords 0] [lindex $coords 1] punto1 $numfig
}



 
# ----------------------------------------------------------------------
# METHOD: rotate
# ----------------------------------------------------------------------
 
body Text::rotate {center_ref_x center_ref_y} {
 
      global page

      set bbox [$canvas bbox $numfig]
      set center_fig [calculate_center $bbox]
      set new_center [new_center $center_fig $center_ref_x \
           $center_ref_y]
 
     eval $canvas move $numfig $new_center
}


# ----------------------------------------------------------------------
# METHOD: saveFile
# ----------------------------------------------------------------------
 
body Text::saveFile {fileId swapFileId} {
   global page 

   set canvas $page($mypag,canvas)
   set coords [$canvas coords $numfig]
   set opciones_t [$canvas itemconfigure $numfig]
   set opciones [get_tags $opciones_t ""]

   set ind [lsearch -exact $opciones "-text"]
   incr ind
   set texto [lindex $opciones $ind]
   set t ""

   # Los cambios de linea los sustituyo por "%??%"
   for {set i 0} {$i < [string length $texto]} {incr i} {
       set c [string range $texto $i $i]
       if {$c == "\n"} { set t [append t "%??%"]
       } else { set t [append t $c] }
   }
   set opciones [lreplace $opciones $ind $ind $t]

   set figura [list "%??%" $type $coords $opciones]
   puts $fileId $figura

   set l [list "%??%" $type $numfig -exist $exist]
   puts $swapFileId $l

}
 



