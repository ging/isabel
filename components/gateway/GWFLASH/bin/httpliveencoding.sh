#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/ffmpeg/lib

# Video capture data
FRAMERATE=10
CAPTUREW=1024
CAPTUREH=768
CAPTUREFRAMESIZE="$CAPTUREW"x"$CAPTUREH"
CAPTUREDISPLAY=$2 #0.0
VIDEOBITRATE=$3 #300
BUFSIZE=$VIDEOBITRATE

# Audio capture data
AUDIOINPORT=$5 #5004
AUDIOINADDR=$4 #"0.0.0.0"
AUDIOINCODEC=103
AUDIOINSDPFILE="/tmp/audio-$RANDOM.sdp"
AUDIOOUTBITRATE=$6 #32
AUDIOOUTCODEC=libfaac
AUDIOOUTCODECRATE=44100
AUDIOOUTCHANNELS=1

# General capture and output params
VIDEOOUTFRAMESIZE=$CAPTUREFRAMESIZE
ASPECTRATIO="4:3"
MAXBITRATE="$((VIDEOBITRATE+AUDIOOUTBITRATE))"
BITRATETOLERANCE=$MAXBITRATE
OUTPIPENAME="/tmp/pipe$RANDOM.ts"
FFMPEGPATH=/usr/local/ffmpeg/bin/ffmpeg
SEGMENTERPATH=/usr/local/segmenter/segmenter
FFMPEGPIDFILE=/tmp/ffmpeg.pid
SEGMENTERPIDFILE=/tmp/segmenter.pid
LOCKFILE=/tmp/httplive.lock

# Segmenter params
SEGMENTDURATION=5
SEGMENTPREFIX=$7 #stream
PLAYLISTFILE=$SEGMENTPREFIX.m3u8
#WEBSERVER="http://$(/sbin/ifconfig eth0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}')"/
WEBSERVER=$8
MAXSEGMENTS=10
OUTPUTPATH=$9 #"/var/www"

case "$1" in
start)
	if [ -e $LOCKFILE ]; then
		echo "HTTP Live Encoding seems to be running, lock file $LOCKFILE is present"
		exit 1
	else
        	touch $AUDIOINSDPFILE
		echo "v=0" > $AUDIOINSDPFILE
		echo "s=HTTPLiveStreamingAudio" >> $AUDIOINSDPFILE
		echo "c=IN IP4 $AUDIOINADDR/127" >> $AUDIOINSDPFILE
		echo "o=- 1150320775 1150320775 IN IP4 localhost" >> $AUDIOINSDPFILE
		echo "t=0 0" >> $AUDIOINSDPFILE
		echo "m=audio $AUDIOINPORT RTP/AVP $AUDIOINCODEC" >> $AUDIOINSDPFILE

		case "$AUDIOINCODEC" in
		103)
			AUDIOINCODECNAME="PCMU"
			AUDIOINCODECRATE=16000
			AUDIOINCODECCHANNELS=1
			;;
		*)
			echo "Only audio codec 0 is supported"
			exit 1
			;;
		esac
		echo "a=rtpmap:$AUDIOINCODEC $AUDIOINCODECNAME/$AUDIOINCODECRATE/$AUDIOINCODECCHANNELS" >> $AUDIOINSDPFILE

		if [ ! -e $OUTPIPENAME ]; then
			mkfifo $OUTPIPENAME
		fi
		$FFMPEGPATH -f x11grab -r $FRAMERATE -s $CAPTUREFRAMESIZE -i $CAPTUREDISPLAY -f sdp -i $AUDIOINSDPFILE -f mpegts -acodec $AUDIOOUTCODEC -ar $AUDIOOUTCODECRATE -ac $AUDIOOUTCHANNELS -ab "$AUDIOOUTBITRATE"k -s $VIDEOOUTFRAMESIZE -vcodec libx264 -b "$VIDEOBITRATE"k -flags +loop -cmp +chroma -partitions +parti4x4+partp8x8+partb8x8 -subq 5 -trellis 1 -refs 1 -coder 0 -me_range 16 -keyint_min 25 -sc_threshold 40 -i_qfactor 0.71 -bt "$BITRATETOLERANCE"k -maxrate "$MAXBITRATE"k -bufsize "$BUFSIZE"k -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.6 -qmin 10 -qmax 51 -qdiff 4 -level 30 -aspect $ASPECTRATIO -g 30 -async 2 -y $OUTPIPENAME &
		echo $! > $FFMPEGPIDFILE
		cd $OUTPUTPATH
		$SEGMENTERPATH - $SEGMENTDURATION $SEGMENTPREFIX $PLAYLISTFILE $WEBSERVER $MAXSEGMENTS < $OUTPIPENAME &
		echo $! > $SEGMENTERPIDFILE
		touch $LOCKFILE
		echo $AUDIOINSDPFILE $OUTPIPENAME > $LOCKFILE
	fi
	;;

