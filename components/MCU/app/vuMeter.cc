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
// $Id: vuMeter.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "vuMeter.h"
#include <math.h>

bool
vuMeter_t::checkSilence(u8 *pBuffer, long BufferLen, int SL)
{
    dbCount++;
    double db = evalPower(pBuffer,BufferLen);
    dbMedia = (dbMedia + db)/2;
    if (dbMedia<SL) return true;
    return false;
}

vuMeter_t::vuMeter_t(void)
{
    dbMedia =0;
    dbCount =0;
}

vuMeter_t::~vuMeter_t(void)
{
}

double
vuMeter_t::evalPower(u8 *buffer, unsigned bufferLen)
{
  double media = 0.0;
  double s1, s2;
  short int *p = (short int *)buffer;;
  double db = 0.0;
  double max, min;
  max = 0;
  min = 60000;
  s1 = 0.0;
  s2 = 0.0;

  for (unsigned i = 0; i < bufferLen/2 ; i++)
  {
    if (p[i] > max) max = p[i];
    if (p[i] < min) min = p[i];
    s1 += (double)p[i] * (double)p[i];
    s2 += (double)p[i];
  }

  s1 /= (double)(bufferLen/2);
  s2 /= (double)(bufferLen/2);
  media = (s1-s2*s2);

  db = 10.0*log10(media/134217728.0);
  db += 96;

  return db < 0 ? 0 : db;
}

