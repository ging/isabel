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
# $Id: wb_undo.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Almacena las operaciones necesarias para restablecer el 
#   estado anterior a una modificacion en una pagina.
#
#########################################################################
 



# ----------------------------------------------------------------------
# PROC: undo
# ----------------------------------------------------------------------

proc undo { } {

   global active_pag latestAction
   clear_mouse_events

   exec_undo $active_pag
   ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
	   exec_undo $active_pag

   eval $latestAction
}



proc exec_undo {text } {
   global operation_undo page

   if {!$page($text,enable)} { return }

   for {set i 0} {$i < [llength \
      $operation_undo($text,$page($text,ind_undo))]} {incr i} {
       eval [list_range $operation_undo($text,$page($text,ind_undo)) $i]
   }

   unset operation_undo($text,$page($text,ind_undo))
   incr page($text,ind_undo) -1

   if {$page($text,ind_undo) < 0} {disable_undo}
}
	


proc enable_undo { } {
   .wbtl.barra.menu.editar.m entryconfigure "Undo" -state normal
}



proc disable_undo { } {
   .wbtl.barra.menu.editar.m entryconfigure "Undo" -state disabled
}






# ----------------------------------------------------------------------
# PROC: Undo change colour
# ----------------------------------------------------------------------

proc save_undo_pattern {pag list} {

   global operation_undo page

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)
   incr page($pag,ind_undo)

   foreach el $list {
      set fill [$canvas itemcget $el -fill]
      set pattern [$canvas itemcget $el -stipple]
      lappend l "$canvas itemconfigure $el -fill \"$fill\" \
         -stipple \"$pattern\" "
   }

   set operation_undo($pag,$page($pag,ind_undo)) $l
   enable_undo
}
		



proc save_change_anchor {pag list sel} {

   global page operation_undo 

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)
   set l ""
   foreach el $sel {
       lappend l "$pag$el delete_selection"
   }

   foreach el $list {
     lappend l "$canvas itemconfigure $el -anchor \
        [$canvas itemcget $el -anchor]"
   }

   foreach el $sel {lappend l "$pag$el select"}

   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $l
   enable_undo
}


proc save_change_font {pag list sel} {
   global page operation_undo 

   if {!$page($pag,enable)} {return}

   set canvas $page($pag,canvas)
   set l ""
   foreach el $sel {
      lappend l "$pag$el delete_selection"
   }

   foreach el $list {
      set old_font [$canvas itemcget $el -font]
      lappend l "$canvas itemconfigure $el -font $old_font" 
   }

   foreach el $sel {lappend l "$pag$el select"}

   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $l
   enable_undo
}

proc save_change_justify {pag list sel} {
   global page operation_undo

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)
   set l ""
   foreach el $sel { 
       lappend l "$pag$el delete_selection" 
   }

   foreach el $list {
      set old_justify [$canvas itemcget $el -justify]
      lappend l "$canvas itemconfigure $el -justify $old_justify"
   }

   foreach el $sel { lappend l "$pag$el select" }

   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $l
   enable_undo
}

proc save_undo_fill {pag l} {
   global page operation_undo 

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)
   set list ""
   foreach el $l {
      set tipo [$pag$el cget -type]

      if {$tipo =="bitmap"} { set tag "-foreground"
      } elseif {($tipo=="line") || ($tipo == "text")} { set tag "-fill" 
      } else { set tag "-outline" }	

      set old_colour [$canvas itemcget $el $tag]
      lappend list "$canvas itemconfigure $el $tag $old_colour" 
   }
   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}


proc save_undo_width {canvas pag l} {
   global page operation_undo 
 
   if {!$page($pag,enable)} { return }

   set list ""
   foreach el $l {
      set tipo [$pag$el cget -type]
      set old_width [$canvas itemcget $el -width]
      lappend list "$canvas itemconfigure $el -width $old_width"
   }
   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}


proc save_undo_arrow {canvas pag l} {
   global page operation_undo
 
   if {!$page($pag,enable)} { return }

   set list ""
   foreach el $l {
      set tipo [$pag$el cget -type]
      set old_arrow_mode [$canvas itemcget $el -arrow]
      lappend list \
         "$canvas itemconfigure $el -arrow $old_arrow_mode"
   }
   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}



