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
   <name>ctrlProtocol.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of ctrlProtocol.h
   </descr>
//</head>
**/


#include "general.h"
#include "recchannel.h"
#include "playchannel.h"
#include "camMgr.h"
#include "sharedDisplay.h"
#include "ctrlProtocol.h"


#define MY_INTER_VRFY_ARGC(b,x,msg)      \
    if(argc!= (x)) {                     \
        sprintf(b, "ERROR= %s\n", msg);  \
        return b;                        \
    }

#define MY_INTER_VRFY_ARGC_range(b,x,y, msg)          \
    if((argc< (x)) || (argc> (y))) {                  \
        sprintf(b, "ERROR= %s\n", msg);               \
        return b;                                     \
    }

#define APP static_cast<sched_t*>((this)->get_owner())

binding_t<video_InterfaceDef> video_methodBinding[]=
{

	{ "video_query_id",					&video_InterfaceDef::video_query_id    },
	{ "shdis_query_id",					&video_InterfaceDef::video_query_id    },

	{ "video_rec_channel",				&video_InterfaceDef::video_assign_channel    },
	{ "video_net_channel",				&video_InterfaceDef::video_nop    },
	{ "video_winGrabber_channel",	    &video_InterfaceDef::video_winGrabber_channel   },
    { "shdis_create_source_channel",	&video_InterfaceDef::video_winGrabber_channel  },
	{ "video_play_channel",			    &video_InterfaceDef::video_new_channel    },
	{ "shdis_create_sink_channel",		&video_InterfaceDef::video_new_channel},
	{ "video_delete_channel",			&video_InterfaceDef::video_delete_channel    },
    { "shdis_delete_channel",			&video_InterfaceDef::video_unmap_channel },

	{ "video_map_channel",				&video_InterfaceDef::video_map_channel    },
    { "video_unmap_channel",			&video_InterfaceDef::video_unmap_channel  },
    { "shdis_map_channel",				&video_InterfaceDef::video_map_channel   },
    { "shdis_unmap_channel",			&video_InterfaceDef::video_unmap_channel  },

    { "shdis_play",                      &video_InterfaceDef::video_play },
    { "shdis_stop",                      &video_InterfaceDef::video_stop },

	{ "video_set_buffering",			&video_InterfaceDef::video_set_buffering  },
  
	{ "video_set_window_title",			&video_InterfaceDef::video_set_window_title  },
   	{ "video_window_title",			&video_InterfaceDef::video_set_window_title  },
  	{ "shdis_window_title",				&video_InterfaceDef::video_set_window_title  },
	{ "video_window_geometry",		    &video_InterfaceDef::video_set_window_geometry  },
    { "shdis_window_geometry",			&video_InterfaceDef::video_set_window_geometry  },
	{ "video_set_window_size",			&video_InterfaceDef::video_set_window_size  },
	{ "video_follow_size",			    &video_InterfaceDef::video_follow_size  },
	{ "shdis_follow_size",				&video_InterfaceDef::video_follow_size  },
	{ "video_full_screen_window",		&video_InterfaceDef::video_full_screen_window  },
	{ "video_set_window_owner",		    &video_InterfaceDef::video_set_window_owner  },
    { "video_configure_channel",        &video_InterfaceDef::video_configure_channel  },

	//
	// bindings
	//
	{ "video_bind",						&video_InterfaceDef::video_bind  },
	{ "video_unbind",					&video_InterfaceDef::video_unbind  },
	{ "video_show_bindings",			&video_InterfaceDef::video_show_bindings  },
    { "shdis_bind",						&video_InterfaceDef::video_bind  },
    { "shdis_unbind",					&video_InterfaceDef::video_unbind  },
    { "shdis_show_bindings",			&video_InterfaceDef::video_show_bindings  },

	{ "video_channel_bandwidth", 		&video_InterfaceDef::video_channel_bandwidth  },
	{ "shdis_channel_bandwidth",		&video_InterfaceDef::video_channel_bandwidth},
   	{ "video_bandwidth_on",				&video_InterfaceDef::video_bandwidth_on	},
	{ "video_bandwidth_off",			&video_InterfaceDef::video_bandwidth_off	},


	{ "video_rtcp_bind",				&video_InterfaceDef::video_rtcp_bind      },
    { "video_rtcp_unbind",				&video_InterfaceDef::video_rtcp_unbind    },
    { "video_rtcp_show_bindings",		&video_InterfaceDef::video_rtcp_show_bindings },
    { "shdis_rtcp_bind",				&video_InterfaceDef::video_rtcp_bind       },
    { "shdis_rtcp_unbind",				&video_InterfaceDef::video_rtcp_unbind     },
    { "shdis_rtcp_show_bindings",		&video_InterfaceDef::video_rtcp_show_bindings },
	
	{ "video_set_sdesinfo",				&video_InterfaceDef::video_set_sdesinfo   },
    { "shdis_set_sdesinfo",				&video_InterfaceDef::video_show_sdesinfo  },
   	{ "video_show_sdesinfo",			&video_InterfaceDef::video_show_sdesinfo  },
    { "shdis_show_sdesinfo",			&video_InterfaceDef::video_show_sdesinfo  },
  	{ "video_show_receivers",			&video_InterfaceDef::video_show_receivers },
    { "shdis_show_receivers",			&video_InterfaceDef::video_show_receivers },
  	{ "video_get_lost",					&video_InterfaceDef::video_get_lost       },
    { "shdis_get_lost",					&video_InterfaceDef::video_get_lost        },
   	{ "video_get_local_lost",			&video_InterfaceDef::video_get_local_lost },
    { "video_get_fractionlost",			&video_InterfaceDef::video_get_fractionlost   },
    { "shdis_get_fractionlost",			&video_InterfaceDef::video_get_fractionlost   },
    { "video_get_roundtrip_time",		&video_InterfaceDef::video_get_roundtrip_time },
    { "shdis_get_roundtrip_time",		&video_InterfaceDef::video_get_roundtrip_time },
    { "video_get_jitter",				&video_InterfaceDef::video_get_jitter    },
   	{ "shdis_get_jitter",				&video_InterfaceDef::video_get_jitter    },
   
	{ "video_bucket_size",				&video_InterfaceDef::video_bucket_size    },
	{ "shdis_bucket_size",				&video_InterfaceDef::video_bucket_size    },
	{ "video_compute_stats",			&video_InterfaceDef::video_compute_stats    },
  	{ "shdis_compute_stats",			&video_InterfaceDef::video_compute_stats    },
	
	{ "shdis_set_retrans",				&video_InterfaceDef::video_nop    },
      
    //SNMP
    { "video_do_stats",					&video_InterfaceDef::video_nop       },
    { "shdis_do_stats",					&video_InterfaceDef::video_nop       },
	//END OF SNMP

	//
    // codecs
    //
    { "video_list_codecs",				&video_InterfaceDef::video_list_codecs_ex    },
	{ "shdis_list_codecs",				&video_InterfaceDef::video_list_codecs_ex    },
    { "video_select_codec",			    &video_InterfaceDef::video_select_codec   },
	{ "shdis_select_codec",				&video_InterfaceDef::video_select_codec   },
	{ "video_set_payload",			    &video_InterfaceDef::video_set_payload    },
    { "video_describe_codec",			&video_InterfaceDef::video_nop },
    { "shdis_describe_codec",			&video_InterfaceDef::video_nop },
    { "video_list_codecs_ex",			&video_InterfaceDef::video_list_codecs_ex },
    { "shdis_list_codecs_ex",			&video_InterfaceDef::video_nop },
    { "video_describe_codec",			&video_InterfaceDef::video_nop },
    { "video_codec_quality",			&video_InterfaceDef::video_nop },
    { "shdis_codec_quality",			&video_InterfaceDef::video_nop },
	{ "video_set_h263_mode",			&video_InterfaceDef::video_nop },
    { "shdis_set_h263_mode",			&video_InterfaceDef::video_nop },
    { "video_set_mjpeg_buffering",		&video_InterfaceDef::video_nop },
    { "shdis_set_mjpeg_buffering",		&video_InterfaceDef::video_nop },
    { "video_throw_lost",				&video_InterfaceDef::video_nop },
    { "video_cell_threshold",			&video_InterfaceDef::video_nop },
    { "shdis_cell_threshold",			&video_InterfaceDef::video_nop },
    { "video_cell_refresh_period",		&video_InterfaceDef::video_nop },
    { "shdis_cell_refresh_period",		&video_InterfaceDef::video_nop },
    { "shdis_set_mpeg_pbetweeni",		&video_InterfaceDef::video_nop },
    
	//
    // sources
    //
    
	{ "video_list_sources",				&video_InterfaceDef::video_list_sources   },
    { "shdis_list_sources",				&video_InterfaceDef::video_list_sources   },
	{ "video_select_source",			&video_InterfaceDef::video_select_source  },
    { "shdis_select_source",			&video_InterfaceDef::video_nop  },
	{ "video_current_source",			&video_InterfaceDef::video_current_source },
	{ "shdis_current_source",			&video_InterfaceDef::video_current_source },
	{ "video_source_quality",			&video_InterfaceDef::video_nop },
	{ "video_list_source_formats",		&video_InterfaceDef::video_list_source_formats },
	{ "video_source_format",			&video_InterfaceDef::video_source_format  },
    { "video_grab_size",				&video_InterfaceDef::video_grab_size  },
	{ "video_set_test_source",          &video_InterfaceDef::video_set_test_source  },
	{ "video_set_shared_display_source",&video_InterfaceDef::video_set_shared_display_source  },
	{ "video_frame_rate",               &video_InterfaceDef::video_set_source_rate  },
	{ "video_set_rect",                 &video_InterfaceDef::video_set_rect  },
   	{ "shdis_set_rect",                 &video_InterfaceDef::video_set_rect  },
   
    { "shdis_interactive_select",		&video_InterfaceDef::video_nop  },
    { "shdis_select_by_winid",			&video_InterfaceDef::video_nop },

	//
	// system
	//
	{ "video_nop",						&video_InterfaceDef::video_nop  },
	{ "shdis_nop",						&video_InterfaceDef::video_nop  },
    { "video_bye",						&video_InterfaceDef::video_bye },
    { "shdis_bye",						&video_InterfaceDef::video_bye },
	{ "video_quit",						&video_InterfaceDef::video_quit  },
	{ "shdis_quit",						&video_InterfaceDef::video_quit  },
	{ "shdis_query_id",					&video_InterfaceDef::video_nop },

    { NULL, NULL }

};


