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
# $Id: wb_draw.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#########################################################################

# ------------------------------------------------------------------------
# paint_polypoint_figure
#
#   crea poligonos y lineas con mas de 2 puntos
# ------------------------------------------------------------------------

proc paint_polypoint_figure {clase} {

    global page primer_punto active_pag pulsado 

    set posx 0
    set posy 0
    set primer_punto 0
    clear_mouse_events
    set pulsado 0

    bind $page($active_pag,canvas) <ButtonPress-1> \
        "press_button1_paint_polypoint_figure \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"

    bind $page($active_pag,canvas) <Any-B1-Motion> \
        "any_button1_paint_polypoint_figure \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"

    bind $page($active_pag,canvas) <Any-Motion> \
        "any_button1_paint_polypoint_figure \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"

    bind $page($active_pag,canvas) <ButtonRelease-3> \
        "release_button3_paint_polypoint_figure $clase \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"
}

proc change_interface_paint_polypoint_figure {state} {
    change_general_interface $state
    .wbtl.f1.note configure -state $state
    .wbtl.barra.menu.fichero configure -state $state
    .wbtl.barra.menu.texto configure -state $state
    .wbtl.barra.menu.config configure -state $state
    .wbtl.barra.menu.window configure -state $state
    .wbtl.optionsBar.file.new configure -state $state
    .wbtl.optionsBar.file.save  configure -state $state
    .wbtl.barra.pointer configure -state $state
}

proc press_button1_paint_polypoint_figure {posix posiy} {
    global primer_punto pulsado busy active_pag page posx posy

    set posx $posix
    set posy $posiy

    incr pulsado

    # ------------------------------------------
    # No dejo que el usuario cambie la pagina,
    # hasta que termine de pintar la figura
    # ------------------------------------------
    change_interface_paint_polypoint_figure disabled

    if {!$busy} { incr busy }

    set page($active_pag,id) -1
    set primer_punto 1
    bind $page($active_pag,canvas) <Any-B2-Motion> { }
    bind $page($active_pag,canvas) <ButtonPress-2> { }
    bind $page($active_pag,canvas) <ButtonRelease-2> { }
}

proc any_button1_paint_polypoint_figure {posix posiy} {
    global posx posy primer_punto active_pag page general

    if {!$primer_punto} { return }
    $page($active_pag,canvas) delete $page($active_pag,id)
    set page($active_pag,id) \
       [$page($active_pag,canvas) create line $posx $posy $posix \
        $posiy -tag selabierta -width $general(Width) \
        -fill $general(Color)]
}


proc release_button3_paint_polypoint_figure {clase posix posiy} {
    global primer_punto pulsado busy page general active_pag

    if {$pulsado == 1} {
        incr busy -1
        DebugTraceMsg "Wb:: paint_polypoint_figure: DEC  paint_items" 9
        return

    } elseif { $pulsado <2 } {
        select_button $posix $posiy
        incr busy -1
        DebugTraceMsg "Wb:: paint_polypoint_figure: DEC paint_items" 9
        examine_peticion_queue
        return
    }

    # -------------------------------
    # assert($pulsado >= 2)
    # -------------------------------

    set list [$page($active_pag,canvas) find withtag selabierta]
    set coord [$page($active_pag,canvas) coords [lindex $list 0]]
    set list [lreplace $list 0 0 ]

    foreach elem $list {
        set coords [$page($active_pag,canvas) coord $elem]
        set coord [concat $coord [list_range $coords 2]]
        set coord [concat $coord [list_range $coords 3]]
    }

    set coord_inicio [lrange $coord 0 1]
    set ultimo [expr [ llength $coord] -1]

    while {$coord_inicio == [lrange $coord [expr $ultimo -1] $ultimo]} {
        set coord [lreplace $coord [expr $ultimo -1] $ultimo]
        set ultimo [expr [ llength $coord] -1]
    }

    $page($active_pag,canvas) delete selabierta

    set fg $page($active_pag,FigNumber)
    paint_polypoint_figure_cmd $fg \
        $active_pag $clase $coord \
        \"$general(FillColor)\" \"$general(Color)\" \
        $general(Stipple) \
        $general(Width) $general(Arrow) $general(ArrowShape) \
        $general(Font) $general(Point) \
        $general(Weight) $general(Anchor) \
        $general(Justif) \"\"
                 
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        paint_polypoint_figure_cmd $fg \
        $active_pag $clase $coord \
        \"$general(FillColor)\" \"$general(Color)\" \
        $general(Stipple) \
        $general(Width) $general(Arrow) $general(ArrowShape) \
        $general(Font) $general(Point) \
        $general(Weight) $general(Anchor) \
        $general(Justif) \"\"
           
    set primer_punto 0

    # porque al presionar 3 tambien se incrementa
    # del clear_mouse_events
    incr busy -2

    # Activo el cambio de pagina
    change_interface_paint_polypoint_figure normal

    examine_peticion_queue
    set pulsado 0
    activa_boton_2
}



