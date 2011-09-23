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
# $Id: wb_gui.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#########################################################################


################################################################
# CREA EL INTERFAZ
################################################################
# InitPizarraGUI 
# PutResize

# ##############################################################
# ACTIVA/DESACTIVA INTERFAZ
# ##############################################################
# activa_boton_2
# activa_eventos
# change_interface
# desactiva_eventos
# set_ui_state 
# clear_mouse_events
#
################################################################

################################################################
#
#    CREA EL INTERFAZ
#
################################################################

proc filled_option {} {
    global general active_pag 

    if {$general(Filled)} {
       # Relleno la figura

       change_pattern SOLID $active_pag \
           $general(FillColor) $general(OldFillColor)
       ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
           change_pattern SOLID $active_pag \
           $general(FillColor) $general(OldFillColor)
       set general(FillColor) $general(OldFillColor)
       .wbtl.optionsBar.color itemconfigure fillcolor \
                -fill $general(FillColor) -stipple ""
       # set general(OldFillColor) $general(FillColor)
       return

     }

     # Figura hueca
     set general(Stipple) ""
     set general(OldFillColor) $general(FillColor)
     set general(FillColor) ""
     .wbtl.optionsBar.color itemconfigure fillcolor -fill "" -stipple ""
     change_pattern NONE $active_pag $general(FillColor) $general(OldFillColor)
     ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
         change_pattern NONE $active_pag \
         $general(FillColor) $general(OldFillColor)
}

#---------------------------------------------------------------
# create_top_menu: crea el interfaz de la pizarra 
#		  definiendo zonas de botones barras de colores
#		  y estado, menus y hoja para dibujar
#---------------------------------------------------------------
proc create_top_menu {} {

   global dir_iconos

   frame .wbtl.barra  -bd 1 -relief sunken
   pack  .wbtl.barra -fill x -expand 0

   frame .wbtl.barra.menu -bd 2 -height 300  

   set menu .wbtl.barra.menu

   menubutton $menu.fichero -text "File" -menu $menu.fichero.m 
   menubutton $menu.editar -text "Edit" -menu $menu.editar.m 
   menubutton $menu.formas -text "Draw" -menu $menu.formas.m 
   menubutton $menu.config -text "Config" -menu $menu.config.m 
   menubutton $menu.texto -text "Text" -menu $menu.texto.m 
   menubutton $menu.window -text "Window" -menu $menu.window.m 
   # menubutton $menu.help -text "Help" -menu $menu.help.m 


# ------------------------- MENU FILE ---------------------------------

   set file_m $menu.fichero.m

   menu $file_m -tearoff 0 -disabledforeground cornsilk

   $file_m add command -label "Read master" -command "read_master"
   $file_m add command -label "New" -command "new_page"
   $file_m add command -label "Open..." -command "open_file_window"
   $file_m add cascade -label "Save as..." -menu $file_m.save

   menu $file_m.save -tearoff 0 
       $file_m.save add command -label "Whiteboard format" \
            -command "save_file_window_as_whiteboard_format"
       $file_m.save add command -label "Photo..." \
            -command "save_file_window_as_photo"

   $file_m add command -label "Close" -command "close_page"

   $file_m add separator

   $file_m add cascade -label "Import picture..." \
       -menu $file_m.wb
   menu $file_m.wb -tearoff 0 
       $file_m.wb add command -label "Whiteboard format" \
           -command wb_file_window
       $file_m.wb add command -label "Photo" \
           -command "image_file_window"

   $file_m add command -label "Scan picture" -command scanner

   $file_m add separator

   $file_m add command -label "Print..." -command "print_page"

   # No hay entrada para salir del componente
   # El componente no puede salir por si mismo
   # $m add separator
   # $m add command -label "Exit" -command "QuitCB" 


# --------------- MENU FORMAS: RECTANGULO, ELIPSE, LINEA, ARCO Y TEXTO--------	

   set draw_m $menu.formas.m
   menu $draw_m -tearoff 0

   $draw_m add command -label "Pointer" -command "pointer" 

   $draw_m add separator

   $draw_m add command -label "Pen"  -command  "invoke_pen_button"

   $draw_m add separator

   $draw_m add checkbutton -label "Filled" -variable general(Filled) \
      -onvalue 1 -offvalue 0 \
      -selectcolor black -command "filled_option"

   $draw_m add separator
   $draw_m add command -label "Rectangle"  -command {
        invoke_paint_figure_button rectangle "Rectangle" \
              "Rectangle. Click and drag to draw a rectangle."
   }

   $draw_m add cascade -label "Circle" -menu $draw_m.circunf
   menu $draw_m.circunf -tearoff 0
      $draw_m.circunf add command -label "Specify radio" -command {
          invoke_paint_figure_button circunf_radio "Circle" \
              "Circle, specify radio. Click \
               and drag to draw a circle."
      }

      $draw_m.circunf add command -label "Specify diameter" -command { 
          invoke_paint_figure_button circunf_diam "Circle" \
              "Circle, specify diameter. Click \
               and drag to draw a circle."
      }

   $draw_m add command -label "Oval" -command {
          invoke_paint_figure_button oval "Oval" \
              "Oval. Click and drag to draw an oval."
   }

   $draw_m add cascade -label "Line" -menu $draw_m.linea
      menu $draw_m.linea -tearoff 0
      $draw_m.linea add command -bitmap @${dir_iconos}sin_flecha.xbm -command {
         set general(Arrow) none
         if {[winfo exists .wbtl.optionsBar.options.arrow]} {
             .wbtl.optionsBar.options.arrow.mb.m invoke 0
         }
         invoke_paint_figure_button line "Line" \
             "Line. Click and drag to draw a line."
   }
   $draw_m.linea add command -bitmap @${dir_iconos}flecha_d_r.xbm -command {
      set general(Arrow) last
      if {[winfo exists .wbtl.optionsBar.options.arrow]} {
             .wbtl.optionsBar.options.arrow.mb.m invoke 1
      }
      invoke_paint_figure_button line "Line" \
          "Line. Click and drag to draw a line."
   }

   $draw_m.linea add command -bitmap @${dir_iconos}flecha_i_r.xbm -command {
      set general(Arrow) first
      if {[winfo exists .wbtl.optionsBar.options.arrow]} {
             .wbtl.optionsBar.options.arrow.mb.m invoke 2
      }
      invoke_paint_figure_button line "Line" \
          "Line. Click and drag to draw a line."
   }

   $draw_m.linea add command -bitmap @${dir_iconos}flecha_re.xbm -command {
      set general(Arrow) both
      if {[winfo exists .wbtl.optionsBar.options.arrow]} {
             .wbtl.optionsBar.options.arrow.mb.m invoke 3
      }
      invoke_paint_figure_button line "Line" \
          "Line. Click and drag to draw a line."
   }

   $draw_m add command -label "Polyline" -command {
      invoke_polypoint_figure_button linea_abierta "Polyline" \
          "Polyline. Click left button and drag. \
           Click right button when finished."
   }

   $draw_m add command -label "Spline" -command {
      invoke_polypoint_figure_button curva_abierta \
          "Spline" "Spline. Click left button and \
           drag. Click right button when finished."
   }

   $draw_m add command -label "Polygon"  -command {
      invoke_polypoint_figure_button linea_cerrada \
           "Polygon" "Polygon. Click left button and drag. \
            Click right button when finished."
   }

   $draw_m add command -label "Closed spline" -command {
       invoke_polypoint_figure_button curva_cerrada \
           "Closed spline" "Closed spline. Click \
            left button and drag. Click right button finished."
   }

   $draw_m add command -label "Arrow"  -command {
       invoke_paint_figure_button arrow \
            "Arrow" "Arrow. Click and drag to draw an arrow."
   }

   $draw_m add separator

   $draw_m add command -label Text -command "invoke_text_button"

   $draw_m add separator

   $draw_m add command -label "Bitmaps..." -command "bitmap"



# ----------------- MENU EDITAR: MOVER, BORRAR, REDIMENSIONAR, PRIMER PLANO --

   set edit_m $menu.editar.m
   menu $edit_m -tearoff 0 -disabledforeground cornsilk

   $edit_m add command -label "Undo" -command "undo"

   $edit_m add separator

   $edit_m add command -label "Select an object" -command "select"
       
   $edit_m add command -label "Multiselect" -command "multiselect"

   $edit_m add command -label "Select none" -command "select_none"

   $edit_m add separator

   $edit_m add command -label "Move" -command "move"
   $edit_m add command -label "Move points" -command "move_points"
   $edit_m add command -label "Scale" -command "scale_function"

   $edit_m add command -label "Change an object" -command \
       "invoke_edit_function_from_menu img_show_info show_info \
       \"Object information\" \"Click on an object to get its information.\""

   $edit_m add command -label "Align objects" -command "align"

   $edit_m add separator

   $edit_m add command -label "Group" -command \
       "invoke_edit_function_from_menu img_group group Group \
       \"Group. All selected objects are grouped.\""

   $edit_m add command -label "No Group" -command \
       "invoke_edit_fuction_from_menu img_nogroup no_group \"No group\" \
       \"No Group. The selected group is free.\""

   $edit_m add separator

   $edit_m add command -label "Copy" -command \
       "invoke_edit_function_from_menu img_copiar copy Copy \
       \"Copy objects. The selected objects are copied.\""

   $edit_m add command -label "Delete" -command \
       "invoke_edit_function_from_menu img_delete delete_items Delete \
       \"Delete objects. Click on the object to delete it.\""

   $edit_m add separator

   $edit_m add command -label "Rotate ninety degrees" -command \
       "invoke_edit_function_from_menu img_rotate rotate_ninety Rotate \
       \"Click on the object to rotate ninety degrees.\""

   $edit_m add command -label "Simmetry, vertical axis" \
       -command  "invoke_edit_function_from_menu img_v_sym symmetry_vertical \
          \"Vertical symmetry\" \"Vertical Symmetry. \
            Click on the object to change it.\""

   $edit_m add command -label "Simmetry, horizontal axis" \
        -command "invoke_edit_function_from_menu img_h_sym symmetry_horizontal \
          \"Horizontal symmetry\" \"Horizontal Symmetry. \
            Click on the object to change it.\""

   $edit_m add separator

   $edit_m add command -label "Above" -command \
    "invoke_edit_function_from_menu img_above top_level Above \
    \"Above. Select an object an click on the button to show up the object.\""

   $edit_m add command -label "Below" -command \
    "invoke_edit_function_from_menu img_below down_level Below \
    \"Below. Select an object an click on the button to show down the object.\""

   $edit_m add separator

   $edit_m add command -label Clear -command \
        "invoke_edit_function_from_menu img_clear clear \"Clear page\" \
        \"Delete all objects in the page.\""



# ----------------------- CONFIG MENU -----------------------------------

   set config_m $menu.config.m
   menu $config_m -tearoff 0

   $config_m add command -label "Configure arrow..." -command "config_arrow"
   $config_m add command -label "Change width..." -command "config_width"


# --------------------- TEXT MENU --------------------------------

   set text_m $menu.texto.m
   menu $text_m -tearoff 0

if {0} {
   $text_m add cascade -label "Justify" -menu $text_m.justif
   menu $text_m.justif -tearoff 0

       $text_m.justif add radio -label Letf -variable general(Justif) \
          -value left -selectcolor black

       $text_m.justif add radio -label Center -variable general(Justif) \
          -value center -selectcolor black

       $text_m.justif add radio -label Right -variable general(Justif) \
          -value right -selectcolor black
}

   $text_m add cascade -label "Anchor" -menu $text_m.anchor
   menu $text_m.anchor -tearoff 0

       foreach valor {NW N NE W C E SW S SE} {
          $text_m.anchor add radio -label $valor -variable general(Anchor) \
          -value $valor -selectcolor black -command {
              if {[winfo exists .wbtl.optionsBar.options.text]} {
                  .wbtl.optionsBar.options.text.f0 select $general(Anchor)
              }
              set anchor [change_pos $general(Anchor)]
              change_anchor $anchor $active_pag
              ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                  change_anchor $anchor $active_pag
          }

       }

   $text_m add command -label "Font..." -command "config_font"




# -------------------------- HELP MENU ---------------------------------

   # set m .wbtl.barra.menu.help.m
   # menu $m -tearoff 0

   # $m add command -label "Help..." -command { whiteboard_help }




# -------------------------  WINDOW MENU -------------------------------------

   set window_m $menu.window.m
   menu $window_m -tearoff 0

   pack append .wbtl.barra.menu $menu.fichero left  
   pack append .wbtl.barra.menu $menu.editar left  
   pack append .wbtl.barra.menu $menu.formas left  
   pack append .wbtl.barra.menu $menu.config left  
   pack append .wbtl.barra.menu $menu.texto left  
   pack append .wbtl.barra.menu $menu.window left  
   # pack append $menu $menu.help left  
   pack append .wbtl.barra $menu {left}

}

