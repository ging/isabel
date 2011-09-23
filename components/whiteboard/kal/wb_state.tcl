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
# $Id: wb_state.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#     Se encarga de modificar variables, de mantener el estado
#
#########################################################################
 

##########################################################################
# CHANGE FLOOR VARIABLES
##########################################################################
# change_floor_variables
# i_am_new_master

##########################################################################
# CHANGE GUI VARIABLES
##########################################################################
# config_width
# length_line_text
# config_arrow
# config_font

##########################################################################
# CHANGE INITIAL VARIABLES
##########################################################################
# init_var

##########################################################################
# CHANGE PAGE VARIABLES
##########################################################################
# exec_send_variables
# create_var_new_page 
# libera




##########################################################################
# CHANGE GUI VARIABLES
##########################################################################

# ------------------------------------------------------------------------
# change_floor_variables: actualiza el valor del nuevo master de la pagina
# ------------------------------------------------------------------------
proc change_floor_variables {request_pag name request_userid} {
    global page 

    write_mode_off $request_pag
    set page($request_pag,master_name) $name
    set page($request_pag,master_id) $request_userid

    # 
    # Avisa al Coord Manager
    #

    Wb_change_coordinator Whiteboard $request_userid $request_pag
}



# ------------------------------------------------------------------------
# i_am_new_master: el nuevo master actualiza sus variables, la interfaz
#       y las del resto de participantes
# ------------------------------------------------------------------------

proc i_am_new_master {pag } {
   global page myuserid mysite_id active_pag latestAction uniqueMaster

   set page($pag,write) 1
   set page($pag,master_id) $myuserid
   set page($pag,master_name) $mysite_id

   if {$active_pag == $pag} {
       if {!$uniqueMaster} {
           .wbtl.barra.floor.floor select
       }
       activa_eventos
       eval $latestAction
       change_interface normal
   }

   ISA_RDOgroup Cmd only_if_enabled_page $pag write_mode_off $pag
   ISA_RDOgroup Cmd only_if_enabled_page $pag set page($pag,master_id) $myuserid
   ISA_RDOgroup Cmd only_if_enabled_page $pag set page($pag,master_name) $mysite_id
}


##########################################################################
# CHANGE GUI VARIABLES
##########################################################################

# ------------------------------------------------------------------------
# config_width: configura a traves de una ventana el grueso de la linea
# ------------------------------------------------------------------------

proc examine_incr { var min} {
   if {$var == 250} { return $min
   } else { incr var ; return $var}
}

proc examine_dec { var min} {
   if {$var == $min} { return 250
   } else { incr var -1 ; return $var}
}

proc config_width { } {

    global general ej ej_linea page active_pag
    set ej_linea $general(Width)
    catch {destroy .wbtl.width}
    toplevel .wbtl.width
	update
	grab .wbtl.width
    wm group .wbtl.width .wbtl
    wm title .wbtl.width "Change width..."

    ::iwidgets::spinint .wbtl.width.f1 \
        -labeltext "Change width to:" -width 3 -range {1 999} \
	-decrement {
	   set ej_linea [examine_dec $ej_linea 1]
	   .wbtl.width.f2 itemconfigure $ej -width $ej_linea} \
	-increment {
	   set ej_linea [examine_incr $ej_linea 1] 
	   .wbtl.width.f2 itemconfigure $ej -width $ej_linea}
    .wbtl.width.f1 configure -textvariable ej_linea
    pack .wbtl.width.f1

    canvas .wbtl.width.f2 -width 150 -height 50 -bd 3 -relief groove
    pack .wbtl.width.f2 
    set ej [.wbtl.width.f2 create line 20 25 130 25 -width $ej_linea]

    frame .wbtl.width.f3
    pack .wbtl.width.f3
    button .wbtl.width.f3.ok -text OK -command "button_ok_config_width"

    button .wbtl.width.f3.cancel -text Cancel -command { destroy .wbtl.width }
    pack .wbtl.width.f3.ok .wbtl.width.f3.cancel  -side left

    if (!$page($active_pag,write)) { desactive_width_window }
}

