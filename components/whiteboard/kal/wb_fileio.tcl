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
# $Id: wb_fileio.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#########################################################################


###############################################################
#                       FILE ACCESS 
################################################################

############# File Menu Functions ###############################
#
# New:         new_page
# Open:        open_file_window
# Save as:     save_file_window
# Close:       close_page
# Import:      image_file_window
# 




################################################################
# FILE -> New
################################################################

#---------------------------------------------------------------
# new_page
#
#   Crea una nueva pagina (New...)
#---------------------------------------------------------------

proc new_page { } {

   global page active_pag myuserid busy mysite_id n_page all_pages 

   incr busy

   set namepage [format "%s.%s" $mysite_id $n_page]
   incr n_page

   # Compruebo que nadie ha creado una pagina con mi site_id
   while {[lsearch -glob $all_pages $namepage ] != -1} {
       set namepage [format "%s.%s" $mysite_id $n_page]
       incr n_page
   }

   set active_pag $namepage

   exec_new_page $namepage $myuserid $mysite_id 1
   ISA_RDOgroup Cmd exec_new_page $namepage $myuserid $mysite_id 1

   select_tabnotebook $namepage 1

   .wbtl.barra.floor.l2 configure \
      -textvariable page($namepage,master_name)
   
   write_mode_on $namepage

   #
   # informo al CoordManager de que existe nueva pagina 
   #
   Wb_set_coordinator Whiteboard $myuserid $namepage \
        "network RDOGroup whiteboard \"Cmd wb_notify_new_coordinator %U %P\"" \
        "network RDOGroup whiteboard \"Cmd wb_nobody_is_synch %U %P\""
   Wb_add_sync_user Whiteboard $myuserid $namepage

   incr busy -1
   examine_peticion_queue
}
 

proc select_tabnotebook {pag {propagate 0}} {
    global uniqueView

    update

    # seleccionar la solapa
    # Nota: puede que no exista esa pagina
    #catch {.wbtl.f1.note select $pag}
    catch {
	.wbtl.f1.note pageconfigure $pag -command ""
	.wbtl.f1.note view $pag
	.wbtl.f1.note pageconfigure $pag -command "select_tabnotebook $pag 1"
    }

    # mostrar el camvas con el contenido de la pagina
    select_page $pag

    # propagar la orden
    if {$uniqueView && $propagate} {
	ISA_RDOgroup Cmd select_tabnotebook $pag 
    }

}

proc select_page {pag {propagate 0}} {
    global page balloon active_pag uniqueMaster uniqueView myuserid
    
    set active_pag $pag   
    .wbtl.barra.floor.l2 configure \
	-textvariable page($active_pag,master_name)
    
    if {$page($active_pag,lock)} {
	.wbtl.barra.floor.lock select
	.wbtl.barra.floor.lock configure -image lock
	balloon add .wbtl.barra.floor.lock "Unlock floor"
	
    } elseif {!$uniqueMaster} {
	# no esta bloqueada, pero si existe un master unico
	# no existe este boton
	.wbtl.barra.floor.lock deselect
	.wbtl.barra.floor.lock configure -image unlock
	balloon add .wbtl.barra.floor.lock "Lock floor"
    }
    
    set_ui_state
    
    if {$page($active_pag,sending)} {
	# set state disabled
	desactiva_eventos
	change_interface disabled
    }
    
    if {$page($active_pag,ind_undo) < 0} { 
	disable_undo
    } else { 
	enable_undo 
    }
}


proc exec_new_page {namepage master_id master_name enable} {
    global active_pag page all_pages myuserid uniqueMaster uniqueView
    
    if {[lsearch -glob $all_pages $namepage] != -1} { return }
    create_var_new_page $namepage $master_id $master_name
    
    .wbtl.f1.note add -label $namepage
    set f [.wbtl.f1.note childsite $namepage]
    
    create_canvas $f $namepage
    
    #set page($namepage,canvas) [$f.c childsite]
    set page($namepage,canvas) [$f.c component canvas]
    
    # selecciona la que tenia antes
    if {![info exist active_pag]} { set active_pag $namepage }
    .wbtl.f1.note pageconfigure $active_pag -command ""
    .wbtl.f1.note view $active_pag
    .wbtl.f1.note pageconfigure $active_pag -command "select_tabnotebook $active_pag 1"
    
    if 0 {
	.wbtl.barra.menu.window.m add command -label $namepage -command {
	    set number [.wbtl.barra.menu.window.m index active]
	    set pag [.wbtl.barra.menu.window.m entrycget $number -label]
	    select_page $pag
	    .wbtl.f1.note view $pag
	}
    } else {
	.wbtl.barra.menu.window.m add command \
	    -label $namepage \
	    -command "select_tabnotebook $namepage 1"
    }
    
    .wbtl.f1.note pageconfigure $namepage -command "select_tabnotebook $namepage 1"

   #
   # Me a~ado a la lista de usuarios actualizados del CoordManager
   # si la pagina se ha creado al pulsar en la interfaz New page.
   # No lo actualizo si se crea al a~adirse un nuevo usuario, porque
   # deberia recibir primero el contenido de la pagina
   #

   if {$master_id != 0} { Wb_add_sync_user Whiteboard $myuserid $namepage }

   #if {$uniqueMaster} { select_tabnotebook $namepage }

   set page($namepage,enable) $enable
}


#---------------------------------------------------------------
# create_canvas
#
#   crea un nuevo canvas para pintar hay uno por cada pagina
#---------------------------------------------------------------
 
