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
# $Id: wb_f_gui.tcl 20659 2010-06-23 14:40:42Z santiago $ 
#
# Description
#
#   Este fichero contiene todas las funciones asociadas a las
#   facilidades que ofrece la interfaz
#
#########################################################################



# ########################################################################
# FUNCIONES DE LAS FIGURAS
# ########################################################################
# copy 
# move 
# multiselect 
# group 
# no_group 
# clear 
# scale_function 
# delete_items
# rotate_ninety
# align 
# symmetry_horizontal 
# symmetry_vertical 
# select_button
# show_info  
# move_points 
# above_level 
# paint_figure 
# paint_polypoint_figure 
# pen
# text 
# pointer 
#
# ########################################################################
# CAMBIOS EN VARIABLES GENERALES
# ########################################################################
# change_anchor
# change_font
# change_pattern
# change_fill
# change_fillColor
# change_width
# change_arrow
# select_pattern
# select_colour
# create_color_window
# destroy_color_window
#
# ########################################################################
# MISC
# ########################################################################
# examine_tags
# change_item_coords
# item_configure
# ########################################################################






# ########################################################################
# 
#	FUNCIONES DE LAS FIGURAS
#
# ########################################################################



# ------------------------------------------------------------------------
# copy
#
#   Copia elementos
# ------------------------------------------------------------------------
proc copy {} {
   global page active_pag busy general

   incr busy   

   if {$page($active_pag,sel) == ""} {
       incr busy -1
       examine_peticion_queue
       return
   }
   
   $page($active_pag,canvas) configure -cursor clock
   change_interface disabled
   
   set sel_list [ordena_lista $active_pag $page($active_pag,sel) ""]

   copy_cmd $active_pag $sel_list
   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
       copy_cmd $active_pag $sel_list   

   change_interface normal
   $page($active_pag,canvas) configure -cursor left_ptr

   incr busy -1
   examine_peticion_queue
}

proc copy_cmd {pag sel_list} {
  global page
  
  # lista_elem es una lista de los id de los elementos creados, son copia
  # de los elementos seleccionados
  set lista_elem ""

  foreach el $sel_list {
       set replicated $page($pag,FigNumber)
       lappend lista_elem fig$replicated

       lappend page($pag,FigList) fig$replicated 
       incr page($pag,FigNumber)
  
       if {[$pag$el cget -type] == "group"} {
            Group ${pag}fig$replicated $pag $replicated ""
       }
       $pag$el copy $replicated
   }

   save_undo_copy $pag $sel_list $lista_elem
  
   # es necesario seleccionar el elemento
   delete_selection_cmd $pag
   foreach el $lista_elem {$pag$el select}
}


# ------------------------------------------------------------------------
# ordena_lista
#
#   Dada una lista de elementos, la ordena desde el elemento que se 
#   encuentra en el nivel mas inferior hasta el superior
# ------------------------------------------------------------------------

proc ordena_lista {pag list father} {
   global page
   set list_el [get_all_fig_from_group $pag $list]

   foreach elemen $list_el {
      $page($pag,canvas) addtag ordenar withtag $elemen
   }

   set below [$page($pag,canvas) find below ordenar]

   if {$below == ""} {
      set above [lindex $list 0]
      while {[get_below $pag $above] != ""} {
         set above [get_below $pag $above]
      }

   } else {
      set above \
         [get_fig $page($pag,canvas) [$page($pag,canvas) find above $below]]
   }


   # $above es el elemento del grupo que esta en el nivel mas bajo

   while {[$pag$above cget -father] != $father} {
      set above [$pag$above cget -father]
   }

   set l $above
   $page($pag,canvas) dtag ordenar ordenar
   set ind [lsearch -glob $list $l]
   set list [lreplace $list $ind $ind]
   while {[llength $list] > 1} {
       set above [lindex $list 0]
       set el [ord_below $pag $list $father]
       lappend l $el
       set ind [lsearch -glob $list $el]
       set list [lreplace $list $ind $ind]
   }

   eval lappend l $list
   return $l
}





# ------------------------------------------------------------------------
# ord_below
#
#   Obtiene el elemento que se encuentra en el nivel mas abajo dentro
#   de una lista de elementos
# ------------------------------------------------------------------------

proc ord_below {pag list father} {
   global page
   set list [get_all_fig_from_group $pag $list]

   foreach elem $list {
      $page($pag,canvas) addtag ordenar withtag $elem
   }

   set below [$page($pag,canvas) find below ordenar]
   set above [get_fig $page($pag,canvas) \
       [$page($pag,canvas) find above $below]]
   $page($pag,canvas) dtag ordenar ordenar

   while {[$pag$above cget -father] != $father} {
	   set above [$pag$above cget -father]
   }

   return $above
}





# ------------------------------------------------------------------------
# move
#
#   Mueve elementos. Igual que con el boton 2 del raton
# ------------------------------------------------------------------------

proc move {} {
   global page active_pag general latestAction balloon

   .wbtl.optionsBar.buttons.draw.b configure -image img_move
   balloon add .wbtl.optionsBar.buttons.draw.b "Move"
   activa_eventos

   set general(LeftButton) \
        "Move object. Click and drag to move an object."
   set latestAction "move"
   catch {destroy .wbtl.menuDraw}
   catch {destroy .wbtl.optionsBar.options}
	
   clear_mouse_events
   bind $page($active_pag,canvas) <ButtonPress-1> \
        "press_move_button \[$page($active_pag,canvas) canvasx %x\] \
                           \[$page($active_pag,canvas) canvasy %y\]" 

   bind $page($active_pag,canvas) <Any-B1-Motion> \
        "any_motion_move_button \[$page($active_pag,canvas) canvasx %x\] \
                                \[$page($active_pag,canvas) canvasy %y\]"

   bind $page($active_pag,canvas) <ButtonRelease-1> \
        "release_move_button \[$page($active_pag,canvas) canvasx %x\] \
                             \[$page($active_pag,canvas) canvasy %y\]" 
}



proc press_move_button {posix posiy} {

   global active_pag page busy posx posy inicx inicy elemento
 
   catch {destroy .wbtl.menuDraw}
   incr busy
   $page($active_pag,canvas) configure -cursor fleur

   set posx $posix
   set inicx $posix
   set posy $posiy
   set inicy $posiy
   set elemento [find_figure $posx $posy]

   if {$elemento == ""} { return }
 
   while {[$active_pag$elemento cget -father] != ""} {
      set elemento [$active_pag$elemento cget -father]
   }
   exec_press_move_button $active_pag $elemento
}


proc exec_press_move_button {pag elemento} {
    global page 
    save_undo_move $pag $elemento
    if {![$pag$elemento cget -selec]} {
        delete_selection_cmd $pag
        $pag$elemento select
    }
}



proc any_motion_move_button {posix posiy} {

   global active_pag page posx posy elemento
   if {$elemento == ""} {return}

   set difx [expr $posix - $posx]
   set dify [expr $posiy - $posy]
   change_coords_selected_items $active_pag $difx $dify
   set posx $posix
   set posy $posiy
}



proc change_coords_selected_items {pag difx dify} {
   global page
   foreach elemento $page($pag,sel) {
      $pag$elemento move $difx $dify
      $page($pag,canvas) move sel$elemento $difx $dify
   }
}


proc release_move_button {posix posiy} {

   global page active_pag busy posx posy inicx inicy elemento
 
   if {$elemento != ""} {
       set difx [expr $posix - $inicx]
       set dify [expr $posiy - $inicy]
       change_coords_selected_items $active_pag \
           [expr $posix -$posx] [expr $posiy - $posy]
       ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
           remote_exec_move $active_pag $page($active_pag,sel) $difx $dify
   }
   $page($active_pag,canvas) configure -cursor left_ptr
   incr busy -1
   examine_peticion_queue
}


