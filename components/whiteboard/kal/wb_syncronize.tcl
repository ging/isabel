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
# $Id: wb_syncronize.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#     Se encarga de realizar las operaciones de sincronizacion
#     de enviar las paginas a los usuarios nuevos que se conectan
#     y de atender las peticiones del tipo 'Read master'
#
#########################################################################
 


################################################################
#			 SYNCRONIZATION PAGES
################################################################

############# External Functions ###############################
#
# create_list_of_pages 
# ask_page_content 
# not_send_delete_all 
# send_page_content 
# examine_peticion_queue 
#
################################################################


proc askSynchronism {} {
    global all_pages page
    foreach pag $all_pages {
        if {!$page($pag,enable)} { read_master_pag $pag }
    }
}


#---------------------------------------------------------------
# create_list_of_pages
#
#   si las paginas $pags no existen las solicita a todo el mundo
#---------------------------------------------------------------

proc create_list_of_pages {pags} {
   # en el nuevo
   global all_pages page active_pag
   foreach p $pags {
       if {[lsearch -exact $all_pages $p] != -1} {continue}
       # ojo !!! no pongo master!!!
       exec_new_page $p 0 unknown 0
       set active_pag $p
       set page($p,enable) 0
       set page($p,write) 0
       set_ui_state
       ask_page_content $p 
   }
}

proc read_master {} {
    global active_pag 

    read_master_pag $active_pag
}


proc read_master_pag {pag} {
    global page active_pag myuserid

    if {$myuserid == $page($active_pag,master_id)} { return }

    set page($active_pag,enable) 0
    Wb_del_sync_user Whiteboard $myuserid $active_pag
    not_send_delete_all $active_pag
    write_mode_off $active_pag
    ask_page_content $active_pag
}


#---------------------------------------------------------------
# ask_page_content
#
#   cada 10 segundos envio un msg a todo el mundo para que 
#   el master me envie el contenido de una pagina
#---------------------------------------------------------------

proc ask_page_content {pag} {
  # nuevo
  global id_time_out mysite_id myuserid
  set id_time_out($pag) [after 10000 ask_page_content $pag ]
  ISA_RDOgroup Cmd send_page_content $pag $myuserid $mysite_id
}





#---------------------------------------------------------------
# send_page_content
#
#   si soy el maestro envio la pagina solicitada
#---------------------------------------------------------------

proc send_page_content {pag userid name} { 
   # todos salvo nuevo 
   global page max_peticion min_peticion myuserid active_pag \
          uniqueView

   if {![info exist page($pag,master_id)]} { return }

   # Si soy el coordinador, envio el contenido
   if {$myuserid != $page($pag,master_id)} { return }

   ISA_RDOnode $userid cancel_time_out $pag

   if {$page($pag,lock)} {
       ISA_RDOnode $userid remote_lock_floor $pag
   }

   if {$page($pag,FigNumber)==0} {
       # no es necesario enviar el contenido
       exec_send_variables $pag $userid
       ISA_RDOnode $userid done $pag 
       examine_peticion_queue
       return 
   }
   
   if {[exec_send_page_content $pag $userid $name] == -1} { 
       return 
   }
   
   if {$uniqueView} {
       ISA_RDOnode $userid select_tabnotebook $active_pag
   } else {
       ISA_RDOnode $userid select_tabnotebook $pag
   }

   examine_peticion_queue
}





#---------------------------------------------------------------
# exec_send_page_content
#
#   si estoy libre atiendo la peticion de envio y si no la meto
#   en una cola para atenderla cuando me libere
#---------------------------------------------------------------

proc exec_send_page_content {pag userid name} {
   global sending active_pag peticion page busy \
	   wait_peticion max_peticion min_peticion 

   if {(($sending > 0) || (($busy > 0) && ($pag == $active_pag)))} {
       # no puedo transmitir, encolo la peticion

       while {$wait_peticion} { tkwait variable wait_peticion }
       set wait_peticion 1

       for {set i $min_peticion} {$i < $max_peticion} {incr i} {
           if {(($peticion($i,cmd) == "read") && \
                ($peticion($i,user) == $userid) && \
                ($peticion($i,pag) == $pag))} {
                set wait_peticion 0
                return -1
           }
       }

       set peticion($max_peticion,cmd) "read"
       set peticion($max_peticion,user) $userid
       set peticion($max_peticion,pag) $pag
       set peticion($max_peticion,name) $name
       incr max_peticion
       set wait_peticion 0
       return -1
   } 

   # -------------------------------------------------------
   # Enviando contenido de pagina para manterner coherencia
   # -------------------------------------------------------
   global active_pag latestAction

   set page($pag,sending) 1
   incr sending

   if {$active_pag == $pag} {
       desactiva_eventos 
       change_interface disabled
   }

   exec_send_variables $pag $userid

   save_and_send_page_content $pag $userid $name

   if {$active_pag == $pag} {
        activa_eventos
        eval $latestAction
        change_interface normal
   }
   .wbtl.state.f.info configure -text ""
   delete_sending_window $pag
   incr sending -1
   set page($pag,sending) 0

   return 0
}





