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

APT_HOST=http://ubuntu.dit.upm.es/
SOURCESLIST=/etc/apt/sources.list


which apt-get > /dev/null
if [ "$?" -eq 1 ] ; then
   echo "Error: packages not managed with apt."
   exit 1
fi

if [ ! -f "$SOURCESLIST" ] ; then
   echo "file $SOURCESLIST not found."
   exit 1
fi


DISTCODENAME=$(awk -F = '/DISTRIB_CODENAME/ {print $2}' < /etc/lsb-release)
if [ "$?" -ne 0 ] ; then
   echo "I can't get distribution codename."
   exit 1
fi

SOURCE_LINE="deb $APT_HOST $DISTCODENAME extras"


if [ "$1" = "add" ] ; then

   /bin/ed $SOURCESLIST > /dev/null <<EOF
g@$APT_HOST@d
a

# Update Isabel from $APT_HOST
$SOURCE_LINE
.
wq
EOF

elif [ "$1" = "del" ] ; then

   /bin/ed $SOURCESLIST > /dev/null <<EOF
g@$APT_HOST@d
wq
EOF

elif [ "$1" = "get" ] ; then

   /bin/grep "$APT_HOST" $SOURCESLIST

elif [ "$1" = "install" ] ; then

   xterm -e sh -c "
       apt-get -y update 
       apt-get -y install isabel isabel500 isabelgw500
       update-alternatives --set java /usr/lib/jvm/java-6-sun/jre/bin/java
       echo done
       echo Press Ctrl-C to exit 
       sleep 1000000"

elif [ "$1" = "add_and_install" ] ; then

   /bin/ed $SOURCESLIST > /dev/null <<EOF
g@$APT_HOST@d
a

# Update Isabel from $APT_HOST
$SOURCE_LINE
.
wq
EOF

   xterm -e sh -c "
       apt-get -y update 
       apt-get -y install isabel isabel500 isabelgw500
       update-alternatives --set java /usr/lib/jvm/java-6-sun/jre/bin/java
       echo done
       echo Press Ctrl-C to exit 
       sleep 1000000"

else
   echo "Usage: $0 [add|del|get|install|add_and_install]"
   exit 1
fi

exit 0
