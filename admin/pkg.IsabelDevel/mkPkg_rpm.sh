#! /bin/sh



#
# spec file
#
cat > /tmp/$PKG_NAME-$VERSION.$$.spec << EOF
Summary: Dependencies to compile Isabel
Name: $PKG_NAME 
Version: $VERSION
Release: $RELEASE
License: Agora Systems S.A. Isabel License
BuildRoot: $PKG_DIR
Group: Applications/Multimedia
Requires: unzip, zip, m4, IsabelExtras, xorg-x11-Xvnc
#, sed

%description
Dependencies to compile Isabel.

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
# nota: no se usar el fichero rcfile porque da errores
#
#rpm --rcfile etc/rpmrc -bb $PKG_NAME-$VERSION.spec 
which rpmbuild
if [ "$?" -eq "0" ] ; then
   rpmbuild -bb --rmspec --target i586 /tmp/$PKG_NAME-$VERSION.$$.spec
else
   rpm -bb --rmspec --target i586 /tmp/$PKG_NAME-$VERSION.$$.spec
fi