proc change_coords_users {pag elemento } {
   global page
   if {[${pag}${elemento} cget -type] == "group"} {
       foreach el [${pag}${elemento} cget -figlist] {
          change_coords_users $pag $el 
       }
   } else {
      set coords [$page($pag,canvas) coords $elemento]
      ISA_RDOgroup Cmd only_if_enabled_page $pag \
          change_item_coords $pag $elemento $coords
   }
}


proc remote_exec_move {pag item_list difx dify} {
    global page
    save_undo_move $pag [lindex $item_list 0]
    delete_selection_cmd $pag
    foreach el $item_list {$pag$el select}
    change_coords_selected_items $pag $difx $dify
}



# ------------------------------------------------------------------------
# multiselect
#
#   selecciona los elementos que quedan encerrados en el 
#   rectangulo dado
# ------------------------------------------------------------------------

proc multiselect { } {
   global page active_pag general latestAction balloon

   .wbtl.optionsBar.buttons.draw.b configure -image img_multiselect
   balloon add .wbtl.optionsBar.buttons.draw.b "Multiselect"
   activa_eventos

   set general(LeftButton) "Select some objects. Click to select a\
       rectangular area."
   set latestAction "multiselect"
    catch {destroy .wbtl.menuDraw}
   catch {destroy .wbtl.optionsBar.options}

   clear_mouse_events

   bind $page($active_pag,canvas) <ButtonPress-1> \
      "press_multiselect_button \[$page($active_pag,canvas) canvasx %x\] \
                                \[$page($active_pag,canvas) canvasy %y\]" 

   bind $page($active_pag,canvas) <Any-B1-Motion> \
      "any_motion_multiselect_button \[$page($active_pag,canvas) canvasx %x\] \
                                     \[$page($active_pag,canvas) canvasy %y\]"

   bind $page($active_pag,canvas) <ButtonRelease-1> \
      "release_multiselect_button \[$page($active_pag,canvas) canvasx %x\] \                                      \[$page($active_pag,canvas) canvasy %y\]"
}


proc press_multiselect_button {posix posiy} {
    global busy active_pag posx posy
    incr busy

    set posx $posix
    set posy $posiy
    
    press_multiselect_button_exec $active_pag
    ISA_RDOgroup Cmd press_multiselect_button_exec $active_pag

}

proc press_multiselect_button_exec {pag} {
    global page
    save_undo_multiselect $pag 
    delete_selection_cmd $pag
}

proc any_motion_multiselect_button {posix posiy} {
    global active_pag page posx posy
    $page($active_pag,canvas) delete cuadro
    set page($active_pag,id) [$page($active_pag,canvas) create rectangle \
           $posx $posy $posix $posiy -tag cuadro]
}

proc release_multiselect_button {posix posiy} {
   global busy page active_pag posx posy
   $page($active_pag,canvas) delete cuadro

   if {[$page($active_pag,canvas) find enclosed \
           $posx $posy $posix $posiy] != ""} {
       set lista \
           [$page($active_pag,canvas) find enclosed $posx $posy $posix $posiy]

       foreach el $lista {
           set elemento [get_fig $page($active_pag,canvas) $el]

           if {$elemento != ""} {
               while {[$active_pag$elemento cget -father] != ""} {
                  set elemento [$active_pag$elemento cget -father]
               }
               if {[lsearch -glob $page($active_pag,sel) $elemento] == -1} {
                   $active_pag$elemento select 
                   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                      $active_pag$elemento select 
               }
           }
       }
   } 

   incr busy -1
   examine_peticion_queue
}



# ------------------------------------------------------------------------
# group
#  
#    agrupa los elementos seleccionados, a partir de este momento el
#    grupo funcionara como una unica figura 
# ------------------------------------------------------------------------

proc group { } {
   global page active_pag busy general

   if {$page($active_pag,sel) == ""} { return }

   incr busy
   set fg $page($active_pag,FigNumber)
 
   Group ${active_pag}fig$fg $active_pag $fg group 

   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
       Group ${active_pag}fig$fg $active_pag $fg group

   incr busy -1
   examine_peticion_queue
}





# ------------------------------------------------------------------------
# no_group
#
#   al pulsar sobre un grupo, lo deshace, a partir de ese momento el grupo
#   funcionara como figuras sueltas 
# ------------------------------------------------------------------------

proc no_group { } {

   global page active_pag busy general

   incr busy

   set lista ""
   foreach el $page($active_pag,sel) {
      if {[$active_pag$el cget -type] == "group"} {
          lappend lista $el
      }
   }

   if {$lista != ""} {
      no_group_cmd $active_pag $lista
      ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
          no_group_cmd $active_pag $lista
   }

   incr busy -1
}


proc no_group_cmd {pag sel_list} {

   global page

   save_undo_no_group $pag $sel_list
   
   foreach elemento $sel_list {
      if {[lsearch -glob $page($pag,sel) $elemento] != -1} {
            $pag$elemento delete_selection 
      }

      set ind [lsearch -glob $page($pag,FigList) $elemento]
      set page($pag,FigList) [lreplace $page($pag,FigList) $ind $ind]

      foreach el [$pag$elemento cget -figlist] {
         lappend page($pag,FigList) $el
         $pag$el configure -father ""
      }
   }
}





# ------------------------------------------------------------------------
# clear
#
#   borra todos los elementos de la pagina activa 
# ------------------------------------------------------------------------

proc clear { } {
   global active_pag busy page general 

   if {[$page($active_pag,canvas) find all] == ""} { return }

   incr busy
   
   set list $page($active_pag,FigList)
   clear_cmd $active_pag $list
   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
      clear_cmd $active_pag $list
      
   incr busy -1
   examine_peticion_queue
}


proc clear_cmd {pag list} {
   global page
   save_undo_clear $pag $list
   foreach el $list { $pag$el delete }
}




# ------------------------------------------------------------------------
# scale_function
#
#   escala todos los elementos seleccionados al pulsar sobre un punto
#   de seleccion
# ------------------------------------------------------------------------

proc scale_function {} {
   global page active_pag generla latestAction balloon

   .wbtl.optionsBar.buttons.draw.b configure -image img_scale_function
   balloon add .wbtl.optionsBar.buttons.draw.b "Scale"
   activa_eventos

   set general(LeftButton) \
       "Click on the selected points and drag to scale."
   set latestAction "scale_function"
   catch {destroy .wbtl.menuDraw}
   catch {destroy .wbtl.optionsBar.options}

   clear_mouse_events

   bind $page($active_pag,canvas) <ButtonPress-1> \
       "press_scale_button \[$page($active_pag,canvas) canvasx %x\] \
                           \[$page($active_pag,canvas) canvasy %y\]"

   bind $page($active_pag,canvas) <Any-B1-Motion> \
       "any_motion_scale_button \[$page($active_pag,canvas) canvasx %x\] \
                                \[$page($active_pag,canvas) canvasy %y\]"

   bind $page($active_pag,canvas) <ButtonRelease-1> \
       "release_scale_button \[$page($active_pag,canvas) canvasx %x\] \
                             \[$page($active_pag,canvas) canvasy %y\]"
}

proc press_scale_button {posix posiy} {
   global elemento page active_pag busy
   global prop centro_x centro_y tipo punto_sel total_factor

   set elemento ""
   set punto_sel ""
   set total_factor ""

   set cadena [$page($active_pag,canvas) find overlapping \
      [expr $posix-3] [expr $posiy-3] [expr $posix+3] [expr $posiy+3]]
   set punto [list_range $cadena [expr [llength $cadena] -1]]

   if {[examine_tags $punto sel] == $punto } { return }

   # he pinchado en un punto de seleccion
   incr busy
   set elemento [examine_tags $punto sel]
   set prop [examine_tags $punto punto]
   set punto_sel [string range $prop 5 end]
   set tipo [$active_pag$elemento cget -type]

   if {($tipo != "text") && 
       ($tipo != "bitmap") && ($tipo != "image")} {
        save_undo_scale $active_pag 

       if {$tipo == "group"} {
           set coords [eval $page($active_pag,canvas) bbox \
               [get_all_fig_from_group $active_pag $elemento]]
       } else { 
           set coords [$page($active_pag,canvas) coords $elemento]
       }

       set centro [calculate_center $coords]
       set centro_x [lindex $centro 0]
       set centro_y [lindex $centro 1]
       $page($active_pag,canvas) create rectangle \
           [expr $centro_x - 3] [expr $centro_y -3] \
           [expr $centro_x + 3] [expr $centro_y + 3]  -fill green \
           -tag varicentro
       return
   }
   set punto_sel ""
   incr busy -1
}