proc text_buttons {} {
    global general
    set text_op_f .wbtl.optionsBar.options.text 
    
    if {[winfo exists $text_op_f]} return
    
    if {![winfo exists .wbtl.optionsBar.options]} {
	frame .wbtl.optionsBar.options -relief sunken -bd 1
	pack .wbtl.optionsBar.options -side left 
    }
    
    frame $text_op_f
    pack $text_op_f -side left
    
    ::swc::Optionmenu $text_op_f.f0 -narrow 1 \
	    -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
	    -labelmargin 0 -command {
	set general(Anchor) [.wbtl.optionsBar.options.text.f0 get]
	set anchor [change_pos $general(Anchor)]
	change_anchor $anchor $active_pag
	ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                change_anchor $anchor $active_pag
	
    }
    $text_op_f.f0 insert end NW N NE E C W SW S SE
	    
    switch $general(Anchor) {
      "n"      {set selected "N"}
      "ne"     {set selected "NE"}
      "nw"     {set selected "NW"}
      "s"      {set selected "S"}
      "se"     {set selected "SE"}
      "sw"     {set selected "SW"}
      "e"      {set selected "E"}
      "center" {set selected "C"}
      default  {set selected "W"}
   }

   $text_op_f.f0 select $selected


   ::swc::Optionmenu $text_op_f.f1 -narrow 1 \
	   -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
	   -labelmargin 0 \
           -labelfont -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
	   -command {
       set general(Font) [.wbtl.optionsBar.options.text.f1 get]
       change_font $active_pag $general(Font) $general(Weight) \
	       $general(Point)
       ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
	       change_font $active_pag $general(Font) \
	       $general(Weight) $general(Point)
   }

   $text_op_f.f1 insert end courier helvetica times fixed clean
   
   $text_op_f.f1 select $general(Font)


   ::swc::Optionmenu $text_op_f.f2 -narrow 1 \
      -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
      -labelmargin 0 -command {
         set general(Weight) [.wbtl.optionsBar.options.text.f2 get]
         change_font $active_pag $general(Font) $general(Weight) \
            $general(Point)
         ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_font $active_pag $general(Font) \
            $general(Weight) $general(Point)
   }

   $text_op_f.f2 insert end bold medium

   $text_op_f.f2 select $general(Weight)


   ::swc::Optionmenu $text_op_f.f3 -narrow 1 \
      -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
      -labelmargin 0 -command {
         set f [.wbtl.optionsBar.options.text.f3 get]
         set general(Point) [string range $f 0 [expr [string length $f]-2]]
         change_font $active_pag $general(Font) $general(Weight) \
            $general(Point)
         ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_font $active_pag $general(Font) \
            $general(Weight) $general(Point)
   }

   $text_op_f.f3 insert end \
	   2p 6p 7p 8p 9p 10p 12p 13p 14p 15p 16p 18p 19p 20p \
	   24p 25p 26p 30p 33p 34p 40p
        
   $text_op_f.f3 select [append kk $general(Point) p]
 
if {0} {
   ::swc::Optionmenu $text_op_f.f4 -narrow 1 \
      -items {left center right} \
      -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
      -labelmargin 0 -command {
         set general(Justif) [.wbtl.optionsBar.options.text.f4 get]
         change_justify $active_pag $general(Justif)
         ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_justify $active_pag $general(Justif) 
   }

   $text_op_f.f4 select $general(Justif)
}
   pack $text_op_f.f1 $text_op_f.f2 $text_op_f.f3 $text_op_f.f0 \
      -side left
}


