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
# $Id: wb_image.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Esta clase esta asociada a la figura de tipo image
#   Los objetos de esta clase se nombran de la siguiente forma:
#      ${pag}fig$i
#   donde pag es la pagina a la que pertenecen e i es un entero
#
#########################################################################
 

class Image {
 
   inherit Figure
   
   public variable namefile ""

   private method constructor {args} {}

   private method _constructor {pag fig coords file} 
   private method _constructor_from_file {pag fig coords fileId difConst} 
 
   method show_info {}
   method copy {replicated}
   method select {}
   method scale {factor centro_ref_x centro_ref_y} {}
   method rotate {center_ref_x center_ref_y}
   method saveFile {fileId swapFileId}
}
 

 
# ----------------------------------------------------------------------
#                             CONSTRUCTOR
# ----------------------------------------------------------------------
 
body Image::constructor {args} {
    set len [llength $args]
    if {$len == 4} {
        eval _constructor $args
    } elseif {$len==5} {
        eval _constructor_from_file $args
    } else {
        puts "- isabel_whiteboard:: Image::constructor there was a problem \
              building the Image object with $len parameters!"
    }
}

body Image::_constructor_from_file {pag fig coords fileId difConst} {
    global page WB_TMP_DIR

    set numfig fig$fig
    set canvas $page($pag,canvas)
    set type "image"
    set mypag $pag

    set linea [get_info [gets $fileId]]
    set bytes [lindex $linea 1]
    set namefile [lindex $linea 3]
    # set photoid [lindex $linea 5]
    set uu_file ${namefile}.uu

    cd $WB_TMP_DIR
    set id_image_file [open $uu_file w+]
    puts $id_image_file [read $fileId $bytes]
    close $id_image_file

    set uu_name ${namefile}.uu
    set dir [pwd]

    cd $WB_TMP_DIR
    exec sed "s/^% //" $uu_name | uudecode

    cd $dir

    image create photo photo${pag}fig$fig -file $namefile
    set page($pag,id) [eval $canvas create image $coords \
       -image photo${pag}fig$fig -tags fig$fig]
}



body Image::_constructor {pag fig coords file} {
 
   global page 
 
   set numfig fig$fig
   set canvas $page($pag,canvas)
   set type "image"
   set namefile $file
   set mypag $pag
 
   image create photo photo${pag}fig$fig -file $file
   set page($pag,id) [eval $canvas create image $coords \
      -image photo${pag}fig$fig -tags fig$fig]
}

 
 
# ----------------------------------------------------------------------
# METHOD: show_info
# ----------------------------------------------------------------------
 
body Image::show_info {} {
  $canvas configure -cursor left_ptr
}

 

 
# ----------------------------------------------------------------------
# METHOD: copy
# ----------------------------------------------------------------------
 
body Image::copy {replicated} {

   set coords [$canvas coords $numfig]
   set image [$canvas itemcget $numfig -image]
   set file [$image cget -file]
 
   set l [llength $coords]
   for {set i 0} {$i < $l} {incr i} {
       lappend newCoords [expr [lindex $coords $i] + 10]
   }

   create_figure ${mypag}fig$replicated \
      $mypag $replicated image $newCoords "" "" "" "" \
      "" "" "" "" "" "" "" $file
}
 
 
 
# ----------------------------------------------------------------------
# METHOD: select
# ----------------------------------------------------------------------
 
body Image::select {} {
 
   global page

   lappend page($mypag,sel) $numfig
   $canvas addtag "resaltado" withtag $numfig
   append namefig $mypag $numfig
   $namefig configure -selec 1
 
   # pinta los cuadraditos de la seleccion
   set coords [$canvas bbox $numfig]
 
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
# METHOD: rotate
# ----------------------------------------------------------------------
 
body Image::rotate {center_ref_x center_ref_y} {
 
   set bbox [$canvas bbox $numfig]
   set center_fig [calculate_center $bbox]
   set new_center [new_center $center_fig $center_ref_x \
        $center_ref_y]
   eval $canvas move $numfig $new_center
}



# ----------------------------------------------------------------------
# METHOD: saveFile
# ----------------------------------------------------------------------
 
body Image::saveFile {fileId swapFileId} {
   global page WB_TMP_DIR
   set canvas     $page($mypag,canvas)
   set coords     [$canvas coords $numfig]
   set opciones_t [$canvas itemconfigure $numfig]
   set opciones   [get_tags $opciones_t ""]

   set figura [list "%??%" $type $coords $opciones]
   puts $fileId $figura

   # tengo que incluir la imagen en el fichero
   set name photo$mypag$numfig
   set file_name [file tail $namefile]

   check_tmp_whiteboard_dir
   set f_image_uu ${WB_TMP_DIR}/${file_name}.uu
   if {[file exists $f_image_uu]} { exec rm $f_image_uu }

   set dir [pwd]
   cd $WB_TMP_DIR
   exec uuencode $namefile $file_name | sed "s/^/% /" > $f_image_uu
   cd $dir
   set id_image_file [open $f_image_uu r]
   set bytes [file size $f_image_uu]

   puts $fileId "%??% File $bytes Name $file_name Foto $name"
   puts $fileId [read $id_image_file $bytes]

   close $id_image_file

   set l [list "%??%" $type $numfig -exist $exist]
   puts $swapFileId $l

}
