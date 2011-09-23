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
// $Id: ctrlProtocol.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <Isabel_SDK/systemRegistry.hh>


#include "ctrlProtocol.hh"
#include "srcMgr.hh"
#include "shmSrcApp.hh"

binding_t<shmSrc_InterfaceDef_t> shmSrc_methodBinding[]=
{
    //
    // sources
    //
    { "source_list",           &shmSrc_InterfaceDef_t::source_list           },
    { "source_select",         &shmSrc_InterfaceDef_t::source_select         },
    { "source_get_current",    &shmSrc_InterfaceDef_t::source_get_current    },

    { "source_frame_rate",     &shmSrc_InterfaceDef_t::source_frame_rate     }, //

    { "source_grab_size",      &shmSrc_InterfaceDef_t::source_grab_size      },

    { "source_list_norms",     &shmSrc_InterfaceDef_t::source_list_norms     },
    { "source_set_norm",       &shmSrc_InterfaceDef_t::source_set_norm       },
    { "source_get_norm",       &shmSrc_InterfaceDef_t::source_get_norm       },
    { "source_set_saturation", &shmSrc_InterfaceDef_t::source_set_saturation },
    { "source_get_saturation", &shmSrc_InterfaceDef_t::source_get_saturation },
    { "source_set_brightness", &shmSrc_InterfaceDef_t::source_set_brightness },
    { "source_get_brightness", &shmSrc_InterfaceDef_t::source_get_brightness },
    { "source_set_hue",        &shmSrc_InterfaceDef_t::source_set_hue        },
    { "source_get_hue",        &shmSrc_InterfaceDef_t::source_get_hue        },
    { "source_set_contrast",   &shmSrc_InterfaceDef_t::source_set_contrast   },
    { "source_get_contrast",   &shmSrc_InterfaceDef_t::source_get_contrast   },

    //
    // common stuff
    //
    { "source_query_id",       &shmSrc_InterfaceDef_t::source_query_id  },
    { "source_nop",            &shmSrc_InterfaceDef_t::source_nop       },
    { "source_bye",            &shmSrc_InterfaceDef_t::source_bye       },
    { "source_quit",           &shmSrc_InterfaceDef_t::source_quit      },

    { NULL, NULL }
};


//#define APP ((shmSrcApp_t *)((shmSrcInterface_t *)this)->get_owner())
#define APP static_cast<shmSrcApp_t*>(static_cast<shmSrcInterface_t*>(this)->get_owner())


//
// Auxiliar function to concatenate values
//
void
addDesc(char *retVal, char *desc, bool first)
{
    char buf[512];

    // as ',' is the separator, it is substituted by ' '
    char *comma;
    while ((comma = strchr(desc, ',')) != NULL) *comma= ' ';

    if (first)
    {
        sprintf(buf, "%s", desc);
    }
    else
    {
        sprintf(buf, ",%s", desc);
    }
    strcat(retVal, buf);
}


//
// sources
//
char const *
shmSrc_InterfaceDef_t::source_list(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();

    if (sfia->size() == 0)
    {
        return "ERROR: toy hecho un lio\n";
    }

    memset(result, 0, sizeof(result));

    bool first= true;

    strcat(result,"{");
    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        char *desc= strdup(sfi->getDescriptor()->getID());

        // got "desc::name::device"
        const char *inputPorts= sfi->getDescriptor()->getInputPorts();

        if (inputPorts == NULL)
        {
            addDesc(result, desc, first);
            first= false;
            free(desc);
            continue;
        }

        char **cameras;
        int    numCameras;
        char   aCameraDesc[512];

        // inputPorts comes in the form (Composite1,S-Video,Composite3)
        // so numCameras is 1 more than real arguments
        cameras= buildFormattedArgv(inputPorts, numCameras);
        if (numCameras == 1)
        {
            // inputPorts is in the form of (), meaning than
            // channel has no name. Hope there is only one input
            sprintf(aCameraDesc, "%s;", desc);
            addDesc(result, aCameraDesc, first);
            first= false;
        }
        else
        {
            for (int j= 1; j < numCameras; j++)
            {
                sprintf(aCameraDesc, "%s;%s", desc, cameras[j]);
                addDesc(result, aCameraDesc, first);
                first= false;
            }
        }
        free(desc);
    }
    strcat(result,"}\n");

    return result;
}