proc create_floor_buttons {} {
   global normalbg activebg uniqueMaster
   set floor_f .wbtl.barra.floor

   frame $floor_f -bd 1 -relief sunken
   pack $floor_f -side left -padx 50
   label $floor_f.l1 -text "Floor: " \
      -font -*-times-bold-R-Normal--*-140-*-*-*-*-*-*
   label $floor_f.l2  -foreground black  -background $normalbg \
      -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-* 

   pack $floor_f.l1 $floor_f.l2 -side left
   if {!$uniqueMaster} {
       checkbutton $floor_f.floor -image img_pointer \
          -indicatoron 0 -selectcolor $activebg \
          -selectimage img_no_pointer -command ask_floor
       balloon add $floor_f.floor "Get Floor"
       checkbutton $floor_f.lock -indicatoron 0 -selectcolor LightBlue \
          -image unlock -command lock_floor 
       balloon add $floor_f.lock "Lock Floor"
       pack $floor_f.floor $floor_f.lock -side left
   }
}


proc create_interface_images {} {
   global dir_iconos

   image create photo lock -file ${dir_iconos}lock.gif
   image create photo unlock -file ${dir_iconos}unlock.gif

   image create photo img_group -file ${dir_iconos}group.gif
   image create photo img_ungroup -file ${dir_iconos}ungroup.gif
   image create photo img_above -file ${dir_iconos}above.gif
   image create photo img_below -file ${dir_iconos}below.gif
   image create photo img_h_sym -file ${dir_iconos}h_sym.gif
   image create photo img_v_sym -file ${dir_iconos}v_sym.gif
   image create photo img_rotate -file ${dir_iconos}rotate.gif
   image create photo img_select -file ${dir_iconos}select.gif
   image create photo img_multiselect -file ${dir_iconos}multiselect.gif
   image create photo img_clear -file ${dir_iconos}clear.gif
   image create photo img_delete -file ${dir_iconos}borrar.gif
   image create photo img_move -file ${dir_iconos}move.gif
   image create photo img_move_points -file ${dir_iconos}move_point.gif
   image create photo img_show_info -file ${dir_iconos}info.gif
   image create photo img_copiar -file ${dir_iconos}copiar.gif
   image create photo img_align -file ${dir_iconos}align.gif
   image create photo img_scale_function -file ${dir_iconos}scale.gif
   image create bitmap img_pointer -file ${dir_iconos}floor.xbm
   image create bitmap img_no_pointer -file ${dir_iconos}no_floor.xbm
   image create bitmap img_pointer_to -file ${dir_iconos}point_to.xbm
   image create photo img_pen -file ${dir_iconos}pen.gif
   image create photo img_pen1 -file ${dir_iconos}pen1.gif

   image create photo img_new -file ${dir_iconos}nuevo1.gif
   image create photo img_open -file ${dir_iconos}abrir1.gif
   image create photo img_close -file ${dir_iconos}cerrar1.gif
   image create photo img_print -file ${dir_iconos}print.gif
   image create photo img_scanner -file ${dir_iconos}scanner1.gif

   image create bitmap puntero_id -file ${dir_iconos}hand.xbm 
}

proc create_info_tab {} {
   global entry_colour  general
   set state_f .wbtl.state
   frame $state_f 
   pack $state_f -anchor w -fill x

   frame $state_f.f -bd 2 -relief sunken
   label $state_f.f.info -foreground black \
      -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-*

   pack $state_f.f -side left -fill x -expand 1 -fill x
   pack $state_f.f.info -side left -fill x -expand 1
}



proc InitPizarraGUI {} {

   global interior latestAction entry_colour activebg \
          normalbg general

   button .wbtl.bbb;
   set normalbg [lindex [.wbtl.bbb config -bg] 4]
   set activebg [lindex [.wbtl.bbb config -activebackground] 4]
   entry .wbtl.kkk
   #set entry_colour [lindex [.wbtl.kkk config -insertbackground] 4]
   set entry_colour [IsabelColorsC::GetResource background Background red]

   destroy .wbtl.bbb; destroy .wbtl.kkk

   #BalloonClass balloon

   create_interface_images
   set latestAction pen 

   # frame .wbtl.f 
   # pack .wbtl.f -side left -fill both -expand 1

   # --------- Crea la barra de menu ---------
   create_top_menu

   # --------- Crea botones para gestion de floor ---------
   create_floor_buttons
   pointer_button

   # --------- Crea la barra de botones de texto ---------
   frame .wbtl.optionsBar
   pack  .wbtl.optionsBar -fill x -expand 0
   
   # ---------- Crea barra de operaciones  ----------
   file_buttons

   frame .wbtl.optionsBar.buttons -relief sunken -bd 1
   pack .wbtl.optionsBar.buttons -side left -padx 10

   draw_buttons
   operation_buttons

   frame .wbtl.optionsBar.options -relief sunken -bd 1
   pack .wbtl.optionsBar.options -side left 

   color_canvas

   # --------- Frame para la hoja de dibujo ---------- 
   frame .wbtl.f1
   pack .wbtl.f1 -fill both -expand 1 

   # ----------- Hoja de dibujo ---------
   ::iwidgets::tabnotebook .wbtl.f1.note -equaltabs false \
       -raiseselect false -height 15c
   pack .wbtl.f1.note -fill both -expand 1

   # ---------- Opciones de colores ----------
   set general(FillColor) ""
   set general(OldFillColor) "#ffffff"
   set general(Color) "#0000ff"
   set general(Pattern) "none"
   set interior 1

   # ---------- Crea la barra de informacion de botones ---------
   create_info_tab
}



# ==========================================================================
# 				VIEW FUNCTIONS	
# ==========================================================================

