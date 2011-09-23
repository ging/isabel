#! /bin/sh

#
# Check variables
#
if [ -z "$PKG_DIR" ] ; then echo "PKG_DIR undefined" ; exit ; fi
if [ -z "$PKG_INFO_DIR" ] ; then echo "PKG_INFO_DIR undefined" ; exit ; fi

export HAVE_LIBV4L=""
if [ -z "$LINUX_DISTRIB" ] ; then
   HAVE_LIBV4L=""
else
   if [ "$LINUX_DISTRIB" = "Ubuntu_hardy" ] ; then
       HAVE_LIBV4L=""
   else
      HAVE_LIBV4L=", libv4l-dev"
   fi
fi

export LIBXVID=""
if [ -z "$LINUX_DISTRIB" ] ; then
   LIBXVID=""
else
   # renamed since Ubuntu lucid
   if [ "$LINUX_DISTRIB" = "Ubuntu_karmic" ] ; then
       LIBXVID=", libxvidcore4-dev"
   else
       LIBXVID=", libxvidcore-dev"
   fi
fi

#
# Completando estructura de directorios necesarios para los paquetes DEB
#
mkdir -p $PKG_DIR/DEBIAN

#
# Documentacion
#
mkdir -p $PKG_DIR/usr/share/doc/isabeldevel
echo "Copyright: Agora Systems S.A." > $PKG_DIR/usr/share/doc/isabeldevel/copyright
touch $PKG_DIR/usr/share/doc/isabeldevel/changelog.Debian
gzip $PKG_DIR/usr/share/doc/isabeldevel/changelog.Debian


#
# Control
#
cat > $PKG_DIR/DEBIAN/control << EOF
Package: isabeldevel
Version: $VERSION.$RELEASE
Section: collaboration
Priority: extra
Architecture: i386
Depends: itcl3, itk3, iwidgets4, xutils-dev, make, gcc (>=4.0), g++ (>=4.0), libc6-dev, libx11-dev, libxmu-dev, libxaw7-dev, zlib1g-dev, libjpeg62-dev, libssl-dev, sun-java6-jdk, libbogl-dev, libpng12-dev, libtiff4-dev, itcl3, itk3, iwidgets4, openssh-server, m4, subversion, nasm, yasm, liblcms1-dev, libasound2-dev, libgl1-mesa-dev, libglu1-mesa-dev, freeglut3-dev, libxfixes-dev, libxvmc-dev, libgsm1-dev, libmp3lame-dev, libspeex-dev, libopencore-amrnb-dev, libopencore-amrwb-dev, libx264-dev, portaudio19-dev $LIBXVID $HAVE_LIBV4L
Essential: no
Installed-Size: 1
Maintainer: Agora Systems S.A. <agora-2000@agora-2000.com>
Provides: isabeldevel
Description: Dependencies to compile isabel
EOF

#
# Scripts
#
cp $PKG_INFO_DIR/postinst $PKG_DIR/DEBIAN/.
chmod ugo+rx $PKG_DIR/DEBIAN/postinst


#
# debian-bynary
#
echo 2.0 > $PKG_DIR/DEBIAN/debian-binary

#
# Permisos
#
sudo /bin/chown -R root.root $PKG_DIR
sudo /bin/chmod -R 755       $PKG_DIR

#
# Crear el paquete
#
dpkg -b $PKG_DIR $DEB_DIR/$DEBFILENAME

