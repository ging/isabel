#ifndef __AMR_H__
#define __AMR_H__

#include <aCodecs/codecs.h>

aCoder_t *amrnbNewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *amrnbNewDecoder(int fmt, int sampleRate);

#endif

