#ifndef __G726_H__
#define __G726_H__

#include <aCodecs/codecs.h>

aCoder_t *g726NewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *g726NewDecoder(int fmt, int sampleRate);

#endif