proc save_undo_fillColor {pag l} {

   global page operation_undo 

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)
   set list ""
   foreach el $l {
       set type [$pag$el cget -type]
       if {($type == "rectangle") || ($type == "oval") || \
             ($type == "arc") || ($type == "polygon")} {
               set prop [$canvas itemcget $el -fill]
               if {$prop != ""} {
                  set old_colour [$canvas itemcget $el -fill]
                  lappend list \
                     "$canvas itemconfigure $el -fill \"$old_colour\"" 
               }
       }

   }

   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}





# ----------------------------------------------------------------------
# PROC: Undo Copy Items 
# ----------------------------------------------------------------------

proc save_undo_copy {pag lista_sel lista_borrar} {
   # Hay un grupo de elementos seleccionados
   global page operation_undo general

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)

   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) ""

   foreach el $lista_borrar {
      lappend operation_undo($pag,$page($pag,ind_undo)) \
         "$pag$el delete"
   }

   foreach el $lista_sel {
      lappend operation_undo($pag,$page($pag,ind_undo)) \
        "$pag$el select"
   }
}




# ----------------------------------------------------------------------
# PROC: Undo Group Items 
# ----------------------------------------------------------------------

proc save_undo_group {pag list} {
 
   global operation_undo page general

   if {!$page($pag,enable)} {return}

   set canvas $page($pag,canvas)
   incr page($pag,ind_undo)
   set el [lindex $page($pag,FigList) 0]

   while { [get_below $pag $el] != ""} {
      set el [get_below $pag $el]
   }
   # $el es el primer elemento que hay en la pagina
   set el [get_fig $canvas $el]

   set l ""
   while {[$canvas find above $el] != ""} {
       set elem [$canvas find above $el]
       set elem [get_fig $canvas $elem]
       lappend l "$canvas raise $elem $el"
       set el $elem
   }
		
   lappend l "exec_undo_group $pag [list $list]"
   set operation_undo($pag,$page($pag,ind_undo)) $l 
   enable_undo
}
 



proc exec_undo_group {pag lista} {
   global page
   set group $page($pag,sel)
   
   $pag$group delete_selection 
   set ind [lsearch -glob $page($pag,FigList) $group]
   set page($pag,FigList) [lreplace $page($pag,FigList) $ind $ind]

   $pag$group configure -exist 0
   foreach el [$pag$group cget -figlist] {
		$pag$el configure -father ""
		lappend page($pag,FigList) $el
   }			

   $pag$group configure -figlist ""
   exec_undo_multiselect $pag $lista
}


# ----------------------------------------------------------------------
# PROC: Undo above level 
# ----------------------------------------------------------------------

proc save_undo_above {pag list} {

   global operation_undo page general

   if {!$page($pag,enable)}  return

   set canvas $page($pag,canvas)
   incr page($pag,ind_undo)
   set el [lindex $page($pag,FigList) 0]

   while { [get_below $pag $el] != ""} {
      set el [get_below $pag $el]
   }

   set l ""
   while {[$canvas find above $el] != ""} {
       set elem [$canvas find above $el]
       set elem [get_fig $canvas $elem]
       lappend l "$canvas raise $elem $el"
       set el $elem
   }

   lappend l "exec_undo_multiselect $pag [list $list]"
   set operation_undo($pag,$page($pag,ind_undo)) $l
   enable_undo
}



# ----------------------------------------------------------------------
# PROC: Undo NO Group Items 
# ----------------------------------------------------------------------

proc save_undo_no_group {pag lista } {

   global operation_undo page general

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)

   incr page($pag,ind_undo)
 
   set list ""
   foreach elem $lista {
      lappend list "exec_undo_no_group $pag $elem"
      lappend list "$pag$elem select"
   }
   set operation_undo($pag,$page($pag,ind_undo)) $list 
   enable_undo
}




proc exec_undo_no_group {pag elem} {
   global page

   set canvas $page($pag,canvas)
   $pag$elem configure -exist 1

   foreach el [$pag$elem cget -figlist] {
      $pag$el configure -father $elem
      set ind [lsearch -glob $page($pag,FigList) $el]
      set page($pag,FigList) [lreplace $page($pag,FigList) $ind $ind]
   }

   lappend page($pag,FigList) $elem
}