proc change_info {msg} {
   global general page active_pag
   if {[$page($active_pag,canvas) find withtag sendingWindow] == ""} {
       # no estoy enviando
       if {($msg == "LeftButton") || ($msg == "Operation")} {
           .wbtl.state.f.info configure -text $general($msg)
           return
       }
       .wbtl.state.f.info configure -text $msg
   }
}



proc file_buttons { } {

   global entry_colour general

   # #############################
   #  Botones para ficheros
   # #############################

   set b_file .wbtl.optionsBar.file
  
   frame $b_file -bd 1 -relief sunken
   pack $b_file -side left 

   button $b_file.new -image img_new -command "new_page"
 
   bind $b_file.new <Any-Enter> "+change_info \"New page\""
   balloon add $b_file.new "New page"
 
 
   button $b_file.open -image img_open -command "open_file_window"
 
   bind $b_file.open <Any-Enter> "+change_info \"Open file...\""
   balloon add $b_file.open "Open file..."
 
 
   button $b_file.print -image img_print -command "print_page"
 
   bind $b_file.print <Any-Enter> "+change_info \"Print...\""
   balloon add $b_file.print "Print..."


   button $b_file.scanner -image img_scanner -command scanner
 
   bind $b_file.scanner <Any-Enter> "+change_info \"Scan picture...\""
   balloon add $b_file.scanner "Scan picture..."

   foreach el {new open print scanner} {
       pack $b_file.$el -side left
       bind $b_file.$el <Any-Leave> "+change_info \"\""
   }

} 
 

#---------------------------------------------------------------
# operation_buttons: crea los botones del interfaz para 
#		  realizar operaciones sobre las figuras
#---------------------------------------------------------------
proc operation_buttons {} {

   global dir_iconos entry_colour general

   # #######################################
   # Botones para operaciones sobre figuras
   # #######################################

   # frame .wbtl.optionsBar.buttons.nothing -width 5
   # pack .wbtl.optionsBar.buttons.nothing -side left
   frame .wbtl.optionsBar.buttons.edit
   pack .wbtl.optionsBar.buttons.edit -side left -padx 2

   set edit .wbtl.optionsBar.buttons.edit
  
   frame $edit.f
   pack $edit.f

   button $edit.f.l1 -bitmap @${dir_iconos}flecha_sel1.xbm \
      -foreground black -borderwidth 1 -relief groove
   pack $edit.f.l1 -side left


   menubutton $edit.f.mb -bitmap @${dir_iconos}menu_but1.xbm \
      -relief groove -borderwidth 1 -menu $edit.f.mb.m -anchor center
   pack $edit.f.mb -side left

   set m $edit.f.mb.m
   menu $m -tearoff 0

   $m add command -image img_group -foreground black \
      -command "invoke_edit_function img_group group Group \
      \"Group. All selected objects are grouped.\""

   $m add command -image img_ungroup -foreground black \
      -command "invoke_edit_function img_ungroup no_group \"No group\" \
      \"No Group. The selected group is free.\""

   $m add command -image img_copiar -foreground black \
      -command "invoke_edit_function img_copiar copy Copy \
      \"Copy objects. The selected objects are copied.\""

   $m add command -image img_rotate -foreground black \
      -command "invoke_edit_function img_rotate rotate_ninety Rotate \
      \"Click on the object to rotate ninety degrees.\""

   $m add command -image img_delete -foreground black \
      -command "invoke_edit_function img_delete delete_items Delete \
      \"Delete objects. Click on the object to delete it.\""

   $m add command -image img_h_sym -foreground black \
      -command "invoke_edit_function img_h_sym symmetry_horizontal \
          \"Horizontal symmetry\" \"Horizontal Symmetry. \
            Click on the object to change it.\""

   $m add command -image img_v_sym -foreground black \
      -command "invoke_edit_function img_v_sym symmetry_vertical \
          \"Vertical symmetry\" \"Vertical Symmetry. \
            Click on the object to change it.\""

   $m add command -image img_above -foreground black \
      -command "invoke_edit_function img_above top_level \"Above level\" \
      \"Above. Select an object an click on the button to show up the object.\""

   $m add command -image img_below -foreground black \
    -command "invoke_edit_function img_below down_level \"Below level\" \
    \"Below. Select an object an click on the button to show down the object.\""

   $m add command -image img_show_info -foreground black \
      -command "invoke_edit_function img_show_info show_info \
      \"Object information\" \
      \"Click on an object to get its information.\""

   $m add command -image img_clear -foreground black \
      -command "invoke_edit_function img_clear clear \"Clear page\" \
      \"Delete all objects in the page.\""

   bind .wbtl.optionsBar.buttons.edit.f.l1 <Any-Enter> \
      "+change_info Operation"
   bind .wbtl.optionsBar.buttons.edit.f.l1 <Any-Leave> \
      "+change_info \"\""
}



proc pointer_button {} {
   global activebg
   checkbutton .wbtl.barra.pointer -image img_pointer_to -indicatoron 0 \
      -variable general(Pointer) -onvalue 1 -offvalue 0 \
      -selectcolor $activebg -command {
         global general
         if {$general(Pointer)} {
            pointer
         } else {
             bind $page($active_pag,canvas) <Any-Motion> {}
             bind $page($active_pag,canvas) <Any-Leave> {}
             bind $page($active_pag,canvas) <Any-Enter> {}
         }
   }
   pack .wbtl.barra.pointer -side right
   balloon add .wbtl.barra.pointer "Pointer"
}

proc color_canvas {} {
    canvas .wbtl.optionsBar.color -width 35 -height 35 
    pack .wbtl.optionsBar.color -side right -expand 0 -fill none
    balloon add .wbtl.optionsBar.color "Color window"
    .wbtl.optionsBar.color create rectangle 5 5 25 25 -fill "" -tag fillcolor
    .wbtl.optionsBar.color create rectangle 15 15 35 35 -fill blue -tag color
    bind .wbtl.optionsBar.color <ButtonPress-1> {
        set el [lindex [.wbtl.optionsBar.color find overlapping %x %y %x %y] end]
        if {$el == ""} { 
              create_color_window fill
              return
        }
        set tag [lindex [.wbtl.optionsBar.color itemcget $el -tag] 0]
        create_color_window $tag
    }
}

