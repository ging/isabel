#ifndef __G722_H__
#define __G722_H__

#include <aCodecs/codecs.h>

aCoder_t *g722NewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *g722NewDecoder(int fmt, int sampleRate);

#endif
