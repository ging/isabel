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
/**
 <head> 
   <name>main.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Main procedure.
   </descr>
//</head>
**/




#include "main.h"


#include <icf2/fileIO.hh>

//setUpCodecs(void)::setups --> Codec = ffdShow 
void
setUpCodecs(void)
{
    NOTIFY("Setting enviroment...");
    char path[256];
    char path2[256];
    memset(path,0,256);
    memset(path2,0,256);
    GetCurrentDirectory(256,path2);
    unsigned n=0,m=0;
    while (path2[n]!=0)
    {
        path[m]=path2[n];
        m++;n++;
        if (path2[n]=='\\')
        {
            path[m]='\\';
            m++;
        }
    }

    GetShortPathName(path,path2,256);
    memcpy(path,path2,256);
    _strlwr(path);

    int ID = GetCurrentProcessId();
    char strID[24];
    itoa(ID,strID,10);
    char tmpFile[128];
    memset(tmpFile,0,128);
    sprintf(tmpFile,"ffdshow-cfg-%s.reg",strID);
        
    char cmd[256];
    memset(cmd,0,256);

    {
        
        fileIO_t cfg("ffdshow-cfg.reg");
		fileIO_t cfg2(tmpFile,fileIO_t::fMode_e::WRITE_ONLY,fileIO_t::fAction_e::CREATE);
        char line[256];
        while(true)
        {
            memset(line,0,256);
            char c = 0;
            unsigned i = 0;

            while (cfg.read(&c,1))
            {
                line[i++]=c;
                if (c=='\n')
                    break;
            }
            if (line[0]=='\0')
                break;
            
            char * ptr = NULL;
            if ((ptr = strstr(line,"%IVIDEO_DIR%"))!=NULL)
            {
                char tmp[256];
                memset(tmp,0,256);
                int k =0;
                while(&line[k]!=ptr)
                {
                    tmp[k]=line[k];
                    k++;
                }
                strcat(tmp,path);
                strcat(tmp,ptr + sizeof("%IVIDEO_DIR%") - 1);
                strcpy(line,tmp);
            }
            cfg2.write(line,strlen(line));
        }
    }

    NOTIFY("[OK]\n");

    DWORD key = 0;
    NOTIFY("Setting up codec...");
    
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    // Start the child process. 
    
    sprintf(cmd,"regedit.exe /s  %s",tmpFile);
    if( !CreateProcess( NULL,   // No module name (use command line). 
        TEXT(cmd),        // Command line. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    //remove tmp file
    DeleteFile(tmpFile);
    
	NOTIFY("[OK]\n");

    
}

//global vars
HINSTANCE videoInstance;
videoApp_t * videoApp;
#ifdef _WINDOWS
	static char ** getArgs(char * param,int * argc,char ** argv[])
	{
   
	   int i = 0,j = 1;
	   *argv = (char**)malloc(256*sizeof(char*));
	   (*argv)[0] = (char *)malloc(strlen("IVideo.exe")*sizeof(char));
	   strcpy(*argv[0],"IVideo.exe");
		
	   while(param[i])
	   {
            char arg[100];
			int k=0;
            
            if (param[i] == '\"')
            {
                i++;
                for(;param[i]!='\"' && param[i]!=0;i++,k++)
			    {
				    arg[k] = param[i];
			    }
            }else{
                for(;param[i]!=' ' && param[i]!=0;i++,k++)
                {
                    arg[k] = param[i];
                }
            }
			i++;
			arg[k]=0;
			(*argv)[j] = (char *)malloc(strlen(arg)*sizeof(char)+50);
			strcpy((*argv)[j],arg);
			j++;
	   }
	   *argc = j;
	   return *argv;
	}  


	int WINAPI WinMain(HINSTANCE hInstance, 
					   HINSTANCE hPrevInstance, 
					   LPSTR lpszCmdParam, 
					   int nCmdShow)
	{
	
	char** argv;
	int    argc = 0;



    videoInstance = hInstance;
	getArgs(lpszCmdParam,&argc,&argv);

#else
	int main(int argc,char * argv[])
	{
#endif
		
    void initAPP(int argc,char * argv[]);


    
    //only for debug !!
    //SetCurrentDirectory("C:/Archivos de programa/Mabel/bin"); 
    int hr = CoInitialize(NULL);
    initAPP(argc,argv);
    //setUpCodecs();
    camListInit();
    defaultWindowInit();
    s->run();
  	
return 0;

}

void
initAPP(int argc,char * argv[])
{
    videoApp = new videoApp_t(argc,argv);
}