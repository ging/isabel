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

#
# Script to change the installed isabel version.
#


__PREFIX_DIR=/usr/local/
__POSTFIX_DIR=/bin/isabel


Usage()
{
   echo "Usage: $0 [ get | set <version> | config ]"
   exit
}


GetVersions()
{
   update-alternatives --list isabel | \
   awk -F / '{printf("%s ",$4)}'
}


SelectVersion()
{
   __vers=$(GetVersions)

   __cont=1
   echo "Installed Isabel versions:"
   for v in $__vers ; do
      echo "  $__cont - $v"
      __cont=$(($__cont + 1))
   done 

   echo -n "Select the version to install >> "
   read vopt
   return $vopt
}


InstallVersion()
{
   __ver=$1

   if [ -z "$1" ] ; then
      echo "Error: Invalid version \"$1\"."
      exit 1     
   fi

   if [ ! -f "$__PREFIX_DIR$__ver$__POSTFIX_DIR" ] ; then
      echo "Error: version \"$__ver\" not found."
      exit 1     
   fi

   /usr/local/isabel/bin/isabel_cleanup -nodesktop
   /usr/local/isabel/bin/stop_services
   update-alternatives --set isabel $__PREFIX_DIR$__ver$__POSTFIX_DIR
   killall -q gnome-panel || true
}


if [ "$(whoami)" != "root" ] ; then
   echo "ERROR: You must be root to execute this program."
   exit 1
fi

if [ "$1" = "get" ] ; then

   echo "Installed versions: $(GetVersions)"

elif [ "$1" = "set" ] ; then

   if [ -z "$2" ] ; then
      Usage
   fi

   InstallVersion $2
 
elif [ "$1" = "config" ] ; then

   SelectVersion
   __sel=$?

   InstallVersion $(GetVersions | awk -v vopt=$__sel '{print $vopt}')

else
   Usage
fi


echo Done.
echo "Press Ctrl-C to finish."
sleep 60
