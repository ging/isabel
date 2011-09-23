#!/bin/bash

# General settings
SLEEPTIME=4
XDISPLAY=:33
RFBPORT=53018
GWPASSFILE=/usr/local/isabel/lib/sipgateway.passwd

# Process variables
XVFBPIDFILE=/tmp/xvfb.pid
X11VNCPIDFILE=/tmp/x11vnc.pid
LOCKFILE=/tmp/startxvnc.lock
case "$1" in
start)
	Xvfb $XDISPLAY -screen 0 1024x768x24 -shmem > /dev/null 2>&1 &
	echo $! > $XVFBPIDFILE
	sleep $SLEEPTIME
	x11vnc -rfbport $RFBPORT -display $XDISPLAY -rfbauth $GWPASSFILE -forever -shared > /dev/null 2>&1 &
	echo $! > $X11VNCPIDFILE
	# We hide the X cursor, which is kind of 'elegant'
	unclutter -display $XDISPLAY -root -idle 1 > /dev/null 2>&1 &
	;;

stop)
	if [ -e $X11VNCPIDFILE ]; then
		kill -9 $(cat $X11VNCPIDFILE)
		rm -f $X11VNCPIDFILE
	fi
	if [ -e $XVFBPIDFILE ]; then
		kill -9 $(cat $XVFBPIDFILE)
		rm -f $XVFBPIDFILE
	fi
	;;

*)
        echo "Usage: $0 {start|stop}"
esac

exit 0
