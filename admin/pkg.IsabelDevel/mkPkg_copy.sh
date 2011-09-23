#! /bin/sh

#
# Check variables
#
if [ -z "$PKG_DIR" ] ; then echo "PKG_DIR undefined" ; exit ; fi
if [ -z "$ISA_RELDIR" ] ; then echo "ISA_RELDIR undefined" ; exit ; fi
if [ -z "$SRC_DIR" ] ; then echo "SRC_DIR undefined" ; exit ; fi
if [ -z "$DISTRIB_DIR" ] ; then echo "DISTRIB_DIR undefined" ; exit ; fi

#
# Creando estructura de directorios del paquete
#
sudo rm -rf $PKG_DIR

#
# Copiando ficheros
#

