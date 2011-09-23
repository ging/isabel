#! /bin/sh
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

#-----------------------------------------------------
# isabel directory.
#-----------------------------------------------------
if [ -z "$ISABEL_DIR" ] ; then
  ISABEL_DIR=/usr/local/isabel
  export ISABEL_DIR
fi

#-----------------------------------------------------
# isabel doc directory.
#-----------------------------------------------------
ISABELDOC_DIR=/usr/local/isabeldoc
export ISABELDOC_DIR

#-----------------------------------------------------
# isabel common code
#-----------------------------------------------------
. $ISABEL_DIR/bin/isabel.common.sh

#-----------------------------------------------------
# isabel environment variables
#-----------------------------------------------------
. $ISABEL_DIR/bin/isabel.environ.sh

#-----------------------------------------------------
# Checking ISABEL requirements
#-----------------------------------------------------
#. $ISABEL_DIR/bin/isabel.requires.sh

#-----------------------------------------------------
#
#-----------------------------------------------------

logfile=$(CreateLogFilename Contents)

installed_browser=$(which firefox || which mozilla  || which netscape  || which opera || which konqueror)

if [ "$?" -eq 0 ] ; then
   echo "Browser: $installed_browser" > $logfile  2>&1

   if [ -f $ISABELDOC_DIR/doc/html/documentation.html ] ; then
      $installed_browser file://$ISABELDOC_DIR/doc/html/documentation.html >> $logfile  2>&1
   else
      errmsg1="Isabel documentation not installed."
      errmsg2="Please, install the IsabelDoc package."
      ErrorLogMsg "ERROR: $errmsg1 $errmsg2"
      $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg1" "$errmsg2"
   fi
else
    errmsg="Browser not found."
    ErrorLogMsg "ERROR: $errmsg"
    $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg"
fi