proc filled_button {} {
   global entry_colour 
   set barra .wbtl.optionsBar.options
   
   if {[winfo exists $barra.filled]} return
   if {![winfo exists $barra]} {
       frame .wbtl.optionsBar.options -relief sunken -bd 1
       pack .wbtl.optionsBar.options -side left 
   }

   checkbutton $barra.filled -relief raised  \
      -onvalue 1 -offvalue 0 -variable \
      general(Filled)  -text "Filled" \
      -font -*-times-bold-R-Normal--*-120-*-*-*-*-*-* -command {
         if {$general(Filled)} {
            # Relleno la figura
            if {[winfo exists .wbtl.color]} {
               .wbtl.color.pattern itemconfigure $general(Pattern) -outline black
               set general(Pattern) [.wbtl.color.pattern find withtag white]
               set coords [.wbtl.color.pattern coords $general(Pattern)]
               .wbtl.color.pattern delete $general(Pattern)
               set general(Pattern) [eval .wbtl.color.pattern create rectangle \
                  $coords -fill white -outline white -tags white]
            }
            change_pattern SOLID $active_pag $general(FillColor) \
                $general(OldFillColor)
            ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                change_pattern SOLID $active_pag \
                $general(FillColor) $general(OldFillColor)

            set general(FillColor) $general(OldFillColor)
            .wbtl.optionsBar.color itemconfigure fillcolor \
                -fill $general(FillColor) -stipple ""
            # set general(OldFillColor) $general(FillColor)
            return

         }
         # Figura hueca
         set general(Stipple) ""
         set general(OldFillColor) $general(FillColor)
         set general(FillColor) ""
         .wbtl.optionsBar.color itemconfigure fillcolor -fill "" -stipple ""

         if {[winfo exists .wbtl.color]} {
             .wbtl.color.pattern itemconfigure $general(Pattern) -outline black
             set general(Pattern) [.wbtl.color.pattern find withtag SOLID]
             set coords [.wbtl.color.pattern coords $general(Pattern)]
             .wbtl.color.pattern delete NONE
             .wbtl.color.pattern delete SOLID
             set general(Pattern) [eval .wbtl.color.pattern \
                 create rectangle $coords -fill \
                 white -tag SOLID -outline white]
             .wbtl.color.pattern create text 24 14 -text NONE -justify left \
                -anchor center -tag NONE
         }
         change_pattern NONE $active_pag $general(FillColor) \
             $general(OldFillColor)
         ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
             change_pattern NONE $active_pag \
             $general(FillColor) $general(OldFillColor)
   }
   pack $barra.filled -side left
}


#---------------------------------------------------------------
# width_buttons: crea los botones del interfaz para 
#		 seleccionar grueso de las lineas 
#---------------------------------------------------------------
proc width_buttons {} {

   global dir_iconos entry_colour general
   set barra .wbtl.optionsBar.options
   if {[winfo exists $barra.width]} return

   if {![winfo exists $barra]} {
       frame .wbtl.optionsBar.options -relief sunken -bd 1
       pack .wbtl.optionsBar.options -side left 
   }

   frame $barra.width
   pack $barra.width -side left

   switch $general(Width) {
        "1" { set selected g1.xbm }
        "2" { set selected g2.xbm }
        "3" { set selected g3.xbm }
        "4" { set selected g4.xbm }
        default { set selected g8.xbm }
   }

   label $barra.width.l1 -bitmap @${dir_iconos}$selected -foreground \
       black -background $entry_colour -borderwidth 2 -relief raised
   pack $barra.width.l1 -side left
	  
   balloon add $barra.width.l1  "Selected width"

   menubutton $barra.width.mb -bitmap @${dir_iconos}menu_but.xbm \
      -relief raised -borderwidth 2 -menu $barra.width.mb.m -anchor sw
   pack $barra.width.mb

   balloon add $barra.width.mb  "Change width"

   set m $barra.width.mb.m
   menu $m -tearoff 0 

   $m add command -bitmap @${dir_iconos}g1.xbm -foreground black \
      -background $entry_colour -command {
        .wbtl.optionsBar.options.width.l1 configure -bitmap @${dir_iconos}g1.xbm
        set general(Width) 1
        change_width $active_pag 1
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_width $active_pag 1
   }

   $m add command -bitmap @${dir_iconos}g2.xbm -foreground black \
      -background $entry_colour -command {
        .wbtl.optionsBar.options.width.l1 configure -bitmap @${dir_iconos}g2.xbm
        set general(Width) 2
        change_width $active_pag 2
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_width $active_pag 2
   }

   $m add command -bitmap @${dir_iconos}g3.xbm -foreground black \
      -background $entry_colour -command {
        .wbtl.optionsBar.options.width.l1 configure -bitmap @${dir_iconos}g3.xbm
        set general(Width) 3
        change_width $active_pag 3
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_width $active_pag 3
   }

   $m add command -bitmap @${dir_iconos}g4.xbm -foreground black \
      -background $entry_colour -command {
        .wbtl.optionsBar.options.width.l1 configure -bitmap @${dir_iconos}g4.xbm
        set general(Width) 4
        change_width $active_pag 4
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_width $active_pag 4
   }

   $m add command -bitmap @${dir_iconos}g8.xbm -foreground black \
      -background $entry_colour -command {
        .wbtl.optionsBar.options.width.l1 configure -bitmap @${dir_iconos}g8.xbm
        set general(Width) 8
        change_width $active_pag 8
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_width $active_pag 8
   }
}


proc arrow_buttons {} {
   global dir_iconos entry_colour general
   set barra .wbtl.optionsBar.options

   if {[winfo exists $barra.arrow]} return

   if {![winfo exists $barra]} {
       frame .wbtl.optionsBar.options -relief sunken -bd 1
       pack .wbtl.optionsBar.options -side left 
   }

   frame $barra.arrow
   pack $barra.arrow -side left

   switch $general(Arrow) {
       "both" { set selected l4.xbm }
       "last" { set selected l2.xbm }
       "first" { set selected l3.xbm }
       default { set selected l1.xbm }
   }

   label $barra.arrow.l1 -bitmap @${dir_iconos}$selected -foreground \
       black -background $entry_colour -borderwidth 2 -relief raised
   pack $barra.arrow.l1 -side left

   balloon add $barra.arrow.l1  "Selected arrow style"

   menubutton $barra.arrow.mb -bitmap @${dir_iconos}menu_but.xbm \
      -relief raised -borderwidth 2 -menu $barra.arrow.mb.m -anchor sw
   pack $barra.arrow.mb

   balloon add $barra.arrow.mb  "Change arrow style"

   set m $barra.arrow.mb.m
   menu $m -tearoff 0 

   $m add command -bitmap @${dir_iconos}l1.xbm -foreground black \
      -background $entry_colour -command {
        .wbtl.optionsBar.options.arrow.l1 configure -bitmap @${dir_iconos}l1.xbm
        set general(Arrow) none
        change_arrow $active_pag none
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_arrow $active_pag none
   }

   $m add command -bitmap @${dir_iconos}l2.xbm -foreground black \
      -background $entry_colour  -command {
        .wbtl.optionsBar.options.arrow.l1 configure -bitmap @${dir_iconos}l2.xbm
        set general(Arrow) last
        change_arrow $active_pag last
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_arrow $active_pag last
   }

   $m add command -bitmap @${dir_iconos}l3.xbm -foreground black \
      -background $entry_colour  -command {
        .wbtl.optionsBar.options.arrow.l1 configure -bitmap @${dir_iconos}l3.xbm
        set general(Arrow) first
        change_arrow $active_pag first
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_arrow $active_pag first
   }

   $m add command -bitmap @${dir_iconos}l4.xbm -foreground black \
      -background $entry_colour  -command {
        .wbtl.optionsBar.options.arrow.l1 configure -bitmap @${dir_iconos}l4.xbm
        set general(Arrow) both
        change_arrow $active_pag both
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            change_arrow $active_pag both
   }

}

proc invoke_edit_function {img function balloon_msg info} {
   global general 
   .wbtl.optionsBar.buttons.edit.f.l1 configure -image $img \
       -command $function
   balloon add .wbtl.optionsBar.buttons.edit.f.l1 $balloon_msg
   set general(Operation) $info
}

proc invoke_edit_function_from_menu {img function balloon_msg info} {
    invoke_edit_function $img $function $balloon_msg $info
    $function
}


#---------------------------------------------------------------
# draw_buttons: crea los botones del interfaz para dibujar
#		 figuras 
#---------------------------------------------------------------