char const *
video_InterfaceDef_t::video_query_id(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_query_id");
    static char result[4096];
    INTER_VRFY_ARGC(result, 1);
    //NOTIFY("Control :: video_query_id()\n");

	itoa((u32)this,result,10);
	strcat(result,"\n");
	
    //NOTIFY("Control :: return %s\n");
    return result;
	
}

char const *
video_InterfaceDef_t::video_assign_channel(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_assign_channel");
    
    static char result[4096];
    INTER_VRFY_ARGC(result, 2);
	//NOTIFY("Control :: video_assign_channel(%s)\n",argv[1]);
    int chId = atoi(argv[1]);
	
	if ((channelList->lookUp(chId))!=NULL)
    {
        //NOTIFY("Control :: return ERROR : Channel busy\n");	
		return "Channel busy\n";
    }
	
	char name[100];
	memset(name,0,100);
	sprintf(name,"Localhost %d",chId);
	recChannel_t * channel = new recChannel_t(chId, name);
		
	if(channel->get_camInfo()->getKind() != TEST)
    {
	    //NOTIFY("Control :: return OK\n");	
        return "OK\n";
	}else{
        //NOTIFY("Control :: return OK : Not free sources found, setting test source...\n");	
		return "OK : Not free sources found, setting test source...\n";
	}
}

char const *
video_InterfaceDef_t::video_winGrabber_channel(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_winGrabber_channel");
    static char result[4096];
    INTER_VRFY_ARGC(result, 2);
	int chId = atoi(argv[1]);
	
    ////NOTIFY("Control :: video_winGrabber_channel(%s)\n",argv[1]);

    recChannel_t * channel = NULL;
	if ((channel = static_cast<recChannel_t*>(channelList->lookUp(chId)))!=NULL)
    {
        NOTIFY("SHDISP channel %d is created, mapping...\n",chId);
   	    channel->map();    
        return "OK\n";
    }

	char name[100];
	memset(name,0,100);
	sprintf(name,"Localhost %d",chId);
	channel = new recChannel_t(chId, name);
	char sourceName[20];
	sprintf(sourceName,"sharedDisplay%d",channel->getId());

	camInfo_t *camInfo = createSharedDisplay(channel, sourceName);

	channel->set_all(true);
	channel->select_source(camInfo);
    
    ////NOTIFY("Control :: return OK\n");
	return "OK\n";
}
		


char const *
video_InterfaceDef_t::video_new_channel(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_new_channel");
    static char result[4096];
    INTER_VRFY_ARGC(result, 2);
	int chId = atoi(argv[1]);
	
    ////NOTIFY("Control :: video_new_channel(%s)\n",argv[1]);

	if ((channelList->lookUp(chId))!=NULL)
    {
        ////NOTIFY("Control :: return Channel bussy\n");
		return "Channel busy\n";
    }
	
	char name[100];
	memset(name,0,100);
	sprintf(name,"Remote %d",chId);
	playChannel_t * channel = new playChannel_t(chId, name);
		
	////NOTIFY("Control :: return OK\n");
    return "OK\n";
	
}

