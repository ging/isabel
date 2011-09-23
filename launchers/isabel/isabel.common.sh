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
# Check display access
#-----------------------------------------------------
CheckDisplayAccess()
{
   TARGETDISPLAY=$1
   MSGERROR=$2

   xdpyinfo -display $TARGETDISPLAY > /dev/null 2>&1
   res="$?"
   if [ "$res" != 0 ] ; then 
      ErrorLogMsg "I can't access display \"$TARGETDISPLAY\". $MSGERROR" 
      $ISABEL_TCL $ISABEL_DIR/lib/isatcl/isabel_error \
              WARNING:  \
              "I can't access display \"$TARGETDISPLAY\"." \
              "$MSGERROR" &

   fi
   return $res
}


#-----------------------------------------------------
# Auxiliar function used to restore a gnome database value.
# The value to set is saved in a file.
# Parameters:
#   $1 - file with the value to restore.
#        The file must be placed at $ISABEL_TMP_DIR
#   $2 - value type
#   $3, $4, etc - gnome database entries.
#                 The value is restored in all the existing entries.
#-----------------------------------------------------
AuxGnomeCleanUpValue()
{
   __file="$ISABEL_TMP_DIR/$1"
   shift

   __type=$1
   shift

   __entries=$*

   if [ -f "$__file" ] ; then
     
      for _entry in $__entries ; do 

         _dirname=$(dirname $_entry)
         _basename=$(basename $_entry)

         gconftool-2 --dir-exists=$_dirname
         if [ "$?" -eq 0 ] ; then
            gconftool-2 -R $_dirname | grep "\<$_basename\>" > /dev/null
            if [ "$?" -eq 0 ] ; then
               gconftool-2 \
                  --type $__type \
                  --set $_entry $(cat $__file)
            fi
         fi
      done

      /bin/rm -f $__file
   fi
}