proc any_motion_scale_button {posx posy} {
    global page centro_x centro_y active_pag elemento punto_sel total_factor

    if {$punto_sel == ""} { return }
    $page($active_pag,canvas) configure -cursor clock
    set tipo [$active_pag$elemento cget -type]
    set factor [calculate_scale_factor_x \
         $active_pag $elemento $tipo \
         $punto_sel $centro_x $centro_y $posx $posy]

    if {$total_factor == ""} { set total_factor $factor
    } else { set total_factor [expr $factor * $total_factor] }

    exec_scale $active_pag $factor $centro_x $centro_y
    $page($active_pag,canvas) configure -cursor left_ptr
}



proc exec_scale {pag factor centro_ref_x centro_ref_y} {
   global page
   foreach el $page($pag,sel) {
      $pag$el scale $factor $centro_ref_x $centro_ref_y
      $pag$el delete_selection 
      $pag$el select 
   }
}


proc remote_exec_scale {pag total_factor centro_x centro_y} {
   save_undo_scale $pag
   exec_scale $pag $total_factor $centro_x $centro_y
}


proc release_scale_button {posx posy} {
   global page centro_x centro_y total_factor \
       busy active_pag elemento punto_sel page

   if {$punto_sel == ""} { return} 
   set tipo [$active_pag$elemento cget -type]
   $page($active_pag,canvas) configure -cursor clock
   $page($active_pag,canvas) delete varicentro

   set factor [calculate_scale_factor_x \
         $active_pag $elemento $tipo \
         $punto_sel $centro_x $centro_y $posx $posy]

   if {$total_factor == ""} { set total_factor $factor
   } else { set total_factor [expr $factor * $total_factor] }
 
   exec_scale $active_pag $factor $centro_x $centro_y
   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        remote_exec_scale $active_pag $total_factor $centro_x $centro_y

   $page($active_pag,canvas) configure -cursor left_ptr
   incr busy -1
   examine_peticion_queue
}


proc calculate_scale_factor_x {pag elemento tipo punto_sel \
  centro_ref_x centro_ref_y posx posy} {

  global page sel_anterior

  if {$tipo == "group"} {
	 set coords [eval $page($pag,canvas) bbox [get_all_fig_from_group \
                $pag $elemento]]
  } else {
     set coords [$page($pag,canvas) coords $elemento]
  }

  if {($tipo == "line") || ($tipo == "polygon")} {
     set pto [lindex $coords $punto_sel]
	 set centro_ref $centro_ref_x
	 set pos $posx

  } elseif {($tipo == "rectangle") || ($tipo =="oval") || \
            ($tipo == "arc") || ($tipo == "group")} {

     switch $punto_sel {
     "2" {
         set pto [lindex $coords 1]
         set centro_ref $centro_ref_y
         set pos $posy
         }
     "6" {
         set pto [lindex $coords 3]
         set centro_ref $centro_ref_y
         set pos $posy
         }
      "1" -
      "7" -
      "8" {
         set pto [lindex $coords 0]	
         set centro_ref $centro_ref_x
         set pos $posx
         }
      "3" -
      "4" -
      "5" {
         set pto [lindex $coords 2]	
         set centro_ref $centro_ref_x
         set pos $posx
         }
      }
  }

  set coord [expr abs ($centro_ref - $pto ) ]
  set dist [expr abs($centro_ref - $pos)]

  set retVal 1
  if {$dist != 0} { set retVal [expr $dist/$coord] }
  return $retVal
}



proc calculate_center {coords} {

   set suma_x 0
   set suma_y 0

   for {set i 0} {$i<=[expr [llength $coords] -1]} {incr i 2} {
	set suma_x [expr $suma_x + [lindex $coords $i]]
	set suma_y [expr $suma_y + [lindex $coords [expr $i+1]]]
   }

   set media [expr [llength $coords] /2]
   return [list [expr $suma_x / $media] [expr $suma_y / $media]]

}





# ------------------------------------------------------------------------
# delete_items
#
#   borra elementos al pulsar sobre ellos
# ------------------------------------------------------------------------

proc delete_items {} {

    global page active_pag busy general

    if {$page($active_pag,sel) == ""} return

    incr busy

    $page($active_pag,canvas) configure -cursor pirate

    delete_items_cmd $active_pag 
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
          delete_items_cmd $active_pag 
    
    $page($active_pag,canvas) configure -cursor left_ptr

    incr busy -1
    examine_peticion_queue

}


proc delete_items_cmd {pag} {
   global page
   save_undo_delete $pag $page($pag,sel)
   foreach el $page($pag,sel) { $pag$el delete }
}






# ------------------------------------------------------------------------
# rotate_ninety
#
#   gira noventa grados una figura al pulsar sobre ella
# ------------------------------------------------------------------------

proc rotate_ninety { } {

   global page active_pag busy general
   
   if {$page($active_pag,sel) == ""}  { return }

   incr busy

   set ref [eval $page($active_pag,canvas) bbox \
       [get_all_fig_from_group $active_pag $page($active_pag,sel)]]

   set center_ref [calculate_center $ref]
   set center_ref_x [lindex $center_ref 0]
   set center_ref_y [lindex $center_ref 1]
           
   rotate_ninety_cmd $active_pag $center_ref_x $center_ref_y 
   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
      rotate_ninety_cmd $active_pag $center_ref_x \
      $center_ref_y

   incr busy -1 
   examine_peticion_queue
}


proc rotate_ninety_cmd {pag center_ref_x center_ref_y} {
   global page
   save_undo_rotate $pag $page($pag,sel)
   foreach el $page($pag,sel) {
      $pag$el delete_selection 
      $pag$el rotate $center_ref_x $center_ref_y
      $pag$el select 
   }
}




# ------------------------------------------------------------------------
# align
#
#   alinear elementos de cuatro formas diferentes respecto a la
#   figura seleccionada
# ------------------------------------------------------------------------

proc align { } {
   global page punto align_prueba active_pag busy general 
   global latestAction balloon

   .wbtl.optionsBar.buttons.draw.b configure -image img_align
   balloon add .wbtl.optionsBar.buttons.draw.b "Align"
   activa_eventos

   set general(LeftButton) \
      "Click on an object to align with the selected object."
   set latestAction "align_OK_button"
   catch {destroy .wbtl.menuDraw}
   catch {destroy .wbtl.optionsBar.options}

   incr busy

   clear_mouse_events

   # creando la ventana para seleccionar tipo de align
   catch {destroy .wbtl.align}

   toplevel .wbtl.align
   frame .wbtl.align.f1
   pack .wbtl.align.f1

   update
   after idle grab .wbtl.align

   set align_prueba $general(Align)

   label .wbtl.align.f1.l1 -text "Appearance:"
   entry .wbtl.align.f1.e1 -textvariable align_prueba -width 18
   pack .wbtl.align.f1.l1 .wbtl.align.f1.e1 -side left

   frame .wbtl.align.f2
   pack .wbtl.align.f2

   listbox .wbtl.align.f2.lb1 -height 4 -width 0
   set fich [show_align $general(Align)]
   label .wbtl.align.f2.l1 -bitmap @$fich 
   pack .wbtl.align.f2.lb1 .wbtl.align.f2.l1 -side left 

   set lista [list "horizontal-inferior" "horizontal-superior" \
      "vertical-right" "vertical-left"]
   eval .wbtl.align.f2.lb1 insert end $lista

   bind .wbtl.align.f2.lb1 <ButtonRelease-1> {
      global align_prueba page active_pag
      set posx %x
      set posy %y
      set align_prueba [.wbtl.align.f2.lb1 get @$posx,$posy]
      set fich [show_align $align_prueba]
      .wbtl.align.f2.l1 configure -bitmap @$fich
   }

   frame .wbtl.align.f3
   pack .wbtl.align.f3

   button .wbtl.align.f3.ok -text OK -command "align_OK_button"

   button .wbtl.align.f3.cancel -text Cancel -command "align_cancel_button"
   pack .wbtl.align.f3.ok .wbtl.align.f3.cancel -side left

}



