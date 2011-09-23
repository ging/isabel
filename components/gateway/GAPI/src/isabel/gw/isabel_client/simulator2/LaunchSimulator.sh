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


export ISABEL_DIR=/usr/local/isabel

# isabel environment variables
. $ISABEL_DIR/bin/isabel.environ.sh

LIBJAR=$ISABEL_DIR/libexec/isabel_lib.jar
SECOJAR=$ISABEL_DIR/libexec/isabel_seco.jar
GAPIJAR=$ISABEL_DIR/libexec/isabel_gapi.jar

JARS=$LIBJAR:$SECOJAR:$GAPIJAR

java -cp $JARS \
    -Djava.util.logging.config.file=$ISABEL_CONFIG_DIR/logging.properties \
    isabel.gw.isabel_client.simulator2.Simulator