proc create_canvas {f name_pag} {
 
   global page

   set c $f.c

   ::iwidgets::scrolledcanvas $c -height 40c -width 50c \
       -textbackground white -autoresize 0 \
       -insertbackground black 
 
   pack $c -expand 1 -fill both
 
   $c configure -scrollregion {0 0 50c 40c} -sbwidth 8
 
   # set canvas [$c childsite]

   set canvas [$c component canvas]

#   set v_scroll [$c component vertsb]
#   set posy [$canvas yview]
#   set page($name_pag,v_scroll_old) [lindex $posy 0]

#   bind $v_scroll <ButtonRelease-1> "move_vertical_scroll"
}

proc move_vertical_scroll {} {
    global active_pag page  
    if {!$page($active_pag,write)} {
        $page($active_pag,canvas) yview moveto \
            $page($active_pag,v_scroll_old)
        return
    }
    set positiony [$page($active_pag,canvas) yview]
    set pos [lindex $positiony 0]
    ISA_RDOgroup Cmd only_if_enabled_page $active_pag \
        scroll_y_position $active_pag $pos
}
   
 

################################################################
# FILE -> Import Whiteboard internal format
################################################################
proc wb_file_window {} {
   global selected_file busy ISABEL_WORKDIR 

   incr busy
   set selected_file ""

   # ------------------------------------------
   # No dejo que el usuario cambie la pagina
   # ------------------------------------------
   .wbtl.f1.note configure -state disabled
   .wbtl.barra.menu.window configure -state disabled

   set w .wbtl.wb
   catch {destroy $w}
   cd $ISABEL_WORKDIR
   ::iwidgets::promptdialog $w -modality application -labeltext \
       "Import Whiteboard internal format file..." -title \
       "Import Whiteboard internal format file..."

   set ef [$w component prompt]
   $ef configure -textvariable selected_file -width 25 \
        -command check_open_file

   $w buttonconfigure 0 -command check_open_file
   $w buttonconfigure 1 -text Browse -command {
       catch {destroy .wbtl.wb.fsd}
       ::iwidgets::fileselectiondialog .wbtl.wb.fsd \
           -directory $general(WhiteboardFileDir)
       .wbtl.wb.fsd buttonconfigure OK \
           -command "selected_file_OK_button WhiteboardFileDir .wbtl.wb.fsd"
       .wbtl.wb.fsd activate
   }

   $w buttonconfigure 2 -command "end_selecting_file .wbtl.wb"

   $w activate
}

################################################################
# FILE -> Open
################################################################

#---------------------------------------------------------------
# open_file_window
#
#   crea el menu para seleccionar el fichero
#   que se desea abrir (Open...)
#---------------------------------------------------------------

proc open_file_window { } {
   global selected_file busy ISABEL_WORKDIR

   incr busy

   # ------------------------------------------
   # No dejo que el usuario cambie la pagina 
   # ------------------------------------------
   .wbtl.f1.note configure -state disabled
   .wbtl.barra.menu.window configure -state disabled

   set w .wbtl.open
   catch {destroy $w}
   cd $ISABEL_WORKDIR
   ::iwidgets::promptdialog $w -modality application \
       -labeltext "Open Whiteboard internal format file... " \
       -title "Open Whiteboard internal format file..."

   set ef [$w component prompt]
   $ef configure -textvariable selected_file \
        -width 25 -command "check_open_file 1"

   $w buttonconfigure 0 -command "check_open_file 1"

   $w buttonconfigure 1 -text Browse -command {
       catch {destroy .wbtl.open.fsd}
       ::iwidgets::fileselectiondialog .wbtl.open.fsd \
           -directory $general(OpenFileDir) 
       .wbtl.open.fsd buttonconfigure OK \
           -command "selected_file_OK_button OpenFileDir .wbtl.open.fsd"
       .wbtl.open.fsd activate
   }

   $w buttonconfigure 2 -command "end_selecting_file .wbtl.open"

   $w activate
}