proc align_OK_button {} {
    global general busy align_prueba page active_pag
    set general(Align) $align_prueba
    incr busy -1

    bind $page($active_pag,canvas) <ButtonPress-1> \
        "press_align_button"

    bind $page($active_pag,canvas) <ButtonRelease-1> \
        "release_align_button \[$page($active_pag,canvas) canvasx %x\] \
                              \[$page($active_pag,canvas) canvasy %y\] "

    catch {destroy .wbtl.align}
}

proc align_cancel_button {} {
    global busy
    destroy .wbtl.align
    incr busy -1
}

proc press_align_button {} {
    global busy
    incr busy 
}


proc release_align_button {posx posy} {
    global page active_pag busy general
    set elemento [find_figure $posx $posy]
    if {($page($active_pag,sel) != "") && ($elemento!="")} {
        while {[$active_pag$elemento cget -father] != ""} {
            set elemento [$active_pag$elemento cget -father]
        }
        if {![$active_pag$elemento cget -selec]} { 
            # elemento no resaltado
            align_exec $active_pag $elemento $general(Align)
            ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                align_exec $active_pag $elemento $general(Align)
               
        }
    }
    incr busy -1
    examine_peticion_queue
}

proc show_align {alig } {
   global dir_iconos

   switch $alig {
      "horizontal-inferior" { set fich align_h_i.xbm}
      "horizontal-superior" { set fich align_h_s.xbm}
      "vertical-left" { set fich align_v_i.xbm}
      "vertical-right" {set fich align_v_d.xbm}
   }
   return $dir_iconos$fich
}


proc align_exec {pag elemento align} {
    undo_align $pag $elemento
    $pag$elemento align $align
}




# ------------------------------------------------------------------------
# symmetry_horizontal
#  
#   crea el simetrico de los elementos seleccionados respecto a 
#   un eje horizontal
# ------------------------------------------------------------------------

proc symmetry_horizontal { } {
    global page active_pag busy general

    if {$page($active_pag,sel) == ""} { return }

    incr busy 

    set ref [eval $page($active_pag,canvas) bbox \
        [get_all_fig_from_group $active_pag $page($active_pag,sel)]]

    set center [calculate_center $ref]
    set center_x [lindex $center 0]
    set center_y [lindex $center 1]
           
    symmetry_horizontal_cmd $active_pag $center_x $center_y 
          
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
         symmetry_horizontal_cmd $active_pag $center_x \
         $center_y 
      
    incr busy -1
    examine_peticion_queue
}


proc symmetry_horizontal_cmd {pag center_x center_y} {
    global page
    save_undo_rotate $pag $page($pag,sel)
    foreach el $page($pag,sel) {
         $pag$el symmetry_horizontal $center_x $center_y
         $pag$el delete_selection
         $pag$el select 
    }
}





# ------------------------------------------------------------------------
# symmetry_vertical
#
#   crea el simetrico de los elementos seleccionados respecto a 
#   un eje vertical
# ------------------------------------------------------------------------

proc symmetry_vertical { } {
    global page active_pag busy general

    if {$page($active_pag,sel) == ""} { return }

    incr busy

    set ref [eval $page($active_pag,canvas) bbox \
       [get_all_fig_from_group $active_pag $page($active_pag,sel)]]

    set center [calculate_center $ref]
    set center_x [lindex $center 0]
    set center_y [lindex $center 1]
           
    symmetry_vertical_cmd $active_pag $center_x $center_y 
           
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        symmetry_vertical_cmd $active_pag $center_x \
        $center_y 

    incr busy -1
    examine_peticion_queue
}



proc symmetry_vertical_cmd {pag center_x center_y} {
    global page
    save_undo_rotate $pag $page($pag,sel)
    foreach el $page($pag,sel) {
         $pag$el symmetry_vertical $center_x $center_y
         $pag$el delete_selection 
         $pag$el select 
    }
}


# ------------------------------------------------------------------------
# select_button
#
#   selecciona un elemento al pinchar sobre el. Tambien con el boton 
#   derecho del raton
# ------------------------------------------------------------------------

proc select {} {
    global page active_pag general latestAction balloon

    .wbtl.optionsBar.buttons.draw.b configure -image img_select
    balloon add .wbtl.optionsBar.buttons.draw.b "Select"
    activa_eventos

    set general(LeftButton) "Select an object. Click to select an object."
    set latestAction "select"
    catch {destroy .wbtl.menuDraw}
    catch {destroy .wbtl.optionsBar.options}

    clear_mouse_events

    bind $page($active_pag,canvas) <ButtonPress-1> { incr busy }

    bind $page($active_pag,canvas) <ButtonRelease-1> {
       select_button [$page($active_pag,canvas) canvasx %x] \
                     [$page($active_pag,canvas) canvasy %y]
       incr busy -1
       examine_peticion_queue
    }
}


proc select_button {posx posy} {

   global active_pag

   set elemento [find_figure $posx $posy] 

   if {$elemento == ""} { return }

   while {[$active_pag$elemento cget -father] != ""} {
      set elemento [$active_pag$elemento cget -father]
   }

   select_button_cmd $active_pag $elemento
   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
       select_button_cmd $active_pag $elemento                 
}


proc delete_selection_cmd {pag} {
    global page
    foreach el $page($pag,sel) { 
        $pag$el delete_selection 
    }
}

proc select_button_cmd {pag elemento} {

    global page
    save_undo_multiselect $pag

    if {![$pag$elemento cget -selec]} { 
         # es una figura no resaltada
         # puede que exista otra resaltada, borrar su seleccion
	 delete_selection_cmd $pag
	 $pag$elemento select 
         return
    } 
    # La figura esta seleccionada
    delete_selection_cmd $pag
}


# ------------------------------------------------------------------------
# select_none
#
#  elimina la seleccion de todos los elementos
# ------------------------------------------------------------------------
proc select_none {} {
    global active_pag
    delete_selection_cmd $active_pag
}


# ------------------------------------------------------------------------
# show_info
#
#   muestra la informacion de un elemento
# ------------------------------------------------------------------------

# comprueba si todos los elementos son imagenes
proc __all_images {} {
    global page active_pag
    foreach el $page($active_pag,sel) {
        set type [$active_pag$el cget -type]
        if {$type == "group"} {
            set all_group [get_all_fig_from_group \
                 $active_pag $el]
            foreach el_gr $all_group {
                if {[$active_pag$el_gr cget -type] != "image"} { return 0 }
            }
        } elseif {$type != "image"} { return 0 }
    }
    return 1
}

