/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 <head> 
   <name>ctrlProtocol.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_CTRL_PROTOCOL_H__
#define _IVIDEO_CTRL_PROTOCOL_H__

#include "general.h"
#include <icf2/item.hh>


struct video_InterfaceDef
{
	//
	// channel
	//
	virtual char const *video_query_id		              (int, char**)=0;

	virtual char const *video_assign_channel              (int, char**)=0;
	virtual char const *video_winGrabber_channel          (int, char**)=0;
    virtual char const *video_new_channel                 (int, char**)=0;
    virtual char const *video_delete_channel              (int, char**)=0;

    virtual char const *video_map_channel                 (int, char**)=0;
    virtual char const *video_unmap_channel               (int, char**)=0;
	
	virtual char const *video_set_buffering               (int, char**)=0;
  
	virtual char const *video_set_window_title            (int, char**)=0;
    virtual char const *video_set_window_geometry         (int, char**)=0;
    virtual char const *video_set_window_size             (int, char**)=0;
	virtual char const *video_follow_size                 (int, char**)=0;
	virtual char const *video_full_screen_window          (int, char**)=0;
	virtual char const *video_set_window_owner            (int, char**)=0;

	
	//
	// bindings
	//
	virtual char const *video_bind                        (int, char**)=0;
    virtual char const *video_unbind                      (int, char**)=0;
    virtual char const *video_show_bindings               (int, char**)=0;
	virtual char const *video_channel_bandwidth           (int, char**)=0;
	virtual char const *video_bandwidth_on		          (int, char**)=0;
    virtual char const *video_bandwidth_off		          (int, char**)=0;
  
    virtual char const *video_rtcp_bind                   (int, char**)=0;
    virtual char const *video_rtcp_unbind                 (int, char**)=0;
    virtual char const *video_rtcp_show_bindings          (int, char**)=0;

    virtual char const *video_set_sdesinfo                (int, char**)=0;
    virtual char const *video_show_sdesinfo               (int, char**)=0;
    virtual char const *video_show_receivers              (int, char**)=0;
    virtual char const *video_get_lost                    (int, char**)=0;
    virtual char const *video_get_local_lost              (int, char**)=0;
    virtual char const *video_get_fractionlost            (int, char**)=0;
    virtual char const *video_get_roundtrip_time          (int, char**)=0;
    virtual char const *video_get_jitter                  (int, char**)=0;
	virtual char const *video_compute_stats				  (int, char**)=0;
	virtual char const *video_bucket_size				  (int, char**)=0;
    virtual char const *video_configure_channel           (int, char**)=0; 
	
	//
    // codecs
    //
    virtual char const *video_list_codecs                 (int, char**)=0;
    virtual char const *video_select_codec                (int, char**)=0;
	virtual char const *video_set_payload                 (int, char**)=0;
    virtual char const *video_list_codecs_ex              (int, char**)=0;  

    //
    // sources
    //

	virtual char const *video_list_sources                (int, char**)=0;
    virtual char const *video_select_source               (int, char**)=0;
    virtual char const *video_current_source              (int, char**)=0;
    virtual char const *video_list_source_formats         (int, char**)=0;
	virtual char const *video_source_format               (int, char**)=0;
    virtual char const *video_grab_size                   (int, char**)=0;
	virtual char const *video_set_test_source             (int, char**)=0;
	virtual char const *video_set_shared_display_source   (int, char**)=0;
	virtual char const *video_set_source_rate             (int, char**)=0;
	virtual char const *video_set_rect                    (int, char**)=0;
    virtual char const *video_play                        (int, char**)=0;
    virtual char const *video_stop                        (int, char**)=0;

    
    //
    // system
    //

	virtual char const *video_nop                         (int, char**)=0;
    virtual char const *video_bye                         (int, char**)=0;
    virtual char const *video_quit                        (int, char**)=0;
};


extern binding_t<video_InterfaceDef> video_methodBinding[];

typedef interface_t<video_InterfaceDef, video_methodBinding> videoInterface_t;


/**
 <class> 
   <name>video_InterfaceDef_t</name> 
   <descr>
   This class defines all commands allowed by the $/
   control TCP Server interface.
   </descr>
**/

class video_InterfaceDef_t: public videoInterface_t
{

public:

