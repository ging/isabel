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
   <name>videoApp.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of videoApp.h.
   </descr>
//</head>
**/

#include "videoApp.h"
#include "videoRTPSession.h"


videoApp_t::videoApp_t(int &argc,argv_t &argv)
:application_t(argc, argv), 
 __controlPort(NULL),
 __bandwidth(1000000.0)
{
	strcpy(__rtpPort,"51017");
	strcpy(__rtcpPort,"51019");
  enum myOptions {
       ctrlPort,
	   notify,
	   dport,
	   autoChannel,
	   h
       };
  s = static_cast<sched_t*>(this);
  	  
  optionDefList_t opt;       
  appParamList_t *parList;   

  //Available options 
  opt           << new optionDef_t("@cport", ctrlPort , "Opens a Control Socket in <port>")  
				<< new optionDef_t("@rtpPort", dport , "Receiver data port")
				<< new optionDef_t("@notify", notify , "Notify file")
				<< new optionDef_t("autoChannel", autoChannel , "autoShows playChannels")
				<< new optionDef_t("h", h , "Video Help");  
         		
  parList = getOpt(opt, argc, argv);
  ctrlTask_t * ctrlTask = NULL;
			
  SetAutoChannel = new bool();
  *SetAutoChannel = false;

  for ( ; parList->len(); parList->behead()) {
              switch(parList->head()->parId) {
                case ctrlPort:
                    char * port;
		      		port = strdup(parList->head()->parValue);
					ctrlTask = new ctrlTask_t(port);
                    break;
 
				case dport:
                    char * strDport;
					strDport = strdup(parList->head()->parValue);
					strcpy(__rtpPort,strDport);
					itoa(atoi(strDport) + 2,__rtcpPort,10);
					break;

				case notify:
                    
					char * file;
					file = strdup(parList->head()->parValue);
					setNotifyFile(file);		      		
					break;


				case autoChannel:
                    
					*SetAutoChannel = true;
					break;
				
			  
				case h:
					default:
                    int i;
					for(i=0; i < opt.len(); i++) {
						optionDef_t * op= static_cast<optionDef_t*>(opt.nth(i));
						if(op->optDesc)
							fprintf(stderr, "    %-10s %-5s\t-- %s\n",
							op->optName,
							op->optHasArg?"value":"",
							op->optDesc);
						else
							fprintf(stderr, "    %-10s %-5s\t",
							op->optName,
							op->optHasArg?"":"value");
						}
						exit(0);

				   break;
    
			  }
 	};

	// RTP Object Creation
#ifdef WIN32
	if (getenv("MCU_USE_ONLY_IPv4")){
  		rtpSession= new videoRTPSession_t(this,
                                    "0.0.0.0",
                                    __rtpPort,
                                    "0.0.0.0",
                                    __rtcpPort,
                                    VIDEO_TIMESTAMP_UNIT
                                   );
	}
	else{
		rtpSession= new videoRTPSession_t(this,
                                    NULL,
                                    __rtpPort,
                                    NULL,
                                    __rtcpPort,
                                    VIDEO_TIMESTAMP_UNIT
                                   );
	}
#else
	rtpSession= new videoRTPSession_t(this,
                                    NULL,
                                    __rtpPort,
                                    NULL,
                                    __rtcpPort,
                                    VIDEO_TIMESTAMP_UNIT
                                   );
#endif


    //init log file
      struct _timeb timebuffer;
      char * timeline;

      _ftime( &timebuffer );
      timeline = ctime( & ( timebuffer.time ) );
			    
      char auxFile[20];
      char auxArgv[100];
			    
      memset(auxArgv,0,100);
      for (int i = 1; i<argc; i++)
      {
    	    strcat(auxArgv,argv[i]);
		    strcat(auxArgv," ");
      }

      if (char * n = strstr(auxArgv,"File"))
      {				
		    n = strstr(n," ");
		    strcpy(auxFile,n);

      }else{
		    strcpy(auxFile," Screen");
      }

      // Debug Session Info
      NOTIFY(  	"\n·:======================================================:·\n\n"
			    "	Isabel Video v0.1 Debug Session:\n\n"
			    "   - Options: %s\n"
			    "   - Log File:%s\n"
			    "   - Date: %s\n"
			    "   - Author: Vicente Sirvent Orts\n" 		
			    "\n"
			    "·:=======================================================:·\n\n",
			    auxArgv,
			    auxFile,
			    timeline);
}