proc check_open_file args {
   global selected_file recibidos send_done myuserid ISABEL_TMP_DIR
   global active_pag latestAction WB_TMP_DIR

   if {($selected_file == "") || \
     ([file isfile $selected_file] != 1)} {
  	  wb_tk_msg .wbtl.error_file "Bad selection" \
               "Can't open the selected file." OK 1
          end_selecting_file ""
          return
   } 

   if {(([string range $selected_file 0 0] != "/") &&  \
        ([string range $selected_file 0 0] != "~"))} {
         set dir [pwd]
         set selected_file [append dir "/" $selected_file]
   }

   if {$args == "1"} { new_page }

   set pag $active_pag

   set send_done 0

   .wbtl.f1.note configure -state normal
   .wbtl.barra.menu.window configure -state normal
   .wbtl.barra.menu.fichero.m entryconfigure "New" -state normal
   .wbtl.barra.menu.fichero.m entryconfigure "Open..." -state disabled
   .wbtl.barra.menu.fichero.m entryconfigure "Import picture..." -state disabled
   .wbtl.barra.menu.fichero.m entryconfigure "Scan picture" -state disabled
   .wbtl.optionsBar.file.open configure -state disabled

   read_file $selected_file $pag
   catch {destroy .wbtl.open}
   catch {destroy .wbtl.wb}

   set members [Wb_getMembers Whiteboard]
   set ind [lsearch -exact $members $myuserid]
   set members [lreplace $members $ind $ind]

   if {$members == ""} { 
      .wbtl.barra.menu.fichero.m entryconfigure "Open..." -state normal
      .wbtl.barra.menu.fichero.m entryconfigure "Import picture..." -state normal
      .wbtl.barra.menu.fichero.m entryconfigure "Scan picture" -state normal
      .wbtl.optionsBar.file.open configure -state normal
      end_selecting_file ""
      return 
   }

   # --------------------------------------------------
   # Enviando un fichero con formato de la pizarra a
   # todos los participantes (resultado de Open...)
   # --------------------------------------------------
   desactiva_eventos
   change_interface disabled

   # -----------------------
   # Activo boton de abort
   # -----------------------
   sending_page $active_pag 0 0

   .wbtl.state.f.info configure -text "Sending file to all participants..."

   set recibidos ""
   set filename [file tail $selected_file]
   exec cp $selected_file $WB_TMP_DIR
   Wb_read_file_all Whiteboard $ISABEL_TMP_DIR \
         "isabel_whiteboard/$filename" "isabel_whiteboard/$filename" \
         $pag $members 20000

   while {$send_done == 0} {
        tkwait variable send_done
   }

   if {$send_done == -1} { 
       # --------------------
       # He abortado el envio
       # --------------------
       if {$members != ""} {
           send_msg_no_recibidos $pag $members
       }
   }

   catch {exec rm ${WB_TMP_DIR}/$selected_file}
   .wbtl.state.f.info configure -text ""

   # -------------------------------
   # Vuelvo a activar la interfaz
   # -------------------------------
   delete_sending_window $active_pag
 
   .wbtl.barra.menu.fichero.m entryconfigure "Open..." -state normal
   .wbtl.barra.menu.fichero.m entryconfigure "Import picture..." -state normal
   .wbtl.barra.menu.fichero.m entryconfigure "Scan picture" -state normal
   .wbtl.optionsBar.file.open configure -state normal

   activa_eventos
   change_interface normal
   end_selecting_file ""
}


# ----------------------------------------------------------------------
# Leyendo el fichero
# ----------------------------------------------------------------------
proc read_file {nombre pag} {
  # Read a file. Command 'Open file'
  global page new_fig 
  set fileId [open $nombre r]

  set linea [get_info [gets $fileId]]
  set fin [eof $fileId]
  
  while {($fin == 0) && ($linea != "FIGURES")} {
      set linea [get_info [gets $fileId]]
      set fin [eof $fileId]
  }

  if {$linea == "FIGURES"} {
      set linea [get_info [gets $fileId]]
      set fin [eof $fileId]
      while {($fin == 0) && ($linea != "VARIABLES")} {
          if {$linea != ""} {
              read_figure $fileId $pag $linea
          }
          set linea [get_info [gets $fileId]]
          set fin [eof $fileId]
      }
   }

   set max_fig 0
   set new_fig 0
   if {$linea == "VARIABLES"} {
      set linea [get_info [gets $fileId]]
      set max_fig [string range [lindex $linea 1] 3 end]
      while {($fin == 0) && ($linea != "VARIABLES")} {
         if {[lindex $linea 3] != ""} {
             read_variable $pag $linea
         }
         if {$max_fig < $new_fig } {
             set max_fig $new_fig 
         }
         set linea [get_info [gets $fileId]]
         set fin [eof $fileId]
      }
      if {$page($pag,FigNumber) < [expr $max_fig+1]} {
          set page($pag,FigNumber) [expr $max_fig+1] 
      }
   }
   close $fileId
}


# ----------------------------------------------------------------------
# Leyendo una figura del fichero
# ----------------------------------------------------------------------
proc read_figure {fileId pag linea} {

    global page dir_iconos

    set tipo [list_range $linea 0]
    set coords [list_range $linea 1]
    set opciones [list_range $linea 2]

    set ind1 [lsearch -glob $opciones "-tag*"]
    incr ind1
    set values_tags [list_range $opciones $ind1 ]

    set ind [lsearch -glob $values_tags "fig*"] 
    set new_val [string range [lindex $values_tags $ind] 3 end]

    set new_fig_value [expr $page($pag,FigNumber) + $new_val]
# puts "creando figura $new_fig_value"

    set new_values [lreplace $values_tags $ind $ind \
       fig[expr $page($pag,FigNumber) + $new_val]]
    set opciones [lreplace $opciones $ind1 $ind1 $new_values]

    switch $tipo {
        image { 
                #read_image $pag $new_fig_value $coords $fileId "difConst"
                Image ${pag}fig$new_fig_value $pag \
                    $new_fig_value $coords $fileId "difConst"
        } 

        text   { Text ${pag}fig$new_fig_value $pag $new_fig_value \
                      $coords $opciones } 

        bitmap { Bitmap ${pag}fig$new_fig_value $pag $new_fig_value \
                        $coords $opciones } 

        default {
            set pattern_ind [lsearch -exact $opciones "-stipple"] 
            if {$pattern_ind != -1} {
                incr pattern_ind
                set pattern_file [lindex $opciones $pattern_ind]
                set opciones [lreplace $opciones $pattern_ind $pattern_ind \
                   @$dir_iconos$pattern_file]
            }
            create_figure ${pag}fig$new_fig_value $pag $new_fig_value \
                   $tipo $coords \"\" \"\" \
                   \"\" 1 none {10 10 5} courier 10 bold w center \"\"
 
# puts "Poniendo las opciones $opciones"
            eval $page($pag,canvas) itemconfigure fig$new_fig_value $opciones 
        }
    }
}