proc button_ok_config_width {} {
    global general dir_iconos active_pag ej_linea
    set general(Width) $ej_linea
    if {[winfo exists .wbtl.optionsBar.options.width]} {
        switch $general(Width) {
            "1" { set selected g1.xbm }
            "2" { set selected g2.xbm }
            "3" { set selected g3.xbm }
            "4" { set selected g4.xbm }
            default { set selected g8.xbm }
         }
        .wbtl.optionsBar.options.width.l1 configure \
            -bitmap @${dir_iconos}$selected
    }
    change_width $active_pag $ej_linea
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        change_width $active_pag $ej_linea
    destroy .wbtl.width
}




# ------------------------------------------------------------------------
# length_line_text: configura a traves de una ventana la longitud de
#		linea para el texto
# ------------------------------------------------------------------------

proc length_line_text { } {

    global long_linea unidad long
    clear_mouse_events
    set longitud [string length $long_linea]
    catch {destroy .wbtl.long}
    toplevel .wbtl.long
    wm group .wbtl.long .wbtl
    wm title .wbtl.long "Maximum line length..."
    label .wbtl.long.label -text "Length line text: "
    frame .wbtl.long.l
    entry .wbtl.long.l.entry -textvariable long -width 10
    set long [string range $long_linea 0 [expr $longitud -2] ]
    set unidad [string range $long_linea [expr $longitud -1] \
      [expr $longitud -1]]
    frame .wbtl.long.l.r
    radiobutton .wbtl.long.l.r.radio1 -text cm -variable unidad -value c \
        -selectcolor black 
    radiobutton .wbtl.long.l.r.radio2 -text mm -variable unidad -value m \
        -selectcolor black 
    radiobutton .wbtl.long.l.r.radio3 -text inch -variable unidad -value i \
        -selectcolor black
    radiobutton .wbtl.long.l.r.radio4 -text point -variable unidad -value p \
        -selectcolor black
    frame .wbtl.long.boton
    button .wbtl.long.boton.cancelar -text Cancel \
        -command {destroy .wbtl.long}
    button .wbtl.long.boton.aceptar -text OK -command { 
        set long_linea ""
        append long_linea $long $unidad
        destroy .wbtl.long
    }

    pack .wbtl.long.label .wbtl.long.l .wbtl.long.boton 
    pack .wbtl.long.boton.aceptar .wbtl.long.boton.cancelar -side left
    pack .wbtl.long.l.entry -side left
    pack .wbtl.long.l.r
    pack .wbtl.long.l.r.radio1 .wbtl.long.l.r.radio2 .wbtl.long.l.r.radio3 .wbtl.long.l.r.radio4
}

# ------------------------------------------------------------------------
# config_arrow: configura a traves de una ventana la forma de la
#		punta de flecha para las lineas
# ------------------------------------------------------------------------

