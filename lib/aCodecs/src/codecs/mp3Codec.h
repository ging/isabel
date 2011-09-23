#ifndef __MP3_H__
#define __MP3_H__

#include <aCodecs/codecs.h>

aCoder_t *mp3NewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *mp3NewDecoder(int fmt, int sampleRate);

#endif
