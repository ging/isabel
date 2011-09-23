#! /bin/sh



#
# spec file
#
cat > /tmp/$PKG_NAME-$VERSION.$$.spec << EOF
Summary: Isabel CSCW Application
Name: $PKG_NAME 
Version: $VERSION
Release: $RELEASE
License: Agora Systems S.A. Isabel License
BuildRoot: $PKG_DIR
Group: Applications/Multimedia
Requires: unzip, zip, m4, IsabelExtras, xorg-x11-Xvnc, openvpn
#, sed

%description
Isabel CSCW Application.
Configurable Computer Supported Cooperative Work application
supporting synchronous group collaboration over IP.
It includes specific service configurations for project or corporate
telemeetings, distributed classrooms, distributed congresses or
workshops, etc.

ISABEL sessions support effectively multipoint configurations
with over 30 interactive points. In each session all participants can
share the relevant videos, audios, presentations, whiteboard,
applications, etc in easily controlled configurations.

More information available in:
http://www.agora-2000.com

%post
/$ISA_RELDIR/bin/lin-install

%files
%attr(755,root,bin)  /$ISA_RELDIR
%attr(755,root,bin)  /usr/local/bin/isabel
%attr(755,root,bin)  /usr/local/bin/xedl_launcher
%attr(755,root,bin)  /usr/local/bin/icto_launcher
%attr(755,root,bin)  /usr/local/bin/isabel_launch
%attr(755,root,bin)  /usr/local/bin/fp
%attr(755,root,bin)  /etc/xdg/menus/applications-merged
%attr(755,root,bin)  /etc/xdg/autostart
%attr(755,root,bin)  /opt/kde3/share/autostart
%attr(755,root,bin)  /usr/share/applications
%attr(755,root,bin)  /usr/share/desktop-directories

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

