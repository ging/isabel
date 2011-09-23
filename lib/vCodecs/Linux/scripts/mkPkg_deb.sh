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

echo "haciendo paquete ${DEBFILENAME} en $PKG_DIR"

#
# Completando estructura de directorios necesarios para los paquetes DEB
#
echo "mkdir $PKG_DIR/DEBIAN"
mkdir $PKG_DIR/DEBIAN

#
# Documentacion
#
mkdir -p $PKG_DIR/usr/share/doc/vCodecs
echo "Copyright: Agora Systems S.A." > $PKG_DIR/usr/share/doc/vCodecs/copyright
touch $PKG_DIR/usr/share/doc/vCodecs/changelog.Debian
gzip $PKG_DIR/usr/share/doc/vCodecs/changelog.Debian

#
# Control
#
cat > $PKG_DIR/DEBIAN/control << EOF
Package: vCodecs
Version: $VERSION.$RELEASE
Section: collaboration
Priority: extra
Architecture: i386
Depends:
Suggests:
Essential: no
Installed-Size: 14 MB
Maintainer: Agora Systems S.A. [agora-2000@agora-2000.com]
Provides: vCodecs
Description: video codecs wrapper
EOF

#
# debian-bynary
#
echo 2.0 > $PKG_DIR/DEBIAN/debian-binary

#
# Permisos
#
sudo /bin/chown -R root.bin $PKG_DIR
sudo /bin/chmod -R 755      $PKG_DIR

#
# Crear el paquete
#
echo "dpkg -b $PKG_DIR $DEB_DIR/$DEBFILENAME"
dpkg -b $PKG_DIR $DEB_DIR/$DEBFILENAME

