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
# spec file
#
cat > /tmp/$PKG_NAME-$VERSION.spec <<EOF
Name: $PKG_NAME
Summary:A video codecs/decodecs wrappers library
Version: $VERSION
Release: $RELEASE
License: GPL
BuildRoot: $PKG_DIR
Group: Development/Libraries
#Requires:

%description
vCodecs its a video code/decoder wrapper library that 
supports the most common standard codecs and provides
image colorspace transformations and image processing
funtion utilities.

%post

%files
%attr(755,root,bin) /$VCODECS_RELDIR
%attr(755,root,bin) /usr/share/doc/$PKG_NAME

EOF

#
# Completando estructura de directorios necesarios para los paquetes RPM
#
mkdir -p $PKG_DIR/rpm/BUILD
mkdir -p $PKG_DIR/rpm/SOURCES
mkdir -p $PKG_DIR/rpm/RPMS/i586
mkdir -p $PKG_DIR/rpm/SRPMS
mkdir -p $PKG_DIR/rpm/SPECS

#
# Generacion del paquete rpm.
#
which rpmbuild
if [ "$?" == "0" ] ; then
   rpmbuild -bb --rmspec --target i586 /tmp/$PKG_NAME-$VERSION.spec
else
   rpm -bb --rmspec --target i586 /tmp/$PKG_NAME-$VERSION.spec
fi


