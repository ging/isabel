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
// $Id: ctrlProtocol.hh 22392 2011-05-20 14:09:42Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __ctrl_protocol_hh__
#define __ctrl_protocol_hh__


#include <icf2/stdTask.hh>

class video_InterfaceDef_t
{
public:

    int interID;

    video_InterfaceDef_t(void) { interID= (int)this; }

    virtual ~video_InterfaceDef_t(void)
    {
       // got error if virtual methods and no virtual destructor
    }

    // VIDEO PRIMITIVES

    //
    // channels
    //
    virtual const char *video_rec_channel                 (int, char**);
    virtual const char *video_shdisp_channel              (int, char**);
    virtual const char *video_net_channel                 (int, char**);
    virtual const char *video_play_channel                (int, char**);

    virtual const char *video_delete_channel              (int, char**);

    //
    // transmission
    //
    virtual const char *video_bind                        (int, char**);
    virtual const char *video_unbind                      (int, char**);
    virtual const char *video_show_bindings               (int, char**);

    virtual const char *video_rtcp_bind                   (int, char**);
    virtual const char *video_rtcp_unbind                 (int, char**);
    virtual const char *video_rtcp_show_bindings          (int, char**);

    virtual const char *video_send                        (int, char**);

    virtual const char *video_bucket_size                 (int, char**);
    virtual const char *video_channel_bandwidth           (int, char**);

    //
    // presentation
    //
    virtual const char *video_map_channel                 (int, char**);
    virtual const char *video_unmap_channel               (int, char**);

    virtual const char *video_window_title                (int, char**);
    virtual const char *video_window_geometry             (int, char**);
    virtual const char *video_window_overrideredirect     (int, char**);

    virtual const char *video_follow_size                 (int, char**);
    virtual const char *video_get_winid                   (int, char**);

    //
    // stats
    //
    virtual const char *video_compute_stats               (int, char**);

    virtual const char *video_do_stats                    (int, char**);

    virtual const char *video_set_sdesinfo                (int, char**);
    virtual const char *video_show_sdesinfo               (int, char**);
    virtual const char *video_show_receivers              (int, char**);
    virtual const char *video_get_lost                    (int, char**);
    virtual const char *video_get_local_lost              (int, char**);
    virtual const char *video_get_fractionlost            (int, char**);
    virtual const char *video_get_roundtrip_time          (int, char**);
    virtual const char *video_get_jitter                  (int, char**);

    //
    // general configuration
    //
    virtual const char *video_configure_channel           (int, char**);

    //
    // sources
    //
    virtual const char *video_interactive_select          (int, char**);
    virtual const char *video_select_by_winid             (int, char**);

    virtual const char *video_update_sources              (int, char**);
    virtual const char *video_list_sources                (int, char**);
    virtual const char *video_select_source               (int, char**);
    virtual const char *video_current_source              (int, char**);
    virtual const char *video_avatar                      (int, char**);
    virtual const char *video_avatar_timeout              (int, char**);

    virtual const char *video_frame_rate                  (int, char**);
    virtual const char *video_send_frames                 (int, char**);

    virtual char const *video_source_format               (int, char**);
    virtual const char *video_grab_size                   (int, char**);

    virtual const char *video_list_norms                  (int, char**);
    virtual const char *video_set_norm                    (int, char**);
    virtual const char *video_get_norm                    (int, char**);
    virtual const char *video_list_flickerfreqs           (int, char**);
    virtual const char *video_set_flickerfreq             (int, char**);
    virtual const char *video_get_flickerfreq             (int, char**);
    virtual const char *video_set_saturation              (int, char**);
    virtual const char *video_get_saturation              (int, char**);
    virtual const char *video_set_brightness              (int, char**);
    virtual const char *video_get_brightness              (int, char**);
    virtual const char *video_set_hue                     (int, char**);
    virtual const char *video_get_hue                     (int, char**);
    virtual const char *video_set_contrast                (int, char**);
    virtual const char *video_get_contrast                (int, char**);

    virtual const char *video_set_flip                    (int, char**);
    virtual const char *video_get_flip                    (int, char**);
    virtual const char *video_set_mirror                  (int, char**);
    virtual const char *video_get_mirror                  (int, char**);

    virtual const char *video_crop                        (int, char**);
    virtual const char *video_get_crop                    (int, char**);

    //
    // codecs
    //
    virtual const char *video_list_codecs                 (int, char**);
    virtual const char *video_select_codec                (int, char**);
    virtual const char *video_codec_quality               (int, char**);
    virtual const char *video_set_h263_mode               (int, char**);
    virtual const char *video_set_mpeg_pbetweeni          (int, char**);

    //
    // common stuf
    //
    virtual const char *video_query_id                    (int, char**);
    virtual const char *video_nop                         (int, char**);
    virtual const char *video_bye                         (int, char**);
    virtual const char *video_quit                        (int, char**);
};


extern binding_t<video_InterfaceDef_t> video_methodBinding[];

typedef interface_t
    <video_InterfaceDef_t, video_methodBinding> videoInterface_t;

#endif