#---------------------------------------------------------------
# not_send_delete_all
#
#   borro todo el contenido del canvas porque ha habido un error
#   en la transmision
#---------------------------------------------------------------

proc not_send_delete_all {pag} {
    global page 
    set canvas $page($pag,canvas)
    
    $canvas delete all
    
    set list [itcl::find objects]
    
    foreach el $list { 
	if {[string match $pag* $el]} {
	    delete object $el 
	}
    }
}



#---------------------------------------------------------------
# save_and_send_page_content
#
#   crea el fichero con el contenido de la pagina para la 
#   sincronizacion con un usuario
#---------------------------------------------------------------

proc check_tmp_whiteboard_dir {} {
   global WB_TMP_DIR
   exec mkdir -p $WB_TMP_DIR
}


proc save_and_send_page_content {pag userid useridName} {
   global sent_done_user recibidos page myuserid \
      ISABEL_WORKDIR WB_TMP_DIR ISABEL_TMP_DIR

   check_tmp_whiteboard_dir

   set canvas $page($pag,canvas)
   set name_file ${WB_TMP_DIR}/paint_sync$pag

   set fileId [open $name_file w+]
   set swapFileId [open ${name_file}.wbSWP a+ 0644]
   puts $fileId $pag
   put_items_in_file $pag $fileId $swapFileId

   close $swapFileId
   close $fileId
   exec rm ${name_file}.wbSWP

   .wbtl.state.f.info configure -text "Sending $pag to $useridName..."
   sending_page $pag 1 $userid $useridName

   set sent_done_user($pag,$userid) 0
   set recibidos ""
    Wb_send_pag_read_master Whiteboard \
      "$ISABEL_TMP_DIR" "isabel_whiteboard/paint_sync$pag" \
      "isabel_whiteboard/paint_sync$pag" \
      $userid 20000 $pag

   while {$sent_done_user($pag,$userid) == 0} {
	  tkwait variable sent_done_user($pag,$userid)
   }

   set v_position [$canvas yview]
   set h_position [$canvas xview]
   ISA_RDOnode $userid scroll_y_position $pag [lindex $v_position 0]
}


proc abort_send_page {userid pag} {
   global sent_done_user
   ISA_RDOnode $userid msg_send_error $pag
   set sent_done_user($pag,$userid) -1
}



#---------------------------------------------------------------
# examine_peticion_queue
#
#   examina la cola de peticiones pendientes de atencion 
#---------------------------------------------------------------

proc examine_peticion_queue { } {

   global peticion max_peticion min_peticion wait_peticion myuserid page

   while {$max_peticion > $min_peticion} {

       while {$wait_peticion} { tkwait variable wait_peticion }

       set wait_peticion 1
       set userid $peticion($min_peticion,user)
       set pag $peticion($min_peticion,pag) 
       set cmd $peticion($min_peticion,cmd) 
       set name $peticion($min_peticion,name)

       unset peticion($min_peticion,name) 
       unset peticion($min_peticion,user)
       unset peticion($min_peticion,pag)
       unset peticion($min_peticion,cmd) 

       incr min_peticion 

       if {$myuserid == $page($pag,master_id)} {

	  if {$cmd == "read"} {
	      set wait_peticion 0
  	      trace_busy "examine_peticion_queue:: atiendo READ $pag "

	      # si devuelve -1 es porque no puede atender peticiones, esta
	      # ocupado, espero otra ocasion, si no retornara se quedaria
	      # aqui pegado hasta que estuviera libre, si es porque ha habido
	      # un error con la variable BUSY se bloquearia el programa.

              if {[exec_send_page_content $pag $userid $name] == -1} {
                   return 
              }

          } elseif {$cmd == "floor"} {
	      set wait_peticion 0

	      # si devuelve -1 es porque no puede atender peticiones, esta
	      # ocupado, espero otra ocasion, si no retornara se quedaria
	      # aqui pegado hasta que estuviera libre, si es porque ha habido
	      # un error con la variable BUSY se bloquearia el programa.

              if {[get_floor $pag $userid $name] ==  -1} { return }

          } else { set wait_peticion 0 }

       } else { set wait_peticion 0 }
    }
}
	  



proc cancel_time_out {pag } {
  global id_time_out
  after cancel $id_time_out($pag)
}




proc done {pag} {
   # La pagina esta actualizada, habilito la interfaz 
   # y la registro como candidata a ser coordinador si es la 
   # resincronizacion inicial

   global page myuserid 
   set page($pag,enable) 1
   Wb_add_sync_user Whiteboard $myuserid $pag
}







# ##############################################################
#
# Commands from master
#
# ##############################################################


#---------------------------------------------------------------
# read_file_from_master
#   
#   lee el fichero de sincronizacion que le ha enviado el maestro
#   y conserva los mismos identificadores de los objetos figura
#---------------------------------------------------------------

