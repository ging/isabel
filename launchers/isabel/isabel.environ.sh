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


__SKIP_MKDIR=$(echo $* | grep "\-skipmkdir\>")

#-----------------------------------------------------
# patches
#-----------------------------------------------------
CDIFFS_MAX_NUMBER_OF_CELLS_PER_FRAME=2000
export CDIFFS_MAX_NUMBER_OF_CELLS_PER_FRAME

#-----------------------------------------------------
# Customer configuration
#-----------------------------------------------------
ISABEL_CUSTOMER_NAME=$(GetCustomerValue customerName Isabel)

#-----------------------------------------------------
# Debug mode.
#-----------------------------------------------------
if [ -z "$ISABEL_DEBUG" ] ; then
   ISABEL_DEBUG=0
   export ISABEL_DEBUG
fi

#-----------------------------------------------------
# IPv4 or IPv6
#-----------------------------------------------------
__kk__=$(env | grep ISABEL_USE_IPv6)
if [ "$?" != "0" ] ; then
   $ISABEL_DIR/bin/isabel_ipv6ready
   if [ "$?" = "0" ] ; then
      ISABEL_USE_IPv6=1
      export ISABEL_USE_IPv6
   fi
fi

#-----------------------------------------------------
# TCL interpreter.
#-----------------------------------------------------
if [ -z "$ISABEL_TCL" ] ; then
    export ISABEL_TCL=wish
    for __wish in $(which wish8.5 wish8.4 wish8.3 wish) ; do
	$__wish <<EOF
package require Itcl
namespace import -force itcl::*
package require Iwidgets
exit 0
EOF
	if [ "$?" -eq 0 ] ; then
	    export ISABEL_TCL=$__wish
	    break
	fi
    done
fi

#-----------------------------------------------------
# Search path.
#-----------------------------------------------------
PATH=$ISABEL_DIR/bin:$PATH
export PATH

#-----------------------------------------------------
# LD_LIBRARY_PATH
#-----------------------------------------------------
LD_LIBRARY_PATH=/lib:/usr/lib:$ISABEL_DIR/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

#-----------------------------------------------------
# Ping.
#-----------------------------------------------------
if [ -n "$ISABEL_USE_IPv6" ] ; then
   PING="ping6"
else
   PING="/bin/ping"
fi
export PING

#-----------------------------------------------------
# isabel version 
#-----------------------------------------------------
if [ ! -f $ISABEL_DIR/lib/version.Isabel ] ; then
   ErrorLogMsg "\"$ISABEL_DIR/lib/version.Isabel\" version file not found."
   exit 1
fi
ISABEL_VERSION=$(grep VERSION $ISABEL_DIR/lib/version.Isabel | awk '{print $2}')
export ISABEL_VERSION
ISABEL_RELEASE=$(grep RELEASE $ISABEL_DIR/lib/version.Isabel | awk '{print $2}')
export ISABEL_RELEASE

#-----------------------------------------------------
# work directory.
#-----------------------------------------------------
if [ -z "$ISABEL_WORKDIR" ] ; then
   ISABEL_WORKDIR=$HOME
   export ISABEL_WORKDIR
fi

#-----------------------------------------------------
# Xlim user directory.
#-----------------------------------------------------
if [ -z "$XLIM_DIR" ] ; then
   XLIM_DIR=$HOME/.xlim
   export XLIM_DIR
fi
test -z "$__SKIP_MKDIR" && mkdir -p $XLIM_DIR


#-----------------------------------------------------
# Isabel user directory.
#-----------------------------------------------------
if [ -z "$ISABEL_USER_DIR" ] ; then
   ISABEL_USER_DIR=$HOME/.isabel
   export ISABEL_USER_DIR
fi
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_USER_DIR

#-----------------------------------------------------
# Slides directory.
#-----------------------------------------------------
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_WORKDIR/pres

#-----------------------------------------------------
# Temporal directory.
#-----------------------------------------------------
ISABEL_TMP_DIR=$ISABEL_USER_DIR/tmp
export ISABEL_TMP_DIR
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_TMP_DIR

#-----------------------------------------------------
# Logs.
#-----------------------------------------------------
if [ -z "$ISABEL_LOGS_DIR" ] ; then
   ISABEL_LOGS_DIR=$ISABEL_USER_DIR/logs
   export ISABEL_LOGS_DIR
fi
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_LOGS_DIR


export ISABEL_SESSION_LOGS_DIR=$(/bin/ls -1drt $ISABEL_LOGS_DIR/session.* 2> /dev/null| /usr/bin/tail -1)
if [ -z "$ISABEL_SESSION_LOGS_DIR" ] ; then
   export ISABEL_SESSION_LOGS_DIR=$ISABEL_LOGS_DIR
fi
/bin/ln -snf $ISABEL_SESSION_LOGS_DIR $ISABEL_LOGS_DIR/session
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_SESSION_LOGS_DIR


XLIM_LOGS_DIR=$XLIM_DIR/logs
export XLIM_LOGS_DIR
test -z "$__SKIP_MKDIR" && mkdir -p $XLIM_LOGS_DIR

#-----------------------------------------------------
# Configurations directory.
#-----------------------------------------------------
ISABEL_CONFIG_DIR=$ISABEL_USER_DIR/config
export ISABEL_CONFIG_DIR
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_CONFIG_DIR

#-----------------------------------------------------
# Profiles directory.
#-----------------------------------------------------
ISABEL_PROFILES_DIR=$ISABEL_CONFIG_DIR/profiles/4.11
export ISABEL_PROFILES_DIR
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_PROFILES_DIR

#-----------------------------------------------------
# Sessions directory.
#-----------------------------------------------------
ISABEL_SESSIONS_DIR=$ISABEL_USER_DIR/sessions/4.11
export ISABEL_SESSIONS_DIR
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_SESSIONS_DIR

#-----------------------------------------------------
# Isabel core setup directory.
#-----------------------------------------------------
ISABEL_SETUP_DIR=$ISABEL_SESSIONS_DIR/setup
export ISABEL_SETUP_DIR
test -z "$__SKIP_MKDIR" && mkdir -p $ISABEL_SETUP_DIR

#-----------------------------------------------------
# Displays
#-----------------------------------------------------
if [ -z "$DISPLAY" ] ; then
  WriteLogMsg "ERROR - $0: cannot open DISPLAY" 1>&2
  exit 1
fi

if [ -z "$ADISPLAY" ] ; then
  ADISPLAY=:0
fi
export ADISPLAY

#-----------------------------------------------------
# Xuggle enviroment vars
#-----------------------------------------------------

export XUGGLE_HOME=/usr/local/xuggler
export PATH=$XUGGLE_HOME/bin:$PATH
export LD_LIBRARY_PATH=$XUGGLE_HOME/lib:$LD_LIBRARY_PATH
