#ifndef __G728_H__
#define __G728_H__

#include <aCodecs/codecs.h>

aCoder_t *g728NewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *g728NewDecoder(int fmt, int sampleRate);

#endif