proc paint_polypoint_figure_cmd {fg pag clase_fig coord color outline stipple
width arrow arrowshape font point weight anchor justif bitmap} {
    global page
    lappend page($pag,FigList) fig$fg
    
    create_figure ${pag}fig$fg $pag $fg \
        $clase_fig $coord $color $outline $stipple \
        $width $arrow $arrowshape $font $point \
        $weight $anchor $justif $bitmap

    save_undo_paint_items $pag fig$fg
    incr page($pag,FigNumber)
}



# ------------------------------------------------------------------------
# paint_figure
#
#   crea rectangulos, elipses, arcos, lineas...
# ------------------------------------------------------------------------

proc paint_figure {clase} {

    global page active_pag 

    set posx 0
    set posy 0
    clear_mouse_events

    bind $page($active_pag,canvas) <ButtonPress-1> \
        "press_button1_paint_figure $clase \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"

    bind $page($active_pag,canvas) <Any-B1-Motion> \
        "any_motion_paint_figure $clase \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"

    bind $page($active_pag,canvas) <ButtonRelease-1> \
        "release_button1_paint_figure $clase \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"
}


proc press_button1_paint_figure {clase posix posiy} {
    global busy page active_pag general posx posy

    incr busy
    set posx $posix
    set posy $posiy
    set coords [concat $posix $posiy $posx $posy]
    set page($active_pag,id) -1

    set fg fig$page($active_pag,FigNumber)
    lappend page($active_pag,FigList) $fg
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
         lappend page($active_pag,FigList) $fg

    create_figure \
        $active_pag$fg \
        $active_pag $page($active_pag,FigNumber) $clase \
        $coords \"$general(FillColor)\" \
        \"$general(Color)\" $general(Stipple) $general(Width) \
        $general(Arrow) $general(ArrowShape) $general(Font) \
        $general(Point) $general(Weight) $general(Anchor) \
        $general(Justif) ""
}

proc any_motion_paint_figure {clase posix posiy} {
    global page active_pag posx posy general

    set coords [concat $posx $posy $posix $posiy]
    set fg fig$page($active_pag,FigNumber)

    set info_object [find objects ::$active_pag$fg]
    if {$info_object == "::$active_pag$fg"} {
       $active_pag$fg change_coords $coords
    }
}

proc release_button1_paint_figure {clase posix posiy} {
    global page active_pag general posx posy busy

    set coords [concat $posx $posy $posix $posiy]
    set fg fig$page($active_pag,FigNumber)

    set info_object [find objects ::$active_pag$fg]
#    while {$info_object  != "$active_pag$fg"} {
#        set info_object [find objects ::$active_pag$fg]
#    }

    $active_pag$fg change_coords $coords

    ISA_RDOgroup Cmd only_if_enabled_page $active_pag create_figure \
        $active_pag$fg $active_pag $page($active_pag,FigNumber) \
        $clase $coords \"$general(FillColor)\" \
        \"$general(Color)\" $general(Stipple) $general(Width) \
        $general(Arrow) $general(ArrowShape) $general(Font) \
        $general(Point) $general(Weight) $general(Anchor) \
        $general(Justif) ""
        
    paint_figure_cmd $active_pag $fg
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
         paint_figure_cmd $active_pag $fg
             
    incr busy -1
    DebugTraceMsg "Wb:: paint_figure: DEC paint_items" 9
    examine_peticion_queue
}


proc paint_figure_cmd {pag fg} {
    global page
    save_undo_paint_items $pag $fg
    incr page($pag,FigNumber)
}




# ------------------------------------------------------------------------
# pen
#
#   crea una figura dibujada con un lapiz
# ------------------------------------------------------------------------

