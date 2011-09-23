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
// $Id: videoConfig.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <string>
#include <map>

#include <icf2/notify.hh>

#include "videoConfig.hh"

#ifdef WIN32
int
random(void)
{
    return rand();
}
#endif

using namespace std;

//
// video config IO
//
#define VIDEO_CONFIG_VERSION 2

//
// camera - config file association
//

class camCfgMngr_t
{
private:

    void
    loadCfg(void)
    {
        FILE *fcfgs= fopen(camCfgFileName.c_str(), "r");
        if ( ! fcfgs)
        {
            NOTIFY("---readCfgFilenames: file [%s] not found!\n",
                   camCfgFileName.c_str()
                  );

            return;
        }

        char readBuf[1024];
        char *aux;
        string camera, filename;

        while (1)
        {
            aux= fgets(readBuf, 1024, fcfgs);

            if (aux == NULL) break; // error or EOF

            // get rides of '\n', if any
            if ((aux= strchr(readBuf, '\n')) != NULL)
                *aux= '\0';

            camera= readBuf;

            aux= fgets(readBuf, 1024, fcfgs);

            if (aux == NULL) break; // error or EOF

            // get rides of '\n', if any
            if ((aux= strchr(readBuf, '\n')) != NULL)
                *aux= '\0';

            filename= readBuf;

            fileMap[camera]= filename;
        }
        fclose(fcfgs);


        map<string, string>::iterator iter;
        for (iter = fileMap.begin(); iter != fileMap.end(); ++iter )
        {
            NOTIFY("Camara %s se guarda en %s\n",
                   iter->first.c_str(),
                   iter->second.c_str()
                  );
        }
    }

    string camCfgFileName;
    map<string, string> fileMap;

public:
    camCfgMngr_t(void)
    {
        const char *isaConfDir= getenv("ISABEL_CONFIG_DIR");

#ifdef __BUILD_FOR_WINXP
        camCfgFileName= isaConfDir == NULL ? getenv("TMP") : isaConfDir;
        camCfgFileName += "\\video\\";
        CreateDirectoryA(camCfgFileName.c_str(),NULL);
#else
        camCfgFileName= isaConfDir == NULL ? "/tmp" : isaConfDir;
        camCfgFileName += "/video/";
#endif
        camCfgFileName += "video_cfg_mgr.txt";
        loadCfg();
    }

    void saveCfg(string camera, string fileName)
    {
        fileMap[camera]= fileName;

        FILE *fout= fopen(camCfgFileName.c_str(), "w");

        if ( ! fout)
        {
            NOTIFY("camCfgMngr_t::save: "
                   "cannot create video configuration file: %s\n",
                   camCfgFileName.c_str()
                  );
            return;
        }

        map<string, string>::iterator iter;
        for (iter = fileMap.begin(); iter != fileMap.end(); ++iter)
        {
            fprintf(fout, "%s\n", iter->first.c_str());
            fprintf(fout, "%s\n", iter->second.c_str());
        }

        fclose(fout);
    }

    string getFilename(string camera)
    {
        map<string, string>::iterator iter;
        string filePath, fileName;

        iter= fileMap.find(camera);
        if (iter != fileMap.end())
        {
            return iter->second;
        }

        const char *isaConfDir= getenv("ISABEL_CONFIG_DIR");

#ifdef __BUILD_FOR_WINXP
        filePath= isaConfDir == NULL ? getenv("TMP") : isaConfDir;
#else
        filePath= isaConfDir == NULL ? "/tmp" : isaConfDir;
#endif
        
        char rn8s[512];
        FILE *fin;
        // generates unique name
        while (1)
        {
            long rn= random();
            u8 rn8= (u8)(rn % 256);
            sprintf(rn8s, "%03d", rn8);
#ifdef __BUILD_FOR_WINXP
            fileName = (filePath + "\\video") + rn8s;
#else
            fileName = (filePath + "/video") + rn8s;
#endif
            fileName += ".conf";

            fin= fopen(fileName.c_str(), "r");
            if ( ! fin)
            {
                NOTIFY("+++filename available!\n");

                break;
            }
        }

        return fileName;
    }
};

camCfgMngr_t camCfgMngr;

//
// camera config file methods
//

