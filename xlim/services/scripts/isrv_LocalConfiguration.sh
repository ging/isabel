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
. $ISABEL_DIR/bin/isabel.requires.sh

#-----------------------------------------------------
#
#-----------------------------------------------------

exec java -cp  $ISABEL_DIR/libexec/isabel_tunnel.jar:$ISABEL_DIR/libexec/isabel_xlimservices.jar:$ISABEL_DIR/libexec/xlim_lib.jar:$ISABEL_DIR/libexec/xedl.jar:$ISABEL_DIR/libexec/xerl.jar:$ISABEL_DIR/lib/images/xlim/ -Dprior.config.file=$ISABEL_DIR/lib/xlimconfig/priorxedl.cfg  -Disabel.dir=$ISABEL_DIR/ -Disabel.profiles.dir=$ISABEL_PROFILES_DIR -Disabel.sessions.dir=$ISABEL_SESSIONS_DIR -Disabel.user.dir=$ISABEL_USER_DIR -Disabel.config.dir=$ISABEL_CONFIG_DIR services/isabel/services/options/Options > $XLIM_LOGS_DIR/LocalConfiguration.log 2>&1