proc pen { } {
   global active_pag page
   clear_mouse_events
   
   bind $page($active_pag,canvas) <ButtonPress-1> {
      incr busy
      DebugTraceMsg "Wb:: pen: INCR pen" 9
      set posx [$page($active_pag,canvas) canvasx %x] 
      set posy [$page($active_pag,canvas) canvasy %y] 
      set coord [concat $posx $posy]
   }

   bind $page($active_pag,canvas) <Any-B1-Motion> {
      $page($active_pag,canvas) delete prueba
      set posix [$page($active_pag,canvas) canvasx %x]
      set posiy [$page($active_pag,canvas) canvasy %y]
      set coord [concat $coord $posix $posiy]
      eval $page($active_pag,canvas) create line \
          $coord -fill $general(Color) -tags prueba
   }

   bind $page($active_pag,canvas) <ButtonRelease-1> {
      $page($active_pag,canvas) delete prueba 
      set posix [$page($active_pag,canvas) canvasx %x]
      set posiy [$page($active_pag,canvas) canvasy %y]
      set coord [concat $coord $posix $posiy]

      set fg $page($active_pag,FigNumber)
      
      paint_polypoint_figure_cmd $fg \
           $active_pag line $coord \
           \"$general(FillColor)\" \"$general(Color)\" \
           $general(Stipple) \
           $general(Width) $general(Arrow) $general(ArrowShape) \
           $general(Font) $general(Point) \
           $general(Weight) $general(Anchor) \
           $general(Justif) \"\"
      
      ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
           paint_polypoint_figure_cmd $fg \
           $active_pag line $coord \
           \"$general(FillColor)\" \"$general(Color)\" \
           $general(Stipple) \
           $general(Width) $general(Arrow) $general(ArrowShape) \
           $general(Font) $general(Point) \
           $general(Weight) $general(Anchor) \
           $general(Justif) \"\"
      
      incr busy -1
      DebugTraceMsg "Wb:: pen: DEC pen" 9
      examine_peticion_queue
   }

}




	
# ------------------------------------------------------------------------
# item_text
#
#   crea un elemento de texto
# ------------------------------------------------------------------------

proc item_text {} {

   global active_pag page

   clear_mouse_events 

   bind $page($active_pag,canvas) <ButtonPress-1> {
      incr busy
      DebugTraceMsg "Wb:: item_text: INCR text" 9
   }

   bind $page($active_pag,canvas) <ButtonRelease-1> \
        "release_button1_item_text \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"

   $page($active_pag,canvas) bind texto <KeyPress> \
        "key_press_item_text %A"
}

proc release_button1_item_text {posx posy} {
    global page active_pag general busy

    set lista [$page($active_pag,canvas) find overlapping \
        $posx $posy $posx $posy]
    set type [$page($active_pag,canvas) type [lindex $lista 0]]

    if {$type == "text"} {
       insert_text_cmd $active_pag $posx $posy
       ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
           insert_text_cmd $active_pag $posx $posy

    } else {
        set fg $page($active_pag,FigNumber)
        create_text_cmd $fg $active_pag [list $posx $posy] \
          \"$general(Color)\" $general(Font) $general(Point) \
          $general(Weight) $general(Anchor) $general(Justif)
            
        ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
          create_text_cmd $fg $active_pag [list $posx $posy] \
          \"$general(Color)\" $general(Font) $general(Point) \
          $general(Weight) $general(Anchor) $general(Justif)     
    }

    incr busy -1
    examine_peticion_queue
}

proc key_press_item_text {letra} {
    global busy active_pag id_text
    incr busy
    DebugTraceMsg "Wb:: item_text: INCR text" 9

    insert_char $active_pag $id_text($active_pag) $letra
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag insert_char \
           $active_pag $id_text($active_pag) $letra

    incr busy -1
    DebugTraceMsg "Wb:: item_text: DEC text" 9
    examine_peticion_queue
}


proc insert_text_cmd {pag posx posy} {
   global id_text page

   set canvas $page($pag,canvas)

   set elem [$canvas find overlapping $posx $posy $posx $posy]
   set elem [lindex $elem 0]

   set id_text($pag) [get_fig $canvas $elem]

   $canvas icursor $id_text($pag) @$posx,$posy
   $canvas focus $id_text($pag) 
   focus $canvas
   
    undo_insert_text $pag insert
}