proc config_arrow { } {

    global dir_iconos e1 e2 e3 muestra_flecha general

    set e1 [lindex $general(ArrowShape) 0]
    set e2 [lindex $general(ArrowShape) 1]
    set e3 [lindex $general(ArrowShape) 2]

    catch {destroy .wbtl.flecha}
    toplevel .wbtl.flecha
	update
	grab .wbtl.flecha
    wm group .wbtl.flecha .wbtl
    wm title .wbtl.flecha "Configure arrow..."

    frame .wbtl.flecha.f1  
    pack .wbtl.flecha.f1 -side top 
    set f .wbtl.flecha.f1

    canvas $f.c1 -height 100 -width 100
    pack $f.c1 -side left 

    set fich conf_flecha.xbm
    set ide1 [image create bitmap im1 -file $dir_iconos$fich]
    $f.c1 create image 50 50 -image im1
    frame $f.f1 
    pack $f.f1 

    ::iwidgets::spinint $f.f1.d1 -labeltext "d1:" -width 3 -range {0 999}  \
      -increment { 
          set e1 [examine_incr $e1 1] 
          .wbtl.flecha.f2.c1 itemconfigure $muestra_flecha -arrowshape \
              [list $e1 $e2 $e3]}  \
      -decrement { 
          set e1 [examine_dec $e1 1] 
          .wbtl.flecha.f2.c1 itemconfigure $muestra_flecha -arrowshape \
              [list $e1 $e2 $e3]}
    $f.f1.d1 configure -textvariable e1

    ::iwidgets::spinint $f.f1.d2 -labeltext "d2:" -width 3 -range {0 999} \
        -increment { 
            set e2 [examine_incr $e2 1] 
           .wbtl.flecha.f2.c1 itemconfigure $muestra_flecha -arrowshape \
               [list $e1 $e2 $e3]} \
        -decrement { 
            set e2 [examine_dec $e2 1] 
            .wbtl.flecha.f2.c1 itemconfigure $muestra_flecha -arrowshape \
               [list $e1 $e2 $e3]}
    $f.f1.d2 configure -textvariable e2

    ::iwidgets::spinint $f.f1.d3 -labeltext "d3:" -width 3 -range {0 999}  \
       -increment { 
            set e3 [examine_incr $e3 1] 
            .wbtl.flecha.f2.c1 itemconfigure $muestra_flecha -arrowshape \
               [list $e1 $e2 $e3]} \
       -decrement { 
            set e3 [examine_dec $e3 1] 
            .wbtl.flecha.f2.c1 itemconfigure $muestra_flecha -arrowshape \
               [list $e1 $e2 $e3]}
    $f.f1.d3 configure -textvariable e3

    pack $f.f1.d1 $f.f1.d2 $f.f1.d3

    frame .wbtl.flecha.f2 -bd 2 -relief groove
    pack .wbtl.flecha.f2 -expand 1 -fill x
    canvas .wbtl.flecha.f2.c1 -height 80 -width 120 
    set muestra_flecha [.wbtl.flecha.f2.c1 create line 40 40 80 40 -arrow last \
       -arrowshape $general(ArrowShape)]
    pack .wbtl.flecha.f2.c1 
	
    frame .wbtl.flecha.f3
    pack .wbtl.flecha.f3 
    button .wbtl.flecha.f3.ok -text OK -command {
        set general(ArrowShape) [list $e1 $e2 $e3]
        destroy .wbtl.flecha
    }
    button .wbtl.flecha.f3.cancel -text cancel -command { destroy .wbtl.flecha}
    pack .wbtl.flecha.f3.ok .wbtl.flecha.f3.cancel  -side left
}

# ------------------------------------------------------------------------
# config_font: configura a traves de una ventana el tipo de letra
#		para el texto
# ------------------------------------------------------------------------

proc config_font { } {

    global letra1 ant_estilo ant_tamano ant_weight
    global general normalbg c active_pag
    set ant_estilo $general(Font)
    set ant_tamano $general(Point)
    set ant_weight $general(Weight)
    catch {destroy .wbtl.letra}
    toplevel .wbtl.letra
	update
	grab .wbtl.letra
    wm group .wbtl.letra .wbtl
    wm title .wbtl.letra "Font..."
    frame .wbtl.letra.f1  
    pack .wbtl.letra.f1 -side top 
    set f .wbtl.letra.f1
    label $f.l1 -text "The font to use for the text object:" -background \
       $normalbg -foreground black \
       -font -*-times-bold-R-Normal--*-140-*-*-*-*-*-*
    pack $f.l1 

    frame .wbtl.letra.f2 -bd 2 -relief groove
    pack .wbtl.letra.f2 -fill both
    ::iwidgets::combobox .wbtl.letra.f2.cb1 -labeltext "Family" -width 15 \
       -listheight 100 -editable false -completion 0 -selectioncommand {
            .wbtl.letra.muestra.c itemconfigure $letra1 \
           -font -*-$general(Font)-$general(Weight)-R-Normal--*-[expr \
           $general(Point) * 10]-*-*-*-*-*-* 
    }
    .wbtl.letra.f2.cb1 insert list end courier helvetica times fixed clean

    .wbtl.letra.f2.cb1 configure -textvariable general(Font)
    ::iwidgets::combobox .wbtl.letra.f2.cb2 -labeltext "Weight" -width 10 \
       -editable false -listheight 60 -completion 0 -selectioncommand {
            .wbtl.letra.muestra.c itemconfigure $letra1 \
            -font -*-$general(Font)-$general(Weight)-R-Normal--*-[expr \
            $general(Point) * 10]-*-*-*-*-*-* 
    }
    .wbtl.letra.f2.cb2 insert list end bold medium

    .wbtl.letra.f2.cb2 configure -textvariable general(Weight)
    ::iwidgets::combobox .wbtl.letra.f2.cb3 -labeltext "Point" -width 10 \
       -listheight 100 -editable false -completion 0 -selectioncommand {
            .wbtl.letra.muestra.c itemconfigure $letra1 \
            -font -*-$general(Font)-$general(Weight)-R-Normal--*-[expr \
            $general(Point) * 10]-*-*-*-*-*-* 
    }
    .wbtl.letra.f2.cb3 insert list end \
	    2 6 7 8 9 10 12 13 14 15 16 18 19 20 24 25 26 30 33 34 40
    .wbtl.letra.f2.cb3 configure -textvariable general(Point)
    pack .wbtl.letra.f2.cb1 .wbtl.letra.f2.cb2 .wbtl.letra.f2.cb3 -side left

    frame .wbtl.letra.muestra -bd 2 -relief groove 
    pack .wbtl.letra.muestra -fill x
    canvas .wbtl.letra.muestra.c  -height 80 -width 500 -scrollregion \
        {-100 0 600 40} \
        -xscrollcommand ".wbtl.letra.muestra.scroll set"
    scrollbar .wbtl.letra.muestra.scroll -command ".wbtl.letra.muestra.c xview" \
       -orient horiz
    pack .wbtl.letra.muestra.c  -fill x
    pack .wbtl.letra.muestra.scroll -fill x 
    set letra1 [.wbtl.letra.muestra.c create text 250 40 -text \
        "a b c d  A B C D  1 2 3 4" \
        -font -*-$general(Font)-$general(Weight)-R-Normal--*-[expr \
        $general(Point) * 10]-*-*-*-*-*-* ]
	
    frame .wbtl.letra.f3
    pack .wbtl.letra.f3 -side bottom 

    button .wbtl.letra.f3.ok -text OK  -command {
        change_font $active_pag $general(Font) \
            $general(Weight) $general(Point)
        if {[winfo exists .wbtl.optionsBar.options.text]} {
            .wbtl.optionsBar.options.text.f1 select $general(Font)
            .wbtl.optionsBar.options.text.f2 select $general(Weight)
            .wbtl.optionsBar.options.text.f3 select $general(Point)p
        }
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_font $active_pag $general(Font) \
            $general(Weight) $general(Point)
        destroy .wbtl.letra
    }
    button .wbtl.letra.f3.cancelar -text Cancel \
      -command {
        set general(Font) $ant_estilo
        set general(Point) $ant_tamano
        set general(Weight) $ant_weight
        destroy .wbtl.letra
    }
    pack .wbtl.letra.f3.ok .wbtl.letra.f3.cancelar -side left
}