proc create_multi_figure_info {} {
   global page active_pag
   catch {destroy .wbtl.info}
   toplevel .wbtl.multi_info
   update
   after idle grab .wbtl.multi_info

   frame .wbtl.multi_info.etiq1
   pack .wbtl.multi_info.etiq1

   label .wbtl.multi_info.etiq1.e1 -text "Select the object you \
     would like to change: " -foreground black \
     -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*
   pack .wbtl.multi_info.etiq1.e1 -side left

   set all_el ""
   foreach fig $page($active_pag,sel) {
       if {[$active_pag$fig cget -type] == "group"} {
             set all_el [eval lappend all_el \
                 [get_all_fig_from_group $active_pag $fig]]
       } else { set all_el [lappend all_el $fig]}
   }

   set coord_box [eval $page($active_pag,canvas) bbox $all_el]
   set width [expr [lindex $coord_box 2] - [lindex $coord_box 0] +20 ]
   set height [expr [lindex $coord_box 3] - [lindex $coord_box 1] +20 ]

   canvas .wbtl.multi_info.canvas -background white -height $height -width $width
   pack .wbtl.multi_info.canvas -expand 1 -fill both
 
   set canvas .wbtl.multi_info.canvas 

   set xrange [expr [lindex $coord_box 0] -10]
   set yrange [expr [lindex $coord_box 1] -10]

   foreach fig $all_el {
       set tipo [$page($active_pag,canvas) type $fig]
       if {$tipo == "image"} {continue}

       set coords [$page($active_pag,canvas) coords $fig]
       for {set i 0} {$i < [llength $coords]} {incr i 2} {
            set x [lindex $coords $i]
            set y [lindex $coords [expr $i +1]]
            set coords [lreplace $coords $i [expr $i +1] \
                [expr $x - $xrange] [expr $y - $yrange]]
       }
       set opciones_t [$page($active_pag,canvas) itemconfigure $fig]
       set opciones [get_tags $opciones_t ""]
       if {$tipo == "text"} {
           set ind [lsearch -glob $opciones -text]
           incr ind
           set texto [lindex $opciones $ind]
           set t ""
           for {set i 0} {$i < [string length $texto]} {incr i} {
               set c [string range $texto $i $i]
               if {$c == "\n"} {
                   set t [append t "%??%"]
               } else {
                   set t [append t $c]
               }
           }
       }
       eval $canvas create $tipo $coords $opciones
   }

   bind $canvas <ButtonRelease-1> {
      global active_pag page
      set posx [.wbtl.multi_info.canvas canvasx %x]
      set posy [.wbtl.multi_info.canvas canvasy %y]

      set cadena [.wbtl.multi_info.canvas find overlapping $posx $posy $posx $posy]
      if {$cadena == ""} {
         set cadena [.wbtl.multi_info.canvas find closest $posx $posy]
      }
      set elemento [lindex $cadena end]

      set tags [.wbtl.multi_info.canvas itemcget $elemento -tags]
      set ind [lsearch -glob $tags fig*]
      if {$ind != -1} {
         set elemento [lindex $tags $ind]
         destroy .wbtl.multi_info
         $page($active_pag,canvas) configure -cursor clock
         set elemento [get_fig $page($active_pag,canvas) $elemento]

         save_undo_info $active_pag $elemento
         ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                         save_undo_info $active_pag $elemento

         $active_pag$elemento show_info
      }
   }

   button .wbtl.multi_info.b1 -text "Cancel" -command {destroy .wbtl.multi_info}
   pack .wbtl.multi_info.b1
}



proc show_info {} {
   global general page active_pag busy

   set latestAction "show_info"
   catch {destroy .wbtl.menuDraw}
   # catch {destroy .wbtl.optionsBar.options}

   if {$page($active_pag,sel) == ""} {
       examine_peticion_queue
       return
   }

   if {([llength $page($active_pag,sel)] == 1) && 
       ([$active_pag$page($active_pag,sel) cget -type] != "group")} {
         incr busy
         $page($active_pag,canvas) configure -cursor clock
         save_undo_info $active_pag $page($active_pag,sel) 
         ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
			 save_undo_info $active_pag $page($active_pag,sel)
         $active_pag$page($active_pag,sel) show_info

   } else {
        # compruebo que todas las figuras no son imagenes
        if {[__all_images]} {return}
        incr busy
        create_multi_figure_info
   } 

   incr busy -1
}





# ------------------------------------------------------------------------
# move_points
#
#   mueve los vertices de las figuras sin conservar relacion de aspecto,
#   al pinchar sobre los puntos de seleccion de una figura
# ------------------------------------------------------------------------

proc move_points {} {
   global active_pag general latestAction balloon page
 
   .wbtl.optionsBar.buttons.draw.b configure -image img_move_points
   balloon add .wbtl.optionsBar.buttons.draw.b "Move Points"
   activa_eventos

   set general(LeftButton) "Click and drag to move a selected point."
   set latestAction "move_points"
   catch {destroy .wbtl.menuDraw}
   catch {destroy .wbtl.optionsBar.options}

   clear_mouse_events 

   bind $page($active_pag,canvas) <ButtonPress-1> \
     "press_move_points_button \[$page($active_pag,canvas) canvasx %x\] \
                                \[$page($active_pag,canvas) canvasy %y\]"

   bind $page($active_pag,canvas) <Any-B1-Motion> \
     "any_motion_move_points_button \[$page($active_pag,canvas) canvasx %x\] \
                                \[$page($active_pag,canvas) canvasy %y\]"


   bind $page($active_pag,canvas) <ButtonRelease-1> \
     "release_move_points_button \[$page($active_pag,canvas) canvasx %x\] \
                                \[$page($active_pag,canvas) canvasy %y\]"
}

proc release_move_points_button {posx posy} {
    global active_pag page elemento prop busy 

    if {$elemento == ""} { return }

    $active_pag$elemento move_points $prop $posx $posy

    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        remote_exec_move_points $active_pag $elemento $prop $posx $posy
    
    incr busy -1
    examine_peticion_queue
}

proc any_motion_move_points_button {posx posy} {
    global active_pag prop elemento
    if {$elemento == ""} { return }
    $active_pag$elemento move_points $prop $posx $posy
}


proc remote_exec_move_points {pag elemento prop posx posy} {
    global centro_x centro_y punto_sel page
    set canvas $page($pag,canvas)
    save_undo_move $pag $elemento
    if {[$pag$elemento cget -type] == "group"} {
        set punto_sel [string range $prop 5 end]
        set coords [eval $canvas bbox \
            [get_all_fig_from_group $pag $elemento]]
        set centro [calculate_center $coords]
        set centro_x [lindex $centro 0]
        set centro_y [lindex $centro 1]
    }
    $pag$elemento move_points $prop $posx $posy
}


proc press_move_points_button {posx posy} {
    global busy page active_pag elemento
    global prop punto_sel centro_x centro_y

    set canvas $page($active_pag,canvas)
    set cadena ""
    set cadena [$canvas find overlapping [expr $posx-3] [expr $posy-3] \
               [expr $posx+3] [expr $posy+3]]
    if {$cadena == ""} { return}
    set punto [lindex $cadena end]
    if {[examine_tags $punto sel] == $punto} { return }

    # he pinchado en un punto de seleccion
    # solo puede haber 1 elemento seleccionado
    incr busy
    set prop [examine_tags $punto punto]
    set elemento [examine_tags $punto sel]
    save_undo_move $active_pag $elemento

    if {[$active_pag$elemento cget -type] == "group"} {
        set punto_sel [string range $prop 5 end]
        set coords [eval $canvas bbox [get_all_fig_from_group \
            $active_pag $elemento]]
        set centro [calculate_center $coords]
        set centro_x [lindex $centro 0]
        set centro_y [lindex $centro 1]
    }
    $active_pag$elemento move_points $prop $posx $posy
}


# ------------------------------------------------------------------------
# above_level
#
#   muestra la figura en un nivel superior
# ------------------------------------------------------------------------

proc top_level {} {

    global page active_pag busy general
  
    if {$page($active_pag,sel) == ""} { return }

    incr busy

    exec_top_level $active_pag
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        exec_top_level $active_pag

    incr busy -1
    examine_peticion_queue
}

