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
# Descripcion: Script para ejecutar una aplicación que lanza
#              un master en local o una sesion de prueba
# Autor: Diego Andres Acosta
# Fecha: 08/11/2005
#-----------------------------------------------------
# isabel directory.
#-----------------------------------------------------
if [ -z "$ISABEL_DIR" ] ; then
  ISABEL_DIR=/usr/local/isabel
  export ISABEL_DIR
fi

#-----------------------------------------------------
# isabel common code
#-----------------------------------------------------
. $ISABEL_DIR/bin/isabel.common.sh

#-----------------------------------------------------
# isabel environment variables
#-----------------------------------------------------
. $ISABEL_DIR/bin/isabel.environ.sh

#-----------------------------------------------------
# Checking ISABEL requirements
#-----------------------------------------------------
. $ISABEL_DIR/bin/isabel.requires.sh

#-----------------------------------------------------
#            Default Media Server
#-----------------------------------------------------

DEFAULT_XEDL=""

if [ -z "$1" ]; then
   echo Starting gxedl without xedl file
else
  echo Startin with the xedl file:$1
  DEFAULT_XEDL="-xedlFile $1"
fi

#-----------------------------------------------------
# Launching the recorder.
#-----------------------------------------------------

XMLRPCJARS=$ISABEL_DIR/libexec/xmlrpc/commons-logging-1.1.jar:$ISABEL_DIR/libexec/xmlrpc/ws-commons-util-1.0.2.jar:$ISABEL_DIR/libexec/xmlrpc/xmlrpc-client-3.1.jar:$ISABEL_DIR/libexec/xmlrpc/xmlrpc-common-3.1.jar:$ISABEL_DIR/libexec/xmlrpc/xmlrpc-server-3.1.jar

jargxedl=$ISABEL_DIR/libexec/isabel_gxedl.jar
jarlib=$XMLRPCJARS:$ISABEL_DIR/libexec/isabel_xlimservices.jar:$ISABEL_DIR/libexec/xedl.jar:$ISABEL_DIR/lib/images/xlim/:$ISABEL_DIR/libexec/isabel_xlim.jar:$ISABEL_DIR/lib/:$ISABEL_DIR/lib/images/:$ISABEL_DIR/libexec/isabel_lib.jar:$ISABEL_DIR/libexec/isabel_tunnel.jar

$ISAJAVA -cp $jarlib \
            -Djava.util.logging.config.file=$ISABEL_CONFIG_DIR/launcher/gxedl_log.prop \
            -Disabel.dir=$ISABEL_DIR/ \
            -Disabel.tmp.dir=$ISABEL_TMP_DIR \
            -Dprior.config.file=$ISABEL_DIR/lib/xlimconfig/priorxedl.cfg \
            -Dvpn.scripts.dir=$ISABEL_DIR/bin \
            -Dvpn.lib.dir=$ISABEL_DIR/lib/vpn \
            services.isabel.services.gxedl.editorUtils.SessionPanel $DEFAULT_XEDL  &
   echo $! > $ISABEL_TMP_DIR/gxedl.pid

#-----------------------------------------------------
# Clean up.
#-----------------------------------------------------

exit 0

