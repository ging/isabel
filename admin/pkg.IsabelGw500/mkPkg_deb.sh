#! /bin/sh

#
# Check variables
#
if [ -z "$PKG_DIR" ] ; then echo "PKG_DIR undefined" ; exit ; fi
if [ -z "$PKG_INFO_DIR" ] ; then echo "PKG_INFO_DIR undefined" ; exit ; fi

#
# Completando estructura de directorios necesarios para los paquetes DEB
#
mkdir $PKG_DIR/DEBIAN

#
# Documentacion
#
mkdir -p $PKG_DIR/usr/share/doc/isabelgw500
echo "Copyright: Agora Systems S.A." > $PKG_DIR/usr/share/doc/isabelgw500/copyright
touch $PKG_DIR/usr/share/doc/isabelgw500/changelog.Debian
gzip $PKG_DIR/usr/share/doc/isabelgw500/changelog.Debian




#
# Control
#
cat > $PKG_DIR/DEBIAN/control << EOF
Package: isabelgw500
Version: $VERSION.$RELEASE
Section: collaboration
Priority: extra
Architecture: i386
Pre-Depends: sun-java6-jre
Depends: isabel500 (>=$VERSION)
Essential: no
Installed-Size: 22 MB
Maintainer: Agora Systems S.A. [agora-2000@agora-2000.com]
Provides: isabelgw500
Description: Isabel CSCW Application - Gateway.

EOF

#
# Scripts
#
cp $PKG_INFO_DIR/postinst $PKG_DIR/DEBIAN/.
cp $PKG_INFO_DIR/postrm   $PKG_DIR/DEBIAN/.
cp $PKG_INFO_DIR/prerm    $PKG_DIR/DEBIAN/.
cp $PKG_INFO_DIR/preinst  $PKG_DIR/DEBIAN/.


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
dpkg -b $PKG_DIR $DEB_DIR/$DEBFILENAME

