#ifndef __SPEEX_H__
#define __SPEEX_H__

#include <aCodecs/codecs.h>

aCoder_t *speexNewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *speexNewDecoder(int fmt, int sampleRate);

#endif