##########################################################################
# CHANGE INITIAL VARIABLES
##########################################################################

# ------------------------------------------------------------------------
# init_var: inicializa valores para las variables
# ------------------------------------------------------------------------

proc init_var { } {

    global id_text
    global ISABEL_DIR ISABEL_WORKDIR dir_iconos 
    set dir_iconos      "$ISABEL_DIR/lib/images/whiteboard/"

  # cola de mensajes para atender
    global wait_peticion
    set wait_peticion 0
    global max_peticion min_peticion
    set max_peticion 0
    set min_peticion 0
    global sending
    set sending 0
    global send_done
    set send_done 1

  # get floor
    global busy lock
    set busy 0
    set lock 0

  # General options
    global general
    set general(Width) 1
    set general(Stipple) ""
    set general(Font) courier
    set general(Point) 12
    set general(Weight) bold
    set general(Anchor) W
    set general(Align) "horizontal-inferior"
    set general(Justif) center
    set general(Arrow) none
    set general(ArrowShape) [list 10 10 5]
    set general(Info) ""
    set general(LeftButton) ""
    set general(Operation) ""
    set general(Filled) 0
    set general(Pointer) 0
    set general(ImageDir)          $ISABEL_WORKDIR
    set general(WhiteboardFileDir) $ISABEL_WORKDIR
    set general(OpenFileDir)       $ISABEL_WORKDIR
    set general(SaveDirAsPhoto)    $ISABEL_WORKDIR
    set general(SaveDirAsWbFormat) $ISABEL_WORKDIR
    set general(SaveFormat)        GIF
    set general(Mapped)        0

    global selectedSaveFileAsPhoto selectedSaveFileAsWbFormat
    set selectedSaveFileAsPhoto    $general(SaveDirAsPhoto)
    set selectedSaveFileAsWbFormat $general(SaveDirAsWbFormat)

    global mask
    set mask "*"

  # pagina
    global all_pages n_page
    set all_pages ""
    set n_page 1

  # active button
    global c
    set c "inicio"
}

