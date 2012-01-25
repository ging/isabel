#!/bin/sh

export ISAOS=$(uname)
if [ "$ISAOS" != "Linux" ] ; then
  echo "Host:`hostname` with $ISAOS cannot compile for Linux"
  exit 1
fi

export LINUX_DISTRIB=""
grep Ubuntu /etc/lsb-release > /dev/null
if [ "$?" -eq 0 ] ; then
    LINUX_DISTRIB=$(/usr/bin/awk -F = '
                       /DISTRIB_ID/        {d_id = $2} 
                       /DISTRIB_CODENAME/  {d_codename = $2}
                       END {print d_id "_" d_codename}' /etc/lsb-release)
else
    echo "Unsupported Linux distribution."
    exit 1
fi


export HAVE_LIBV4L=""
if [ "$LINUX_DISTRIB" = "Ubuntu_hardy" ] ; then
   HAVE_LIBV4L=""
else
   HAVE_LIBV4L="libv4l-dev"
fi

export LIBXVID=""
# renamed since Ubuntu lucid
if [ "$LINUX_DISTRIB" = "Ubuntu_karmic" ] ; then
   LIBXVID="libxvidcore4-dev"
else
   LIBXVID="libxvidcore-dev"
fi


apt-get install -y itcl3 itk3 iwidgets4 
apt-get install -y xutils-dev make gcc g++  libc6-dev libx11-dev nasm yasm m4
apt-get install -y libxmu-dev libxaw7-dev zlib1g-dev libjpeg62-dev libssl-dev

if [ "$LINUX_DISTRIB" = "Ubuntu_oneiric" ] ; then
   add-apt-repository -y ppa:ferramroberto/java
   apt-get update -y
fi
apt-get install -y sun-java6-jdk

apt-get install -y libbogl-dev libpng12-dev libtiff4-dev openssh-server liblcms1-dev libasound2-dev libgl1-mesa-dev libglu1-mesa-dev
apt-get install -y freeglut3-dev libxfixes-dev libxvmc-dev libgsm1-dev libmp3lame-dev libspeex-dev libopencore-amrnb-dev libopencore-amrwb-dev libx264-dev portaudio19-dev

apt-get install -y  $LIBXVID $HAVE_LIBV4L


exit 0




