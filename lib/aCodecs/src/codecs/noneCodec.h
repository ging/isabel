#ifndef __None_H__
#define __None_H__

#include <aCodecs/codecs.h>

aCoder_t *noneNewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *noneNewDecoder(int fmt, int sampleRate);

#endif