proc create_text_cmd {fg pag coord color font point weight anchor justif} {
   global page id_text
   lappend page($pag,FigList) fig$fg

   Text ${pag}fig$fg $pag $page($pag,FigNumber) $coord $color \
        $font $point $weight $anchor $justif

   set id_text($pag) fig$fg
   undo_insert_text $pag end
   incr page($pag,FigNumber)
}



proc insert_char {pag elemento char} {
 
   global page
   set canvas $page($pag,canvas)

   if {[string compare $char \r] == 0} {
      $canvas insert $elemento insert \n

   } elseif {([string compare $char \b] == 0) || \
     ([string compare $char \x7f] == 0)} {
      set ind [$canvas index $elemento insert]
      $canvas dchars $elemento [expr $ind -1] [expr $ind -1]

   } else {
      $canvas insert $elemento insert $char
   }

}
 




# ------------------------------------------------------------------------
# bitmap
#
#   crea un menu para la seleccion de bitmaps
# ------------------------------------------------------------------------

proc bitmap { } {
   global page dir_iconos active_bg busy general 

   incr busy

   toplevel .wbtl.bitmaps
   update
   after idle grab .wbtl.bitmaps

   frame .wbtl.bitmaps.f
   pack .wbtl.bitmaps.f

   label .wbtl.bitmaps.f.l1 -text "Select a bitmap: "
   pack .wbtl.bitmaps.f.l1


   # #### BITMAP SIGNAL ####

   button .wbtl.bitmaps.f.b1 -bitmap @${dir_iconos}sen.xbm -command {
      bitmap_command sen 
      incr busy -1
      examine_peticion_queue
   }


   # #### BITMAP QUESTION ####

   button .wbtl.bitmaps.f.b2 -bitmap @${dir_iconos}preg.xbm -command {
      bitmap_command preg 
      incr busy -1
      examine_peticion_queue
   }



   # #### BITMAP ADMIRATION ####

   button .wbtl.bitmaps.f.b3 -bitmap @${dir_iconos}adm.xbm -command {
      bitmap_command adm
      incr busy -1
      examine_peticion_queue
   }


   # #### BITMAP PROHIBITED ####

   button .wbtl.bitmaps.f.b4 -bitmap @${dir_iconos}pro.xbm -command {
      bitmap_command pro 
      incr busy -1
      examine_peticion_queue
   }


   # #### BITMAP TRIANGLE ####

   button .wbtl.bitmaps.f.b5 -bitmap @${dir_iconos}tri.xbm -command {
      bitmap_command tri 
      incr busy -1
      examine_peticion_queue
   }


   # #### BITMAP STOP ####

   button .wbtl.bitmaps.f.b6 -bitmap @${dir_iconos}stop.xbm -command {
      bitmap_command stop
      incr busy -1
      examine_peticion_queue
   }

   pack .wbtl.bitmaps.f.b1 .wbtl.bitmaps.f.b2 .wbtl.bitmaps.f.b3 .wbtl.bitmaps.f.b4 .wbtl.bitmaps.f.b5 \
      .wbtl.bitmaps.f.b6 -side left
   tkwait window .wbtl.bitmaps
}




# ------------------------------------------------------------------------
# bitmap_command
#
#   crea un bitmap al pulsar con el raton
# ------------------------------------------------------------------------