proc exec_top_level {pag} {
    global page

    set canvas $page($pag,canvas)
    set old_sel $page($pag,sel)
    set all_sel_elements ""
    foreach el $page($pag,sel) {
       eval lappend all_sel_elements [get_all_fig_from_group $pag $el]
    }
    delete_selection_cmd $pag

    save_undo_above $pag $old_sel

    foreach el $all_sel_elements {
       $page($pag,canvas) addtag above withtag $el
    }
    # above= elemento por encima de todos los seleccionados

    $page($pag,canvas) raise above

    $page($pag,canvas) dtag above above
    foreach el $old_sel { $pag$el select }
}



proc above_level {} {

    global page active_pag busy general
    
    if {$page($active_pag,sel) == ""} { return }

    incr busy

    exec_above_level $active_pag
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        exec_above_level $active_pag

    incr busy -1
    examine_peticion_queue
}


proc exec_above_level {pag} {
    global page

    set canvas $page($pag,canvas)
    set old_sel $page($pag,sel)
    set all_sel_elements ""
    foreach el $page($pag,sel) {
       eval lappend all_sel_elements [get_all_fig_from_group $pag $el]
    }
    delete_selection_cmd $pag

    save_undo_above $pag $old_sel

    foreach el $all_sel_elements {
       $page($pag,canvas) addtag above withtag $el
    }
    # above= elemento por encima de todos los seleccionados

    set above [get_fig $page($pag,canvas) \
       [$page($pag,canvas) find above above]]

    if {$above != ""} { 
        set tags [$page($pag,canvas) gettags $above]
        $page($pag,canvas) raise above $above
    } else { $page($pag,canvas) raise above }

    $page($pag,canvas) dtag above above
    foreach el $old_sel { $pag$el select }
}

# ------------------------------------------------------------------------
# below_level
#
#   muestra la figura en un nivel inferior 
# ------------------------------------------------------------------------

proc down_level {} {

    global page active_pag busy general
  
    if {$page($active_pag,sel) == ""} { return }

    incr busy

    exec_down_level $active_pag
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        exec_down_level $active_pag

    incr busy -1
    examine_peticion_queue
}

proc exec_down_level {pag} {
    global page

    set canvas $page($pag,canvas)
    set old_sel $page($pag,sel)
    set all_sel_elements ""
    foreach el $page($pag,sel) {
       eval lappend all_sel_elements [get_all_fig_from_group $pag $el]
    }
    delete_selection_cmd $pag

    save_undo_above $pag $old_sel

    foreach el $all_sel_elements {
       $page($pag,canvas) addtag below withtag $el
    }
    # above= elemento por encima de todos los seleccionados

    $page($pag,canvas) lower below

    $page($pag,canvas) dtag below below
    foreach el $old_sel { $pag$el select }
}




# ------------------------------------------------------------------------
# print_page
# ------------------------------------------------------------------------

proc print_page {} {
      global active_pag page
      set c $page($active_pag,canvas)
      set pc "lpr"
      set ps "A4"
      catch {destroy .wbtl.print}
      ::iwidgets::canvasprintdialog .wbtl.print
      .wbtl.print setcanvas $c
      .wbtl.print activate
      .wbtl.print buttonconfigure OK -command {
         .wbtl.print print
         destroy .wbtl.print
      }
}




# ########################################################################
#
#			 CAMBIOS EN VARIABLES GENERALES
#
# ########################################################################
 


#---------------------------------------------------------------
# change_anchor
#
#   cambia la posicion de los elementos seleccionados en la 
#   interfaz
#---------------------------------------------------------------
 
proc change_anchor {anchor pag} {
   global page

   if {$page($pag,sel) == ""} { return }

   set canvas $page($pag,canvas)

   set l ""
   set sel $page($pag,sel)

   foreach el $sel {
       eval lappend l [find_text $pag $el]
   }
   delete_selection_cmd $pag

   if {$l!= ""} {
      save_change_anchor $pag $l $sel
      foreach el $l {
         $canvas itemconfigure $el -anchor $anchor
      }
   }

   foreach el $sel {$pag$el select}
}


proc find_text {pag el} {
   set type [$pag$el cget -type]
   set l ""

   if {$type == "group"} {
      foreach elem [$pag$el cget -figlist] {
         eval lappend l [find_text $pag $elem]
      }
      return $l

   } elseif {$type == "text"} { return  $el

   } else {return}

}
 




#---------------------------------------------------------------
# change_font
#
#   cambia la letra  de los elementos seleccionados de la
#   interfaz
#---------------------------------------------------------------
 
 
proc change_font {pag font weight point} {
   global general page

   if {$page($pag,sel) == ""} { return }

   set canvas $page($pag,canvas)

   set l ""
   set sel $page($pag,sel)

   foreach el $sel {
      eval lappend l [find_text $pag $el]
   }
   delete_selection_cmd $pag

   if {$l!= ""} {
      save_change_font $pag $l $sel
      foreach el $l {
         $canvas itemconfigure $el -font \
            "-*-$font-$weight-R-Normal--*-[expr \
            $point * 10]-*-*-*-*-*-*"
      }
   }

   foreach el $sel { $pag$el select }
}
 
 
#---------------------------------------------------------------
# change_justify
#---------------------------------------------------------------
proc change_justify {pag justify} {
   global general page

   if {$page($pag,sel) == ""} { return }

   set canvas $page($pag,canvas)

   set l ""
   set sel $page($pag,sel)

   foreach el $sel {
      eval lappend l [find_text $pag $el]
   }
   delete_selection_cmd $pag

   if {$l!= ""} {
      save_change_justify $pag $l $sel
      foreach el $l {
         $canvas itemconfigure $el -justify $justify 
      }
   }

   foreach el $sel {$pag$el select}
}




#---------------------------------------------------------------
# change_pattern
#
#   cambia el pattern de los elementos seleccionados
#---------------------------------------------------------------
 
proc find_pattern {pag el} {
   global page general
   set l ""
   set type [$pag$el cget -type]

   if {($type == "rectangle") || ($type == "arc") || ($type == "oval") \
     || ($type == "polygon") } {
	   return $el

   } elseif {$type == "group"} {

	   foreach elem [$pag$el cget -figlist] {
		  set l [eval lappend l [find_pattern $pag $elem]]
	   }

	   return $l

   } else { return }

}


proc change_pattern {new_p pag color old_color} {
   global page general dir_iconos

   if {$page($pag,sel) == ""} { return }

   set canvas $page($pag,canvas)
   set l ""
   foreach el $page($pag,sel) {
      set l [eval lappend l [find_pattern $pag $el]]
   }
   if {$l == ""} { return }

   save_undo_pattern $pag $l
   foreach el $l {
       switch $new_p {
           "NONE" {
              if {[$canvas itemcget $el -outline] != "" } {
                  $canvas itemconfigure $el -fill "" -stipple ""
              }
           }
           "SOLID" {
               if {$color==""} { set color $old_color }
               if {[$canvas itemcget $el -fill] == ""} {
                   $canvas itemconfigure $el -stipple "" -fill $color
               } else { $canvas itemconfigure $el -stipple "" }
           }
           default {
               if {[$canvas itemcget $el -fill] == ""} {
                  $canvas itemconfigure $el -stipple $new_p -fill $color
               } else { 
                  $canvas itemconfigure $el -stipple @${dir_iconos}$new_p
               }
           }
        }
    }
}





#---------------------------------------------------------------
# select_pattern
#
#   espera una accion del raton sobre la barra de
#   patrones y ordena los cambios pertinentes
#---------------------------------------------------------------
 
