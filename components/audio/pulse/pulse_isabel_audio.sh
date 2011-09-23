#!/bin/sh

#
# Pulse
#
# Usamos pacmd para consultar y poner el volumen de la fuente de audio de la que esta capturando isabel_audio.
#

#
# Usage
#
Usage()
{
   echo "Usage:"
   echo "    $0         Return the capture volume of the source used by isabel_audio."
   echo "    $0 <VOL>   Set the capture volume of the source used by isabel_audio."
   echo ""
   echo "    The returned value and VOL are integers between 0 and 100."
   exit
}


#
# Listo los source-outputs del pulse y busco el que tiene como cliente a isabel_audio.
# Si no lo encuentro devuelvo ""
# Si lo encuentro, devuelve el indice de su source.
#  
# La source es la entrada de audio que esta usando el source-output.
#
get_isabel_audio_source()
{
   echo "list-source-outputs" | \
   pacmd | \
   awk 'BEGIN { last_source_found = ""
                isabel_audio_found = 0
              }
        /^\W*index:/  { if (isabel_audio_found != 0) exit
                        last_source_found = ""
                      }
        /^\W*source:/ { last_source_found = $2
                      }
        /^\W*client:.*\[isabel_audio\]/ {
                       isabel_audio_found = 1
                      }
        END { print last_source_found
            }
      '
}

#
# Pone el volumen de la source de la que esta capturando isabel_audio.
#
# Argumentos: 
#   $1 - El valor del volumen a programar.
#        Debe ser un valor entre 0 y 100
set_isabel_audio_volume()
{
   _vol=$1
   _source=$(get_isabel_audio_source)

   test -z "$_vol" && exit 1

   test $_vol -lt  0   &&  _vol=0 
   test $_vol -gt 100  &&  _vol=100 

   test -z "$_source" && exit 1

   _abs_vol=$(echo "scale = 0 ; $_vol * 65535 / 100 " | bc)

   echo "set-source-volume $_source $_abs_vol" | pacmd > /dev/null
}


#
# Devuelve el volumen actual de la source de la que esta capturando isabel_audio.
#
# Devuelve un valor entre 0 y 100
#
get_isabel_audio_volume()
{
   _source=$(get_isabel_audio_source)

   test -z "$_source" && return 0

   echo "list-sources" | \
   pacmd | \
   awk -v isabel_source=$_source \
        'BEGIN { 
                 source_found = 0
                 volumen = 0
               }
        /^\W*index:/ { 
                 if ($2 == isabel_source) {
                     source_found = 1
                 }
               }
        /^\W*\*\Windex:/ { 
                 if ($3 == isabel_source) {
                     source_found = 1
                 }
               }
        /^\W*volume:/ { 
                  if (source_found != 0) {
                     volumen = $3
                     exit
                  }
               }
        END { 
              print strtonum(volumen)
            }
      '
}

#
# MAIN
#

test "$1" = "-h" && Usage

test "$1" && set_isabel_audio_volume $1 || get_isabel_audio_volume

exit 0