proc read_file_from_master {pag} {
   # Read a file and this proc doesnt ignore the figure's identifiers
   # It needs to have the same identifiers as the master

   global page WB_TMP_DIR

   set nombre ${WB_TMP_DIR}/paint_sync$pag

   if {![file isfile $nombre]} { return }
   set fileId [open $nombre r]
   set pag [gets $fileId]

   set linea [get_info [gets $fileId]]
   set fin [eof $fileId]
   while {($fin == 0) && ($linea != "FIGURES")} { 
	 set linea [get_info [gets $fileId]]
	 set fin [eof $fileId]
   }

   if {$linea == "FIGURES"} {
      set linea [get_info [gets $fileId]]

      while {($fin == 0) && ($linea != "VARIABLES")} { 
          if {$linea != ""} { analize_figure $fileId $pag $linea } 
          set linea [get_info [gets $fileId]]
          set fin [eof $fileId]
      }
   }

   if {$linea == "VARIABLES"} {
      set linea [get_info [gets $fileId]]
      set fin [eof $fileId]

      while {($fin == 0) && ($linea != "VARIABLES")} { 

         # ojo si en la hoja original he borrado un elemento, 
         # su valor -exist sera cero y no lo creare ahora
	 # si se hace undo para y se redibuja de nuevo la 
	 # figura habra que crear el objeto!!!
	 # OJO!!los grupos siempre existen y por tanto siempre
	 # se crean!!!

         set pos [lsearch -glob $linea "-exist"]
         incr pos 
         set exist [lindex $linea $pos]

         if {$exist == 1} {

             set type [lindex $linea 0]
             set fig [lindex $linea 1]

             # los grupos hay que crearlos despues porque he almacenado 
             # las figuras del canvas, y ahi no aparecen los grupos!!

             if {$type == "group"} {
                 Group $pag$fig $pag [string range $fig 3 end] \"\"
             }

             # set num_fig [string range $fig 3 end]
             set config [lrange $linea 2 end]
             eval $pag$fig configure $config
         }

         set linea [get_info [gets $fileId]]
         set fin [eof $fileId]
      }
   }

   close $fileId
	
   set sel $page($pag,sel)
   $page($pag,canvas) dtag resaltado resaltado

   set page($pag,sel) ""
   foreach el $sel { $pag$el select }

}



proc create_image_file {fileId name bytes} {
    global WB_TMP_DIR

    set uu_file ${WB_TMP_DIR}/${name}.uu

    set id_image_file [open $uu_file w+]
    puts $id_image_file [read $fileId $bytes]
    close $id_image_file

    set uu_name ${name}.uu
    set dir [pwd]

    cd $WB_TMP_DIR
    exec sed "s/^% //" $uu_name | uudecode

    cd $dir
    return ${WB_TMP_DIR}/$name
}



proc extractText {texto} {
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

    set t [list_range $t 0]
    set long [expr [string length $t] -2]
    return [string range $t 1 $long]
}


proc analize_figure {fileId pag linea} {
    global page dir_iconos

    set tipo [list_range $linea 0]
    set coords [list_range $linea 1]
    set opciones [list_range $linea 2]

    set ind [lsearch -glob $opciones -tag*]
    set tags [lindex $opciones [expr $ind+1]]
    set ind [lsearch -glob $tags fig*]
    set t_fig [lindex $tags $ind]

    if {$tipo == "image"} {

        set linea [get_info [gets $fileId]]
        set bytes [lindex $linea 1]
        set name [lindex $linea 3]
        set name_file [create_image_file $fileId $name $bytes]

        Image $pag$t_fig $pag [string range $t_fig 3 end] \
            $coords $name_file

    } elseif {$tipo == "text"} {

        set ind [lsearch -exact $opciones "-text"]

        if {$ind != -1} {
            incr ind
            set texto [extractText [lindex $opciones $ind]]
            set opciones [lreplace $opciones $ind $ind $texto]
        }
       
        Text $pag$t_fig $pag [string range $t_fig 3 end] "$coords" \
             black courier 12 bold w center
        eval $page($pag,canvas) itemconfigure $t_fig $opciones

    } elseif {$tipo =="bitmap"} {

        set ind [lsearch -glob $opciones -bitmap]
        incr ind
        set bitmap [file tail [lindex $opciones $ind]]

        Bitmap $pag$t_fig $pag [string range $t_fig 3 end] \
            $coords black $bitmap

        set opciones [lreplace $opciones [expr $ind -1] $ind]
        eval $page($pag,canvas) itemconfigure $t_fig $opciones

    } else {

        create_figure $pag$t_fig $pag [string range $t_fig 3 end] \
            $tipo $coords  \"\" \"\" \
            \"\" 1 none [list 5 10 10] \"\" \"\" \"\" \"\" \"\" \"\"
        set index [lsearch -exact $opciones -stipple]
        if {$index != -1} {
            set stipple [file tail \
                [lindex $opciones [expr $index +1]]]
            set opciones [lreplace $opciones $index [expr $index+1]]
            eval $page($pag,canvas) itemconfigure $t_fig $opciones \
                -stipple @${dir_iconos}/$stipple

        } else {
            eval $page($pag,canvas) itemconfigure $t_fig $opciones
        }
    }

}