char const *
video_InterfaceDef_t::video_delete_channel(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_delete_channel");
    static char result[4096];
    INTER_VRFY_ARGC(result, 2);
    int   chId= atoi(argv[1]);
	//NOTIFY("Control :: video_delete_channel(%s)\n",argv[1]);
    
    recChannel_t * recChannel = NULL;
	playChannel_t * playChannel = NULL;
	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n"); 
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {
		recChannel = static_cast<recChannel_t *>(channel);
		delete recChannel;
	}
	
	if (channel->getKind() == PLAY)
    {
		playChannel = static_cast<playChannel_t *>(channel);
		delete playChannel;
	}
	
    ////NOTIFY("Control :: return OK\n");
   	return "OK\n";
}

char const *
video_InterfaceDef_t::video_map_channel(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_map_channel");
    static char result[4096];
    INTER_VRFY_ARGC(result, 2);
    int   chId= atoi(argv[1]);
	////NOTIFY("Control :: video_map_channel(%s)\n",argv[1]);

	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (!channel->getMapping())
	{
		int hr = channel->map();
	}
    return "OK: Channel will be mapped on the first received KeyFrame\n";
}

char const *
video_InterfaceDef_t::video_unmap_channel(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_unmap_channel");
    static char result[4096];
    INTER_VRFY_ARGC(result, 2);
    int   chId= atoi(argv[1]);
	////NOTIFY("Control :: video_unmap_channel(%s)\n",argv[1]);

	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	int hr = channel->unmap();
	if (hr)
    {
	    ////NOTIFY("Control :: return ERROR: can't unmap REC channel\n");
        return "ERROR: can't unmap REC channel\n";
    
    }else{
        
        ////NOTIFY("Control :: return OK\n");
		return "OK\n";
    }
}

char const *
video_InterfaceDef_t::video_set_buffering(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_buffering");
	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
    int   chId= atoi(argv[1]);
	int   buffer = atoi(argv[2]);

    ////NOTIFY("Control :: video_set_buffering(%s,%s)\n",
           //argv[1],
           //argv[2]);

	if (buffer<5)
		buffer = 5;

	if (buffer>200)
		buffer = 200;
		
	channel_t * channel = NULL;
	playChannel_t * playChannel = NULL;

	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == PLAY)
    {
		playChannel = static_cast<playChannel_t *>(channel);
		playChannel->setBuffer(buffer);

        ////NOTIFY("Control :: return OK\n");
		return "OK\n";
	}

    ////NOTIFY("Control :: return ERROR: Bad Channel\n");
	return "ERROR: Bad Channel\n";
}


char const *
video_InterfaceDef_t::video_set_window_title(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_window_title");
	static char result[4096];
    INTER_VRFY_ARGC(result, 4);
    int   chId= atoi(argv[1]);
	
    ////NOTIFY("Control :: video_set_window_title(%s,%s,%s)\n",
           //argv[1],
           //argv[2],
           //argv[3]);

	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	int hr = channel->set_window_title(argv[3]);
		hr = channel->set_acronym(argv[2]);
	if (!hr)
    {
        ////NOTIFY("Control :: return OK\n");
		return "OK\n";
    }else{
        ////NOTIFY("Control :: return ERROR\n");
		return "ERROR\n";
    }
}

char const *
video_InterfaceDef_t::video_set_window_geometry(int argc, char **argv)
{	
    __CONTEXT("video_InterfaceDef_t::video_set_window_geometry");
	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
    
    ////NOTIFY("Control :: video_set_window_geometry(%s,%s)\n",
           //argv[1],
           //argv[2]);

    windowInfo_t wInfo;
    memset(&wInfo,0,sizeof(wInfo));
    char aux[4];
    memset(aux,0,strlen(aux));
    int i=0;
    int j=0;
    
    for (;argv[2][i] != 'x' && argv[2][i] != 0 && argv[2][i] != '+';i++,j++)
    {
        aux[j]=argv[2][i];
    }
    wInfo.width = atoi(aux);
    memset(aux,0,strlen(aux));
    j = 0;
    if (argv[2][i] && argv[2][i] != '+')
    {
        i++;
        for (;argv[2][i] != '+' && argv[2][i] != 0 ;i++,j++)
        {
            aux[j]=argv[2][i];
        }
        wInfo.heigth = atoi(aux)+30;
        memset(aux,0,strlen(aux));
        j = 0;
    }
    if (argv[2][i])
    {
        i++;
        for (;argv[2][i] != '+' && argv[2][i] != 0;i++,j++)
        {
            aux[j]=argv[2][i];
        }
        wInfo.left = atoi(aux);
        memset(aux,0,strlen(aux));
        j = 0;
    }
    if (argv[2][i])
    {
        i++;
        for (;argv[2][i] != 0;i++,j++)
        {
            aux[j]=argv[2][i];
        }
        wInfo.top = atoi(aux);
    }
    channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(atoi(argv[1])))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	int hr = channel->set_window_geometry(wInfo);
	
    if (hr)
    {
        ////NOTIFY("Control :: return ERROR: can't set window geometry\n");
		return "ERROR: can't set window geometry\n";
    }else{
        ////NOTIFY("Control :: return OK\n");
		return "OK\n";
    }
}
char const *
video_InterfaceDef_t::video_set_window_size(int argc, char **argv)
{	
    __CONTEXT("video_InterfaceDef_t::video_set_window_size");
	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
 	int   chId  = atoi(argv[1]);
	
    ////NOTIFY("Control :: video_set_window_size(%s,%s)\n",
           //argv[1],
           //argv[2]);

	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	windowInfo_t wInfo = channel->get_window_geometry();
		
	char width[5];
	char heigth[5];
	memset(width,0,5);
	memset(heigth,0,5);
	int i=0;
	for (i = 0;argv[2][i]!='x' && argv[2][i]!='X';i++)
	{
		 width[i] = argv[2][i];
	}
	
    int j=0;
	for(i++;argv[2][i]!=0;i++,j++)
	{
		heigth[j] = argv[2][i];
	}

	wInfo.width = atoi(width);
	wInfo.heigth= atoi(heigth);

	int hr = channel->set_window_geometry(wInfo);
	if (channel->getMapping()) channel->pControl->Run();
	
    if (hr)
    {
        ////NOTIFY("Control :: return ERROR: can't set window size\n");
		return "ERROR: can't set window size\n";
    
    }else{

        ////NOTIFY("Control :: return OK\n");
		return "OK\n";
    }
}

char const *
video_InterfaceDef_t::video_follow_size(int argc, char **argv)
{	
    __CONTEXT("video_InterfaceDef_t::video_follow_size");
	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
 	int   chId   = atoi(argv[1]);
	bool  followFlag= atoi(argv[2]) != 0;
	int hr = 0;
	channel_t * channel = NULL;
	
    //NOTIFY("Control :: video_follow_size(%s,%s)\n", argv[1], argv[2]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        ////NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind()!= PLAY)
    {
	    ////NOTIFY("Control :: return ERROR: This command only works on play channels\n");
        return "ERROR: This command only works on play channels\n";
    }

	playChannel_t * playChannel = static_cast<playChannel_t *>(channel);

	playChannel->setFollowSize(followFlag);
	
	if (followFlag)
    {
	    if (playChannel->getMapping())
        {
			playChannel->map();
        }
		////NOTIFY("Control :: return OK\n");
		return "OK\n";
    }
}