#-----------------------------------------------------
# CleanUp Desktop:
#    - Restore background
#    - Restore auto_hide panel options
#    - Restore desktop icons
#-----------------------------------------------------
CleanUpDesktop()
{
   # Clean X background
   xsetroot -display $ADISPLAY -cursor_name left_ptr -solid "#36648b"

   # if GNOME is running 
   which gconftool-2
   if [ "$?" -eq 0 ] ; then
      if [ "$($ISABEL_DIR/bin/isabel_fp -a0 gnome-panel)" != "" ] ; then

         if [ -f $ISABEL_TMP_DIR/background_opts ] ; then
            gconftool-2 \
               --type string \
               --set /desktop/gnome/background/picture_options $(cat $ISABEL_TMP_DIR/background_opts)
            /bin/rm -f $ISABEL_TMP_DIR/background_opts
         else
             gconftool-2 --unset /desktop/gnome/background/picture_options
         fi
         if [ -f $ISABEL_TMP_DIR/background_name ] ; then
            gconftool-2 \
               --type string \
               --set /desktop/gnome/background/picture_filename $(cat $ISABEL_TMP_DIR/background_name)
            /bin/rm -f $ISABEL_TMP_DIR/background_name
         else
             gconftool-2 --unset /desktop/gnome/background/picture_filename
         fi

         if [ -f $ISABEL_TMP_DIR/autohide_g_top_panel ] ; then
            gconftool-2 \
               --type bool \
               --set /apps/panel/toplevels/top_panel_screen0/auto_hide $(cat $ISABEL_TMP_DIR/autohide_g_top_panel)
            /bin/rm -f $ISABEL_TMP_DIR/autohide_g_top_panel
         else
             gconftool-2 --unset /apps/panel/toplevels/top_panel_screen0/auto_hide
         fi
         if [ -f $ISABEL_TMP_DIR/autohide_g_bottom_panel ] ; then
            gconftool-2 \
               --type bool \
               --set /apps/panel/toplevels/bottom_panel_screen0/auto_hide $(cat $ISABEL_TMP_DIR/autohide_g_bottom_panel)
            /bin/rm -f $ISABEL_TMP_DIR/autohide_g_bottom_panel
         else
             gconftool-2 --unset /apps/panel/toplevels/bottom_panel_screen0/auto_hide
         fi


         AuxGnomeCleanUpValue screensaver_activated \
                              bool \
                              /apps/gnome-screensaver/idle_activation_enabled


         AuxGnomeCleanUpValue power_manager_ac_sleep_computer \
                              integer \
                              /apps/gnome-power-manager/ac_sleep_computer \
                              /apps/gnome-power-manager/timeout/sleep_computer_ac
         AuxGnomeCleanUpValue power_manager_battery_sleep_computer \
                              integer \
                              /apps/gnome-power-manager/battery_sleep_computer \
                              /apps/gnome-power-manager/timeout/sleep_computer_battery

         AuxGnomeCleanUpValue power_manager_ac_sleep_display \
                              integer \
                              /apps/gnome-power-manager/ac_sleep_display \
                              /apps/gnome-power-manager/timeout/sleep_display_ac
         AuxGnomeCleanUpValue power_manager_battery_sleep_display \
                              integer \
                              /apps/gnome-power-manager/battery_sleep_display \
                              /apps/gnome-power-manager/timeout/sleep_display_battery

         AuxGnomeCleanUpValue power_manager_action_ac_button_lid \
                              string \
                              /apps/gnome-power-manager/action_ac_button_lid \
                              /apps/gnome-power-manager/buttons/lid_ac 
         AuxGnomeCleanUpValue power_manager_action_battery_button_lid \
                              string \
                              /apps/gnome-power-manager/action_battery_button_lid \
                              /apps/gnome-power-manager/buttons/lid_battery 


         #gconftool-2 --type bool --set /apps/nautilus/preferences/show_desktop false
	 #/bin/ps -C nautilus && nautilus -q
         gconftool-2 --type bool --set /apps/nautilus/preferences/show_desktop true
         nohup nautilus --no-default-window > /tmp/nohup.$$.out 2>&1 &

         return 
      fi
   fi


   # if KDE is running
   which dcop
   if [ "$?" -eq 0 ] ; then
      if [ "$($ISABEL_DIR/bin/isabel_fp -a0 kdesktop)" != "" ] ; then

         if [ -f $ISABEL_TMP_DIR/background_dcop ] ; then
            dcop kdesktop KBackgroundIface setWallpaper $(cat $ISABEL_TMP_DIR/background_dcop) 6  
            /bin/rm -f $ISABEL_TMP_DIR/background_dcop
         else
             dcop kdesktop KBackgroundIface setColor "#36648b" false
         fi

         if [ -f $ISABEL_TMP_DIR/desktop_icons_dcop ] ; then
            dcop kdesktop KDesktopIface setIconsEnabled $(cat $ISABEL_TMP_DIR/desktop_icons_dcop)
            /bin/rm -f $ISABEL_TMP_DIR/desktop_icons_dcop
         else
             dcop kdesktop KDesktopIface setIconsEnabled true
         fi


          return
      fi
  fi

}

