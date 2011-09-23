#ifndef __GSM_H__
#define __GSM_H__

#include <aCodecs/codecs.h>

aCoder_t *gsmNewCoder(int fmt, int sampleRate, int bandwidth);
aDecoder_t *gsmNewDecoder(int fmt, int sampleRate);

#endif