# ----------------------------------------------------------------------
# Creando image
# ----------------------------------------------------------------------
proc read_image {fileId pag new_fig_value coords} {
    global page WB_TMP_DIR
    set linea [get_info [gets $fileId]]
    set bytes [lindex $linea 1]
    set name [lindex $linea 3]
    # set photoid [lindex $linea 5]

    set uu_file ${WB_TMP_DIR}/${name}.uu

    set id_image_file [open $uu_file w+]
    puts $id_image_file [read $fileId $bytes]
    close $id_image_file

    set uu_name ${name}.uu
    set dir [pwd]

    cd $WB_TMP_DIR
    exec sed "s/^% //" $uu_name | uudecode

    cd $dir

    Image ${pag}fig$new_fig_value $pag $new_fig_value \
       $coords ${WB_TMP_DIR}/$name
}


# ----------------------------------------------------------------------
# Leyendo variable del fichero
# ----------------------------------------------------------------------
proc read_variable {pag linea} {
     global page new_fig 

# puts "Reading $linea"
     set ind [lsearch -glob $linea "-exist"]
     if {($ind == -1) || ([lindex $linea [expr $ind+1]] == 0)} { return }

     set type [lindex $linea 0]
     set fig [lindex $linea 1]
     set new_fig [expr [string range $fig 3 end] \
              + $page($pag,FigNumber)]
            
     if {$type == "group"} {
# puts "Grouping [string range $fig 3 end]"
         Group ${pag}fig$new_fig $pag [string range $fig 3 end] ""
     }

     for {set i 4} {$i < [llength $linea] } {incr i 2} {
         # Modificando los valores de los fig que contiene el grupo
         set option [lindex $linea $i]
         if {$option != ""} {
             set value [list_range $linea [expr $i +1] ] 
             set n_value ""

             foreach f $value {
                 set old_fig [string range $f 3 end]
                 set num_fig [expr $old_fig + $page($pag,FigNumber)]
                 set n_fig fig$num_fig
                 $pag$n_fig configure -father fig$new_fig
                 lappend n_value $n_fig
             }
# puts "Configurando con los nuevos fig el grupo $option $n_value"
             ${pag}fig$new_fig configure $option $n_value
         }
     }
     # Actualizo la lista total de figuras de la pagina
     if {[${pag}fig$new_fig cget -father] == ""} {
         lappend page($pag,FigList) fig$new_fig
     }
# puts "Lista de figuras $page($pag,FigList)"

}




################################################################
# FILE -> Save as...
################################################################

#---------------------------------------------------------------
# save_file_window
#
#   crea el menu para seleccionar el fichero que se desea
#   guardar (Save...)
#---------------------------------------------------------------

proc save_file_window_as_photo { } {
   global general selectedSaveFileAsPhoto
   set w .wbtl.save
   catch {destroy $w}
   cd $general(SaveDirAsPhoto)
   ::iwidgets::promptdialog $w -labeltext "Save file as..." \
        -modality application
   wm title $w "Save as photo ..."

   set ef [$w component prompt]
   $ef configure -textvariable selectedSaveFileAsPhoto \
       -width 25 -command {check_save_file 1}

   set efFrame [$ef childsite]
   ::swc::Optionmenu $efFrame.op -narrow 0 \
       -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
       -labelfont -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
       -command {
            global general
            set frame [[.wbtl.save component prompt] childsite] 
            set general(SaveFormat) [$frame.op get]
   }

   $efFrame.op insert end GIF XPM PS 

   $efFrame.op select $general(SaveFormat)
   pack $efFrame.op
    
   $w buttonconfigure 0 -command {check_save_file 1}

   $w buttonconfigure 1 -text Browse -command save_file_as_photo_browse_button

   $w buttonconfigure 2 -command {destroy .wbtl.save}

   $w activate
}

proc save_file_window_as_whiteboard_format { } {
   global general selectedSaveFileAsWbFormat
   set w .wbtl.save
   catch {destroy $w}
   cd $general(SaveDirAsWbFormat)
   ::iwidgets::promptdialog $w -labeltext "Save file..." \
        -modality application
   wm title $w "Save as whiteboard format..."

   set ef [$w component prompt]
   $ef configure -textvariable selectedSaveFileAsWbFormat \
       -width 25 -command { check_save_file 0}

   $w buttonconfigure 0 -command {check_save_file 0}

   $w buttonconfigure 1 -text Browse \
       -command save_file_as_wb_format_browse_button

   $w buttonconfigure 2 -command {destroy .wbtl.save}

   $w activate
}

proc newDirSelection {widget value} {
    set entryWidget \
        [[[$widget component fsb] component selection] component entry]
    set dir [$entryWidget get]
    $entryWidget selection clear
    $entryWidget insert 0 $dir$value
}

