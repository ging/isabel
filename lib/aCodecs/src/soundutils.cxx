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

#include <string.h>
#include <math.h>

#include <aCodecs/soundutils.h>




SoundUtils::Resampler::Resampler(void): last(0)
{
}

int
SoundUtils::Resampler::resample(const unsigned char *indata,
                                int indatas,
                                int inrate,
                                unsigned char *outdata,
                                int outdatas,
                                int outrate,
                                bool useLast)
{

    int insamples, outsamples;
    int inms;

    if (indatas == 0) {
        return 0;
    }

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

    short * inptr = (short *)indata;
    short * outptr = (short *)outdata;

    if (!useLast)
        last = inptr[0];

    if (outsamples*2 > outdatas)
    {
        return 0;
    }


    // mejorar calidad
    if (outsamples > insamples)
    {
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
                double pendiente = ((double)(inptr[i]) - (double)(last))/(inUsPerSample);
                outptr[n] = last + (short int)(pendiente*((double)(n*outUsPerSample) - (double)((i)*inUsPerSample)));
                n++;
            }
            last = inptr[i];
        }
        outsamples = n;

    } else {

        // Empeorar calidad
        double pasos = (double)insamples/(double)outsamples;
        int k = 1;
        outptr[0] = inptr[0];
        for (int i= 1; i < insamples; i++)
        {
            if (i/pasos>k)
            {
                double a = (k*outUsPerSample) - ((i-1)*inUsPerSample);
                double pendiente = ((double)(inptr[i]) - (double)(inptr[i-1]))/(inUsPerSample);
                outptr[k] =  (short)((double)(inptr[i-1]) + (double)(pendiente*a));
                k++;
            }
        }
        last = outptr[outsamples] = inptr[insamples];
    }
    return outsamples*2;
}

int SoundUtils::EchoSuppressor::level = 0;
int SoundUtils::EchoSuppressor::echo_threshold = SoundUtils::EchoSuppressor::DEFAULT_ECHO_THRESHOLD;
int SoundUtils::EchoSuppressor::echo_window = 10;

double SoundUtils::EchoSuppressor::data[MAX_ECHO_WINDOW] = {0};

void SoundUtils::EchoSuppressor::nextData(double pow)
{
    static int index = 0;
    data[index++%echo_window] = pow;
}

float
SoundUtils::EchoSuppressor::getAttenuation(void)
{
    double mean = 0;
    for (int i = 0; i < echo_window; i++) {
        mean += data[i];
    }
    mean = mean/echo_window;

    if (level > 0 && mean > echo_threshold) {
        return -level*10.0;
        //return -((float)level/3)*((float)mean-echo_threshold);
    }
    else {
        return 0;
    }
}


int SoundUtils::CAG::level = 0;
int SoundUtils::CAG::cag_threshold = SoundUtils::CAG::DEFAULT_CAG_THRESHOLD;
int SoundUtils::CAG::cag_window = 10;

double SoundUtils::CAG::data[MAX_CAG_WINDOW] = {0};

void SoundUtils::CAG::nextData(double pow)
{
    static int index = 0;
    data[index++%cag_window] = pow;
}

float
SoundUtils::CAG::getAttenuation(void)
{
    double mean = 0;
    for (int i = 0; i < cag_window; i++) {
        mean += data[i];
    }
    mean = mean/cag_window;

    if (level > 0 && mean > cag_threshold) {
        return -((float)level/3)*((float)mean-cag_threshold);
    }
    else {
        return 0;
    }
}


void
SoundUtils::bufferAtenuator(unsigned char *buffer, int len, float dbs)
{
    short int *p;
    double factor;

    // Si son pocos DB pues no hago nada
    if (dbs < 0.5 && dbs > -0.5) return;

    if (dbs > 0.0) {
        factor = sqrt(pow(10.0, dbs/10.0));
    } else {
        factor = 1.0/sqrt(pow(10.0, -dbs/10.0));
    }
    p = (short int*)buffer;
    int temp = 0;
    for ( int i = 0 ; i < len/2 ; i++ )
    {
        temp = (int) (p[i] * factor);
          if (temp > 32767)         // 32767 = MAX_SIGNED_SHORT
            p[i] = 32767;
        else if (temp < -32767)
            p[i] = -32767;
        else
            p[i] = (short int)temp;

    }
}

double
SoundUtils::evalPower(unsigned char *buffer, int len)
{
    double media = 0;
    double s1, s2;
    short* data = (short*)buffer;
    double db;

    int n = len/2;

    s1 = 0.0;
    s2 = 0.0;

    for ( int i = 0 ; i < n ; i++ )
    {
        s1 += (double)data[i] * (double)data[i];
        s2 += (double)data[i];
    }

    s1 = s1/n;
    s2 = s2/n;

    media = s1-s2*s2;

    if (media < 0.00001)
        return -50;

    db = 10.0*log10(media) - 90; // 90 es lo que corresponde a la maxima potencia posible

    if (db < -50) db = -50;
    if (db > 0  ) db = 0;

    return db;
}