proc bitmap_command {fich} {
   global page active_pag general dir_iconos latestAction

   catch {destroy .wbtl.menuDraw}
   catch {destroy .wbtl.optionsBar.options}
   catch {destroy .wbtl.bitmaps}
   activa_eventos

   set latestAction "bitmap_command $fich"
   .wbtl.optionsBar.buttons.draw.b configure \
       -image "" -bitmap @${dir_iconos}${fich}_icono1.xbm

   set general(LeftButton) "Click bitmap to add bitmap to image."
   set general(Bitmap) ${fich}.xbm

   clear_mouse_events

   bind $page($active_pag,canvas) <ButtonPress-1> { 
      incr busy
   }

   bind $page($active_pag,canvas) <ButtonRelease-1> {
      set posx [$page($active_pag,canvas) canvasx %x]
      set posy [$page($active_pag,canvas) canvasy %y]
      set coord [list $posx $posy]

      set fg $page($active_pag,FigNumber)
      
      paint_polypoint_figure_cmd $fg \
           $active_pag bitmap $coord \
           \"$general(Color)\" \"$general(FillColor)\" \
           $general(Stipple) \
           $general(Width) $general(Arrow) $general(ArrowShape) \
           $general(Font) $general(Point) \
           $general(Weight) $general(Anchor) \
           $general(Justif) $general(Bitmap) 
      
      ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
           paint_polypoint_figure_cmd $fg \
           $active_pag bitmap $coord \
           \"$general(Color)\" \"$general(FillColor)\" \
           $general(Stipple) \
           $general(Width) $general(Arrow) $general(ArrowShape) \
           $general(Font) $general(Point) \
           $general(Weight) $general(Anchor) \
           $general(Justif) $general(Bitmap) 

      incr busy -1
      examine_peticion_queue
   }
}


 
# --------------------------------------------------------------------------
# create_figure
#
#   crea el objeto figura dependiendo del tipo de la misma
# --------------------------------------------------------------------------
 
 
proc create_figure {el text fig clase coords color outline stipple \
 width arrow arrowshape font point weight anchor justify file args} {
 
   global general page dir_iconos

   set canvas $page($text,canvas)

   switch $clase {
 
        "rectangle" { Rectangle $el $text $fig $coords \
                         $color $outline $stipple $width}
 
        "arc" { Arc $el $text $fig $coords $color \
                         $outline $stipple $width} 
        "oval" -
        "circunf_diam" -
        "circunf_radio" { Oval $el $text $fig $clase $coords $color \
                         $outline $stipple $width} 
 
        "linea_abierta" -
        "curva_abierta" -
        "line" { Line $el $text $fig $clase $coords \
                         $outline $width $arrow $arrowshape}
 
        "polygon" -
        "linea_cerrada" -
        "curva_cerrada" -
        "arrow" { Polygon $el $text $fig $clase $coords $color \
                         $outline $stipple $width} 
 
        "image" { Image $el $text $fig $coords $file}
 
        "bitmap" { Bitmap $el $text $fig $coords $color $file}
        
        "text"   {Text $el $text $fig $coords $color $font $point $weight \
                       $anchor $justify}
       
        "group"  {Group $el $text $fig group}
    }
}
 
 
# ------------------------------------------------------------------------
# pointer
#
#   crea un puntero para se~alar
# ------------------------------------------------------------------------

proc pointer { } {
 
   global page general active_pag 
   set general(Pointer) 1
   bind $page($active_pag,canvas) <Any-Motion> \
        "any_motion_pointer \
            \[$page($active_pag,canvas) canvasx %x\] \
            \[$page($active_pag,canvas) canvasy %y\]"

   bind $page($active_pag,canvas) <Any-Leave> {
      delete_pointer $active_pag $myuserid
      ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
          delete_pointer $active_pag $myuserid
   }

   bind $page($active_pag,canvas) <Any-Enter> {
      set posx [$page($active_pag,canvas) canvasx %x]
      set posy [$page($active_pag,canvas) canvasy %y]
 
      create_pointer $active_pag $mysite_id $myuserid [list $posx $posy]
      ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
         create_pointer $active_pag  $mysite_id $myuserid \
         [list $posx $posy]
   }
}


proc any_motion_pointer {posx posy} {
    global active_pag page myuserid

    set coords [$page($active_pag,canvas) coords pointer_name_$myuserid]
    set difx [expr $posx - [lindex $coords 0]]
    set dify [expr $posy - [lindex $coords 1]]

    move_pointer $active_pag $myuserid $difx $dify
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
          move_pointer $active_pag $myuserid $difx $dify
}

proc delete_pointer {pag userid} {
    global page myuserid
    $page($pag,canvas) delete pointer_name_$userid
    if {$userid == $myuserid} {
        $page($pag,canvas) configure -cursor ""
    } else {
        $page($pag,canvas) delete pointer_image_$userid
    }
}

proc move_pointer {pag userid difx dify} {
   global page myuserid
   $page($pag,canvas) move pointer_name_$userid $difx [expr $dify+25]
   if {$userid != $myuserid} {
       $page($pag,canvas) move pointer_image_$userid $difx [expr $dify+25]
   }
}


proc create_pointer {pag name userid coords} {
    global page myuserid
    set posx [lindex $coords 0]
    set posy [lindex $coords 1]
    $page($pag,canvas) create text $posx [expr $posy+25] \
       -text $name -tag pointer_name_$userid
    if {$userid == $myuserid} {
        $page($pag,canvas) configure -cursor hand1
    } else {
        $page($pag,canvas) create image $posx $posy \
           -image puntero_id -tag pointer_image_$userid -anchor ne
    }
}
 




 
