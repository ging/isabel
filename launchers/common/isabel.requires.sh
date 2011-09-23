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
# Check JDK 1.6 or later
#-----------------------------------------------------

# Use "java" in PATH.
ISAJAVA=$(which java)

# If not found "java" in PATH, use "java" in JDK_HOME or JAVA_PATH.
if [ -z "$ISAJAVA" ] ; then
   if [ -f "$JDK_HOME/bin/java" ] ; then
       export PATH=$PATH:$JDK_HOME/bin
       ISAJAVA=$JDK_HOME/bin/java
   elif [ -f "$JAVA_PATH/bin/java" ] ; then
       export PATH=$PATH:$JAVA_PATH/bin
       ISAJAVA=$JAVA_PATH/bin/java
   fi
fi
export ISAJAVA

eval $ISAJAVA -version > $ISABEL_TMP_DIR/ir.$$ 2>&1
rc=$?
if [ "$rc" != "0" ] ; then
    errmsg1="I cannot find \"java\"."
    errmsg2="Please set the JDK_HOME environment variable to your JRE or JDK installation directory."
    ErrorLogMsg "ERROR: $errmsg1 $errmsg2"
    $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg1" "$errmsg2"
    exit 1
fi

awk '{ if ($3 >= "\"1.6\"") exit 0; else exit 1}' $ISABEL_TMP_DIR/ir.$$ 
rc=$?
if [ "$rc" != "0" ] ; then
    errmsg1="ISABEL needs Java JRE or JDK version 1.6 or later."
    errmsg2="Please update your Java package."
    ErrorLogMsg "ERROR: $errmsg1 $errmsg2"
    $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg1" "$errmsg2"
    exit 1
fi

rm $ISABEL_TMP_DIR/ir.$$



#-----------------------------------------------------
# Check xmlrpc-3.1.jar instalation.
#-----------------------------------------------------

CheckXmlRpcJar ()
{
   if [ ! -f $ISABEL_DIR/libexec/xmlrpc/xmlrpc-common-3.1.jar ] ; then
      errmsg1="ISABEL needs \"xmlrpc-3.1.jar\" to execute LIM services."
      ErrorLogMsg "ERROR: $errmsg1"
      $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg1"
      exit 1
   fi
}

#-----------------------------------------------------
# Check Xvnc installation.
#-----------------------------------------------------

CheckXvnc ()
{
   pathlist=$(echo $PATH | tr : \\\ )
   for p in $pathlist ; do
       if [ ! -x $p/Xvnc ] ; then
          return
       fi
   done

   errmsg1="ISABEL needs \"Xvnc\" to execute the \"$ISABEL_ROLE\" role."
   errmsg2="Please, install VNC server application."
   ErrorLogMsg "ERROR: $errmsg1 $errmsg2"
   $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg1" "$errmsg2"
   exit 1
}

#-----------------------------------------------------
# Check openssl installation.
#-----------------------------------------------------

# look for openssl
which openssl > /dev/null 2>&1
if [ "$?" != "0" ] ; then
    errmsg1="I cannot find \"openssl\" command."
    errmsg2="Please install \"openssl\" package."
    ErrorLogMsg "ERROR: $errmsg1 $errmsg2"
    $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg1" "$errmsg2"
    exit 1
fi

#-----------------------------------------------------
# Check m4 installation.
#-----------------------------------------------------

# look for m4
which m4 > /dev/null 2>&1
if [ "$?" != "0" ] ; then
    errmsg1="I cannot find \"m4\" command."
    errmsg2="Please install \"m4\" package."
    ErrorLogMsg "ERROR: $errmsg1 $errmsg2"
    $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error "ERROR:" "$errmsg1" "$errmsg2"
    exit 1
fi