# ----------------------------------------------------------------------
# PROC: Undo Multiselect Items 
# ----------------------------------------------------------------------

 
proc save_undo_multiselect {pag} {
 
   global operation_undo page

   if {!$page($pag,enable)} { return }
   set canvas $page($pag,canvas)
   incr page($pag,ind_undo)

   if {$page($pag,sel) != ""} {
       set operation_undo($pag,$page($pag,ind_undo)) [list \
          "exec_undo_multiselect $pag [list $page($pag,sel)]"]
   } else {
       set operation_undo($pag,$page($pag,ind_undo)) [list \
          "undo_multiselect $pag"]
   }
   enable_undo
}
 

proc exec_undo_multiselect {pag lista_elem} {
    global page
    undo_multiselect $pag
    foreach el $lista_elem { 
        $pag$el select 
    }
}

proc undo_multiselect {pag} {
   global page
   delete_selection_cmd $pag
}




# ----------------------------------------------------------------------
# PROC: Undo Align Items 
# ----------------------------------------------------------------------

# Borrar ya no se utiliza

proc undo_align {pag elemento} {
   global page operation_undo
   if {!$page($pag,enable)} { return }

   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) [list \
      "$page($pag,canvas) coords $elemento \
       [$page($pag,canvas) coords $elemento]"]
   enable_undo
}




# ----------------------------------------------------------------------
# PROC: Undo Change Information Items 
# ----------------------------------------------------------------------

proc save_undo_info {pag elemento } {
   global operation_undo page

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)

   set coords [$canvas coords $elemento]
   set opciones [$canvas itemconfigure $elemento]
   set arg ""
   set arg [get_tags $opciones $arg]
   set tipo [$canvas type $elemento]
   incr page($pag,ind_undo)

   if {($tipo != "bitmap") && ($tipo != "image")} {

     if {[$canvas itemcget $elemento -stipple] == ""} {
        set arg [concat $arg -stipple \"\"]
     } 
     if {[$canvas itemcget $elemento -fill] == ""} {
        set arg [concat $arg -fill \"\"]
     }
   }

   set operation_undo($pag,$page($pag,ind_undo)) [list \
       "undo_info $pag $elemento [list $coords] [list $arg]"]
   enable_undo
}



proc undo_info {pag elemento coords arg} {
   global page
   set canvas $page($pag,canvas)
   set larg [ expr [string length $arg] -1]
   set primer [string range $arg 0 0]
   set ultimo [string range $arg $larg $larg]
   if { ($primer == "{") && ($ultimo == "}")} {
      set arg [string range $arg 1 [expr $larg -1]]
   }
   eval $canvas coords $elemento $coords
   eval $canvas itemconfigure $elemento $arg
   if {[lsearch -exact [$canvas gettags $elemento] resaltado] != -1} {
       # el elemento estaba resaltado, borrarlo y volver a seleccionar
       # por si han cambiado sus coordenadas
       $pag$elemento delete_selection 
       $pag$elemento select 
   }
}



# ----------------------------------------------------------------------
# PROC: Undo Insert Text Items 
# ----------------------------------------------------------------------

proc undo_insert_text {pag fin} {
   global operation_undo page id_text 

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)

   set ind [$canvas index $id_text($pag) insert]
   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) [list \
      "$canvas dchars $id_text($pag) $ind $fin"]

   enable_undo
}




# ----------------------------------------------------------------------
# PROC: Undo Paint  Items 
# ----------------------------------------------------------------------

proc save_undo_paint_items {pag item} {
   global page operation_undo

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)

   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) [list \
      "$pag$item delete"]
   enable_undo
}





# ----------------------------------------------------------------------
# PROC: Undo Delete  Items 
# ----------------------------------------------------------------------

proc save_undo_clear {pag lista} {
   global page operation_undo

   if {!$page($pag,enable)} { return }

   set list [exec_save_undo_clear $pag $lista]
   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}