    int  interID;

    video_InterfaceDef_t(io_ref &io): videoInterface_t(io)
	{ 
		interID= (int)this; 
	}

	//
	// channel
	//
	virtual char const *video_query_id		              (int, char**);

	virtual char const *video_assign_channel              (int, char**);
	virtual char const *video_winGrabber_channel          (int, char**);
    virtual char const *video_new_channel                 (int, char**);
	virtual char const *video_delete_channel              (int, char**);
   
	virtual char const *video_map_channel                 (int, char**);
    virtual char const *video_unmap_channel               (int, char**);
    
	virtual char const *video_set_buffering               (int, char**);

	virtual char const *video_set_window_title            (int, char**);
    virtual char const *video_set_window_geometry         (int, char**);
    virtual char const *video_set_window_size             (int, char**);
	virtual char const *video_follow_size                 (int, char**);
	virtual char const *video_full_screen_window          (int, char**);
	virtual char const *video_set_window_owner            (int, char**);
    virtual char const *video_configure_channel           (int, char**);

	//
	// bindings
	//
    virtual char const *video_bind                        (int, char**);
    virtual char const *video_unbind                      (int, char**);
    virtual char const *video_show_bindings               (int, char**);
	virtual char const *video_channel_bandwidth           (int, char**);
	virtual char const *video_bandwidth_on		          (int, char**);
    virtual char const *video_bandwidth_off		          (int, char**);
  
    virtual char const *video_rtcp_bind                   (int, char**);
    virtual char const *video_rtcp_unbind                 (int, char**);
    virtual char const *video_rtcp_show_bindings          (int, char**);

    virtual char const *video_set_sdesinfo                (int, char**);
    virtual char const *video_show_sdesinfo               (int, char**);
    virtual char const *video_show_receivers              (int, char**);
    virtual char const *video_get_lost                    (int, char**);
    virtual char const *video_get_local_lost              (int, char**);
    virtual char const *video_get_fractionlost            (int, char**);
    virtual char const *video_get_roundtrip_time          (int, char**);
    virtual char const *video_get_jitter                  (int, char**);
	virtual char const *video_compute_stats				  (int, char**);
	virtual char const *video_bucket_size				  (int, char**);
	
    
	//
    // codecs
    //
    virtual char const *video_list_codecs                 (int, char**);
	virtual char const *video_set_payload                 (int, char**);
    virtual char const *video_select_codec                (int, char**);
    virtual char const *video_list_codecs_ex              (int, char**);
 	
	//
    // sources
    //
    virtual char const *video_list_sources                (int, char**);
    virtual char const *video_select_source               (int, char**);
    virtual char const *video_current_source              (int, char**);
    virtual char const *video_list_source_formats         (int, char**);
	virtual char const *video_source_format               (int, char**);
    virtual char const *video_grab_size                   (int, char**);
	virtual char const *video_set_test_source             (int, char**);
	virtual char const *video_set_shared_display_source   (int, char**);
	virtual char const *video_set_source_rate             (int, char**);
	virtual char const *video_set_rect                    (int, char**);
    virtual char const *video_play                        (int, char**);
    virtual char const *video_stop                        (int, char**);

    
	//
    // system
    //
    virtual char const *video_nop                         (int, char**);
    virtual char const *video_bye                         (int, char**);
    virtual char const *video_quit                        (int, char**);
    
    virtual void bailOut(void){ s->removeTask(this); }
};

/**
 <class> 
   <name>ctrlTask_t</name> 
   <descr>
   This class is used to create a control TCP Server $/
   at any port. This control Server is used to send commands $/
   to the video daemon app.   
   </descr>
**/

class ctrlTask_t : public item_t
{
public:

	ctrlTask_t(char * port)
	{
		inetAddr_t ctrlAddr("0.0.0.0", port , serviceAddr_t::STREAM);
		streamSocket_t *ctrlSock= new streamSocket_t(ctrlAddr);
		ctrlSock->listen(ctrlAddr);
		task_ref ctrltask = new tcpServer_t<video_InterfaceDef_t>(*ctrlSock);
		s->insertTask(ctrltask); 
		debugMsg(dbg_App_Normal,
			"CtrlProtocol",
			"CtrlPort Created at %s",
			port);

	}
};

#endif