proc select_pattern {posx posy } {
   if {[.wbtl.color.pattern find overlapping $posx $posy $posx $posy] == ""} {
        return }
   global general page active_pag dir_iconos
   set canvas page($active_pag,canvas)

   .wbtl.color.pattern itemconfigure $general(Pattern) -outline black
   set general(Pattern) [.wbtl.color.pattern find overlapping \
       $posx $posy $posx $posy]
   set coords [.wbtl.color.pattern coords $general(Pattern)]
   set general(Stipple) [.wbtl.color.pattern itemcget \
       $general(Pattern) -stipple]

   if {$general(Stipple) != ""} { 
       # He seleccionado un pattern
       set general(Stipple) [file tail $general(Stipple)]
       .wbtl.color.pattern delete $general(Pattern)

       set general(Pattern) [eval .wbtl.color.pattern create rectangle \
           $coords -fill white -stipple @${dir_iconos}$general(Stipple) \
           -tag $general(Stipple) -outline white]

       if {!$general(Filled)} { set general(FillColor) $general(OldFillColor) }
       .wbtl.optionsBar.color itemconfigure fillcolor \
           -stipple @${dir_iconos}$general(Stipple) \
           -fill $general(FillColor)

       set general(Filled) 1

       change_pattern $general(Stipple) $active_pag \
           $general(FillColor) $general(OldFillColor)
       ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
           change_pattern $general(Stipple) $active_pag \
           $general(FillColor) $general(OldFillColor)

       return
    } 

    # He seleccionado figura hueca o solida
    set pattern1 [lindex $general(Pattern) 0]

    if {[.wbtl.color.pattern type $pattern1] == "text"} {
        set general(Pattern) [lindex $general(Pattern) 1]
    } else { set general(Pattern) $pattern1 }

    set coords [.wbtl.color.pattern coords $general(Pattern)]
    .wbtl.optionsBar.color itemconfigure color -stipple $general(Stipple) 
    set tag [.wbtl.color.pattern itemcget $general(Pattern) -tag]
    set is_none [lsearch -exact $tag none]

    if {$is_none >=0} {
        # he pulsado NONE
        .wbtl.color.pattern delete NONE
        .wbtl.color.pattern delete none
        set general(Pattern) [eval .wbtl.color.pattern create \
           rectangle $coords -fill white -tag none -outline white]
        .wbtl.color.pattern create text 24 14 -text NONE -justify \
            left -anchor center -tag NONE

        if {$general(Filled)} {
            set general(OldFillColor) $general(FillColor)
            ISA_RDOgroup Cmd set general(OldFillColor) $general(FillColor)
            set general(Color) ""
        }
        set general(Filled) 0

        change_pattern NONE $active_pag $general(FillColor) \
            $general(OldFillColor)
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_pattern NONE $active_pag  \
            $general(FillColor) $general(OldFillColor)

        .wbtl.optionsBar.color itemconfigure fillcolor -fill "" -stipple ""

        return
    }  

    # He pulsado SOLID
    .wbtl.color.pattern delete $general(Pattern)
    set general(Pattern) [eval .wbtl.color.pattern create \
        rectangle $coords -fill white -tag solid -outline white]

    if {!$general(Filled)} { set general(FillColor) $general(OldFillColor) }
        set general(Filled) 1

    change_pattern SOLID $active_pag $general(FillColor) \
        $general(OldFillColor)
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        change_pattern SOLID $active_pag  \
        $general(FillColor) $general(OldFillColor)

    .wbtl.optionsBar.color itemconfigure fillcolor \
        -fill $general(FillColor) -stipple ""

}

 

#---------------------------------------------------------------
# create_color_window
#
#    crea la ventana para modificar los colores
#---------------------------------------------------------------
proc create_other_color_window {} {
    global active_pag page general interior
    if {$interior} { set color $general(Color) 
    } else {set color $general(FillColor)}
    set color [tk_chooseColor -title "Other colors..." \
               -initialcolor $color]
    if {(!$page($active_pag,write)) || ($color == "")} { return }
    change_color $color
}

proc create_color_window {mode} {

   global interior page active_pag dir_iconos general

   if {[winfo exists .wbtl.color]} {
       raise .wbtl.color
       if {$mode == "color"} { .wbtl.color.rb.r1 invoke 
       } elseif {$mode == "fillcolor"} { .wbtl.color.rb.r2 invoke }
       return
   }

   toplevel .wbtl.color
   update
   grab .wbtl.color
   wm group .wbtl.color .wbtl
   wm title .wbtl.color "Whiteboard colors ..."

   frame .wbtl.color.rb
   pack .wbtl.color.rb

   radiobutton .wbtl.color.rb.r1 -text "Color" -variable interior -value 1 \
	  -justify left -anchor w 

   radiobutton .wbtl.color.rb.r2 -text "Fill Color" -variable interior -value 0 \
	  -justify left -anchor w 

   pack .wbtl.color.rb.r1 .wbtl.color.rb.r2 -side left

   if {$mode == "color"} { .wbtl.color.rb.r1 invoke
   } elseif {$mode == "fillcolor"} { .wbtl.color.rb.r2 invoke }

   frame .wbtl.color.type_color
   pack .wbtl.color.type_color
   set f .wbtl.color.type_color

   foreach red {00 80 ff} {
       frame $f.red_col${red}
       foreach green {00 80 ff} {
           frame $f.green_row${red}${green}
           foreach blue {00 80 ff} {
               frame $f.blue${red}${green}${blue} -relief raised -height 10m \
                  -width 10m -highlightthickness 0 -bd 1 \
                  -bg "#${red}${green}${blue}"
               pack $f.blue${red}${green}${blue} -side top \
                   -in $f.green_row${red}${green} \
                   -fill both -expand 1 
               bind $f.blue${red}${green}${blue} <1> \
                   "%W configure -relief sunken"
               bind $f.blue${red}${green}${blue} <ButtonRelease-1> {
                    %W configure -relief raised
                    set color [%W cget -bg]
                    change_color $color
               }
            }
            pack $f.green_row${red}${green} -side left \
               -in $f.red_col${red} -fill both -expand 1
        }
        pack $f.red_col${red} -side left -fill both -expand 1 -in $f
   }


   button .wbtl.color.other -text "Other colors..." -command \
      create_other_color_window 
   pack .wbtl.color.other

   # PATTERN
   canvas .wbtl.color.pattern -height 44 -width 317 -bd 1 -relief ridge
   pack .wbtl.color.pattern

   button .wbtl.color.close -text "Close" -command {
	  catch {destroy .wbtl.color}
   }
   pack .wbtl.color.close -fill x 

   set f .wbtl.color.pattern

   set posx 4
   $f create rectangle $posx 4 [expr $posx + 35] 24 -fill white -tag none
   $f create text 24 14 -text NONE -justify left -anchor center -tag NONE
   set posx [expr $posx + 35]
   $f create rectangle $posx 4 [expr $posx +35] 24 \
      -fill white -tag solid -outline black
   set posx [expr $posx + 35]

   for {set i 1} {$i <= 7} {incr i } {
      set ptrn "pattern$i.xbm"
      $f create rectangle $posx 4 [expr $posx +35]  24 -fill white \
         -stipple @$dir_iconos$ptrn -tag $ptrn
      set posx [expr $posx + 35]
   }
   set posx 4
   for {set i 8} {$i <= 16} {incr i } {
      set ptrn "pattern$i.xbm"
      $f create rectangle $posx 25 [expr $posx +35]  45 -fill white \
         -stipple @$dir_iconos$ptrn -tag $ptrn
      set posx [expr $posx + 35]
   }

   .wbtl.color.pattern itemconfigure $general(Pattern) -outline white
   if {$page($active_pag,write)} { 
       bind .wbtl.color.pattern <ButtonPress-1> "select_pattern %x %y"
       bind .wbtl.color.pattern <ButtonRelease-1> ".wbtl.color.close invoke"
   }

   if {!$page($active_pag,write)} { desactiva_eventos }
}


proc destroy_color_window { } {
   catch {destroy .wbtl.color}
}