proc save_file_as_photo_browse_button {} {
    global general selectedSaveFileAsPhoto mask
    catch {destroy .wbtl.save.fsd}

    ::iwidgets::fileselectiondialog .wbtl.save.fsd -directory \
         $general(SaveDirAsPhoto) 

    set entryWidget \
        [[[.wbtl.save.fsd component fsb] component selection] component entry]
    $entryWidget selection clear
    $entryWidget insert 0 $general(SaveDirAsPhoto)

    if {$mask==$general(SaveFormat)} {
        set mask "*"
    } else {
        set mask [string tolower $general(SaveFormat)]
    }

    .wbtl.save.fsd configure -mask "*.$mask"
    set frameFile [[[.wbtl.save.fsd component fsb] component filter] childsite]

    ::swc::Optionmenu $frameFile.op -narrow 0 \
        -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
        -labelfont -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
        -command {
            global general
            set frameFile \
                [[[.wbtl.save.fsd component fsb] component filter] childsite]
            set general(SaveFormat) [$frameFile.op get]
            set mask [string tolower $general(SaveFormat)]
            .wbtl.save.fsd configure -mask "*.$mask"
            .wbtl.save.fsd filter
            set frame [[.wbtl.save component prompt] childsite] 
            $frame.op select $general(SaveFormat)
    }

    $frameFile.op insert end GIF XPM PS
    
    $frameFile.op select $general(SaveFormat)

    pack $frameFile.op 

    .wbtl.save.fsd buttonconfigure OK -command {
        global selectedSaveFileAsPhoto general
        set selectedSaveFileAsPhoto [.wbtl.save.fsd get]
        if {[file isfile $selectedSaveFileAsPhoto]} {
            set general(SaveDirAsPhoto) [file dirname $selectedSaveFileAsPhoto]
        } else {
            set general(SaveDirAsPhoto) $selectedSaveFileAsPhoto
        }
        destroy .wbtl.save.fsd
    }
    .wbtl.save.fsd activate
}




proc save_file_as_wb_format_browse_button {} {
    global general selectedSaveFileAsWbFormat
    catch {destroy .wbtl.save.fsd}

    ::iwidgets::fileselectiondialog .wbtl.save.fsd -directory \
        $general(SaveDirAsWbFormat)

    set entryWidget \
        [[[.wbtl.save.fsd component fsb] component selection] component entry]
    $entryWidget selection clear
    $entryWidget insert 0 $general(SaveDirAsWbFormat)

    .wbtl.save.fsd buttonconfigure OK -command {
        global selectedSaveFileAsWbFormat general
        set selectedSaveFileAsWbFormat [.wbtl.save.fsd get]
        set general(SaveDirAsWbFormat) \
            [file dirname $selectedSaveFileAsWbFormat]
        destroy .wbtl.save.fsd
    }
    .wbtl.save.fsd activate
}



# --------------------------------------------------------------
# check_save_file
#
#   comprueba si el nombre fichero para guardar las figuras
#   es valido
# --------------------------------------------------------------

proc check_file {filename} {
   if {$filename == ""} {
      wb_tk_msg  .wbtl.error_file "Bad selection" \
        "Can't open the selected file." OK 1
      return 1
   } 

   if {[file isfile $filename]} {
      exec rm $filename
   }

   return 0
}

proc create_postscript_file {filename} {
    global page active_pag 
    set selection $page($active_pag,sel)
    foreach el $selection {
        $active_pag$el delete_selection
    }

    $page($active_pag,canvas) postscript -file $filename

    foreach el $selection {
        $active_pag$el select
    }
}

proc check_save_file {isPhoto} {
   global page active_pag selectedSaveFileAsPhoto \
       general selectedSaveFileAsWbFormat WB_TMP_DIR

   if {($isPhoto == 1) && ($general(SaveFormat)!="PS")} {
      global mask
      if {[check_file $selectedSaveFileAsPhoto]} { return }
      # SALVAR FOTO, sin implementar
      set rootName [file rootname [file tail $selectedSaveFileAsPhoto]]
      set dir $WB_TMP_DIR

      create_postscript_file $dir${rootName}Wb.ps

      set extension [file extension $selectedSaveFileAsPhoto]
      if {$extension == ""} {
          set extension [append "." [string tolower $general(SaveFormat)]]
      } else {
          set extension ""
      }
      switch $general(SaveFormat) {
          "GIF" {
               if { [catch " exec echo \"($dir${rootName}Wb.ps) run\" | gs -r72x72 -dQUIET -dPAUSE -sDEVICE=ppmraw -sOutputFile=$dir${rootName}Wb.ppm; exec pnmcrop < $dir${rootName}Wb.ppm | ppmtogif > $selectedSaveFileAsPhoto$extension" errmsg]!= 0 } {
                puts "- isabel_whiteboard Error:: $errmsg"
               }
               catch {exec rm $dir${rootName}Wb.ppm}
           }

           "XPM" {
               if { [catch " exec echo \"($dir${rootName}Wb.ps) run\" | gs -r72x72 -dQUIET -dPAUSE -sDEVICE=ppmraw -sOutputFile=$dir${rootName}Wb.ppm; exec pnmcrop < $dir${rootName}Wb.ppm | ppmtoxpm > $selectedSaveFileAsPhoto$extension" errmsg]!= 0 } {
                puts "- isabel_whiteboard Error:: $errmsg"
               }
               catch {exec rm $dir${rootName}Wb.ppm}
           }
 
           default: {
           } 
      }
      catch {exec rm $dir${rootName}Wb.ps}

   } else {
      # SALVAR WB FORMAT, .ps mas formato interno
      check_file $selectedSaveFileAsWbFormat
      create_file $selectedSaveFileAsWbFormat a+
   }
   destroy .wbtl.save
   examine_peticion_queue
}

# --------------------------------------------------------------
# create_file
#
#   Crea el fichero con las figuras, primero mete el ps y 
#   despues cada una de las figuras y la informacion sobre
#   las variables
# --------------------------------------------------------------