proc invoke_paint_figure_button {clase balloon_msg info} {
    global dir_iconos general latestAction 
    catch {destroy .wbtl.menuDraw}
    catch {destroy .wbtl.optionsBar.options.text}
    activa_eventos
    width_buttons
    if {$clase == "line"} {
        catch {destroy .wbtl.optionsBar.options.filled}
        arrow_buttons
    } else {
        catch {destroy .wbtl.optionsBar.options.arrow}
        filled_button
    }
    balloon add .wbtl.optionsBar.buttons.draw.b $balloon_msg
    .wbtl.optionsBar.buttons.draw.b configure \
       -bitmap @${dir_iconos}${clase}1.xbm -image ""
    set general(LeftButton) $info
    set latestAction "paint_figure $clase"
    paint_figure $clase
}

proc invoke_polypoint_figure_button {clase balloon_msg info} {
    global dir_iconos general latestAction
    catch {destroy .wbtl.menuDraw}
    catch {destroy .wbtl.optionsBar.options.filled}
    catch {destroy .wbtl.optionsBar.options.text}
    activa_eventos
    eval $latestAction
    if {($clase == "linea_cerrada") || ($clase == "curva_cerrada")} {
        filled_button
    }
    width_buttons
    arrow_buttons
    balloon add .wbtl.optionsBar.buttons.draw.b $balloon_msg
    .wbtl.optionsBar.buttons.draw.b configure \
       -bitmap @${dir_iconos}${clase}1.xbm -image ""
    set general(LeftButton) $info
    set latestAction "paint_polypoint_figure $clase"
    paint_polypoint_figure $clase
}

proc invoke_pen_button {} {
    global dir_iconos latestAction general 
    catch {destroy .wbtl.menuDraw}
    catch {destroy .wbtl.optionsBar.options.text}
    catch {destroy .wbtl.optionsBar.options.filled}
    activa_eventos
    eval $latestAction
    width_buttons
    .wbtl.optionsBar.buttons.draw.b configure -image img_pen
    set general(LeftButton) "Pen. Click and drag."
    set latestAction "pen"
    pen
}

proc invoke_text_button {} {
    global dir_iconos latestAction general 
    catch {destroy .wbtl.menuDraw}
    catch {destroy .wbtloptionsBar.options.arrow}
    catch {destroy .wbtl.optionsBar.options.width}
    catch {destroy .wbtl.optionsBar.options.filled}
    activa_eventos
    eval $latestAction
    text_buttons
    .wbtl.optionsBar.buttons.draw.b configure \
        -bitmap @${dir_iconos}texto1.xbm -image ""
    set general(LeftButton) "Text. Click to add text to image."
    set latestAction "item_text"
    item_text
}


proc create_draw_options {} {
    global dir_iconos
    if {[winfo exists .wbtl.menuDraw]} { return }

    desactiva_eventos
    toplevel .wbtl.menuDraw

    set position [winfo pointerxy .wbtl.optionsBar.buttons.draw.b]
    set posx [lindex $position 0]
    set posy [lindex $position 1]
    wm geometry .wbtl.menuDraw +$posx+$posy

    button .wbtl.menuDraw.rectangle -bitmap @${dir_iconos}rectangle1.xbm \
       -foreground black -borderwidth 2 -relief raised  \
       -command {
          invoke_paint_figure_button rectangle "Rectangle" \
              "Rectangle. Click and drag to draw a rectangle."
    }
    balloon add .wbtl.menuDraw.rectangle "Rectangle"

    button .wbtl.menuDraw.circunf_radio \
      -bitmap @${dir_iconos}circunf_radio1.xbm -foreground black \
      -command {
          invoke_paint_figure_button circunf_radio "Circle" \
              "Circle, specify radio. Click \
               and drag to draw a circle."
    }
    balloon add .wbtl.menuDraw.circunf_radio "Circle"

    button .wbtl.menuDraw.circunf_diam \
      -bitmap @${dir_iconos}circunf_diam1.xbm -foreground black \
      -command {
          invoke_paint_figure_button circunf_diam "Circle" \
              "Circle, specify diameter. Click \
               and drag to draw a circle."
    }
    balloon add .wbtl.menuDraw.circunf_diam "Circle"

    button .wbtl.menuDraw.oval -bitmap @${dir_iconos}oval1.xbm \
      -foreground black -command {
          invoke_paint_figure_button oval "Oval" \
              "Oval. Click and drag to draw an oval."
    }
    balloon add .wbtl.menuDraw.oval "Oval"

    button .wbtl.menuDraw.line -bitmap @${dir_iconos}line1.xbm \
      -foreground black -command {
          invoke_paint_figure_button line "Line" \
             "Line. Click and drag to draw a line."
    }
    balloon add .wbtl.menuDraw.line "Line"

    button .wbtl.menuDraw.arrow \
      -bitmap @${dir_iconos}arrow1.xbm -foreground black \
      -command {
          invoke_paint_figure_button arrow "Arrow" \
               "Arrow. Click and drag to draw an arrow."
    }
    balloon add .wbtl.menuDraw.arrow "Arrow"

    button .wbtl.menuDraw.pen -image img_pen -foreground black \
      -command {
          invoke_pen_button
          balloon add .wbtl.optionsBar.buttons.draw.b "Pen"
    }
    balloon add .wbtl.menuDraw.pen "Pen"

    button .wbtl.menuDraw.text \
      -bitmap @${dir_iconos}texto1.xbm -foreground black \
      -command {
          invoke_text_button
          balloon add .wbtl.optionsBar.buttons.draw.b "Text"
    }
    balloon add .wbtl.menuDraw.text "Text"

    button .wbtl.menuDraw.linea_abierta  \
      -bitmap @${dir_iconos}linea_abierta1.xbm -foreground black \
      -command {
          invoke_polypoint_figure_button linea_abierta \
              "Polyline" "Polyline. Click left button and drag. \
               Click right button when finished."
    }
    balloon add .wbtl.menuDraw.linea_abierta "Polyline"

    button .wbtl.menuDraw.linea_cerrada \
      -bitmap @${dir_iconos}linea_cerrada1.xbm -foreground black \
      -command {
          invoke_polypoint_figure_button linea_cerrada \
              "Polygon" "Polygon. Click left button and drag. \
               Click right button when finished."
    }
    balloon add .wbtl.menuDraw.linea_cerrada "Polygon"

    button .wbtl.menuDraw.curva_abierta \
      -bitmap @${dir_iconos}curva_abierta1.xbm -foreground black \
      -command {
          invoke_polypoint_figure_button curva_abierta \
              "Spline" "Spline. Click left button and \
               drag. Click right button when finished."
    }
    balloon add .wbtl.menuDraw.curva_abierta "Spline"

    button .wbtl.menuDraw.curva_cerrada \
      -bitmap @${dir_iconos}curva_cerrada1.xbm -foreground black \
      -command {
          invoke_polypoint_figure_button curva_cerrada \
              "Closed Spline" "Closed spline. Click \
               left button and drag. Click right button finished."
    }
    balloon add .wbtl.menuDraw.curva_cerrada "Closed spline"
   
    foreach l {adm preg pro sen stop tri} {
       button .wbtl.menuDraw.$l \
           -bitmap @${dir_iconos}${l}_icono1.xbm -foreground black \
           -command "bitmap_command $l; \
              balloon add .wbtl.optionsBar.buttons.draw.b \"Closed spline\""
       balloon add .wbtl.menuDraw.$l "Bitmap"
    }

    foreach l {select multiselect move move_points scale_function align} {
       button .wbtl.menuDraw.$l -image img_$l -command "$l"
    }

    balloon add .wbtl.menuDraw.select         "Select"
    balloon add .wbtl.menuDraw.multiselect    "Multiselect"
    balloon add .wbtl.menuDraw.move           "Move"
    balloon add .wbtl.menuDraw.move_points    "Move Points"
    balloon add .wbtl.menuDraw.scale_function "Scale"
    balloon add .wbtl.menuDraw.align          "Align"


    set row_index 1
    foreach l {rectangle oval pen curva_abierta adm sen select multiselect} {
       grid .wbtl.menuDraw.$l -row $row_index -column 1
       incr row_index
    }
    set row_index 1
    foreach l {circunf_radio arrow line linea_cerrada preg stop move scale_function} {
       grid .wbtl.menuDraw.$l -row $row_index -column 2
       incr row_index
    }
    set row_index 1
    foreach l {circunf_diam text linea_abierta curva_cerrada pro \
              tri move_points align} {
       grid .wbtl.menuDraw.$l -row $row_index -column 3
       incr row_index
    }

    wm overrideredirect .wbtl.menuDraw 1
    bind .wbtl <ButtonRelease-1> "destroyMenuDraw"
}

