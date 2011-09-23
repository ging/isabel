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

echo "---------------Test3----------------------"
echo "Prueba de mensajes sincronizados para el SeCo"
echo "----En el archivo README se encuentra el ESQUEMA" 
echo "de esta prueba y su explicación----"


ISABEL_DIR=/usr/local/isabel
SECOJAR=$ISABEL_DIR/libexec/isabel_seco.jar
LIBJAR=$ISABEL_DIR/libexec/isabel_lib.jar
SECOTESTJAR=$ISABEL_DIR/libexec/isabel_seco_tests.jar
JARS=${SECOJAR}:${LIBJAR}:${SECOTESTJAR}:


if [ $# -gt 1 ] || [ "$1" = "-h" ]; then 
    echo "Usage:"
    echo "   $0 [<logFile>]"
    exit 1
fi


if [ "$#" == 1 ] ; then 

echo "Using log file: " $1
xterm -e sh -c "java -Djava.util.logging.config.file=/home/jcvalle/.isabel/config/logging.properties -cp $JARS isabel.seco.server.SeCo ; echo fin; sleep 1000;" & 
xterm -e sh -c "java -Djava.util.logging.config.file=/home/jcvalle/.isabel/config/logging.properties -cp $JARS isabel.seco.server.SeCo -ctrlport 51012 -dataport 53024 localhost; echo fin; sleep 1000;" &  
xterm -e sh -c "java -Djava.util.logging.config.file=/home/jcvalle/.isabel/config/logging.properties -cp $JARS isabel.seco.server.SeCo -ctrlport 51013 -dataport 53025 localhost; echo fin; sleep 1000;" &  
xterm -e sh -c "sleep 3; java -Djava.util.logging.config.file=/home/jcvalle/.isabel/config/logging.properties -cp $JARS isabel.seco.tests.test3.StartAplications $1; echo fin; sleep 1000;" &

else 
	
echo "Using log file: ResultadosTest3.txt"
xterm -e sh -c "java -cp $JARS isabel.seco.server.SeCo ; echo fin; sleep 1000;" & 
xterm -e sh -c "java -cp $JARS isabel.seco.server.SeCo -ctrlport 51012 -dataport 53024 localhost; echo fin; sleep 1000;" &  
xterm -e sh -c "java -cp $JARS isabel.seco.server.SeCo -ctrlport 51013 -dataport 53025 localhost; echo fin; sleep 1000;" &  
xterm -e sh -c "sleep 3; java -cp $JARS isabel.seco.tests.test3.StartAplications; echo fin; sleep 1000;" &

fi

    echo "JARS=" $JARS