char const *
video_InterfaceDef_t::video_set_window_owner(int argc, char **argv)
{	
    static char result[4096];
    INTER_VRFY_ARGC(result, 3);
 	int      chId   = atoi(argv[1]);
	OAHWND   HWnd   = (unsigned long)atol(argv[2]);
	int      hr     = 0;
	channel_t * channel = NULL;
	NOTIFY("Channel %d owner is now %d\n",chId,HWnd);
	if((channel = channelList->lookUp(chId))==NULL)
    {
		return "ERROR: Bad Channel\n";
    }
	channel->setOwner(HWnd);	
	return "OK";
}

char const *
video_InterfaceDef_t::video_full_screen_window(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_full_screen_window");
	
	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
 	int   chId  = atoi(argv[1]);
	bool  active = false;
	int hr = 0;
	channel_t * channel = NULL;
	
    ////NOTIFY("Control :: video_full_screen_window(%s,%s)\n",
           //argv[1],
           //argv[2]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
	
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
        return "ERROR: Bad Channel\n";
    }
	
	if (strcmp(argv[2],"true")==0) active = true;
	
	channel->setFullScreen(active);
	if (channel->getMapping())
	{
			channel->map();
	}
	
	if (hr)
    {
        //NOTIFY("Control :: return ERROR: can't set full screen window\n");
		return "ERROR: can't set full screen window\n";
    }else{
        
        //NOTIFY("Control :: return OK\n");
		return "OK\n";
    }
}
const char *
video_InterfaceDef_t::video_configure_channel(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_configure_channel");
    if (argc!=7)
    {
        return "ERROR: video_channel_configure(chId,fr,grabSize,codecId,qlty,bw) \n";
    }

    int chId = atoi(argv[1]);
    int fr   = atoi(argv[2]); 
    int BW   = atoi(argv[6]);
    
    //char *  codecId = argv[4] <-- MPEG4 forced
    //int Q    = atoi(argv[5]); <-- Use CBR Q not supported

	captureInfo_t cInfo;	
	char width[5];
	char heigth[5];
	memset(width,0,5);
	memset(heigth,0,5);
	
	int i=0;
	for (i = 0;argv[3][i]!='x' && argv[3][i]!='X';i++)
	{
		 width[i] = argv[3][i];
	}
	cInfo.width = atoi(width);
	int j=0;
	for(i++;argv[3][i]!=0;i++,j++)
	{
		heigth[j] = argv[3][i];
	}
	cInfo.heigth= atoi(heigth);

    //find channel
    channel_t * channel = NULL;
    if((channel = channelList->lookUp(chId))==NULL)
    {
	    return "ERROR: Bad Channel\n";
    }
    if (channel->getKind()!= REC)
    {
	    return "ERROR: This command only works on REC channels\n";
    }

    recChannel_t * recChannel = NULL;
    recChannel = static_cast<recChannel_t *>(channel);
    recChannel->set_rate(fr); //set rate

    //set grab size
    if (recChannel->get_camInfo()->getKind() == CAM)
    {
		recChannel->set_captureInfo(cInfo);
    }

    //set bandwidth (that refresh channel)
    recChannel->pSender->set_bandwidth(BW);
    if (recChannel->getMapping())
    {
        recChannel->set_all(false);
        recChannel->select_source(recChannel->get_camInfo()); 
    }
    
    return "OK\n";
}
   
const char *
video_InterfaceDef_t::video_play(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_play");
   	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
 	int    chId    = atoi(argv[1]);
	double period  = (33.3333/atof(argv[2]))*30; //normilized in millis
	int hr = 0;
	
    channel_t * channel = NULL;
    recChannel_t * recChannel = NULL;

    if (period<1 || period >30)
    {
        return "ERROR: Bad period\n";
    }

	if((channel = channelList->lookUp(chId))==NULL)
    {
	    //NOTIFY("Control :: return ERROR: Bad Channel\n");
        return "ERROR: Bad Channel\n";
    }
	
    if (channel->getKind()!= REC)
    {
	    ////NOTIFY("Control :: return ERROR: This command only works on REC channels\n");
        return "ERROR: This command only works on REC channels\n";
    }

    recChannel = static_cast<recChannel_t *>(channel);
    recChannel->set_rate(period);
    
    if (!recChannel->getMapping())
	{
        recChannel->select_source(recChannel->get_camInfo());
        recChannel->map();
    }
    return "OK\n";
}

const char *
video_InterfaceDef_t::video_stop(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_stop");
   	static char result[4096];
    INTER_VRFY_ARGC(result, 2);
 	int    chId    = atoi(argv[1]);

    channel_t * channel = NULL;
    recChannel_t * recChannel = NULL;
 
    if((channel = channelList->lookUp(chId))==NULL)
    {
	    //NOTIFY("Control :: return ERROR: Bad Channel\n");
        return "ERROR: Bad Channel\n";
    }
	
    if (channel->getKind()!= REC)
    {
	    ////NOTIFY("Control :: return ERROR: This command only works on REC channels\n");
        return "ERROR: This command only works on REC channels\n";
    }

    recChannel = static_cast<recChannel_t *>(channel);
    recChannel->unmap();
    return "OK\n";
}


//
// bindings
//
char const *
video_InterfaceDef_t::video_bind(int argc, char**argv)
{

    __CONTEXT("video_InterfaceDef_t::video_bind");
	static char result[4096];
    INTER_VRFY_ARGC(result, 4);
 	int   chId  = atoi(argv[1]);
	char * dest = argv[2];
	char * port = argv[3];
	int hr = 0;
	channel_t *    channel    = NULL;
	recChannel_t * recChannel = NULL;

    //NOTIFY("Control :: video_bind(%s,%s,%s)\n",
           //argv[1],
           //argv[2],
           //argv[3]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
	{
		recChannel = static_cast<recChannel_t *>(channel);	
		itoa(recChannel->pSender->video_bind(dest,port),result,10);
		strcat(result,"\n");
        //NOTIFY("Control :: return %s",result);
		return result;
	}else{
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
	}
}

char const *
video_InterfaceDef_t::video_unbind(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_unbind");

	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
 	int   chId  = atoi(argv[1]);
	int   ID    = atoi(argv[2]);
	
	channel_t *    channel    = NULL;
	recChannel_t * recChannel = NULL;

    //NOTIFY("Control :: video_unbind(%s,%s)\n",
           //argv[1],
           //argv[2]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
	{
		recChannel = static_cast<recChannel_t *>(channel);	
		bool hr = recChannel->pSender->video_unbind(ID);
		if (hr)
		{
            //NOTIFY("Control :: return OK\n");
			return "OK\n";
		}else{
            //NOTIFY("Control :: return ID not found in bindList\n");
			return "ID not found in bindList\n";
		}
	}else{
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
	}

}