proc exec_save_undo_clear {pag list} {

   global page
   set grupo ""
   set canvas $page($pag,canvas)

   foreach el $list {
      if {[$pag$el cget -selec] == 1} { lappend grupo $el }
   }

   set list_el [get_all_fig_from_group $pag $list]

   foreach elem $list_el {

      if {[$canvas find below $elem] == ""} {
         set first [get_fig $canvas $elem]
      }
   }

   if {$first == ""} { return }

   set list [list $first]
   set above $first

   while {[$canvas find above $above] != ""} {
      set above [$canvas find above $above]
      if {[lsearch -glob [$canvas gettags $above] sel*] == -1} {
         lappend list [get_fig $canvas $above]
      }
   }

   set list [exec_save_undo_delete $pag $list]
   foreach el $grupo { lappend list "$pag$el select" }

   lappend list "set page($pag,FigList) [list $page($pag,FigList)]"
   return $list
}



proc save_undo_delete {pag lista_elem} {
   global page operation_undo list1

   if {!$page($pag,enable)} { return }

   set list [exec_save_undo_delete $pag $lista_elem]
   eval lappend list $list1
   incr page($pag,ind_undo)
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}



proc exec_save_undo_delete {pag lista_elem} {
   global page list1
   set list ""
   set list1 ""
   set canvas $page($pag,canvas)

   foreach elem $lista_elem {
      set type [$pag$elem cget -type]

      if {$type != "group"} { 
          set opciones [$canvas itemconfigure $elem]
          set arg [get_tags $opciones ""]
          set coords [$canvas coords $elem]
          set above [$canvas find above $elem]

          while {($above != "") && \
              ([lsearch -glob [$canvas gettags $above] sel*] != -1)} {
		set above [$canvas find above $above]
          }

          $pag$elem configure -undo [list \
              "$pag$elem configure -cobj \[ $canvas create $type \
              [$canvas coords $elem ] -tags $elem \] " \
              "$canvas itemconfigure $elem $arg"]
          if {$above != ""} {
              set above [get_fig $canvas $above]
              lappend list1 "$canvas lower $elem $above"
          }
          lappend list "redo $canvas $pag $elem $type \
              [list $coords] [list $arg]" 

      } else {
          eval lappend list [exec_save_undo_delete $pag \
              [$pag$elem cget -figlist]]
      }

      if {[$pag$elem cget -father] != ""} {
         lappend list "$pag$elem configure -father [$pag$elem cget -father]"
      }

      if {[$pag$elem cget -selec]} { lappend list "$pag$elem select" }

   }
   return $list
}


proc redo {canvas pag fig tipo coords opciones} {

   global page
 
   if {!$page($pag,enable)} { return }

   # set existe [info object $pag$fig]
   set existe [itcl::find object $pag$fig]
   set el $pag$fig

   # -----------------
   # Existe el objeto
   # -----------------
   if {$existe == $el} {
       set undo [$pag$fig cget -undo]
       for {set i 0} {$i < [llength $undo]} {incr i} {
          set cmd [list_range $undo $i ]
          eval $cmd
       }
 
       if {[$pag$fig cget -father] == ""} {
          lappend page($pag,FigList) $fig
          $pag$fig configure -exist 1
 
       } else {
 
          while {[$pag$fig cget -father] != ""} {
             set fig [$pag$fig cget -father]
          }
 
          set ind [lsearch -glob $page($pag,FigList) $fig]
   
          if {$ind == -1} {
             lappend page($pag,FigList) $fig
             $pag$fig configure -exist 1
          }
 
       }

       return
   } 

   # --------------------
   # El objeto no existe
   # --------------------
   if {$tipo == "image"} {
 
       catch {destroy .wbtl.undo_image}
       tk_dialog .wbtl.undo_image "You must syncronization" \
           "Please 'Read master'" \
            {} 0 OK
       set page($pag,enable) 0
 
   } elseif {$tipo == "text"} {
       set ind [lsearch -exact $opciones "-text"]
 
       if {$ind != -1} {
           incr ind
           set texto [lindex $opciones $ind]
           trace_busy "redo:: EL TEXTO es $texto"
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
                } else {
                  set t [append t $char]
                }
            }
 
            set t [list_range $t 0]
            set long [expr [string length $t] -2]
            set t [string range $t 1 $long]
            set opciones [lreplace $opciones $ind $ind $t]
        }
 
        create_figure $pag$fig $pag \
            [string range $fig 3 end] \
            $tipo $coords "black" \"\" \
            \"\" 1 \"\" \"\" "courier" "12" "bold" "w" "center" \"\"
 
        eval $canvas itemconfigure $fig $opciones

     } elseif {$tipo =="bitmap"} {
 
        set ind [lsearch -glob $opciones -bitmap]
        incr ind
        set bitmap [lindex $opciones $ind]
        set bitmap [file tail $bitmap]
 
        create_figure $pag$fig $pag \
            [string range $fig 3 end] \
            $tipo $coords black \"\" \
            \"\" 1 \"\" \"\" \"\" \"\" \"\" \"\" \"\" $bitmap
        eval $canvas itemconfigure $fig $opciones
 
     } else {
        create_figure $pag$fig $pag \
	   [string range $fig 3 end] \
           $tipo $coords  \"\" \"\" \
           \"\" 1 none [list 5 10 10] \"\" \"\" \"\" \"\" \"\" \"\"
        eval $canvas itemconfigure $fig $opciones
     }
}