proc create_file {nombre_fichero permiso} {

   global file page active_pag 

   create_postscript_file $nombre_fichero

   if {[file isfile $nombre_fichero] == 1} {
      exec chmod 644 $nombre_fichero
   }

   set fileId [open $nombre_fichero a+ 0644]
   set swapFileId [open ${nombre_fichero}.wbSWP a+ 0644]
   put_items_in_file $active_pag $fileId $swapFileId

   close $swapFileId
   close $fileId
   exec rm ${nombre_fichero}.wbSWP
}


proc put_items_in_file {pag fileId swapFileId} {
    global page
   
    set c $page($pag,canvas)
    set first [get_fig $c [first_figure $pag [concat $page($pag,FigList)]]]
   
    # no puedo eliminar aqui los id que yo he puesto, porque
    # si los quito luego no se darselos de nuevo, al leer el 
    # fichero tengo que tener cuidado y pasar de ellos
   
    puts $fileId "%??% FIGURES"
    if {$first == ""} {
        # por el Undo
        puts $fileId "%??% VARIABLES"
        saveGroups $fileId
        return
    }
   
    $pag$first saveFile $fileId $swapFileId
    set next [$c find above $first]
    set tags [$c itemcget $next -tags]

    while {$next != ""} { 
        if {([lsearch -glob $tags sel*] == -1) && \
            ([lsearch -glob $tags pointer*] == -1)} {
               set fig [get_fig $c $next]
               $pag$fig saveFile $fileId $swapFileId
        }
        set next [$c find above $next]
        set tags [$c itemcget $next -tags]
    }

    puts $fileId "%??% VARIABLES"
    seek $swapFileId 0
    set fin [eof $swapFileId]
    while {$fin == 0} {
       set linea [gets $swapFileId]
       puts $fileId $linea
       set fin [eof $swapFileId]
    }
    saveGroups $fileId          
    return	  
}

proc saveGroups {fileId } {
   set grupos [find  objects * -class Group]
   foreach grp $grupos {
       set fig [$grp cget -numfig]
       set l [list "%??%" group $fig -exist [$grp cget -exist]]

       if {[$grp cget -father] != ""} {
           lappend l -father [$grp cget -father]
       }

       if {[$grp cget -figlist] != ""} {
           lappend l -figlist [$grp cget -figlist]
       }

       puts $fileId $l
   }
}



proc first_figure {pag list} {
    global page
    set canvas $page($pag,canvas)
    foreach el $list {
        if {[$pag$el cget -type] != "group"} {
            if {[$canvas find below $el] == ""} {
                return $el
            } 
        } else {
            set first [first_figure $pag [$pag$el cget -figlist]]
            if {$first != ""} { return $first }
        }
    }
}





################################################################
# FILE -> Close
################################################################

#---------------------------------------------------------------
# close_page
#
#   crea una ventana para asegurarse si se quiere
# 	cerrar la pagina sin guardar antes (Close...)
#---------------------------------------------------------------

proc close_page { } {
   global active_pag 
   catch {destroy .wbtl.close}
   toplevel .wbtl.close
   wm group .wbtl.close .wbtl
   set fichero $active_pag

   label .wbtl.close.l1 -text "Are you sure you want to close $fichero \
		before saving?"
   pack .wbtl.close.l1

   ::iwidgets::buttonbox .wbtl.close.bb

   .wbtl.close.bb add ok -text OK -command exec_close_page
   .wbtl.close.bb add cancel -text Cancel -command "catch {destroy .wbtl.close}"
   .wbtl.close.bb add save -text "Save as..." -command save_file_window 

   pack .wbtl.close.bb
}

proc exec_close_page {} {
    global active_pag all_pages

    set pag $active_pag
    if {[lsearch -glob $all_pages $pag] == -1} {
        puts "- isabel_whiteboard Error:: \
              The page $active_page does not seem to exist"
        return
    }

    libera $pag
    ISA_RDOgroup Cmd libera $pag

    set len [llength $all_pages]
    if {$len > 0} {
        incr len -1
        set newPage [lindex $all_pages $len]
        .wbtl.f1.note select $newPage
        ISA_RDOgroup Cmd remoteSelectPage $pag $newPage

    } else {
        # vamos a cerrar la unica paginaabierta que habia, abrir otra
        new_page
        ISA_RDOgroup Cmd .wbtl.f1.note select $active_pag
    }

    #
    # Avisa al CoordManager para que borre toda la info de la pag
    #
    Wb_unset_pag Whiteboard $pag

    delete_page $pag
    ISA_RDOgroup Cmd delete_page $pag

    destroy .wbtl.close
}

proc remoteSelectPage {oldPage newPage} {
    global active_pag
    if {$active_pag == $oldPage} {
	    .wbtl.f1.note select $newPage
	}
}


proc delete_page {pag} {
    .wbtl.f1.note delete $pag $pag
    .wbtl.barra.menu.window.m delete $pag
}



 

################################################################
# FILE -> Import *.ppm *.gif picture...
################################################################

#---------------------------------------------------------------------
# image_file_window 
#
#   crea la ventana para seleccionar el fichero que contiene la foto
#   (Import...)
#---------------------------------------------------------------------