char const *
video_InterfaceDef_t::video_show_bindings(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_show_bindings");

	static char result[4096];
    INTER_VRFY_ARGC(result, 2);
 	int   chId  = atoi(argv[1]);
	
	channel_t *    channel    = NULL;
	recChannel_t * recChannel = NULL;

    //NOTIFY("Control :: video_show_bindings(%s)\n",argv[1]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
	    //NOTIFY("Control :: return ERROR: Bad Channel\n");
        return "ERROR: Bad Channel\n";
    }

	recChannel = static_cast<recChannel_t *>(channel);	
	strcpy(result,recChannel->pSender->video_show_bindings());
	strcat(result,"\n");
    //NOTIFY("Control :: return %s",result);
	return result;
		
}

char const *
video_InterfaceDef_t::video_channel_bandwidth (int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_channel_bandwidth");
	static char result[4096];
    INTER_VRFY_ARGC(result, 3);
 	int   chId  = atoi(argv[1]);
	long  BW    = atoi(argv[2]);
	
	channel_t *    channel    = NULL;
	recChannel_t * recChannel = NULL;

    //NOTIFY("Control :: video_channel_bandwidth (%s,%s)\n",
           //argv[1],
           //argv[2]);

	if((channel = channelList->lookUp(chId))!=NULL)
    {
		if (channel->getKind() == REC)
		{
			recChannel = static_cast<recChannel_t *>(channel);	
			if (recChannel->pSender->set_bandwidth(BW))
            {
                if (recChannel->getMapping())
                {
                    recChannel->set_all(false);
                    recChannel->select_source(recChannel->get_camInfo()); 
                }
            }
            return "OK\n";
		}
    }

    //NOTIFY("Control :: return ERROR: Bad Channel\n");
    return "ERROR: Bad Channel\n";
}

char const *
video_InterfaceDef_t::video_bandwidth_on(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_bandwidth_on");
	static char result[4096];
    INTER_VRFY_ARGC(result, 2);
 	int   chId  = atoi(argv[1]);
	
	channel_t *    channel    = NULL;
	recChannel_t * recChannel = NULL;

    //NOTIFY("Control :: video_bandwidth_on(%s,%s)\n",
           //argv[1],
           //argv[2]);

	if((channel = channelList->lookUp(chId))!=NULL)
    {
		if (channel->getKind() == REC)
		{
				recChannel = static_cast<recChannel_t *>(channel);	
				recChannel->pSender->RunBWControl(true);
	            //NOTIFY("Control :: return OK\n");
				return "OK\n";
		}
    }

    //NOTIFY("Control :: return ERROR: Bad Channel\n");
	return "ERROR: Bad Channel\n";
}

char const *
video_InterfaceDef_t::video_bandwidth_off(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_bandwidth_off");
	static char result[4096];
    INTER_VRFY_ARGC(result, 2);
 	int   chId  = atoi(argv[1]);
	
	channel_t *    channel    = NULL;
	recChannel_t * recChannel = NULL;

    //NOTIFY("Control :: video_bandwidth_off(%s,%s)\n",
           //argv[1],
           //argv[2]);

	if((channel = channelList->lookUp(chId))!=NULL)
    {
		if (channel->getKind() == REC)
		{
				recChannel = static_cast<recChannel_t *>(channel);	
				recChannel->pSender->RunBWControl(false);
	            
                //NOTIFY("Control :: return OK\n");
				return "OK\n";
		}
    }

    //NOTIFY("Control :: return ERROR: Bad Channel\n");
	return "ERROR: Bad Channel\n";
}

//
// codecs
//

char const *
video_InterfaceDef_t::video_list_codecs(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_list_codecs");
	static char result[4096];
	INTER_VRFY_ARGC(result, 1);
    //NOTIFY("Control :: video_list_codecs()\n");

//this code is used to list system codecs
#if 0
    channel_t * channel;
	recChannel_t * recChannel;
		
	ql_t <int> * channelIdList = channelList->getKeys();
	if (channelIdList->len())
	{
		
		for (int n=0;n<channelIdList->len();n++)
		{	
			channel = channelList->lookUp(channelIdList->nth(n));
			if (channel->getKind() == REC)
			{
				recChannel = static_cast<recChannel_t *>(channel);	
				strcpy(result,recChannel->pSender->GetCodecList());
				strcat(result,"\n");
                //NOTIFY("Control :: return %s",result);
				return result;
			}
		}
		return "ERROR: Must have at least one REC channel created\n";
	}else{
		return "ERROR: Must have at least one REC channel created\n";
	}
	delete  channelIdList;
#else
    //ffvfw codecs
    strcpy(result,SUPPORTED_CODECS);
    strcat(result,"\n");
    //NOTIFY("Control :: return %s",result);
    return result;
#endif

}

char const *
video_InterfaceDef_t::video_select_codec(int argc, char** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_select_codec");

#ifdef __ARES
    static char result[4096];
	INTER_VRFY_ARGC(result, 3);
	channel_t * channel;
	recChannel_t * recChannel;

	int   chId   = atoi(argv[1]);
	char * codec = argv[2];
	
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }
	
	recChannel = static_cast<recChannel_t *>(channel);	
    recChannel->pSender->SetActualCodec(codec);

	recChannel->set_all(true);
	recChannel->select_source(recChannel->get_camInfo());

#else
    NOTIFY("WARNING :: IVideo forced to use always MPEG4 (by Vicen)\n");
#endif
    return "OK\n";

}

char const *
video_InterfaceDef_t::video_list_codecs_ex(int argc, char** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_list_codecs_ex");
    //NOTIFY("Control :: video_list_codecs_ex()\n");
    //NOTIFY("Control :: return"
    //       "{{MPEG4,MPEG4,ffvfw codec MPEG4}"
    //       "{MPEG1,MPEG1,ffvfw codec MPEG1}"
    //       "{H263,H263,ffvfw codec H263}}\n"
    //		);

    return "{{MPEG4,MPEG-4,ffvfw codec MPEG4}"
           ",{MPEG1,MPEG-1,ffvfw codec MPEG1}"
           ",{H263,H.263,ffvfw codec H263}}\n";
}
char const *
video_InterfaceDef_t::video_set_payload(int argc, char** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_payload");

	static char result[4096];
	INTER_VRFY_ARGC(result, 3);
	channel_t * channel;
	recChannel_t * recChannel;

    //NOTIFY("Control :: video_set_payload(%s,%s)\n",
           //argv[1],
           //argv[2]);

	int   chId   = atoi(argv[1]);
	char * payload = argv[2];

	if((channel = channelList->lookUp(chId))==NULL)
    {
	    //NOTIFY("Control :: return ERROR: Bad Channel\n");
        return "ERROR: Bad Channel\n";
    }
	
	recChannel = static_cast<recChannel_t *>(channel);
	if(!recChannel->pSender->sampleGrabber->SetPayLoad(payload))
    {
        //NOTIFY("Control :: return OK\n");
		return "OK\n";
    }else{
        //NOTIFY("Control :: return Unknown payload type\n");
		return "ERROR: Unknown payload type\n";
    }
}
	