# ----------------------------------------------------------------------
# PROC: Undo Move  Items 
# ----------------------------------------------------------------------

proc save_undo_scale {pag} {
   global page operation_undo

   if {!$page($pag,enable)} { return }

   incr page($pag,ind_undo)
   set list ""

   foreach el $page($pag,sel) {
       lappend list "$pag$el delete_selection"
       eval lappend list [get_all_coords $pag $el]
       lappend list "$pag$el select"
   }
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}



proc save_undo_move {pag elemento} {
   global page operation_undo 

   if {!$page($pag,enable)} { return }
        
   incr page($pag,ind_undo)
   set list ""
   if {[lsearch -glob $page($pag,sel) $elemento] != -1} {
       # el elemento estaba seleccionado
       foreach el $page($pag,sel) {
           lappend list "$pag$el delete_selection" 
           eval lappend list [get_all_coords $pag $el] 
           lappend list "$pag$el select"
       }
   } else {
       foreach el $page($pag,sel) {
           lappend list "$pag$el select"
       }
       lappend list "$pag$elemento delete_selection" 
       eval lappend list [get_all_coords $pag $elemento] 
   }
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}


proc save_undo_rotate {pag elementos} {
   global page operation_undo

   if {!$page($pag,enable)} { return }

   incr page($pag,ind_undo)
   set list ""
   foreach el $elementos {
       lappend list "$pag$el delete_selection"
       eval lappend list [get_all_coords $pag $el]
       lappend list "$pag$el select"
   }
   set operation_undo($pag,$page($pag,ind_undo)) $list
   enable_undo
}


proc get_all_coords {pag elemento} {

   global page
   if {[$pag$elemento cget -type] == "group"} { 
      set l ""
      foreach el [$pag$elemento cget -figlist] {
          set l [concat $l [get_all_coords $pag $el]]
      }
      return $l

   }
   set coords [$page($pag,canvas) coords $elemento]
   return [list [eval list $page($pag,canvas) coords $elemento $coords]]
}



# ----------------------------------------------------------------------
# PROC: Undo Above  Items 
# ----------------------------------------------------------------------

proc undo_above {pag list list_above list_sel} {
   global page operation_undo 

   if {!$page($pag,enable)} { return }

   set canvas $page($pag,canvas)
   incr page($pag,ind_undo)
   set l ""
   lappend l "exec_undo_above $pag [list $list] [list $list_above]"
   foreach elem $list_sel  {
        lappend l "$pag$elem select"
   } 
   set operation_undo($pag,$page($pag,ind_undo)) $l
   enable_undo
}




proc exec_undo_above {pag list list_above } {
   global page 

   set canvas $page($pag,canvas)
   delete_selection_cmd $pag

   if {$list != ""} {
      foreach el $list { $canvas addtag below withtag $el }
   }

   if {$list_above != ""} {
      foreach el $list_above { $canvas addtag above withtag $el }
   }

   $canvas raise above below
   $canvas dtag above
   $canvas dtag below
}