stop)
	if [ ! -e $LOCKFILE ]; then
		echo "HTTP Live Encoding not running?"
	else
		rm -f $(cat $LOCKFILE)
	fi
	rm -f $LOCKFILE
	if [ -e $FFMPEGPIDFILE ]; then
		kill -9 $(cat $FFMPEGPIDFILE) > /dev/null 2>&1
	fi
	if [ -e $SEGMENTERPIDFILE ]; then
		kill -9 $(cat $SEGMENTERPIDFILE) > /dev/null 2>&1
	fi
	rm -f $FFMPEGPIDFILE
	rm -f $SEGMENTERPIDFILE
	;;

restart)
	rm -f $(cat $LOCKFILE)
	rm -f $LOCKFILE
	if [ -e $FFMPEGPIDFILE ]; then
		kill -9 $(cat $FFMPEGPIDFILE) > /dev/null 2>&1
		rm -f $FFMPEGPIDFILE
	fi
	if [ -e $SEGMENTERPIDFILE ]; then
		kill -9 $(cat $SEGMENTERPIDFILE) > /dev/null 2>&1
		rm -f $SEGMENTERPIDFILE
	fi
       	touch $AUDIOINSDPFILE
	echo "v=0" > $AUDIOINSDPFILE
	echo "s=HTTPLiveStreamingAudio" >> $AUDIOINSDPFILE
	echo "c=IN IP4 $AUDIOINADDR/127" >> $AUDIOINSDPFILE
	echo "o=- 1150320775 1150320775 IN IP4 localhost" >> $AUDIOINSDPFILE
	echo "t=0 0" >> $AUDIOINSDPFILE
	echo "m=audio $AUDIOINPORT RTP/AVP $AUDIOINCODEC" >> $AUDIOINSDPFILE

	case "$AUDIOINCODEC" in
	0)
		AUDIOINCODECNAME="PCMU"
		AUDIOINCODECRATE=8000
		AUDIOINCODECCHANNELS=1
		;;
	*)
		echo "Only audio codec 0 is supported"
		exit 1
		;;
	esac
	echo "a=rtpmap:$AUDIOINCODEC $AUDIOINCODECNAME/$AUDIOINCODECRATE/$AUDIOINCODECCHANNELS" >> $AUDIOINSDPFILE

	if [ ! -e $OUTPIPENAME ]; then
		mkfifo $OUTPIPENAME
	fi
	$FFMPEGPATH -f x11grab -r $FRAMERATE -s $CAPTUREFRAMESIZE -i $CAPTUREDISPLAY -f sdp -i $AUDIOINSDPFILE -f mpegts -acodec $AUDIOOUTCODEC -ar $AUDIOOUTCODECRATE -ac $AUDIOOUTCHANNELS -ab "$AUDIOOUTBITRATE"k -s $VIDEOOUTFRAMESIZE -vcodec libx264 -b "$VIDEOBITRATE"k -flags +loop -cmp +chroma -partitions +parti4x4+partp8x8+partb8x8 -subq 5 -trellis 1 -refs 1 -coder 0 -me_range 16 -keyint_min 25 -sc_threshold 40 -i_qfactor 0.71 -bt "$BITRATETOLERANCE"k -maxrate "$MAXBITRATE"k -bufsize "$BUFSIZE"k -rc_eq 'blurCplx^(1-qComp)' -qcomp 0.6 -qmin 10 -qmax 51 -qdiff 4 -level 30 -aspect $ASPECTRATIO -g 30 -async 2 -y $OUTPIPENAME &
	echo $! > $FFMPEGPIDFILE
	cd $OUTPUTPATH
	$SEGMENTERPATH - $SEGMENTDURATION $SEGMENTPREFIX $PLAYLISTFILE $WEBSERVER $MAXSEGMENTS < $OUTPIPENAME &
	echo $! > $SEGMENTERPIDFILE
	touch $LOCKFILE
	echo $AUDIOINSDPFILE $OUTPIPENAME > $LOCKFILE
	;;

*)
	echo "Usage: $0 {start|stop|restart}"
esac

exit 0
