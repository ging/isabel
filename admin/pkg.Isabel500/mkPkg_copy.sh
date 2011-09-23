#! /bin/sh

#
# Check variables
#
if [ -z "$SRC_DIR" ] ; then echo "SRC_DIR undefined" ; exit ; fi
if [ -z "$PKG_DIR" ] ; then echo "PKG_DIR undefined" ; exit ; fi
if [ -z "$ISA_RELDIR" ] ; then echo "ISA_RELDIR undefined" ; exit ; fi
if [ -z "$DISTRIB_DIR" ] ; then echo "DISTRIB_DIR undefined" ; exit ; fi

if [ -z "$VERSION" ] ; then echo "VERSION undefined" ; exit ; fi
if [ -z "$RELEASE" ] ; then echo "RELEASE undefined" ; exit ; fi

#
# Creando estructura de directorios del paquete
#
mkdir -p $PKG_DIR/$ISA_RELDIR/bin
mkdir -p $PKG_DIR/$ISA_RELDIR/lib
mkdir -p $PKG_DIR/$ISA_RELDIR/libexec
mkdir -p $PKG_DIR/$ISA_RELDIR/test
mkdir -p $PKG_DIR/$ISA_RELDIR/doc
mkdir -p $PKG_DIR/$ISA_RELDIR/env

#
# Copiando ficheros
#

cp -r $DISTRIB_DIR/isabelcore/*      $PKG_DIR/$ISA_RELDIR/.

cat $DISTRIB_DIR/isabelcore/lib/sessions/localtest.xedl | \
    sed "s/%%ISABEL_VERSION%%/$VERSION/" | \
    sed "s/%%ISABEL_RELEASE%%/$RELEASE/" > $PKG_DIR/$ISA_RELDIR/lib/sessions/localtest.xedl

#
# Creando fichero de version
#
echo "VERSION $VERSION \nRELEASE $RELEASE"  > $PKG_DIR/$ISA_RELDIR/lib/version.Isabel

