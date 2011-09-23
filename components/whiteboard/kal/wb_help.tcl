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
# $Id: wb_help.tcl 20206 2010-04-08 10:55:00Z gabriel $ 
#
# Description
#
#   Invoca al los ficheros de ayuda en html
#
#########################################################################
 



proc whiteboard_about { } {


}

proc whiteboard_help { } {
   global ISABEL_DIR
   ::iwidgets::hyperhelp .wbtl.painthelp -title "Whiteboard help" \
	-helpdir $ISABEL_DIR/lib/help/whiteboard \
	-topics {Index }
   .wbtl.painthelp activate
   .wbtl.painthelp showtopic Index
   

}