proc image_file_window {} {
   global busy general

   incr busy
   clear_mouse_events
   set selected_file ""

   # ------------------------------------------
   # No dejo que el usuario cambie la pagina 
   # ------------------------------------------
   .wbtl.f1.note configure -state disabled
   .wbtl.barra.menu.window configure -state disabled

   set w .wbtl.image
   catch {destroy $w}

   ::iwidgets::promptdialog $w -modality application \
	  -labeltext "Import BMP, GIF, JPEG, TIFF, PNG, PPM, XBM, XPM, PS..." \
	  -title "Import photo" 

   set ef [$w component prompt]
   $ef configure -textvariable selected_file -width 25 \
       -command check_image_file 

   $w buttonconfigure 0 -command check_image_file

   $w buttonconfigure 1 -text Browse -command browse_image_files 

   $w buttonconfigure 2 -command "end_selecting_file .wbtl.image"

   $w activate
}

proc end_selecting_file {widget} {
    global busy latestAction
    .wbtl.f1.note configure -state normal
    .wbtl.barra.menu.window configure -state normal
    incr busy -1
    if {$widget != ""} {
        catch {destroy $widget}
    }
    eval $latestAction
    examine_peticion_queue
}

proc browse_image_files {} {
    global general
    catch {destroy .wbtl.image.fsd}
    ::iwidgets::fileselectiondialog .wbtl.image.fsd \
         -directory $general(ImageDir) -mask "*.gif" 
    set frameFile [[[.wbtl.image.fsd component fsb] component filter] childsite]

    ::swc::Optionmenu $frameFile.op -narrow 0 \
        -font -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
        -labelfont -*-times-bold-R-Normal--*-100-*-*-*-*-*-* \
        -command "change_file_mask .wbtl.image.fsd"

    $frameFile.op insert end GIF BMP JPEG TIFF PNG PPM XBM XPM PS 

    pack $frameFile.op
      
    .wbtl.image.fsd buttonconfigure OK -command \
         "selected_file_OK_button ImageDir .wbtl.image.fsd"
    
    .wbtl.image.fsd activate
}

proc selected_file_OK_button {typeDir widget} {
    global selected_file general
    set selected_file [$widget get]
    set general($typeDir) [file dirname $selected_file]
    catch {destroy $widget}
}

proc change_file_mask {widget} {
    global mask
    set frameFile [[[$widget component fsb] component filter] childsite]
    set value [$frameFile.op get]
    set mask [string tolower $value]
    set newMask "*.$mask"
    $widget configure -mask $newMask
    $widget filter
}

# --------------------------------------------------------------
# check_image_file
#
#   Comprueba si el fichero con la imagen existe y es un GIF o 
#   PPM para poder leerlo
# --------------------------------------------------------------
 
proc check_image_file {} {
   global selected_file busy page active_pag latestAction

   # EL fichero existe
   if {($selected_file == "")             || \
       ([file isfile $selected_file] != 1)    } {
      wb_tk_msg .wbtl.error_file "Bad selection" \
          "Can't open the selected file." OK 1
      end_selecting_file ""
      return
   } 

   # Se reconoce el formato de la imagen
   if [catch {image create photo photo$selected_file \
                -file $selected_file} errmsg] {
       puts "- isabel_whiteboard warning:: Unrecognized format file"
       catch {destroy .wbtl.image}
       wb_tk_msg .wbtl.photoError "Error loading photo" \
           "Unrecognized file  format" Continue 1
       end_selecting_file ""
       return
   }

   $page($active_pag,canvas) configure -cursor clock

   if {(([string range $selected_file 0 0] != "/") &&  \
          ([string range $selected_file 0 0] != "~"))} {
        set dir [pwd]
        set selected_file [append dir "/" $selected_file]
   }

   place_image $selected_file
   catch {destroy .wbtl.image}
}
 



 
#---------------------------------------------------------------
# place_image
#
#   presenta la imagen en las coordenadas dadas
#---------------------------------------------------------------
 
proc place_image {image_file} {
 
   global page active_pag first_place_image
   set first_place_image 1

   change_interface disabled
   .wbtl.barra.menu.fichero configure -state disabled

   bind $page($active_pag,canvas) <Any-Motion> "any_motion_place_image \
       $image_file \
       \[$page($active_pag,canvas) canvasx %x\] \
       \[$page($active_pag,canvas) canvasy %y\] "
 
   bind $page($active_pag,canvas) <ButtonRelease-1> \
       "button_release_place_image \
       $image_file \
       \[$page($active_pag,canvas) canvasx %x\] \
       \[$page($active_pag,canvas) canvasy %y\] "
}



proc any_motion_place_image {image_file posix posiy} {
    global first_place_image active_pag page posx posy
    global medio_width medio_height 

    # ---------------------------------------------------------
    # La primera vez, crea  un cuadrado del tama~o de la imagen
    # ---------------------------------------------------------
    if {$first_place_image} {
        set posx $posix
        set posy $posiy

        set image_height [image height photo$image_file]
        set image_width [image width photo$image_file]
        set medio_width [expr $image_width /2]
        set medio_height [expr $image_height /2]

        $page($active_pag,canvas) create rectangle  \
            [expr $posix - $medio_width] [expr $posiy - $medio_height] \
            [expr $posix + $medio_width] [expr $posiy + $medio_height] \
            -tag image_test
        set first_place_image 0
        $page($active_pag,canvas) configure -cursor left_ptr
        return
    } 

    # ----------------------------------------------------------------
    # El resto de las veces, mueve el cuadrado del tama~o de la imagen
    # ----------------------------------------------------------------
    set difx [expr $posix - $posx]
    set dify [expr $posiy - $posy]
    set posx $posix
    set posy $posiy
    set item [$page($active_pag,canvas) find withtag image_test]
    $page($active_pag,canvas) move $item $difx $dify
}



