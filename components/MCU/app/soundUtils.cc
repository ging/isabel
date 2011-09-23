/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: soundUtils.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "soundUtils.h"

double
evalPower(unsigned char *buffer, size_t len)
{
    double media = 0.0;
    double s1, s2;
    short int *p;
    double db;

    p = (short int *)buffer;
    s1 = 0.0;
    s2 = 0.0;
    for ( unsigned int i = 0 ; i < len/2 ; i++ )
    {
        s1 += (double)p[i] * (double)p[i];
        s2 += (double)p[i];
    }

    s1 /= (double)(len/2);
    s2 /= (double)(len/2);

    //   media = sqrt(s1-s2*s2);
    //   return media;
    media = (s1-s2*s2);
    db = 10.0*log10(media/1.073741e+09);

    return db < -96.0 ? -96.0 : db;
}

int
bufferAtenuator(unsigned char *buffer, size_t len, float dbs)
{
    short int *p;
    double factor;

    if (dbs > 0.0)
    {
        factor = sqrt(pow(10.0, dbs/10.0));
    }
    else
    {
        factor = 1.0/sqrt(pow(10.0, -dbs/10.0));
    }

    p = (short int *)buffer;
    for ( unsigned int i = 0 ; i < len/2 ; i++ )
    {
        p[i] = (short int) ((double)p[i] * factor);
    }

    return 1;
}

double
factorDBStoK(double dbs)
{
    return sqrt(pow(10.0, dbs/10.0));
}

int
doAGC(unsigned char *buffer, size_t len, double MAXDB, double MINDB)
{
    double db;

    db = evalPower(buffer, len);
    if (db > MAXDB) // Bajar (MAXDB - db) dbs)
    {
        bufferAtenuator(buffer, len, MAXDB - db);
    }

    if (db < MINDB) // Subir (MINDB -db) dbs)
    {
        bufferAtenuator(buffer, len, MINDB - db);
    }

    return 1;
}

int
resample(unsigned char *indata,
         int indatas,
         int inrate,
         unsigned char *outdata,
         int outdatas,
         int outrate
        )
{
    int insamples, outsamples;
    int inms;

    if (inrate == outrate)
    {
        memcpy(outdata, indata, indatas);
        return indatas;
    }

    insamples = (indatas/2);
    outsamples = (indatas*outrate/inrate)/2;
    inms = (1000*indatas)/(2*inrate);

    double inUsPerSample = (1000000.0)/(double)inrate;
    double outUsPerSample = (1000000.0)/(double)outrate;

    short *inptr = (short *)indata;
    short *outptr = (short *)outdata;

    if (outsamples*2 > outdatas)
    {
        return 0;
    }

    // mejorar calidad
    if (outsamples > insamples)
    {
        short int anterior = inptr[0];
        int k = 0;
        int n = 0;
        int i = 0;

        for (; i < insamples ; i++ )
        {
          int pasos = 0;
          while (true)
          {
              double a = (int)((i+1)*inUsPerSample);
              double b = (double)(k*outUsPerSample);
              if (b<a)
              {
                  pasos++;
                  k++;
              }else{
                  break;
              }
          }

          for (int j = 0 ; j < pasos ; j++ )
          {
              double pendiente = ((double)(inptr[i]) - (double)(anterior))/(inUsPerSample);
              outptr[n] = anterior + (short int)(pendiente*((double)(n*outUsPerSample) - (double)((i)*inUsPerSample)));
              n++;
          }
          anterior = inptr[i];
        }
        outsamples = n;
    }
    else
    {
        // Empeorar calidad
        double pasos = (double)insamples/(double)outsamples;
        int k = 1;
        outptr[0] = inptr[0];
        for (int i=1;i<insamples;i++)
        {
            if (i/pasos>k)
            {
                double a = (k*outUsPerSample) - ((i-1)*inUsPerSample);
                double pendiente = ((double)(inptr[i]) - (double)(inptr[i-1]))/(inUsPerSample);
                outptr[k] =  (short)((double)(inptr[i-1]) + (double)(pendiente*a));
                k++;
            }
        }
        outptr[outsamples] = inptr[insamples];
    }

    return outsamples*2;
}

silenceDetector_t::silenceDetector_t(void)
{
    this -> bitsPerSample = 16;
    this -> rate = 8000;
    this -> channels = 1;
    this -> msSilence = 250;
    this -> mindb = -45;
    msInSilence = 0;
}

silenceDetector_t::~silenceDetector_t(void)
{
}

void
silenceDetector_t::setParameters(int bitsPerSample,
                                 unsigned short int rate,
                                 int channels,
                                 double msSilence,
                                 double mindb
                                )
{
    this -> bitsPerSample = bitsPerSample;
    this -> rate = rate;
    this -> channels = channels;
    this -> msSilence = msSilence;
    this -> mindb = mindb;
    msInSilence = 0;
}

bool
silenceDetector_t::check(unsigned char *data, unsigned int datasize)
{
    double db = 0;
    double ms = 0;

    db = evalPower(data, datasize);
    ms = ((datasize*1000)/(rate*(bitsPerSample/8)*channels));

    // Detector de silencio
    // Si hay mas potencia de la minima, se considera que ya no hay ruido
    if ( db > mindb )
    {
        //    if ( msInSilence == 0 ) NOTIFY("Silence Break\n");
        msInSilence = 0;
    }
    else
    {
        // Si no sumamos ms al tiempo que llevemos en silencio
        //    if ( msInSilence == msSilence ) NOTIFY("Silence Detected\n");
        msInSilence += ms;
    }

    return (msInSilence >= msSilence);
}

