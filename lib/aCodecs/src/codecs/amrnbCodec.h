#ifndef __ARM_H__
#define __ARM_H__

#include <aCodecs/codecs.h>

aCoder_t *amrnbNewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *amrnbNewDecoder(int fmt, int sampleRate);

#endif