proc destroyMenuDraw {} {
    global latestAction
    bind .wbtl <ButtonRelease-1> {}
    if {[winfo exists .wbtl.menuDraw] } {
        # he pulsado fuera del menu
        catch {destroy .wbtl.menuDraw}
        activa_eventos
        eval $latestAction
    }
}


proc draw_buttons {} {
   global dir_iconos general

   set draw .wbtl.optionsBar.buttons.draw

   frame $draw 
   pack  $draw -side left -ipadx 3


   button $draw.b -image img_pen \
      -foreground black -borderwidth 1 -relief groove

   pack $draw.b
   balloon add .wbtl.optionsBar.buttons.draw.b "Pen"

   bind $draw.b <ButtonRelease-1> create_draw_options
   bind $draw.b <Any-Enter> "+change_info LeftButton"
   bind $draw.b <Any-Leave> "+change_info \"\""
}



# ===========================================================================
#		CAMBIA GEOMETRIA DE LA VENTANA PRINCIPAL
# ===========================================================================

#---------------------------------------------------------------
# PutResize:  para cambiar el tamanio de la ventana principal
#---------------------------------------------------------------

proc PutResize {resize} {
    if {!$resize} {
        # No permito cambiar el tamano de la ventana
        wm resizable .wbtl 0 0
        return
    }
}


################################################################
#
#		ACTIVA/DESACTIVA  INTERFAZ
#
################################################################

#---------------------------------------------------------------
# activa_boton_2: Activa el boton 2 para mover figuras
#---------------------------------------------------------------
proc activa_boton_2 { } {
   global page active_pag 

   set canvas $page($active_pag,canvas) 

   bind $canvas <ButtonPress-2> "press_move_button \
                            \[$canvas canvasx %x\] \
                            \[$canvas canvasy %y\]"

   bind $canvas <Any-B2-Motion> "any_motion_move_button \
                            \[$canvas canvasx %x\] \
                            \[$canvas canvasy %y\]"

   bind $canvas <ButtonRelease-2> "release_move_button \
                            \[$canvas canvasx %x\] \
                            \[$canvas canvasy %y\]"
}

#---------------------------------------------------------------
# activa_eventos: Activa las acciones del raton sobre la pizarra
#---------------------------------------------------------------
proc activa_eventos {} {
    global page c busy active_pag ok latestAction 
    set c $page($active_pag,canvas)

    activa_boton_2

    bind $page($active_pag,canvas) <ButtonPress-3> {
        catch {destroy .wbtl.menuDraw}
        incr busy 
        DebugTraceMsg "Wb:: activa_eventos: INCR activa eventos" 9
    }

    bind $page($active_pag,canvas) <ButtonRelease-3> {
        select_button [$c canvasx %x] [$c canvasy %y]
        incr busy -1
        DebugTraceMsg "Wb:: activa_eventos: DEC activa_eventos" 9 
        examine_peticion_queue
    }

    bind $page($active_pag,canvas) <Shift-ButtonPress-3> {
        incr busy 
        DebugTraceMsg "Wb:: activa_eventos: INCR add_group" 9
    }

    bind $page($active_pag,canvas) <Shift-ButtonRelease-3> {
        global page  active_pag
        save_undo_multiselect $active_pag
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
            save_undo_multiselect $active_pag

        set posx [$page($active_pag,canvas) canvasx %x]
        set posy [$page($active_pag,canvas) canvasy %y]

        set fig [find_figure $posx $posy]

        if {$fig != ""} {
            while {[$active_pag$fig cget -father] != ""} {
                set fig [$active_pag$fig cget -father]
            }
            if {[$active_pag$fig cget -selec]} {
                $active_pag$fig delete_selection 
                ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                    $active_pag$fig delete_selection 
            } else {
                $active_pag$fig select 
                ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
                    $active_pag$fig select
            }
         }
         incr busy -1
         DebugTraceMsg "Wb:: activa_eventos: DEC add_group" 9
         examine_peticion_queue
    }

    bind .wbtl <Delete> {delete_items}
    if {[winfo exists .wbtl.color]} {active_color_window}
    # eval $latestAction

    bind .wbtl.optionsBar.buttons.draw.b <ButtonRelease-1> create_draw_options
}


proc active_color_window {} {
   set f .wbtl.color.type_color
   foreach red {00 80 ff} {
       foreach green {00 80 ff} {
           foreach blue {00 80 ff} {
                bind $f.blue${red}${green}${blue} <1> \
                   "%W configure -relief sunken"
                bind $f.blue${red}${green}${blue} <ButtonRelease-1> {
                    %W configure -relief raised
                     set color [%W cget -bg]
                     change_color $color
               }
		   }
       }
   }
   bind .wbtl.color.pattern <ButtonPress-1> "select_pattern %x %y"
   bind .wbtl.color.pattern <ButtonRelease-1> ".wbtl.color.close invoke"
}


#---------------------------------------------------------------
# change_pos: trace la posicon tomada del interfaz al formato
#       que entiende tcl
#---------------------------------------------------------------
proc change_pos {fig_pos} {
    switch $fig_pos {
        "NE" { set fig_pos ne }
        "N"  { set fig_pos n }
        "NW" { set fig_pos nw }
        "E"  { set fig_pos e }
        "C"  { set fig_pos "center" }
        "W"  { set fig_pos w }
        "SE" { set fig_pos se }
        "S"  { set fig_pos s }
        "SW" { set fig_pos sw }
    }
    return $fig_pos
}
 



#---------------------------------------------------------------
# desactiva_eventos: Desactiva las acciones del raton sobre 
# 		la pizarra
#---------------------------------------------------------------

proc desactiva_eventos { } {
   global active_pag page c 
   clear_mouse_events
   set c $page($active_pag,canvas)

   bind $page($active_pag,canvas) <ButtonPress-1> { }

   bind $page($active_pag,canvas) <Any-Motion> { }

   bind $page($active_pag,canvas) <ButtonPress-2> { }

   bind $page($active_pag,canvas) <Any-B2-Motion> { }

   bind $page($active_pag,canvas) <ButtonRelease-2> { }

   bind $page($active_pag,canvas) <ButtonPress-3> { }

   bind $page($active_pag,canvas) <ButtonRelease-3> { }

   bind $page($active_pag,canvas) <Shift-ButtonPress-3> { }

   bind $page($active_pag,canvas) <Shift-ButtonRelease-3> { }

   bind .wbtl <Delete> {}

   bind .wbtl.optionsBar.buttons.draw.b <ButtonRelease-1> {}

   if {[winfo exists .wbtl.color]} { desactive_color_window }
   if {[winfo exists .wbtl.width]} { desactive_width_window }
   if {[winfo exists .wbtl.letra]} { desactive_font_window }
}

