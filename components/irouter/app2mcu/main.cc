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
// $Id: main.cc 10748 2007-09-06 10:04:02Z ebarra $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/notify.hh>

#include "transApp.hh"
#include "webirouterConfig.hh"

#ifdef WIN32
#include <icf2/icfTime.hh>
#include <io.h>
#endif

#ifdef __BUILD_FOR_LINUX
#include <dirent.h>
#include <netdb.h>
#endif

extern "C" long gethostid(void);    // bloody Solaris prototype(less)
u32 localNodeIdentifier;

u32
getRandomId()
{
    timeval myTime;
    gettimeofday(&myTime, NULL);
    return myTime.tv_sec;
}

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
void
createDirIfNotExist(const char *name)
{
    DIR *dir= opendir(name);
    if ( ! dir)
    {
        NOTIFY("Creating dir %s\n",name);
        char cmd[512];
        memset(cmd, 0, 512);
        sprintf(cmd, "mkdir %s", name);
        system(cmd);
        memset(cmd, 0, 512);
        sprintf(cmd, "chmod 777 %s", name);
        system(cmd);
    }
    else
    {
        closedir(dir);
    }
}
#elif defined(WIN32)
void
createDirIfNotExist(const char *name)
{
    CreateDirectory(name,NULL);
}
#else
#error "Please, implement createDirIfNotExist in you O.S."
#endif

u32
getIrouterLocalId()
{
    char name[512];
    memset(name, 0, 512);
    char *configDir=getenv("ISABEL_CONFIG_DIR");

    // generate irouter ID fileName
    if (configDir)
        sprintf(name, "%s/irouter.id", configDir);
    else
#ifdef __BUILD_FOR_LINUX
    sprintf(name, "/tmp/.irouter/irouter.id");
    int fd = open(name, O_RDONLY);
#endif
#ifdef WIN32
    sprintf(name, "irouter.id");
    int fd = _open(name, _O_RDONLY);
#endif

    char buf[256];
    memset(buf, 512, 0);

    // Leo el ID si existe
    if (fd>0)
    {
#ifdef __BUILD_FOR_LINUX
        int n=read(fd, buf, 256);
        if (n<=0)
            NOTIFY("irouter.id exists but can't read!\n");
        int id = atoi(buf);
        close(fd);
#endif
#ifdef WIN32
        _read(fd, buf, 256);
        int id = atoi(buf);
        _close(fd);
#endif
       return id;
    }

    // si no existe
    // creamos el directorio
    // donde guardar el id
    if (configDir)
    {
        createDirIfNotExist(configDir);
    } else {

#ifdef __BUILD_FOR_LINUX
        createDirIfNotExist("/tmp/.irouter");
#endif
    }

    int id= getRandomId();
    sprintf(buf, "%d", id);
    NOTIFY("Generating irouter.id = %d\n",id);
#ifdef __BUILD_FOR_LINUX
    int fd2 = open(name, O_CREAT | O_RDWR | O_EXCL, S_IROTH | S_IRGRP | S_IRUSR);
#endif
#ifdef WIN32
    int fd2 = _open(name, _O_CREAT | _O_RDWR | _O_EXCL, 0);//_S_IREAD | _S_IWRITE);
#endif
    if ( fd2 <= 0 )
    {
       NOTIFY("irouter:: could not create [%s] file\n", name);
       exit(-1);
    }
#ifdef __BUILD_FOR_LINUX
    write(fd2, buf, strlen(buf));
    close(fd2);
#endif
#ifdef WIN32
    _write(fd2, buf, strlen(buf));
    _close(fd2);
#endif

    return id;
}

int
getmyip(u32 &myIp)
{
    char hname[512];
    memset(hname, 0, 256);

    if (gethostname(hname, 256)==-1) {
        NOTIFY("Gethostname returns -1 !\n");
        return -1;
    }

    struct hostent *h = gethostbyname(hname);

    if (!h)
    {
        NOTIFY("Gethostbyname returns no hostent struct!\n");
        return -1;
    }

    int *addr = (int *)h->h_addr_list[0];

    if (!addr)
    {
        NOTIFY("Gethotbyname returns no addr struct!\n");
        return -1;
    }

    myIp = ntohl(*addr);
    return 0;
}

#ifdef WIN32

#ifdef _WINDOWS // If running without console

static char **
getArgs(char * param,int * argc,char ** argv[])
{
   int i = 0,j = 1;
   *argv = (char**)malloc(256*sizeof(char*));
   (*argv)[0] = (char *)malloc(strlen("Irouter.exe")*sizeof(char));
   strcpy(*argv[0],"Irouter.exe");

   while(param[i]!='\0')
   {
        char arg[100];
        int k=0;
        for(;param[i]!=' ' && param[i]!=0;i++,k++)
        {
            arg[k] = param[i];
        }
        i++;
        arg[k]=0;
        (*argv)[j] = (char *)malloc(strlen(arg)*sizeof(char));
        strcpy((*argv)[j],arg);
        j++;
   }
   *argc = j-1;
   return *argv;
}


int
WINAPI WinMain(HINSTANCE hInstance,
               HINSTANCE hPrevInstance,
               LPSTR lpszCmdParam,
               int nCmdShow)
{
    char** argv;
    int    argc = 0;

    getArgs(lpszCmdParam,&argc,&argv);

#else

int
main(int argc,char * argv[]) // if running with console
{
#endif
#endif

#ifdef __BUILD_FOR_LINUX
int
main(int argc, char *argv[])
{
#endif

    transApp_t theApp(argc, argv);

    srand(time(NULL));

    localNodeIdentifier = getIrouterLocalId();
    NOTIFY("localhostid= %u\n", localNodeIdentifier);

    theApp.run();


    return 0; // keeps compiler happy
}