proc temporally_change_interface {state} {
    .wbtl.barra.menu.fichero.m entryconfigure "Open..." -state $state
    .wbtl.barra.menu.fichero.m entryconfigure "Import picture..." \
      -state $state
    .wbtl.barra.menu.fichero.m entryconfigure "Scan picture" -state $state
    .wbtl.optionsBar.file.open configure -state $state
}



proc button_release_place_image {image_file posx posy} {
    global page busy send_done active_pag
    global myuserid recibidos active_pag general latestAction
    global ISABEL_TMP_DIR WB_TMP_DIR

    # modificando interfaz
    $page($active_pag,canvas) configure -cursor clock
    .wbtl.barra.menu.fichero configure -state normal
    .wbtl.optionsBar.file.new configure -state normal
    .wbtl.barra.menu.fichero.m entryconfigure "New" -state normal
      
    image delete photo$image_file
    $page($active_pag,canvas) delete \
       [$page($active_pag,canvas) find withtag image_test]

    set fg fig$page($active_pag,FigNumber)
    lappend page($active_pag,FigList) $fg

    Image $active_pag$fg $active_pag $page($active_pag,FigNumber) \
         [list $posx $posy] $image_file

    save_undo_paint_items $active_pag fig$page($active_pag,FigNumber)

    incr page($active_pag,FigNumber)

    set members [Wb_getMembers Whiteboard]
    set ind [lsearch -exact $members $myuserid]
    set members [lreplace $members $ind $ind]

    # ----------------------------------------
    # Calculo el resto de participantes a
    # los que tengo que enviar la imagen
    # ----------------------------------------
    if {$members == ""} {
        end_place_image  
        return
    }

    set send_done 0

    # reconfigurando interfaz
    desactiva_eventos
    change_interface disabled
    temporally_change_interface disabled

    # -----------------------
    # Activo boton de abort
    # -----------------------
    sending_page $active_pag 0 0

    .wbtl.state.f.info configure -text "Sending image to all participants..."

    # -------------------------------
    # Llamo al FTP y me quedo pegado 
    # esperando a que todo el mundo
    # consiga la foto
    # -------------------------------
    set filename [file tail $image_file]
    set recibidos ""
    exec cp $image_file ${WB_TMP_DIR}/$filename

    Wb_send_photo_all Whiteboard \
        $ISABEL_TMP_DIR "isabel_whiteboard/$filename" \
        "isabel_whiteboard/$filename" \
        $active_pag $posx $posy $members 20000

    while {$send_done== 0} { tkwait variable send_done }

    if {($send_done == -1) && ($members != "")} { 
       # --------------------
       # He abortado el envio
       # --------------------
       send_msg_no_recibidos $active_pag $members
    }

    catch {exec rm ${WB_TMP_DIR}/$filename}
    # -------------------------------
    # Vuelvo a activar la interfaz
    # -------------------------------
    delete_sending_window $active_pag
    temporally_change_interface normal
    end_place_image 
}

proc end_place_image {} {
    global page active_pag
    .wbtl.state.f.info configure -text ""
    bind $page($active_pag,canvas) <Any-Motion> { }
    bind $page($active_pag,canvas) <ButtonRelease-1> { }
    $page($active_pag,canvas) configure -cursor left_ptr
    activa_eventos
    change_interface normal
    end_selecting_file ""
}



#---------------------------------------------------------------
# get_info: Elimina la marca que indica informacion de figuras
#		creadas dentro del fichero postscrip. Para 
#		el lenguaje postscript funciona como un 
#		comentario
#---------------------------------------------------------------

proc get_info {cadena} {
    set marca [string range $cadena 0 3 ]
    if {$marca == "%??%"} { 
        return [string range $cadena 5 end]
    } else {return ""}
}


proc sending_page {pag sync userid {userName ""}} {
   global page dir_iconos

   if {![winfo exists $page($pag,canvas).f1]} {
       frame $page($pag,canvas).f1 -bd 4 -relief groove
       pack $page($pag,canvas).f1 -padx 1c -pady 1c 

       label $page($pag,canvas).f1.bitmap -bitmap @$dir_iconos/adm.xbm \
           -foreground red
       frame $page($pag,canvas).f1.f2
       pack $page($pag,canvas).f1.bitmap -side left -padx 0.5c -pady 0.5c
       pack $page($pag,canvas).f1.f2 
       
       label $page($pag,canvas).f1.f2.label \
           -font -*-times-bold-R-Normal--*-160-*-*-*-*-*-* \
           -text "Sending file to all participants..."

       if {$sync} {
           $page($pag,canvas).f1.f2.label configure -text \
               "Sending page $pag to a user $userName..."
       }

       button $page($pag,canvas).f1.f2.button -text Abort -command \
           "abort_sending $pag $sync $userid"

       pack $page($pag,canvas).f1.f2.label \
           -padx 0.5c -pady 0.5c
       pack $page($pag,canvas).f1.f2.button \
           -padx 0.3c -pady 0.3c
   }

   $page($pag,canvas) create window 13c 7c -window $page($pag,canvas).f1 \
       -tags sendingWindow
}

proc abort_sending {pag sync userid} {
   if {$sync} {
       global sent_done_user
       ISA_RDOnode $userid msg_send_error $pag
       set sent_done_user($pag,$userid) -1

   } else {  
       global send_done
       set send_done -1
   }

   delete_sending_window $pag
}
proc delete_sending_window {pag} {
    global page
    catch {$page($pag,canvas) delete sendingWindow}
    catch {destroy $page($pag,canvas).f1}
}

