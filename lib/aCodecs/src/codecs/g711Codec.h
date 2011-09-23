#ifndef __G711_H__
#define __G711_H__

#include <aCodecs/codecs.h>

aCoder_t *g711NewCoder(int fmt, int sampleRate, int bandwidth);

aDecoder_t *g711NewDecoder(int fmt, int sampleRate);

#endif