//
// sources
//

char const *
video_InterfaceDef_t::video_list_sources(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_list_sources");

    static char result[4096];
    INTER_VRFY_ARGC(result, 1);
    //NOTIFY("Control :: video_list_sources()\n");

	char srcInfo[4096];
	enumVideoSources(srcInfo);

    if (srcInfo == NULL)
    {
    
        //NOTIFY("Control :: return ERROR: no source available\n");
		return "ERROR: no source available\n";
    }

    sprintf(result, "%s\n", srcInfo);
    //NOTIFY("Control :: return %s",result);
    return result;
}


char const *
video_InterfaceDef_t::video_select_source(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_select_source");
    static char result[4096];
    
	INTER_VRFY_ARGC(result,3);
	int hr = 0;
    bool free = false;
	int    chId   = atoi(argv[1]);
    char  *camera = argv[2];
	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
    //NOTIFY("Control :: video_select_source(%s,%s)\n",
           //argv[1],
           //argv[2]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {
		recChannel = static_cast<recChannel_t *>(channel);

		camInfo_t *camInfo = NULL;

		for (int  i= 0; i < camArray.size(); i++)
        {
			camInfo= camArray.elementAt(i);
			if (strcmp(camInfo->getCamName(), camera) == 0 && camInfo->getFree())
            {
				free = true;
				break;
			}
		}
		if (free)
        {
			recChannel->set_all(true);
			hr = recChannel->select_source(camInfo);
		}
		else
		{	
			if (strcmp(camera, "testSource")==0)
			{
				argv[2] = NULL;
				return video_set_test_source(argc-1,argv);
			}
			if (strcmp(camera,"winGrabber")==0)
			{
				argv[2] = NULL;
				return video_set_shared_display_source(argc-1,argv);
			}

			if (strstr(camera,"file"))
            {
				int i=0;

				char fileName[100];
				memset(fileName,0,100);

				do
                {
					  i++;
				}while(argv[2][i]!=':');
				
                i++;
				int j = 0;
				do
                {
					fileName[j] = argv[2][i];
					i++;
					j++;
				}while(argv[2][i]);
				
				strcpy(argv[2],fileName);
				return video_set_test_source(argc, argv);
			}
            //NOTIFY("Control :: return ERROR: Source is bussy\n");
			return "ERROR: Source is bussy\n";
		
		}
		errorCheck(hr);

		//NOTIFY("Control :: return OK\n");
		return "OK\n";
	}
	else
	{
		sprintf(result,"ERROR: Channel %d is not a REC channel\n",chId);
        //NOTIFY("Control :: return %s",result);
		return result;
	}
}

char const *
video_InterfaceDef_t::video_current_source(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_current_source");
    static char result[4096];
    INTER_VRFY_ARGC(result, 2);
    int   chId= atoi(argv[1]);
	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
    //NOTIFY("Control :: video_current_source(%s)\n",
           //argv[1]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {

		recChannel = static_cast<recChannel_t *>(channel);
		const char *srcInfo = recChannel->current_source();

		if (srcInfo == NULL)
        {
            //NOTIFY("Control :: return ERROR: no source available\n");
			return "ERROR: no source available\n";
        }

		sprintf(result, "%s\n", srcInfo);
        //NOTIFY("Control :: return %s",result);
		return result;

	}else{
		
        sprintf(result,"ERROR: Channel %d is not a REC channel\n",chId);
        //NOTIFY("Control :: return %s",result);
		return result;
	}
}

char const *
video_InterfaceDef_t::video_list_source_formats(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_list_source_formats");

	static char result[4096];
    INTER_VRFY_ARGC(result,2);
    
	int   chId= atoi(argv[1]);
	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
    //NOTIFY("Control :: video_list_source_formats(%s)\n",
           //argv[1]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {

		recChannel = static_cast<recChannel_t *>(channel);
		sprintf(result, "%s\n", recChannel->get_camInfo()->getSupportedFormats());
        //NOTIFY("Control :: return %s",result);
		return result;
	
    }else{
	
        sprintf(result,"ERROR: Channel %d is not a REC channel\n",chId);
        //NOTIFY("Control :: return %s",result);
		return result;
	
    }
}

char const *
video_InterfaceDef_t::video_source_format(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_source_format");
    //this code changes source format
    //actually DirectX administrates his own
    //formats, so let's DirectX work

#if 0

	static char result[4096];
    INTER_VRFY_ARGC(result,3);
    
	int   chId= atoi(argv[1]);
	char *format = argv[2];

	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }
	
	recChannel = static_cast<recChannel_t *>(channel);
	if (channel->getKind() == REC)
	{
		if (recChannel->get_camInfo()->getKind() == SHARED)
        {
			return "OK\n";
        }
		
		if (strstr(recChannel->get_camInfo()->getSupportedFormats(),format))
		{
			AM_MEDIA_TYPE * mediaFormat = NULL;
			int hr = recChannel->get_AM_MEDIA_TYPE(format,&mediaFormat);
			if (mediaFormat)
				recChannel->set_actual_format(*mediaFormat);
			strcpy(recChannel->get_source_format(),format);
			recChannel->set_all(true);
			recChannel->select_source(recChannel->get_camInfo());
			if (!hr)
            {
			    return "OK\n";
            }
		}else{

			return "ERROR: Could not change source format\n";
		}

	}else{

		sprintf(result,"ERROR: Channel %d is not a REC source\n",chId);
		//NOTIFY("Control :: return %s",result);
        return result;
	}
#else
    ////NOTIFY("Control :: return OK\n");
    return "OK\n";
#endif
}

char const *
video_InterfaceDef_t::video_grab_size(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_grab_geometry");
    static  char    result[4096];
	int hr = 0;
	captureInfo_t cInfo;
	
    INTER_VRFY_ARGC(result, 3);

	int chId    = atoi(argv[1]);

    //NOTIFY("Control :: video_set_grab_geometry(%s,%s)\n",
           //argv[1],
           //argv[2]);
	
	char width[5];
	char heigth[5];
	memset(width,0,5);
	memset(heigth,0,5);
	
	int i=0;
	for (i = 0;argv[2][i]!='x' && argv[2][i]!='X';i++)
	{
		 width[i] = argv[2][i];
	}
	cInfo.width = atoi(width);
	int j=0;
	for(i++;argv[2][i]!=0;i++,j++)
	{
		heigth[j] = argv[2][i];
	}
	cInfo.heigth= atoi(heigth);
		
	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	recChannel = static_cast<recChannel_t *>(channel);

	if (channel->getKind() == REC && 
        recChannel->get_camInfo()->getKind() == CAM)
    {

		recChannel = static_cast<recChannel_t *>(channel);
		if (recChannel->set_captureInfo(cInfo))
        {
            recChannel->set_all(false);
            recChannel->select_source(recChannel->get_camInfo());
            //NOTIFY("Control :: return OK\n");
            return "OK\n";    
		}
        //NOTIFY("Control :: return ERROR:Resolution not supported\n");
	    return "ERROR:Resolution not supported\n";

	}else{

		sprintf(result,"ERROR: Channel %d isn't using CAM source\n",chId);
        //NOTIFY("Control :: return %s",result);
		return result;
	}
}