char const *
shmSrc_InterfaceDef_t::source_select(int argc, char **argv)
{
    static char result[4096];

    char *srcInfo, *camera;

    INTER_VRFY_ARGC(result, 2);

    NOTIFY("%s(%s)\n", argv[0], argv[1]);

    srcInfo = argv[1];
    camera = strchr(srcInfo, ';');

    if (camera)
    {
        *camera= '\0';
        camera++;
    }

    APP->srcMgr->setSource(srcInfo, camera);

    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_get_current(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    const char *srcInfo= NULL;
    const char *camera = NULL;

    srcInfo= APP->srcMgr->getSourceInfo();

    if (srcInfo == NULL)
    {
        return "ERROR: no source available\n";
    }

    camera = APP->srcMgr->getCamera();

    if (camera != NULL)
    {
        sprintf(result, "%s;%s\n", srcInfo, camera);
    }
    else
    {
        sprintf(result, "%s\n", srcInfo);
    }

    return result;
}


char const *
shmSrc_InterfaceDef_t::source_frame_rate(int argc, char **argv)
{
    static char result[4096];

    float frameRate;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    frameRate= atof(argv[1]);

    if (frameRate <= 0)
    {
        sprintf(result, "ERROR: invalid frame rate [%f]\n", frameRate);
        return result;
    }

    APP->srcMgr->setFrameRate(frameRate);

    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_grab_size(int argc, char **argv)
{
    static char result[4096];

    const char *grabSize;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    grabSize= argv[1];

    if (strcmp("SQCIF", grabSize) == 0)
        grabSize= "128x96";
    else if (strcmp("QCIF", grabSize) == 0)
        grabSize= "176x144";
    else if (strcmp("CIF", grabSize) == 0)
        grabSize= "352x288";
    else if (strcmp("4CIF", grabSize) == 0)
        grabSize= "704x576";
    else if (strcmp("16CIF", grabSize) == 0)
        grabSize= "1408x1152";

    unsigned w, h;

    if (sscanf(grabSize, "%ux%u", &w, &h)== 2)
    {
        if ((w==0)||(h==0))
        {
            return "ERROR: size cannot be 0";
        }
    }

    APP->srcMgr->setGrabSize(w, h);

    return "OK\n";
}


const char *
shmSrc_InterfaceDef_t::source_list_norms(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s()\n", argv[0]);

    std::string normsList= APP->srcMgr->getVideoStandardList();

    if ( normsList.empty() )
    {
        return "ERROR: no video norms available\n";
    }

    sprintf(result, "{%s}\n", normsList.c_str());
    return result;
}

char const *
shmSrc_InterfaceDef_t::source_set_norm(int argc, char **argv)
{
    static char result[4096];

    char *vnorm;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    vnorm= argv[1];

    if ( ! APP->srcMgr->setVideoStandard(vnorm) )
    {
        sprintf(result, "ERROR: cannot set video standard [%s]\n", vnorm);
        return result;
    }


    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_get_norm(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    std::string vnorm= APP->srcMgr->getVideoStandard();

    if ( vnorm.empty() )
    {
        return "ERROR: cannot get video norm\n";
    }

    sprintf(result, "%s\n", vnorm.c_str());
    return result;
}


char const *
shmSrc_InterfaceDef_t::source_set_saturation(int argc, char **argv)
{
    static char result[4096];

    int value;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    value= atoi(argv[1]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    APP->srcMgr->setVideoSaturation(value);

    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_get_saturation(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    int value= APP->srcMgr->getVideoSaturation();

    if (value == -1)
    {
        return "ERROR: cannot get Saturation\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}


char const *
shmSrc_InterfaceDef_t::source_set_brightness(int argc, char **argv)
{
    static char result[4096];

    int value;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    value= atoi(argv[1]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    APP->srcMgr->setVideoBrightness(value);

    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_get_brightness(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    int value= APP->srcMgr->getVideoBrightness();

    if (value == -1)
    {
        return "ERROR: cannot get Brightness\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}


char const *
shmSrc_InterfaceDef_t::source_set_hue(int argc, char **argv)
{
    static char result[4096];

    int value;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    value= atoi(argv[1]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    APP->srcMgr->setVideoHue(value);

    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_get_hue(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    int value= APP->srcMgr->getVideoHue();

    if (value == -1)
    {
        return "ERROR: cannot get Hue\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}


char const *
shmSrc_InterfaceDef_t::source_set_contrast(int argc, char **argv)
{
    static char result[4096];

    int value;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    value= atoi(argv[1]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    APP->srcMgr->setVideoContrast(value);

    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_get_contrast(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    int value= APP->srcMgr->getVideoContrast();

    if (value == -1)
    {
        return "ERROR: cannot get Contrast\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}


//
// common stuff
//
char const *
shmSrc_InterfaceDef_t::source_query_id(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    sprintf(result, "0x%x\n", interID);
    return result;
}


char const *
shmSrc_InterfaceDef_t::source_nop(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    return "OK\n";
}


char const *
shmSrc_InterfaceDef_t::source_bye(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    (*APP)-(static_cast<shmSrcInterface_t*>(this));
    return (char const *)-1;
}


char const *
shmSrc_InterfaceDef_t::source_quit(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    APP->shutdown();

    return (char const *)-1;
}


