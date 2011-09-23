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
// $Id: videoConfig.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __video_config_hh__
#define __video_config_hh__

const int MAX_CFGSTR_LEN= 1024;

class videoConfig_t
{
public:
    char  camera[MAX_CFGSTR_LEN];
    char  inputport[MAX_CFGSTR_LEN];
    char  norm[MAX_CFGSTR_LEN];
    char  flickerfreq[MAX_CFGSTR_LEN];
    int   saturation;
    int   brightness;
    int   hue;
    int   contrast;

    videoConfig_t(void);
    ~videoConfig_t(void);

    void reset(void);
};

bool readConfigFile(const char *camName, videoConfig_t &vidcfg);
void writeConfigFile(const videoConfig_t *vidcfg);

#endif

