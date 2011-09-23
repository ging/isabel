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

#
# comprobacion variables
#
if [ -z "$PKG_DIR" ] ; then echo "PKG_DIR undefined" ; exit ; fi
if [ -z "$VCODECS_RELDIR" ] ; then echo "VCODECS_RELDIR undefined" ; exit ; fi
if [ -z "$DISTRIB_DIR" ] ; then echo "DISTRIB_DIR undefined" ; exit ; fi


#
# creacion estructura de directorios del paquete
#
sudo rm -rf $PKG_DIR
mkdir -p $PKG_DIR/$VCODECS_RELDIR/lib
mkdir -p $PKG_DIR/$VCODECS_RELDIR/include/vCodecs
mkdir -p $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib/codecs # otros includes
mkdir -p $PKG_DIR/usr/share/doc/vCodecs

#
# copia de los ficheros
#

#libs
cp $VCODECS_DIR/build/h263/libH263.a		$PKG_DIR/$VCODECS_RELDIR/lib
cp $VCODECS_DIR/build/xvid/libxvidcore.a	$PKG_DIR/$VCODECS_RELDIR/lib
cp $VCODECS_DIR/build/jpeg/libJPEG.a		$PKG_DIR/$VCODECS_RELDIR/lib
cp $VCODECS_DIR/build/vCodecs/libvcodecs.a	$PKG_DIR/$VCODECS_RELDIR/lib/libvcodecs.a
cp $VCODECS_DIR/build/libavcodec/libavcodec.a   $PKG_DIR/$VCODECS_RELDIR/lib/libh264dec.a
cp $VCODECS_DIR/build/libavcodec/libavutil.a    $PKG_DIR/$VCODECS_RELDIR/lib/libvcodecutil.a
cp $VCODECS_DIR/build/h264/libx264.a            $PKG_DIR/$VCODECS_RELDIR/lib/libh264enc.a

#headers 
cp $VCODECS_DIR/include/vCodecs/codec.h		$PKG_DIR/$VCODECS_RELDIR/include/vCodecs/codec.h
cp $VCODECS_DIR/include/vCodecs/codecs.h	$PKG_DIR/$VCODECS_RELDIR/include/vCodecs/codecs.h
cp $VCODECS_DIR/include/vCodecs/payloads.h	$PKG_DIR/$VCODECS_RELDIR/include/vCodecs/payloads.h
cp $VCODECS_DIR/include/vCodecs/vUtils.h	$PKG_DIR/$VCODECS_RELDIR/include/vCodecs/vUtils.h
cp $VCODECS_DIR/include/vCodecs/general.h       $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/general.h
cp $VCODECS_DIR/include/vCodecs/hostArch.h      $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/hostArch.h
cp $VCODECS_DIR/include/vCodecs/colorspace.h    $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/colorspace.h
cp $VCODECS_DIR/lib/codecs/H263Codec.h          $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib/codecs/H263Codec.h
cp $VCODECS_DIR/lib/codecs/MJPEGCodec.h         $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib/codecs/MJPEGCodec.h
cp $VCODECS_DIR/lib/codecs/H264Codec.h          $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib/codecs/H264Codec.h
cp $VCODECS_DIR/lib/codecs/tables.h             $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib/codecs/tables.h
cp $VCODECS_DIR/lib/codecs/XVIDCodec.h          $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib/codecs/XVIDCodec.h


#doc
cp $VCODECS_DIR/README.txt   		        $PKG_DIR/usr/share/doc/vCodecs
cp $VCODECS_DIR/examples/test.cxx               $PKG_DIR/usr/share/doc/vCodecs

#permissions

chmod -R 644 $PKG_DIR
chmod 755 $PKG_DIR
chmod 755 $PKG_DIR/$VCODECS_RELDIR
chmod 755 $PKG_DIR/$VCODECS_RELDIR/lib
chmod 755 $PKG_DIR/$VCODECS_RELDIR/include
chmod 755 $PKG_DIR/$VCODECS_RELDIR/include/vCodecs
chmod 755 $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib
chmod 755 $PKG_DIR/$VCODECS_RELDIR/include/vCodecs/lib/codecs
chmod 755 $PKG_DIR/usr
chmod 755 $PKG_DIR/usr/share
chmod 755 $PKG_DIR/usr/share/doc
chmod 755 $PKG_DIR/usr/share/doc/vCodecs