#-----------------------------------------------------
# CleanUp
#-----------------------------------------------------
CleanUp()
{
   echo "Launcher Cleanup"
 
   $ISABEL_DIR/bin/shmkill

   /bin/kill $(cat $ISABEL_TMP_DIR/*.pid 2> /dev/null ) > /dev/null 2>&1
   /bin/rm -f $ISABEL_TMP_DIR/*.pid > /dev/null 2>&1

   /usr/bin/killall -q isabel_vncreflector

   $ISABEL_DIR/bin/isabel_fp -k vncShmCapture $ISABEL_DIR/lib/isatcl/isacomp_

   $ISABEL_DIR/bin/httpliveencoding.sh stop
   $ISABEL_DIR/bin/xservervnc.sh stop

   # Remove flag indicating that isabel is running
   /bin/rm -f $ISABEL_TMP_DIR/isabel_is_running
 
   #$ISABEL_DIR/bin/isabel_openvpn clean_up

   #/bin/kill $(cat $ISABEL_TMP_DIR/*.pid2 2> /dev/null ) > /dev/null 2>&1
   #/bin/rm -f $ISABEL_TMP_DIR/*.pid2 > /dev/null 2>&1

   #/usr/bin/killall -q isabel_openvpn

   CleanUpDesktop

   # kill vnc server at :89
   nohup nautilus -q --display=:89 > /dev/null 2>&1
   /usr/bin/killall -q Xvnc Xtightvnc

   # Remove Xvnc ussed files:
   /bin/rm -f /tmp/.X89-lock 
   /bin/rm -f /tmp/.X11-unix/X89 
}


#-----------------------------------------------------
# Usage
#-----------------------------------------------------
isabel_usage()
{
   echo "Usage:"
   case $(basename $0) in 
      isabel_launch)
              echo "  isabel_launch <sessionID> <siteID>"
              ;;
      isabel)
              echo "  isabel"
              ;;
   esac
   echo ""
}

#-----------------------------------------------------
# Return a Customer configuration value.
# Arguments:
#   1 - key to search for
#   2 - default value
#-----------------------------------------------------
GetCustomerValue(){

   key=$1
   default=$2

   if [ -r "$ISABEL_CONFIG_DIR/customer.cfg" ] ; then
      cconffile="$ISABEL_CONFIG/customer.cfg"
   elif [ -r "$ISABEL_DIR/lib/customer.cfg" ] ; then
      cconffile="$ISABEL_DIR/lib/customer.cfg"
   else 
      echo $default
      return
   fi

   res=$(cat "$cconffile" | \
            tr "\t" "\ " | \
            egrep "^ *$key *=" | \
            sed "s/[^=]*= *//")

   if [ "$res" != "" ] ; then
      echo $res
   else
      echo $default
   fi 
}

#--------------------------------------------------------------------
# Log functions
#--------------------------------------------------------------------
WriteLogMsg()
{
   echo "$(date) -- isabel -- INFO:: $1"
}

ErrorLogMsg()
{
   echo "$(date) -- isabel -- ERROR:: $1"
}

WarningLogMsg()
{
   echo "$(date) -- isabel -- WARNING:: $1"
}

DebugLogMsg()
{
   if [ "$ISABEL_DEBUG" != "" ] ; then
      echo "$(date) -- isabel -- DEBUG:: $1"
   fi
}

CreateLogFilename()
{
   _prefix=$1
   _extension=$2

   if [ -z "$ISABEL_LOGS_DIR" ] ; then
      export ISABEL_LOGS_DIR=/tmp
   fi

   if [ -z "$_prefix" ] ; then
      _prefix=$$.log
   fi

   if [ -z "$_extension" ] ; then
      _extension=log
   fi

   # Delete old files:
   /usr/bin/find $ISABEL_LOGS_DIR -maxdepth 1 -name "${_prefix}.*.${_extension}" -atime +7 -type f -exec /bin/rm \{\} \; > /dev/null


   _date=$(/bin/date "+%Y%m%d.%H%M%S")

   echo "$ISABEL_LOGS_DIR/${_prefix}.${_date}.${_extension}"
}

CreateNewSessionLogsDir()
{
   # Create new session logs directory:
   __sdate=$(/bin/date "+%Y%m%d.%H%M%S")
   export ISABEL_SESSION_LOGS_DIR=$ISABEL_LOGS_DIR/session.$__sdate
   mkdir -p $ISABEL_SESSION_LOGS_DIR

   # delete old directories:
   /usr/bin/find $ISABEL_LOGS_DIR -maxdepth 1 -name "session.*" -atime +7 -type d -exec /bin/rm -rf \{\} \; > /dev/null

   # Link session to the new directory:
   /bin/ln -snf $ISABEL_SESSION_LOGS_DIR $ISABEL_LOGS_DIR/session
}