proc desactive_color_window {} { 
    set f .wbtl.color.type_color
    foreach red {00 80 ff} {
       foreach green {00 80 ff} {
          foreach blue {00 80 ff} {
              bind $f.blue${red}${green}${blue} <1> {}
              bind $f.blue${red}${green}${blue} <ButtonRelease-1> {}
          }
       }
    }

    bind .wbtl.color.pattern <ButtonPress-1> {}
    bind .wbtl.color.pattern <ButtonRelease-1> {}
}

proc desactive_width_window {} {
    .wbtl.width.f3.ok configure -state disabled
}

proc desactive_font_window {} {
    .wbtl.letra.f3.ok configure -state disabled
}

#---------------------------------------------------------------
# change_interface: Desactiva/Activa algunas opciones del menu
#		dependiendo del estado, ser o no master
#---------------------------------------------------------------

proc change_general_interface {state} {
   global send_done uniqueMaster
   # enable/disable cut and paste menu formas, editar opciones
   
   if {$uniqueMaster} {
       .wbtl.barra.menu.fichero.m entryconfigure "Open..." -state $state

       # SPG
       #.barra.menu.fichero.m entryconfigure "New" -state $state
       #.optionsBar.file.new configure -state $state
       #.f1.note configure -state $state
       #.barra.menu.window configure -state $state
   }

   if {$send_done != 0} {
      # --------------------
      # No estoy enviando
      # --------------------
      # .wbtl.barra.menu.fichero.m entryconfigure "Open..." -state $state
      .wbtl.barra.menu.fichero.m entryconfigure "Import picture..." \
          -state $state
      .wbtl.barra.menu.fichero.m entryconfigure "Scan picture" -state $state
      .wbtl.optionsBar.file.open configure -state $state
      .wbtl.optionsBar.file.scanner configure -state $state
      # .wbtl.optionsBar.file.new configure -state $state
   }

   .wbtl.barra.menu.formas configure -state $state
   .wbtl.barra.menu.fichero.m entryconfigure "Close" -state $state
   .wbtl.barra.menu.editar configure -state $state
   if {!$uniqueMaster} {
       .wbtl.barra.floor.lock configure -state $state
   }

   .wbtl.optionsBar.buttons.edit.f.l1 configure -state $state
   .wbtl.optionsBar.buttons.edit.f.mb configure -state $state

   .wbtl.optionsBar.buttons.draw.b configure -state $state
}

proc change_text_interface {state} {
   if {[winfo exists .wbtl.optionsBar.options.text]} {
      .wbtl.optionsBar.options.text.f0 configure -state $state
      .wbtl.optionsBar.options.text.f1 configure -state $state
      .wbtl.optionsBar.options.text.f2 configure -state $state
      .wbtl.optionsBar.options.text.f3 configure -state $state
   }
}

proc change_filled_interface {state} {
   if {[winfo exists .wbtl.optionsBar.options.filled]} {
       .wbtl.optionsBar.options.filled configure -state $state
   }
}
proc change_width_interface {state} {
   if {[winfo exists .wbtl.optionsBar.options.width]} {
       .wbtl.optionsBar.options.width.mb configure -state $state
   }
}
proc change_arrow_interface {state} {
   if {[winfo exists .wbtl.optionsBar.options.arrow]} {
       .wbtl.optionsBar.options.arrow.mb configure -state $state
   }
}

proc change_interface {state} {
   global normalbg send_done
   change_general_interface $state
   change_text_interface $state
   change_filled_interface $state
   change_width_interface $state
   change_arrow_interface $state
}

#---------------------------------------------------------------
# set_ui_state: Actualiza el interfaz si eres o no master
#---------------------------------------------------------------

proc set_ui_state {} {
   global page active_pag latestAction uniqueMaster

   if {$page($active_pag,write)} {
      if {!$uniqueMaster} {
         .wbtl.barra.floor.floor select
      }
      set state normal
      activa_eventos
      eval $latestAction
   } else {
      if {!$uniqueMaster} {
          .wbtl.barra.floor.floor deselect
      }
      set state disabled
      desactiva_eventos
   }

   change_interface $state
}


#---------------------------------------------------------------
# clear_mouse_events: elemina todos los eventos asociados al
# 	raton, salvo mover con el boton 2 y seleccionar con el 3
#---------------------------------------------------------------
 
proc clear_mouse_events {} {
 
    global active_pag c page general myuserid
    set general(Pointer) 0

    set c $page($active_pag,canvas)

    catch {$page($active_pag,canvas) delete pointer_name_$myuserid}
    catch {$page($active_pag,canvas) delete pointer_image_$myuserid}
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
    delete_item $active_pag pointer_name_$myuserid
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
    delete_item $active_pag pointer_image_$myuserid

    $page($active_pag,canvas) configure -cursor left_ptr
  
    $page($active_pag,canvas) bind texto <KeyPress> { }

    bind $page($active_pag,canvas) <ButtonRelease-1> { }
    bind $page($active_pag,canvas) <ButtonPress-1> { }

    bind $page($active_pag,canvas) <Any-B1-Motion> { }
    bind $page($active_pag,canvas) <Any-Motion> { }
    bind $page($active_pag,canvas) <Any-Enter> { }
    bind $page($active_pag,canvas) <Any-Leave> { }

    bind $page($active_pag,canvas) <ButtonPress-3> {
        catch {destroy .wbtl.menuDraw}
        incr busy
        DebugTraceMsg "Wb:: clear_mouse_events: INCR clear_mouse_events" 9
    }

    bind $page($active_pag,canvas) <ButtonRelease-3> {
        select_button [$page($active_pag,canvas) canvasx %x] \
                      [$page($active_pag,canvas) canvasy %y]
        incr busy -1
        DebugTraceMsg "Wb:: clear_mouse_events: DEC clear_mouse" 9
        examine_peticion_queue
    }
}


proc pizarra_map { } {
    # show whiteboard
    global geometry general
    foreach window {.wbtl .wbtl.info .wbtl.align .wbtl.bitmaps \
       .wbtl.pattern .wbtl.color .wbtl.open .wbtl.close .wbtl.save .wbtl.sending .wbtl.colour .wbtl.width \
       .wbtl.examine .wbtl.long .wbtl.flecha .wbtl.letra .wbtl.menuDraw} {
            catch {wm deiconify $window}
    }

    # Pongo geometria anterior
    wm geometry .wbtl $geometry

    update
    set general(Mapped) 1
}

proc pizarra_unmap { } {
    # hide whiteboard
    global geometry general

    foreach window {.wbtl .wbtl.info .wbtl.align .wbtl.bitmaps .wbtl.pattern \
       .wbtl.color .wbtl.open .wbtl.close .wbtl.save .wbtl.sending .wbtl.colour .wbtl.examine \
       .wbtl.long .wbtl.flecha .wbtl.letra .wbtl.width .wbtl.menuDraw} {
            catch {wm withdraw $window}
    }

    set general(Mapped) 0
}





