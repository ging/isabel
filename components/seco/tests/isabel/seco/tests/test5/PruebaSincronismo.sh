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

echo "---------------PruebaSincronismo----------------------"
echo "Prueba de rendimiento del SeCo"
echo "Entre varios clientes y receptores con mensajes de sincronismo"

rm -f /tmp/matarClientes.pids
rm -f /tmp/matarReceptores.pids

ISABEL_DIR=/usr/local/isabel
SECOJAR=$ISABEL_DIR/libexec/isabel_seco.jar
LIBJAR=$ISABEL_DIR/libexec/isabel_lib.jar
SECOTESTJAR=$ISABEL_DIR/libexec/isabel_seco_tests.jar
JARS=${SECOJAR}:${LIBJAR}:${SECOTESTJAR}:



if [ "$#" == 2 ] ; then 
echo "Clients: " $1
echo "Receivers: " $2
CLIENTES=$1
RECEPTORES=$2
fi
if [ "$#" == 0 ] ; then
echo "Only using one client and one receiver"
CLIENTES=1
RECEPTORES=1
fi


echo "JARS=" $JARS
xterm -e sh -c "java -cp $JARS  -agentlib:hprof=cpu=samples,depth=7,file=analisis,interval=1 isabel.seco.server.SeCo" &
sleep 2;  
cont=1
while true ; do
   java -cp $JARS isabel.seco.tests.test5.PruebaReceptorSincronismo $cont &
   echo $! >> /tmp/matarReceptores.pids
   if [ "$cont" = "$RECEPTORES" ] ; then
       break
   fi
   cont=$(($cont+1))
done
sleep 4;
cont=1
while true ; do
   java -cp $JARS isabel.seco.tests.test5.PruebaClienteSincronismo $cont &
   echo $! >> /tmp/matarClientes.pids
   if [ "$cont" = "$CLIENTES" ] ; then
       break
   fi
   cont=$(($cont+1))
done

read
kill $(cat /tmp/matarReceptores.pids)

echo "Precesos Matados"

