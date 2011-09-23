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

echo "---------------Test1----------------------"
echo "Prueba muy sencilla del funcionamiento del SeCo"
echo "Lanza dos clientes de Seco uno pregunta por la suma de"
echo "dos numeros y el otro le envia la respuesta"
echo "Tiene que estar lanzado un SeCo para que funcione"


if [ "$1" = "-h" ] ; then 
    echo "Usage:"
    echo "   $0 \ "
    echo " args[0] => SECO_HOST "
    echo "    [<remote_host> [<remote_data_port]]"
    exit 1
fi

ISABEL_DIR=/usr/local/isabel
SECOJAR=$ISABEL_DIR/libexec/isabel_seco.jar
LIBJAR=$ISABEL_DIR/libexec/isabel_lib.jar
SECOTESTJAR=$ISABEL_DIR/libexec/isabel_seco_tests.jar
JARS=${SECOJAR}:${LIBJAR}:${SECOTESTJAR}:


echo "JARS=" $JARS
xterm -e sh -c "java -cp $JARS isabel.seco.tests.test1.Aplicacion2 $*; echo fin; sleep 1000;" &  
xterm -e sh -c "sleep 1; java -cp $JARS isabel.seco.tests.test1.Aplicacion1 $*; echo fin; sleep 1000;"