bool
readConfigFile(const char *camera, videoConfig_t &videoConf)
{
    if ((camera == NULL) || (strlen(camera) == 0))
    {
        return false;
    }

    videoConf.reset();
    sprintf(videoConf.camera, "%s", camera);

    string fileName= camCfgMngr.getFilename(camera);

    NOTIFY("+++reading video configuration file: %s\n", fileName.c_str());

    int version= -1;

    FILE *fin= fopen(fileName.c_str(), "r");
    if ( ! fin)
    {
        NOTIFY("+++no video configuration file found!\n");

        return false;
    }

    char readBuf[1024];

    while (fgets(readBuf, 1024, fin) != NULL)
    {
        if (readBuf[0]== '#') continue;

        char *keyword, *param, *aux;

        // get rides of '\n', if any
        if ((aux= strchr(readBuf, '\n')) != NULL)
            *aux= '\0';

        // search for blank
        if ((param= strchr(readBuf, ' ')) == NULL)
        {
            continue;
        }
        keyword= readBuf;
        param[0]= '\0';
        param++;

        if (strcmp("VERSION", keyword)== 0)
        {
            version= atoi(param);
        }
        else if (strcmp("camera", keyword)== 0)
        {
            strncpy(videoConf.camera, param, MAX_CFGSTR_LEN);
        }
        else if (strcmp("inputport", keyword)== 0)
        {
            strncpy(videoConf.inputport, param, MAX_CFGSTR_LEN);
        }
        else if(strcmp("saturation", keyword)== 0)
        {
            videoConf.saturation= atoi(param);
        }
        else if(strcmp("brightness", keyword)== 0)
        {
            videoConf.brightness= atoi(param);
        }
        else if(strcmp("hue", keyword)== 0)
        {
            videoConf.hue= atoi(param);
        }
        else if(strcmp("contrast", keyword)== 0)
        {
            videoConf.contrast= atoi(param);
        }
        else if(strcmp("vnorm", keyword)== 0)
        {
            strncpy(videoConf.norm, param, MAX_CFGSTR_LEN);
        }
        else if(strcmp("ffreq", keyword)== 0)
        {
            strncpy(videoConf.flickerfreq, param, MAX_CFGSTR_LEN);
        }
        else
        {
            NOTIFY("Syntax error in file: '%s'\n", fileName.c_str());
            NOTIFY("\treaded \"%s\"\n", readBuf);
        }
    }
    fclose(fin);

    if ( (version == -1) || (version != VIDEO_CONFIG_VERSION) )
    {
        return false;
    }
    return true;
}

void
writeConfigFile(const videoConfig_t *videoConf)
{
    string fileName= camCfgMngr.getFilename(videoConf->camera);

    FILE *fout= fopen(fileName.c_str(), "w");

    if ( ! fout)
    {
        NOTIFY("cannot create video configuration file: %s\n",
               fileName.c_str()
              );
        return;
    }

    fprintf(fout,
            "#\n"
            "# Automatically generated by %s -- %s"
            "#\n"
            "# DO NOT EDIT\n"
            "VERSION %d\n"
            "\n"
            "# camera\n"
            "camera %s\n"
            "\n"
            "# inputport\n"
            "inputport %s\n\n"
            "# Do edit at your convenience!!!\n"
            "\n"
            "# saturation [0..100] %%\n"
            "saturation %d\n"
            "\n"
            "# brightness [0..100] %%\n"
            "brightness %d\n"
            "\n"
            "# hue [0..100] %% (colour cameras only)\n"
            "hue %d\n"
            "\n"
            "# contrast [0..100] %%\n"
            "contrast %d\n"
            "\n"
            "# norm\n"
            "vnorm %s\n"
            "\n"
            "# Flicker Frequency\n"
            "ffreq %s\n"
            "\n",

            "ISABEL", "video_module.dso",
            VIDEO_CONFIG_VERSION,
            videoConf->camera,
            videoConf->inputport,
            videoConf->saturation,
            videoConf->brightness,
            videoConf->hue,
            videoConf->contrast,
            videoConf->norm,
            videoConf->flickerfreq
           );
    fprintf(fout, "# ... and that's all folks!\n");

    fclose(fout);

    camCfgMngr.saveCfg(videoConf->camera, fileName);
}

videoConfig_t::videoConfig_t(void)
{
    reset();
}

videoConfig_t::~videoConfig_t(void)
{
}

void
videoConfig_t::reset(void)
{
    memset(camera, 0, MAX_CFGSTR_LEN);
    memset(inputport, 0, MAX_CFGSTR_LEN);
    memset(norm, 0, MAX_CFGSTR_LEN);
    memset(flickerfreq, 0, MAX_CFGSTR_LEN);

    saturation = 50;
    brightness = 50;
    hue        = 50;
    contrast   = 50;
}