char const *
video_InterfaceDef_t::video_set_test_source(int argc, char** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_test_source");
    static char result[4096];
    char * fileName;
	if (argc!=3)
    {
		INTER_VRFY_ARGC(result,2);
        //NOTIFY("Control :: video_set_test_source(%s)\n",argv[1]);
    }

    int   chId= atoi(argv[1]);
	
    if (argc==3) 
    {
		fileName = argv[2];
        //NOTIFY("Control :: video_set_test_source(%s,%s)\n",argv[1],argv[2]);
    }else{
		fileName = NULL;
    }

	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {
	
        recChannel = static_cast<recChannel_t *>(channel);
        recChannel->set_file_source(fileName);
        //NOTIFY("Control :: return OK\n");
        return "OK\n";
    
    }else{

		sprintf(result,"ERROR: Channel %d is not a REC channel\n",chId);
        //NOTIFY("Control :: return %s",result);
		return result;
	}
}

char const *
video_InterfaceDef_t::video_set_shared_display_source(int argc, char** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_shared_display_source");
    static char result[4096];
	INTER_VRFY_ARGC(result,2);
    int   chId= atoi(argv[1]);

	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
    //NOTIFY("Control :: video_set_shared_display_source(%s)\n",argv[1]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {
			
		recChannel = static_cast<recChannel_t *>(channel);
    	recChannel->pControl->StopWhenReady();
	
		for(int j=0; j < camArray.size(); j++)
		{
			camInfo_t *camInfo= camArray.elementAt(j);
			if (camInfo->getFree() && camInfo->getKind() == SHARED)
			{
				recChannel->set_all(true);
				recChannel->select_source(camInfo);

                //NOTIFY("Control :: return OK\n");
				return "OK\n";
			}
		}

        char sourceName[20];
        sprintf(sourceName, "sharedDisplay%d", channel->getId()); 
        camInfo_t *camInfo= createSharedDisplay(channel, sourceName);
        
        recChannel->set_all(true);
        recChannel->select_source(camInfo);
        
        //NOTIFY("Control :: return OK\n");
        return "OK\n";
	}
	else
	{
        sprintf(result, "ERROR: Channel %d is not a REC channel\n", chId);

		//NOTIFY("Control :: return %s",result);
		return result;
	}
}

char const *
video_InterfaceDef_t::video_set_rect(int argc, char** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_rect");
    static char result[4096];
	INTER_VRFY_ARGC(result,3);
    
    int   chId= atoi(argv[1]);
	
    RECT  newRect;
    windowInfo_t wInfo;

    char aux[4];
    memset(aux,0,strlen(aux));
    int i=0;
    int j=0;
    
    for (;argv[2][i] != 'x' && argv[2][i] != 0;i++,j++)
    {
        aux[j]=argv[2][i];
    }
    wInfo.width = atoi(aux);
    memset(aux,0,strlen(aux));
    j = 0;
    i++;
    for (;argv[2][i] != '+' && argv[2][i] != 0;i++,j++)
    {
        aux[j]=argv[2][i];
    }
    wInfo.heigth = atoi(aux);
    memset(aux,0,strlen(aux));
    j = 0;
    i++;
    for (;argv[2][i] != '+' && argv[2][i] != 0;i++,j++)
    {
        aux[j]=argv[2][i];
    }
    wInfo.left = atoi(aux);
    memset(aux,0,strlen(aux));
    j = 0;
    i++;
    for (;argv[2][i] != 0;i++,j++)
    {
        aux[j]=argv[2][i];
    }
    wInfo.top = atoi(aux);
    	

    newRect.bottom = wInfo.top + wInfo.heigth;
    newRect.left   = wInfo.left;
    newRect.right  = wInfo.left + wInfo.width;
    newRect.top    = wInfo.top;

    NOTIFY("set_rect :: RECT[top=%d,bottom=%d,left=%d,right=%d]\n",
           newRect.top,
           newRect.bottom,
           newRect.left,
           newRect.right);

    recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
    //NOTIFY("Control :: video_set_rect(%s,%s,%s,%s,%s)\n",
           //argv[1],
           //argv[2],
           //argv[3],
           //argv[4],
           //argv[5]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {
	
        recChannel = static_cast<recChannel_t *>(channel);
        if (recChannel->get_camInfo()->getKind() != SHARED)
        {
            //NOTIFY("Control :: return ERROR: The source isn't a winGrabber\n");
            return "ERROR: The source isn't a winGrabber\n";
        }
        
        recChannel->pControl->Stop();
        if (recChannel->get_camInfo()->setSharedRect(newRect))
        {
            recChannel->select_source(recChannel->get_camInfo());
        }else{
            recChannel->map();
        }

        //NOTIFY("Control :: return OK\n");
		return "OK\n";        
	}

	return "ERROR: Not a REC Channel\n";
}
			