proc change_color {color} {
    global general active_pag page interior
    set canvas $page($active_pag,canvas)
    if {$interior == 0} {
        # Cambio color, borde en las figuras 
        set general(OldFillColor) $color
        ISA_RDOgroup Cmd set general(OldFillColor) $color
        if {[.wbtl.optionsBar.color itemcget color -fill] == ""} { 
            # Figura hueca
            .wbtl.color.close invoke
            return
        }
        set general(FillColor) $color
        .wbtl.optionsBar.color itemconfigure fillcolor -fill $color 
        change_fillColor $color $active_pag
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_fillColor $color $active_pag
        .wbtl.color.close invoke
        return
    } 

    # Cambio fillcolor, el relleno en las figuras 
    set general(Filled) 1
    if {[winfo exist .wbtl.optionsBar.options.filled]} {
        .wbtl.optionsBar.options.filled select
    }
    set general(Color) $color
    .wbtl.optionsBar.color itemconfigure color -fill $color 
    change_fill $color $active_pag
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        change_fill $color $active_pag
    .wbtl.color.close invoke
}


#---------------------------------------------------------------
# change_fill
#
#    cambia el color=outline
#---------------------------------------------------------------

proc find_fill {pag el} {
   global page general
   set ret ""
   set type [$pag$el cget -type]

   if {($type == "rectangle") || ($type == "arc") || ($type == "oval") \
     || ($type == "polygon") || ($type == "bitmap") || ($type == "text") \
	 || ($type == "line")} {
	   set ret $el

   } elseif {$type == "group"} {
           set l ""
	   foreach elem [$pag$el cget -figlist] {
		  eval lappend l [find_fill $pag $elem]
	   }
	   set ret $l
   }

   return $ret
}

  

proc change_fill {new_c text} {
   global page

   if {$page($text,sel) == ""} { return }

   set canvas $page($text,canvas)

   set l ""
   foreach el $page($text,sel) {
       eval lappend l [find_fill $text $el]
   }
   if {$l == ""} { return }

   save_undo_fill $text $l
   foreach el $l {
       set type [$text$el cget -type]

       if {($type == "bitmap")} {
            $canvas itemconfigure $el -foreground $new_c

       } elseif {($type == "line") || ($type == "text")} {
              $canvas itemconfigure $el -fill $new_c

       } elseif {($type == "rectangle") || ($type == "oval") || \
             ($type == "arc") || ($type == "polygon")} {
               set prop [$canvas itemcget $el -outline]
               if {$prop != ""} {
                  $canvas itemconfigure $el -outline $new_c
               }
       }
   }
}
 


#---------------------------------------------------------------
# change_width
#
#    cambia el grosor de linea de los elementos seleccionados
#---------------------------------------------------------------


proc find_width {pag el} {

   global page general
   set ret ""
   set type [$pag$el cget -type]
 
   if {($type == "rectangle") || ($type == "arc") || ($type == "oval") \
     || ($type == "polygon") || ($type == "line")} {
       set ret $el
 
   } elseif {$type == "group"} {
       set l ""
       foreach elem [$pag$el cget -figlist] {
          eval lappend l [find_width $pag $elem]
       }
       set ret $l
   }

   return $ret
}




proc change_width {pag width } {

   global page
 
   if {$page($pag,sel) == ""} { return }

   set canvas $page($pag,canvas)
 
   set l ""
   foreach el $page($pag,sel) {
      eval lappend l [find_width $pag $el]
   }
 
   if {$l == ""} { return }

   save_undo_width $canvas $pag $l
   foreach el $l {
      $canvas itemconfigure $el -width $width
   }
}


#---------------------------------------------------------------
# change_arrow
#
#   cambia los finales de elementos tipo linea seleccionados 
#   por punta de flecha
#---------------------------------------------------------------

 
proc find_arrow {pag el} {
 
   global page general
   set type [$pag$el cget -type]
   set ret ""
 
   if {$type == "line"} {
       set ret $el
 
   } elseif {$type == "group"} {
       set l ""
       foreach elem [$pag$el cget -figlist] {
          eval lappend l [find_arrow $pag $elem]
       }
       set ret $l
   }
   return $ret
}


proc change_arrow {pag arrow_mode} {

   global page
 
   if {$page($pag,sel) == ""} { return }

   set canvas $page($pag,canvas)

   set l ""
   foreach el $page($pag,sel) {
       eval lappend l [find_arrow $pag $el]
   }
 
   if {$l == ""} { return }

   save_undo_arrow $canvas $pag $l
   foreach el $l {
       $canvas itemconfigure $el -arrow $arrow_mode
   }
}


#---------------------------------------------------------------
# change_fillColor
#
#   cambia el color  del borde de los elementos seleccionados
#---------------------------------------------------------------

proc change_fillColor {new_c pag} {
   global page

   set canvas $page($pag,canvas)

   if {$page($pag,sel) == ""} { return }

   set l ""
   foreach el $page($pag,sel) {
       eval lappend l [find_pattern $pag $el]
   }

   if {$l == ""} { return }
   save_undo_fillColor $pag $l
   foreach el $l {
       set type [$pag$el cget -type]
       if {($type == "rectangle") || ($type == "oval") || \
             ($type == "arc") || ($type == "polygon")} {
               set prop [$canvas itemcget $el -fill]
               if {$prop != ""} {
                  $canvas itemconfigure $el -fill $new_c
               }
       }
   }
}
 


 
# ===========================================================================
#           CREAR UNA HOJA NUEVA
# ===========================================================================
 

# ########################################################################
#
# 				MISC	
#
# ########################################################################

#---------------------------------------------------------------
# examine_tags
# 
#    comprueba si el elemento tiene la propiedad $prop
#    que puede valer: sel$id o punto$i
#    Devuelve la propiedad si es del tipo punto$i o el $id
#    si es del tipo sel$id
#    Devuelve el propio elemento $elem si no tiene la $prop
#---------------------------------------------------------------
 
proc examine_tags {elem prop} {
    global active_pag page
    set canvas $page($active_pag,canvas)
    set carac [$canvas gettags $elem]
    set indice [lsearch -glob $carac $prop*]
    if {$indice != -1} {
        set elem [lindex $carac $indice]
        if {$prop == "sel"} {
           set elem [string range $elem 3 end]}
    }
    return $elem
 
}


proc find_figure {posx posy} {
   global active_pag page
   set canvas $page($active_pag,canvas) 
   set cadena [$canvas find overlapping [expr $posx-3] [expr $posy-3] \
              [expr $posx+3] [expr $posy+3]]
if {0} {
   if {$cadena==""} { return ""}
}
   set elemento ""

   set len [expr [llength $cadena] -1]
   for {set i $len} {$i >=0} {incr i -1} {
      set elemento [lindex $cadena $i]
      break
   }

   if {$elemento != ""} {
       set elemento [examine_tags $elemento sel]
       set elemento [get_fig $canvas $elemento]
   }

   return $elemento
}

proc change_item_coords {pag item new_coords} {
   global page 
   eval $page($pag,canvas) coords $item $new_coords
}


proc item_configure {pag item attrib} {
   global page dir_iconos
   set index [lsearch -exact $attrib "-stipple"]
   if {$index != -1} {
      incr index
      set stipple [lindex $attrib $index] 
      if {($stipple!="") && ($stipple!="\"\"") && \
          ([string range $stipple 0 0] != "{") && \
          ([string range $stipple end end] != "}")} {
          set p [file tail $stipple]
          set attrib [lreplace $attrib $index $index @${dir_iconos}$p]
      }
   }
   eval $page($pag,canvas) itemconfigure $item $attrib
}


proc delete_item {pag item} {
   global page 
   catch {$page($pag,canvas) delete $item }
}

proc move_item {pag item difx dify } {
   global page 
   catch {$page($pag,canvas) move $item $difx $dify}
}

proc scroll_y_position {pag pos} {
   global page 
   $page($pag,canvas) yview moveto $pos
   set page($pag,v_scroll_old) $pos
}


proc only_if_enabled_page {pag args} {
    global page
    if {![info exists page($pag,enable)]} { return }

    if {$page($pag,enable)} { eval $args }
}

