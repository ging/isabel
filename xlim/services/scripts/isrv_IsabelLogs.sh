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

exec xterm -T "Isabel logs" \
           -font 6x10 \
           -geometry 120x12+0-0 \
           -e tail --retry \
                   --follow=name \
                      $ISABEL_LOGS_DIR/isabel.*log \
                      $XLIM_LOGS_DIR/notif.xlim.*log \
                      $ISABEL_LOGS_DIR/session/isabel_launch.log \
                      $ISABEL_LOGS_DIR/session/isabel_master.log \
                      $ISABEL_LOGS_DIR/session/isabel_interactive.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Audio.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Chat.log \
                      $ISABEL_LOGS_DIR/session/isacomp_ClassCtrl.log \
                      $ISABEL_LOGS_DIR/session/isacomp_ConferenceCtrl.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Ftp.log \
                      $ISABEL_LOGS_DIR/session/isacomp_IsdefeCtrl.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Monitor.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Notepad.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Questions.log \
                      $ISABEL_LOGS_DIR/session/isacomp_SldFtp.log \
                      $ISABEL_LOGS_DIR/session/isacomp_SpeechMeter.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Titles.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Video.log \
                      $ISABEL_LOGS_DIR/session/isacomp_VNC.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Wallpaper.log \
                      $ISABEL_LOGS_DIR/session/isacomp_Whiteboard.log \
         > /dev/null 2>&1