##########################################################################
# CHANGE PAGE VARIABLES
##########################################################################

# ---------------------------------------------------------------------
# exec_send_variables: durante la sincronizacion el maestro envia
#		todos los valores de sus variables al esclavo
#               (He  quitado page($pag,v_scroll_old))
# ---------------------------------------------------------------------

proc exec_send_variables {pag userid} {
   global page operation_undo  

   ISA_RDOnode $userid \
      send_variables $pag $page($pag,master_id) $page($pag,master_name) \
                  $page($pag,lock) $page($pag,sel) $page($pag,ind_undo) \
                  $page($pag,FigNumber) $page($pag,FigList) $page($pag,id) 
                  

   for {set i 0} {$i <= $page($pag,ind_undo)} {incr i} {
       ISA_RDOnode $userid set operation_undo($pag,$i) $operation_undo($pag,$i)
   }
}

proc send_variables {pag master_id master_name lock sel ind_undo figNumber \
                     figList id} {
    global page
    set page($pag,master_id) $master_id
    set page($pag,master_name) $master_name
    set page($pag,lock) $lock
    set page($pag,sel) $sel
    set page($pag,ind_undo) $ind_undo
    set page($pag,FigNumber) $figNumber
    set page($pag,FigList) $figList
    set page($pag,id) $id
}

# ---------------------------------------------------------------------
# exec_send_variables: inicializa los valores de las variables al
#		crear una nueva pagina
# ---------------------------------------------------------------------

proc create_var_new_page {namepage master_id master_name} {
   global page all_pages
   set page($namepage,master_name) $master_name
   set page($namepage,master_id) $master_id
   set page($namepage,enable) 0
   set page($namepage,write) 0
   set page($namepage,sel) ""
   set page($namepage,ind_undo) -1
   set page($namepage,ind_create) 0
   set page($namepage,id) -1
   set page($namepage,lock) 0
   set page($namepage,sending) 0
   set page($namepage,FigList) "" 
   set page($namepage,FigNumber) 0 
   set all_pages [concat $all_pages $namepage]
}


# ---------------------------------------------------------------------
# libera: destruye variables al cerrar una pagina
# ---------------------------------------------------------------------

proc libera {pag} {
   global page operation_undo all_pages 

   for {set i 0 } {$i <= $page($pag,ind_undo)} {incr i} {
       catch {unset operation_undo($pag,$i)}
   }
   foreach fig $page($pag,FigList) {
	  catch {delete object $pag$fig}
   }
   foreach var {master_name master_id enable file write sel ind_undo FigList \
	FigNumber} { 
       catch {unset page($pag,$var)}
   }
   set ind [lsearch -exact $all_pages $pag]
   if {$ind != -1} {
       set all_pages [lreplace $all_pages $ind $ind]
   }
}


proc get_fig {canvas cobj} {

   if {[$canvas find withtag $cobj] == ""} { return $cobj }

   set tags [$canvas gettags $cobj]
   set ind [lsearch -glob $tags fig*]
   if {$ind != -1} {
      return [lindex $tags $ind]
   } else { return "" }
}

proc get_all_fig_from_group {pag elemento} {
  set l ""
  foreach elem $elemento {
     if {[$pag$elem cget -type] == "group"} {
         foreach el [$pag$elem cget -figlist] {
	        eval lappend l [get_all_fig_from_group $pag $el] 
         }
     } else { lappend l $elem }
  }
  return $l
}

proc get_below {pag elemento} {
   global page
   set below ""
   if {[$pag$elemento cget -type] != "group"} {
       set below [get_fig $page($pag,canvas) \
           [$page($pag,canvas) find below $elemento]]
       return $below
   }

   # Si es grupo
   set listFigList [$pag$elemento cget -figlist]
   set below [get_below $pag [lindex $listFigList 0]]
   set listFigList [lreplace $listFigList 0 0]

   foreach el $listFigList {
       set below_n [get_below $pag $el]
       if {$below_n == ""} {
           # No hay nadie por debajo de ese
           return $below_n
       }
       set ele $below
       while {[$page($pag,canvas) find below $ele] != ""} {
           set ele [get_below $pag $ele]
           # el nuevo bellow esta por debajo del bellow actual
           if {$ele == $below_n} {set below $below_n}
       }
       set listFigList [lreplace $listFigList 0 0]
   }
   return $below
}

