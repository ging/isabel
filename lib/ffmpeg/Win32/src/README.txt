This folder contains all files required to compile under Visual Studio 2008.

You can find:

1) ffmpeg
  - directory ffmpeg/
  - code from version of March 4th, 2011 (2011-03-04), adapted to compile under VS 2008
    (which is not C99 compliant, thus the changes)
  - includes config.h and libavutil/avconfig.h

2) files required by MS Visual Studio 2008:
  - directory msvc_miss
  - stdint.h
  - inttypes.h
  - msvc_miss.c|h for missing funtions
  - poll (not really used, but may be needed if HAVE_POLL_H
