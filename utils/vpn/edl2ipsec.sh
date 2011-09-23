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

## Parámetros de entrada:
## $1: start | stop (Inicia o para IPSEC)
## $2: Fichero xml con el XEDL
## $3: identity del NODO, definida por:
##     <identity> = [ID | DN | FQDN | SITE_ADDRESS]

#  Versión 0.4:
#     - Adaptado al paquete ipsec-tools RPM de SUSE 9.x
#  Versión 0.3:
#     - Descargamos las CRL's desde el DNS
#  Versión 0.2: 
#     - Kernel 2.6 + Racoon (KAME)
#     - Se comprueba si el identificador del sitio están en el XEDL     
#  Versión 0.1:
#     - Soporte para kernel 2.4 + FreeS/WAN
#    

## Localizacion de ficheros:
# Localizacion de la maquina virtual
# Vamos a suponer que se encuentra dentro de la
# la variable JDK_HOME
#if [ -z $JDK_HOME ] ; then
#   JREDIR=/usr/local/j2re1.4.2_01
#else   
#   JREDIR=$JDK_HOME
#fi 
JREDIR=/usr/java/j2re1.4.2_01

# Localizacion de las librerias SAXON
#SAXON=/usr/share/saxon/saxon.jar
#SAXON=/usr/local/saxon/saxon.jar
#SAXON=/usr/local/isabel/extras/libexec/saxon.jar

# Fichero para obtener los certificados por LDAP o DNS
#CERTSCRIPT=./getcert.sh
#CERTSCRIPT=/usr/local/isabel/bin/vpn_getcert.sh

if [ -z $VPN_BIN_DIR ] ; then
        VPN_BIN_DIR=/usr/local/isabel/bin
        export VPN_BIN_DIR
fi

CERTSCRIPT=$VPN_BIN_DIR/vpn_getcert.sh

# Fichero de SALIDA con la configuracion de IPSEC
IPSECCONF=/etc/racoon/setkey.conf

# Fichero en el que se almacenan los LOG's de RACOON
LOGFILE=/var/log/racoon.log

# Directorio con los CERTIFICADOS
CERTDIR=/etc/ipsec.d/certs

## Ficheros internos para las transformaciones:
#XSLPHASE1=phase1.xsl
#XSLPHASE2=phase2.xsl
#XSLPHASE3=phase3.xsl 
#XSLGETIDENTITY=get_identity.xsl
#XSLMCUMASTER=get_mcu_master.xsl

if [ -z $VPN_LIB_DIR ] ; then
	VPN_LIB_DIR=/usr/local/isabel/lib/vpn
	export VPN_LIB_DIR
fi

XSLPHASE1=$VPN_LIB_DIR/phase1.xsl
XSLPHASE2=$VPN_LIB_DIR/phase2.xsl
XSLPHASE3=$VPN_LIB_DIR/phase3.xsl 
XSLGETIDENTITY=$VPN_LIB_DIR/get_identity.xsl
XSLMCUMASTER=$VPN_LIB_DIR/get_mcu_master.xsl

## Comprobacion de los parametros del script
if [ -z "$1" ] ; then
   echo "Usage: $0 {start|stop} [edl_file] [identity]";
   exit 1;
fi

## ¿Hay que parar IPSEC? 
if [ "$1" == "stop" ] ; then
   rcracoon stop;
#######################################
#echo "Stopping ipsec..."
#killall -9 racoon;
#   setkey -F;
#   setkey -FP;
#######################################
   setkey -FP;
   exit 0;
fi

## No, comprobamos que el parametro sea START
if [ "$1" != "start" ] ; then
   echo "Usage: $0 {start|stop} [edl_file] [identity]";
   exit 1;
fi

## Comprobamos que se ha pasado el fichero con el EDL
if [ -z "$2" ] ; then
   echo "Usage: $0 {start|stop} [edl_file] [identity]";
   echo "Error: XEDL File not found";
   exit 1;
fi

## Comprobamos que se ha especificado la identidad
if [ -z "$3" ] ; then
   echo "Usage: $0 {start|stop} [edl_file] [identity]";
   echo "Error: Node identity not specified";
   exit 1;
fi

## 1. Obtenemos el DN del NODO actual
#MYIDENTITY=`$JREDIR/bin/java -jar $SAXON $2 $XSLGETIDENTITY myIdentity=$3`
MYIDENTITY=`$JREDIR/bin/java -jar xalan.jar -IN $2 -XSL $XSLGETIDENTITY -PARAM myIdentity $3
if [ "$MYIDENTITY" == "" ] ; then
   echo "Error: Site identification \"$3\" not found in XEDL";
   exit 1;
fi
   
## 2. Creamos la conexion entre el nodo actual-MCU y nodo actual-MASTER
#$JREDIR/bin/java -jar $SAXON $2 $XSLPHASE1 myIdentity=$3 certdir=$CERTDIR> $IPSECCONF
$JREDIR/bin/java -jar xalan.jar -IN $2 -XSL $XSLPHASE1 -PARAM myIdentity $3 -PARAM certdir $CERTDIR > $IPSECCONF

## 3. Creamos las conexiones entre el nodo actual y sus hijos
#$JREDIR/bin/java -jar $SAXON $2 $XSLPHASE2 myIdentity=$3 certdir=$CERTDIR>> $IPSECCONF
$JREDIR/bin/java -jar xalan.jar -IN $2 -XSL $XSLPHASE2 -PARAM myIdentity $3 -PARAM certdir $CERTDIR >> $IPSECCONF

## 4. Si el nodo actual es el master,
##    creamos las conexiones con el resto de nodos
#$JREDIR/bin/java -jar $SAXON $2 $XSLPHASE3 myIdentity=$3 certdir=$CERTDIR>> $IPSECCONF
$JREDIR/bin/java -jar xalan.jar -IN $2 -XSL $XSLPHASE3 -PARAM myIdentity $3 -PARAM certdir $CERTDIR >> $IPSECCONF

## 5. Obtenemos los certificados necesarios
echo "Obtaining certificates..."
$CERTSCRIPT "$MYIDENTITY"
if [ $? != 0 ] ; then
   echo "Error in $CERTSCRIPT";
   exit 1;
fi
echo "done!"

## 6. Activamos IPSEC (lo paramos primero)
#echo "Starting setkey..."
#setkey -F
#setkey -FP
#setkey -f $IPSECCONF
#echo "done!"
#if [ $? != 0 ]; then
#   echo "Error: Starting setkey";
#   exit 1;
#fi

## 6. Activamos RACOON (lo paramos primero)
echo "Starting IPSEC (setkey+racoon)..."
rcracoon start
#############################################
#racoon -l $LOGFILE
#############################################
echo "done! (see $LOGFILE for details)"
if [ $? != 0 ]; then
   echo "Error: Starting IPSEC (setkey+racoon)";
   exit 1;
fi
echo "done!"

## Todo correcto:
exit 0