char const *
video_InterfaceDef_t::video_set_source_rate(int argc, char** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_source_rate");
    static char result[4096];
    INTER_VRFY_ARGC(result, 3);
    int    chId   = atoi(argv[1]);
	float  FR     = atof(argv[2]);

	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
    //NOTIFY("Control :: video_set_source_rate(%s,%s)\n",
           //argv[1],
           //argv[2]);

	if((channel = channelList->lookUp(chId))==NULL)
    {
        //NOTIFY("Control :: return ERROR: Bad Channel\n");
		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {
	
        recChannel = static_cast<recChannel_t *>(channel);
		recChannel->set_rate(FR);
		//NOTIFY("Control :: return OK\n");
        return "OK\n";

	}else{

		sprintf(result,"ERROR: Channel %d is not a REC channel\n",chId);
        //NOTIFY("Control :: return %s",result);
		return result;
	}

}

//
// system
//

char const *
video_InterfaceDef_t::video_nop(int argc, char **)
{
    return "OK\n";
}


char const *
video_InterfaceDef_t::video_bye(int argc, char **)
{
    __CONTEXT("video_InterfaceDef_t::video_bye");
    static char retVal[512];
    INTER_VRFY_ARGC(retVal, 1);
//   	s->removeTask(this);
    //NOTIFY("Control :: video_bye()\n");
    //NOTIFY("Control :: return OK\n");
    return "OK\n";
}


char const *
video_InterfaceDef_t::video_quit(int argc, char **)
{
    __CONTEXT("video_InterfaceDef_t::video_quit");
    static char retVal[512];
    //NOTIFY("Control :: video_quit()\n");
    INTER_VRFY_ARGC(retVal, 1);
	debugMsg(dbg_App_Normal,"controlProtocol_t","IVIDEO : exiting ...\n");
/*	
	ql_t<int> *channelIdList = channelList->getKeys();	
	for (int i=0; i<channelIdList->len();i++){
		debugMsg(dbg_App_Normal,"controlProtocol_t","IVIDEO : Killing channel %d\n",channelIdList->nth(i));
		delete channelList->lookUp(channelIdList->nth(i));
	}
	delete channelIdList;
	delete channelList;
	delete camList;

	task_ref task = static_cast<task_ref>(this);
	s->removeTask(task);
*/
	//NOTIFY("Control :: return OK\n");
	ExitProcess(0);
	return "OK\n";
	
}

//-- new functions --

char const *
video_InterfaceDef_t::video_rtcp_bind(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_rtcp_bind");
    static char result[4096];

    INTER_VRFY_ARGC(result, 3);

    char *host= argv[1];
    char *port= argv[2];

    int x = rtpSession->rtcpBind(host, port);

    if (x < 0) {
        return "ERROR: bad thing\n";
    }

    sprintf(result, "%d\n", x);
    //NOTIFY("Control :: return %s",result);
    return result;
}


char const *
video_InterfaceDef_t::video_rtcp_unbind(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_rtcp_unbind");
    static char result[4096];

    INTER_VRFY_ARGC(result, 2);

    int bindId= atoi(argv[1]);

    bool res= rtpSession->rtcpUnbind(bindId);

    if ( ! res) {
        return "ERROR: bad thing or invalid binding\n";
    }
    
    //NOTIFY("Control :: return OK\n");
    return "OK\n";
}


char const *
video_InterfaceDef_t::video_rtcp_show_bindings(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_rtcp_show_bindings");
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    char *retVal= rtpSession->rtcpShowBindings();

    if(!retVal) {
        return "ERROR: bad thing\n";
    }

    return retVal;
}


char const *
video_InterfaceDef_t::video_set_sdesinfo(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_set_sdesinfo");
    static char result[4096];

    INTER_VRFY_ARGC_range(result, 4, 5);

    int chId = atoi(argv[1]);

    int identifier = atoi(argv[2]);
    if ((identifier < 2) || (identifier > 8))
        return "ERROR: invalid sdes identifier\n";

    char *info = strdup(argv[3]);

    char *prefix = NULL;
    if (argc > 4){
        if (identifier != 8) {
            return "ERROR: prefix not permitted for identifier\n";
        }
        prefix = strdup(argv[4]);
    }

    if (prefix == NULL)
        rtpSession->setSDESInfo (chId, identifier, info);
    else
        rtpSession->setSDESInfo (chId, identifier, info, prefix);

    return "OK\n";
}


char const *
video_InterfaceDef_t::video_show_sdesinfo(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_show_sdesinfo");
    static char result[4096];

    INTER_VRFY_ARGC(result, 2);

    int chId = atoi (argv[1]);

    char *retVal= rtpSession->showSDESInfo(chId);

    if(!retVal) {
        return "ERROR: bad channel\n";
    }

    return retVal;
}


char const *
video_InterfaceDef_t::video_show_receivers(int argc, char**argv)
{
    __CONTEXT("video_InterfaceDef_t::video_show_receivers");
    static char result[4096];

    INTER_VRFY_ARGC(result, 2);

    int chId = atoi (argv[1]);

    char *retVal= rtpSession->showReceivers (chId);

    if(!retVal) {
        return "ERROR: bad channel\n";
    }

    return retVal;
}


char const *
video_InterfaceDef_t::video_get_lost(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_get_lost");
    static char result[4096];

    INTER_VRFY_ARGC(result, 3);

    int chId = atoi (argv[1]);
    u32 receiver = (u32)atoi (argv[2]);

    char *retVal= rtpSession->getPacketsLost(chId, receiver);

    if (!retVal) {
        return "ERROR: bad channel\n";
    }

    return retVal;
}


char const *
video_InterfaceDef_t::video_get_local_lost(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_get_local_lost");
    static char result[4096];

    INTER_VRFY_ARGC(result, 2);

    int chId = atoi (argv[1]);

    char *retVal= rtpSession->getPacketsLost(chId);

    if (!retVal) {
        return "ERROR: bad channel\n";
    }

    return retVal;
}


char const *
video_InterfaceDef_t::video_get_fractionlost(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_get_fractionlost");
    static char result[4096];

    INTER_VRFY_ARGC(result, 3);

    int chId = atoi (argv[1]);
    u32 receiver = (u32)atoi (argv[2]);

    char *retVal= rtpSession->getFractionLost(chId, receiver);

    if (!retVal) {
        return "ERROR: bad channel\n";
    }

    return retVal;
}


char const *
video_InterfaceDef_t::video_get_roundtrip_time(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_get_roundtrip_time");
    static char result[4096];

    INTER_VRFY_ARGC(result, 3);

    int chId = atoi (argv[1]);
    u32 receiver = (u32)atoi (argv[2]);

    char *retVal= rtpSession->getRoundTripTime(chId, receiver);

    if (!retVal) {
        return "ERROR: bad channel\n";
    }

    return retVal;
}


char const *
video_InterfaceDef_t::video_get_jitter(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_get_jitter");
    static char result[4096];

    INTER_VRFY_ARGC_range(result, 2, 3);

    char *retVal= NULL;

    int chId = atoi(argv[1]);

    if (argc == 2) {
        retVal= rtpSession->getJitter(chId);
    } else {
        if (argc == 3) {
            u32 receiver = (u32)atoi (argv[2]);
            retVal= rtpSession->getJitter(chId, receiver);
        }
    }

    if (!retVal) {
        return "ERROR: bad channel\n";
    }

    return retVal;
}

#define MAX_FRACTION (100.0)
char const *
video_InterfaceDef_t::video_compute_stats(int argc, char **argv)
{
    __CONTEXT("video_InterfaceDef_t::video_compute_stats");
    static char result[4096];

    INTER_VRFY_ARGC(result, 3);

    double bandwidth = (double)atof(argv[1]);
    double fraction  = (double)atof(argv[2]);

    if(fraction < 0) {
        return "ERROR: invalid fraction\n";
    }

    if(fraction> MAX_FRACTION) {
        fraction = MAX_FRACTION;
    }

    rtpSession->computeStatistics(bandwidth, fraction);

    return "OK\n";
}

char const *
video_InterfaceDef_t::video_bucket_size(int argc, char ** argv)
{
    __CONTEXT("video_InterfaceDef_t::video_bucket_size");

	static char result[4096];

    INTER_VRFY_ARGC(result, 3);

    int   chId= atoi(argv[1]);
	int   bucketSize = atoi(argv[2]);

	recChannel_t * recChannel = NULL;
	channel_t * channel = NULL;
	
	if((channel = channelList->lookUp(chId))==NULL)
    {

		return "ERROR: Bad Channel\n";
    }

	if (channel->getKind() == REC)
    {
			
        recChannel = static_cast<recChannel_t *>(channel);
        recChannel->set_bucket_size(bucketSize);
        //NOTIFY("Control :: return OK\n");
        return "OK\n";
    
    }else{
        
        return "ERROR: This command only works on REC Channels\n";
	}
}

